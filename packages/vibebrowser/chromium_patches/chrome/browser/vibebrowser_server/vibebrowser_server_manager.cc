diff --git a/chrome/browser/VibeBrowser_server/VibeBrowser_server_manager.cc b/chrome/browser/VibeBrowser_server/VibeBrowser_server_manager.cc
new file mode 100644
index 0000000000000..e6bd851f42bfa
--- /dev/null
+++ b/chrome/browser/VibeBrowser_server/VibeBrowser_server_manager.cc
@@ -0,0 +1,863 @@
+// Copyright 2024 The Chromium Authors
+// Use of this source code is governed by a BSD-style license that can be
+// found in the LICENSE file.
+
+#include "chrome/browser/VibeBrowser_server/VibeBrowser_server_manager.h"
+
+#include "base/command_line.h"
+#include "base/files/file_path.h"
+#include "base/files/file_util.h"
+#include "base/json/json_writer.h"
+#include "base/logging.h"
+#include "base/path_service.h"
+#include "base/process/kill.h"
+#include "base/process/launch.h"
+#include "base/rand_util.h"
+#include "base/strings/string_number_conversions.h"
+#include "base/system/sys_info.h"
+#include "base/task/thread_pool.h"
+#include "base/threading/thread_restrictions.h"
+#include "build/build_config.h"
+#include "chrome/browser/browser_process.h"
+#include "chrome/browser/VibeBrowser_server/VibeBrowser_server_prefs.h"
+#include "chrome/browser/net/system_network_context_manager.h"
+#include "chrome/browser/profiles/profile.h"
+#include "chrome/browser/profiles/profile_manager.h"
+#include "chrome/common/chrome_paths.h"
+#include "components/metrics/VibeBrowser_metrics/VibeBrowser_metrics_service.h"
+#include "components/metrics/VibeBrowser_metrics/VibeBrowser_metrics_service_factory.h"
+#include "components/prefs/pref_change_registrar.h"
+#include "components/prefs/pref_service.h"
+#include "components/version_info/version_info.h"
+#include "content/public/browser/devtools_agent_host.h"
+#include "content/public/browser/devtools_socket_factory.h"
+#include "content/public/browser/storage_partition.h"
+#include "net/base/net_errors.h"
+#include "net/base/port_util.h"
+#include "net/log/net_log_source.h"
+#include "net/socket/tcp_server_socket.h"
+#include "net/traffic_annotation/network_traffic_annotation.h"
+#include "services/network/public/cpp/resource_request.h"
+#include "services/network/public/cpp/simple_url_loader.h"
+#include "services/network/public/mojom/url_loader_factory.mojom.h"
+#include "url/gurl.h"
+
+namespace {
+
+const int kBackLog = 10;
+
+// Helper function to check for command-line port override.
+// Returns the port value if valid override is found, 0 otherwise.
+int GetPortOverrideFromCommandLine(base::CommandLine* command_line,
+                                    const char* switch_name,
+                                    const char* port_name) {
+  if (!command_line->HasSwitch(switch_name)) {
+    return 0;
+  }
+
+  std::string port_str = command_line->GetSwitchValueASCII(switch_name);
+  int port = 0;
+
+  if (!base::StringToInt(port_str, &port) || !net::IsPortValid(port) ||
+      port <= 0) {
+    LOG(WARNING) << "VibeBrowser: Invalid " << port_name
+                 << " specified on command line: " << port_str
+                 << " (must be 1-65535)";
+    return 0;
+  }
+
+  // Warn about problematic ports but respect explicit user intent
+  if (net::IsWellKnownPort(port)) {
+    LOG(WARNING) << "VibeBrowser: " << port_name << " " << port
+                 << " is well-known (0-1023) and may require elevated "
+                    "privileges";
+  }
+  if (!net::IsPortAllowedForScheme(port, "http")) {
+    LOG(WARNING) << "VibeBrowser: " << port_name << " " << port
+                 << " is restricted by Chromium (may interfere with system "
+                    "services)";
+  }
+
+  LOG(INFO) << "VibeBrowser: " << port_name
+            << " overridden via command line: " << port;
+  return port;
+}
+
+// Launches the VibeBrowser server process on a background thread.
+// This function performs blocking I/O operations (PathExists, LaunchProcess).
+base::Process LaunchProcessOnBackgroundThread(
+    const base::FilePath& exe_path,
+    uint16_t cdp_port,
+    uint16_t mcp_port,
+    uint16_t agent_port,
+    uint16_t extension_port) {
+  // Check if executable exists (blocking I/O)
+  if (!base::PathExists(exe_path)) {
+    LOG(ERROR) << "VibeBrowser: VibeBrowser server executable not found at: "
+               << exe_path;
+    return base::Process();
+  }
+
+  // Build command line
+  base::CommandLine cmd(exe_path);
+  cmd.AppendSwitchASCII("cdp-port", base::NumberToString(cdp_port));
+  cmd.AppendSwitchASCII("http-mcp-port", base::NumberToString(mcp_port));
+  cmd.AppendSwitchASCII("agent-port", base::NumberToString(agent_port));
+  cmd.AppendSwitchASCII("extension-port", base::NumberToString(extension_port));
+
+  // Set up launch options
+  base::LaunchOptions options;
+#if BUILDFLAG(IS_WIN)
+  options.start_hidden = true;
+#endif
+
+  // Launch the process (blocking I/O)
+  return base::LaunchProcess(cmd, options);
+}
+
+// Factory for creating TCP server sockets for CDP
+class CDPServerSocketFactory : public content::DevToolsSocketFactory {
+ public:
+  explicit CDPServerSocketFactory(uint16_t port) : port_(port) {}
+
+  CDPServerSocketFactory(const CDPServerSocketFactory&) = delete;
+  CDPServerSocketFactory& operator=(const CDPServerSocketFactory&) = delete;
+
+ private:
+  std::unique_ptr<net::ServerSocket> CreateLocalHostServerSocket(int port) {
+    std::unique_ptr<net::ServerSocket> socket(
+        new net::TCPServerSocket(nullptr, net::NetLogSource()));
+    if (socket->ListenWithAddressAndPort("127.0.0.1", port, kBackLog) ==
+        net::OK) {
+      return socket;
+    }
+    if (socket->ListenWithAddressAndPort("::1", port, kBackLog) == net::OK) {
+      return socket;
+    }
+    return nullptr;
+  }
+
+  // content::DevToolsSocketFactory implementation
+  std::unique_ptr<net::ServerSocket> CreateForHttpServer() override {
+    return CreateLocalHostServerSocket(port_);
+  }
+
+  std::unique_ptr<net::ServerSocket> CreateForTethering(
+      std::string* name) override {
+    return nullptr;  // Tethering not needed for VibeBrowser
+  }
+
+  uint16_t port_;
+};
+
+}  // namespace
+
+// static
+VibeBrowserServerManager* VibeBrowserServerManager::GetInstance() {
+  static base::NoDestructor<VibeBrowserServerManager> instance;
+  return instance.get();
+}
+
+VibeBrowserServerManager::VibeBrowserServerManager() = default;
+
+VibeBrowserServerManager::~VibeBrowserServerManager() {
+  Shutdown();
+}
+
+void VibeBrowserServerManager::Start() {
+  if (is_running_) {
+    LOG(INFO) << "VibeBrowser: VibeBrowser server already running";
+    return;
+  }
+
+  // Check if server is disabled via command line
+  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
+  if (command_line->HasSwitch("disable-VibeBrowser-server")) {
+    LOG(INFO) << "VibeBrowser: VibeBrowser server disabled via command line";
+    return;
+  }
+
+  LOG(INFO) << "VibeBrowser: Starting VibeBrowser server";
+
+  // Step 1: Start CDP server
+  StartCDPServer();
+
+  // Step 2: Launch VibeBrowser process
+  LaunchVibeBrowserProcess();
+
+  // Step 3: Start health checking every 60 seconds
+  health_check_timer_.Start(FROM_HERE, base::Seconds(60), this,
+                            &VibeBrowserServerManager::CheckServerHealth);
+}
+
+void VibeBrowserServerManager::Stop() {
+  if (!is_running_) {
+    return;
+  }
+
+  LOG(INFO) << "VibeBrowser: Stopping VibeBrowser server";
+  health_check_timer_.Stop();
+  process_check_timer_.Stop();
+
+  TerminateVibeBrowserProcess();
+  StopCDPServer();
+}
+
+bool VibeBrowserServerManager::IsRunning() const {
+  return is_running_ && process_.IsValid();
+}
+
+void VibeBrowserServerManager::Shutdown() {
+  Stop();
+}
+
+void VibeBrowserServerManager::StartCDPServer() {
+  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
+
+  PrefService* prefs = g_browser_process->local_state();
+  if (!prefs) {
+    LOG(ERROR) << "VibeBrowser: Failed to get local state prefs";
+    // Find available ports from standard defaults
+    cdp_port_ = FindAvailablePort(VibeBrowser_server::kDefaultCDPPort);
+    mcp_port_ = FindAvailablePort(VibeBrowser_server::kDefaultMCPPort);
+    agent_port_ = FindAvailablePort(VibeBrowser_server::kDefaultAgentPort);
+    extension_port_ = FindAvailablePort(VibeBrowser_server::kDefaultExtensionPort);
+  } else {
+    // Read CDP port from prefs and find available port
+    int saved_cdp_port = prefs->GetInteger(VibeBrowser_server::kCDPServerPort);
+    int cdp_starting_port = (saved_cdp_port > 0) ? saved_cdp_port : VibeBrowser_server::kDefaultCDPPort;
+    cdp_port_ = FindAvailablePort(cdp_starting_port);
+
+    // Read MCP settings
+    int saved_mcp_port = prefs->GetInteger(VibeBrowser_server::kMCPServerPort);
+    int mcp_starting_port = (saved_mcp_port > 0) ? saved_mcp_port : VibeBrowser_server::kDefaultMCPPort;
+    mcp_port_ = FindAvailablePort(mcp_starting_port);
+
+    // Read Agent port from prefs and find available port
+    int saved_agent_port = prefs->GetInteger(VibeBrowser_server::kAgentServerPort);
+    int agent_starting_port = (saved_agent_port > 0) ? saved_agent_port : VibeBrowser_server::kDefaultAgentPort;
+    agent_port_ = FindAvailablePort(agent_starting_port);
+
+    // Read Extension port from prefs and find available port
+    int saved_extension_port = prefs->GetInteger(VibeBrowser_server::kExtensionServerPort);
+    int extension_starting_port = (saved_extension_port > 0) ? saved_extension_port : VibeBrowser_server::kDefaultExtensionPort;
+    extension_port_ = FindAvailablePort(extension_starting_port);
+
+    mcp_enabled_ = prefs->GetBoolean(VibeBrowser_server::kMCPServerEnabled);
+
+    // Set up preference change observer for MCP enabled flag
+    if (!pref_change_registrar_) {
+      pref_change_registrar_ = std::make_unique<PrefChangeRegistrar>();
+      pref_change_registrar_->Init(prefs);
+      pref_change_registrar_->Add(
+          VibeBrowser_server::kMCPServerEnabled,
+          base::BindRepeating(&VibeBrowserServerManager::OnMCPEnabledChanged,
+                              base::Unretained(this)));
+    }
+  }
+
+  LOG(INFO) << "VibeBrowser: Ports allocated - CDP: " << cdp_port_
+            << ", MCP: " << mcp_port_ << ", Agent: " << agent_port_
+            << ", Extension: " << extension_port_;
+
+  // Check for command-line port overrides
+  int mcp_override = GetPortOverrideFromCommandLine(
+      command_line, "VibeBrowser-mcp-port", "MCP port");
+  if (mcp_override > 0) {
+    mcp_port_ = mcp_override;
+    // Implicitly enable MCP when port is specified
+    mcp_enabled_ = true;
+    LOG(INFO) << "VibeBrowser: MCP server implicitly enabled via command line";
+  }
+
+  int agent_override = GetPortOverrideFromCommandLine(
+      command_line, "VibeBrowser-agent-port", "Agent port");
+  if (agent_override > 0) {
+    agent_port_ = agent_override;
+  }
+
+  int extension_override = GetPortOverrideFromCommandLine(
+      command_line, "VibeBrowser-extension-port", "Extension port");
+  if (extension_override > 0) {
+    extension_port_ = extension_override;
+  }
+
+  LOG(INFO) << "VibeBrowser: Starting CDP server on port " << cdp_port_;
+
+  // Start Chromium's built-in DevTools remote debugging server
+  content::DevToolsAgentHost::StartRemoteDebuggingServer(
+      std::make_unique<CDPServerSocketFactory>(cdp_port_),
+      base::FilePath(),  // No output dir needed
+      base::FilePath()); // No debug frontend dir
+
+  LOG(INFO) << "VibeBrowser: CDP WebSocket server started at ws://127.0.0.1:"
+            << cdp_port_;
+  LOG(INFO) << "VibeBrowser: MCP server port: " << mcp_port_
+            << " (enabled: " << (mcp_enabled_ ? "true" : "false") << ")";
+}
+
+void VibeBrowserServerManager::StopCDPServer() {
+  if (cdp_port_ == 0) {
+    return;
+  }
+
+  LOG(INFO) << "VibeBrowser: Stopping CDP server";
+  content::DevToolsAgentHost::StopRemoteDebuggingServer();
+  cdp_port_ = 0;
+}
+
+void VibeBrowserServerManager::LaunchVibeBrowserProcess() {
+  // Get executable path on UI thread (PathService::Get is thread-safe)
+  base::FilePath exe_path = GetVibeBrowserServerExecutablePath();
+
+  // Capture values to pass to background thread
+  uint16_t cdp_port = cdp_port_;
+  uint16_t mcp_port = mcp_port_;
+  uint16_t agent_port = agent_port_;
+  uint16_t extension_port = extension_port_;
+
+  // Post blocking work to background thread, get result back on UI thread
+  base::ThreadPool::PostTaskAndReplyWithResult(
+      FROM_HERE, {base::MayBlock(), base::TaskPriority::USER_BLOCKING},
+      base::BindOnce(&LaunchProcessOnBackgroundThread, exe_path, cdp_port,
+                     mcp_port, agent_port, extension_port),
+      base::BindOnce(&VibeBrowserServerManager::OnProcessLaunched,
+                     weak_factory_.GetWeakPtr()));
+}
+
+void VibeBrowserServerManager::OnProcessLaunched(base::Process process) {
+  if (!process.IsValid()) {
+    LOG(ERROR) << "VibeBrowser: Failed to launch VibeBrowser server";
+    StopCDPServer();
+    return;
+  }
+
+  process_ = std::move(process);
+  is_running_ = true;
+
+  LOG(INFO) << "VibeBrowser: VibeBrowser server started";
+  LOG(INFO) << "VibeBrowser: CDP port: " << cdp_port_;
+  LOG(INFO) << "VibeBrowser: MCP port: " << mcp_port_;
+  LOG(INFO) << "VibeBrowser: Agent port: " << agent_port_;
+  LOG(INFO) << "VibeBrowser: Extension port: " << extension_port_;
+
+  // Save prefs to Local State now that we know the server launched
+  PrefService* prefs = g_browser_process->local_state();
+  if (prefs) {
+    prefs->SetInteger(VibeBrowser_server::kCDPServerPort, cdp_port_);
+    prefs->SetInteger(VibeBrowser_server::kMCPServerPort, mcp_port_);
+    prefs->SetInteger(VibeBrowser_server::kAgentServerPort, agent_port_);
+    prefs->SetInteger(VibeBrowser_server::kExtensionServerPort, extension_port_);
+    prefs->SetBoolean(VibeBrowser_server::kMCPServerEnabled, mcp_enabled_);
+    LOG(INFO) << "VibeBrowser: Saved prefs to Local State";
+  }
+
+  // Start monitoring the process
+  process_check_timer_.Start(FROM_HERE, base::Seconds(5), this,
+                             &VibeBrowserServerManager::CheckProcessStatus);
+
+  // /init will be sent after first successful periodic health check
+
+  // If MCP is disabled, send control request to disable it
+  if (!mcp_enabled_) {
+    SendMCPControlRequest(false);
+  }
+}
+
+void VibeBrowserServerManager::TerminateVibeBrowserProcess() {
+  if (!process_.IsValid()) {
+    return;
+  }
+
+  LOG(INFO) << "VibeBrowser: Terminating VibeBrowser server process";
+
+  // Reset init flag so it gets sent again after restart
+  init_request_sent_ = false;
+
+  // Try graceful shutdown first
+  process_.Terminate(0, false);
+
+  // Give it some time to shut down, then force kill if still running
+  base::ThreadPool::PostDelayedTask(
+      FROM_HERE, {base::MayBlock()},
+      base::BindOnce(
+          [](base::Process process) {
+            if (process.IsValid()) {
+              // Force kill if still running
+              process.Terminate(0, false);
+            }
+          },
+          process_.Duplicate()),
+      base::Seconds(2));
+
+  is_running_ = false;
+}
+
+void VibeBrowserServerManager::OnProcessExited(int exit_code) {
+  LOG(INFO) << "VibeBrowser: VibeBrowser server exited with code: " << exit_code;
+  is_running_ = false;
+
+  // Stop CDP server since VibeBrowser process is gone
+  StopCDPServer();
+
+  // Restart if it crashed unexpectedly
+  if (exit_code != 0) {
+    LOG(WARNING) << "VibeBrowser: VibeBrowser server crashed, restarting...";
+    Start();
+  }
+}
+
+void VibeBrowserServerManager::CheckServerHealth() {
+  if (!is_running_) {
+    return;
+  }
+
+  // First check if process is still alive
+  if (!process_.IsValid()) {
+    LOG(WARNING) << "VibeBrowser: VibeBrowser server process is invalid, restarting...";
+    RestartVibeBrowserProcess();
+    return;
+  }
+
+  // Build health check URL
+  GURL health_url("http://127.0.0.1:" + base::NumberToString(mcp_port_) + "/health");
+
+  // Create network traffic annotation
+  net::NetworkTrafficAnnotationTag traffic_annotation =
+      net::DefineNetworkTrafficAnnotation("VibeBrowser_health_check", R"(
+        semantics {
+          sender: "VibeBrowser Server Manager"
+          description:
+            "Checks if the VibeBrowser MCP server is healthy by querying its "
+            "/health endpoint."
+          trigger: "Periodic health check every 60 seconds while server is running."
+          data: "No user data sent, just an HTTP GET request."
+          destination: LOCAL
+        }
+        policy {
+          cookies_allowed: NO
+          setting: "This feature cannot be disabled by settings."
+          policy_exception_justification:
+            "Internal health check for VibeBrowser server functionality."
+        })");
+
+  // Create resource request
+  auto resource_request = std::make_unique<network::ResourceRequest>();
+  resource_request->url = health_url;
+  resource_request->method = "GET";
+  resource_request->credentials_mode = network::mojom::CredentialsMode::kOmit;
+
+  // Create URL loader with 10 second timeout
+  auto url_loader = network::SimpleURLLoader::Create(
+      std::move(resource_request), traffic_annotation);
+  url_loader->SetTimeoutDuration(base::Seconds(10));
+
+  // Get URL loader factory from default storage partition
+  auto* url_loader_factory =
+      g_browser_process->system_network_context_manager()
+          ->GetURLLoaderFactory();
+
+  // Keep a raw pointer for the callback
+  auto* url_loader_ptr = url_loader.get();
+
+  // Download response
+  url_loader_ptr->DownloadHeadersOnly(
+      url_loader_factory,
+      base::BindOnce(&VibeBrowserServerManager::OnHealthCheckComplete,
+                     weak_factory_.GetWeakPtr(), std::move(url_loader)));
+}
+
+void VibeBrowserServerManager::CheckProcessStatus() {
+  if (!is_running_ || !process_.IsValid()) {
+    return;
+  }
+
+  // Check if process has exited
+  int exit_code = 0;
+  if (process_.WaitForExitWithTimeout(base::TimeDelta(), &exit_code)) {
+    // Process has exited
+    OnProcessExited(exit_code);
+  }
+}
+
+void VibeBrowserServerManager::OnHealthCheckComplete(
+    std::unique_ptr<network::SimpleURLLoader> url_loader,
+    scoped_refptr<net::HttpResponseHeaders> headers) {
+  if (!is_running_) {
+    return;
+  }
+
+  // Check if we got a valid response
+  int response_code = 0;
+  if (headers) {
+    response_code = headers->response_code();
+  }
+
+  if (response_code == 200) {
+    // Health check passed
+    LOG(INFO) << "VibeBrowser: Health check passed";
+
+    // Send /init request on first successful health check
+    if (!init_request_sent_) {
+      init_request_sent_ = true;
+      SendInitRequest();
+    }
+    return;
+  }
+
+  // Health check failed
+  int net_error = url_loader->NetError();
+  LOG(WARNING) << "VibeBrowser: Health check failed - HTTP " << response_code
+               << ", net error: " << net::ErrorToString(net_error)
+               << ", restarting VibeBrowser server process...";
+
+  RestartVibeBrowserProcess();
+}
+
+void VibeBrowserServerManager::RestartVibeBrowserProcess() {
+  LOG(INFO) << "VibeBrowser: Restarting VibeBrowser server process";
+
+  // Stop the process and monitoring
+  process_check_timer_.Stop();
+  TerminateVibeBrowserProcess();
+
+  // Relaunch the process
+  LaunchVibeBrowserProcess();
+}
+
+void VibeBrowserServerManager::OnMCPEnabledChanged() {
+  if (!is_running_) {
+    return;
+  }
+
+  PrefService* prefs = g_browser_process->local_state();
+  if (!prefs) {
+    return;
+  }
+
+  bool new_value = prefs->GetBoolean(VibeBrowser_server::kMCPServerEnabled);
+
+  if (new_value != mcp_enabled_) {
+    LOG(INFO) << "VibeBrowser: MCP enabled preference changed from "
+              << (mcp_enabled_ ? "true" : "false") << " to "
+              << (new_value ? "true" : "false");
+
+    mcp_enabled_ = new_value;
+    SendMCPControlRequest(new_value);
+  }
+}
+
+void VibeBrowserServerManager::SendMCPControlRequest(bool enabled) {
+  if (!is_running_) {
+    return;
+  }
+
+  GURL control_url("http://127.0.0.1:" + base::NumberToString(mcp_port_) +
+                   "/mcp/control");
+
+  net::NetworkTrafficAnnotationTag traffic_annotation =
+      net::DefineNetworkTrafficAnnotation("VibeBrowser_mcp_control", R"(
+        semantics {
+          sender: "VibeBrowser Server Manager"
+          description:
+            "Sends control command to VibeBrowser MCP server to enable/disable "
+            "the MCP protocol at runtime."
+          trigger: "User changes MCP enabled preference."
+          data: "JSON payload with enabled state: {\"enabled\": true/false}"
+          destination: LOCAL
+        }
+        policy {
+          cookies_allowed: NO
+          setting: "This feature cannot be disabled by settings."
+          policy_exception_justification:
+            "Internal control request for VibeBrowser server functionality."
+        })");
+
+  auto resource_request = std::make_unique<network::ResourceRequest>();
+  resource_request->url = control_url;
+  resource_request->method = "POST";
+  resource_request->credentials_mode = network::mojom::CredentialsMode::kOmit;
+  resource_request->headers.SetHeader("Content-Type", "application/json");
+
+  std::string json_body = enabled ? "{\"enabled\":true}" : "{\"enabled\":false}";
+
+  auto url_loader = network::SimpleURLLoader::Create(
+      std::move(resource_request), traffic_annotation);
+  url_loader->AttachStringForUpload(json_body, "application/json");
+  url_loader->SetTimeoutDuration(base::Seconds(10));
+
+  auto* url_loader_factory =
+      g_browser_process->system_network_context_manager()
+          ->GetURLLoaderFactory();
+
+  auto* url_loader_ptr = url_loader.get();
+
+  url_loader_ptr->DownloadHeadersOnly(
+      url_loader_factory,
+      base::BindOnce(&VibeBrowserServerManager::OnMCPControlRequestComplete,
+                     weak_factory_.GetWeakPtr(), enabled,
+                     std::move(url_loader)));
+
+  LOG(INFO) << "VibeBrowser: Sent MCP control request: {\"enabled\": "
+            << (enabled ? "true" : "false") << "}";
+}
+
+void VibeBrowserServerManager::OnMCPControlRequestComplete(
+    bool requested_state,
+    std::unique_ptr<network::SimpleURLLoader> url_loader,
+    scoped_refptr<net::HttpResponseHeaders> headers) {
+  if (!is_running_) {
+    return;
+  }
+
+  int response_code = 0;
+  if (headers) {
+    response_code = headers->response_code();
+  }
+
+  if (response_code == 200) {
+    LOG(INFO) << "VibeBrowser: MCP control request succeeded - MCP server is now "
+              << (requested_state ? "enabled" : "disabled");
+    return;
+  }
+
+  int net_error = url_loader->NetError();
+  LOG(ERROR) << "VibeBrowser: MCP control request failed - HTTP " << response_code
+             << ", net error: " << net::ErrorToString(net_error);
+}
+
+void VibeBrowserServerManager::SendInitRequest() {
+  if (!is_running_) {
+    return;
+  }
+
+  // Get the default profile to access VibeBrowserMetricsService
+  ProfileManager* profile_manager = g_browser_process->profile_manager();
+  if (!profile_manager) {
+    LOG(ERROR) << "VibeBrowser: Failed to get ProfileManager for /init request";
+    return;
+  }
+
+  Profile* profile = profile_manager->GetLastUsedProfileIfLoaded();
+  if (!profile || profile->IsOffTheRecord()) {
+    LOG(WARNING) << "VibeBrowser: No valid profile available for /init request";
+    return;
+  }
+
+  // Get VibeBrowserMetricsService to retrieve install_id
+  VibeBrowser_metrics::VibeBrowserMetricsService* metrics_service =
+      VibeBrowser_metrics::VibeBrowserMetricsServiceFactory::GetForBrowserContext(
+          profile);
+  if (!metrics_service) {
+    LOG(ERROR) << "VibeBrowser: Failed to get VibeBrowserMetricsService for /init "
+                  "request";
+    return;
+  }
+
+  // Build the /init payload
+  base::Value::Dict payload;
+  payload.Set("client_id", metrics_service->GetInstallId());
+  payload.Set("version", version_info::GetVersionNumber());
+  payload.Set("os", base::SysInfo::OperatingSystemName());
+  payload.Set("arch", base::SysInfo::OperatingSystemArchitecture());
+
+  std::string json_payload;
+  if (!base::JSONWriter::Write(payload, &json_payload)) {
+    LOG(ERROR) << "VibeBrowser: Failed to serialize /init payload";
+    return;
+  }
+
+  GURL init_url("http://127.0.0.1:" + base::NumberToString(mcp_port_) +
+                "/init");
+
+  net::NetworkTrafficAnnotationTag traffic_annotation =
+      net::DefineNetworkTrafficAnnotation("VibeBrowser_server_init", R"(
+        semantics {
+          sender: "VibeBrowser Server Manager"
+          description:
+            "Sends initialization metadata to VibeBrowser MCP server including "
+            "install ID, browser version, OS, and architecture."
+          trigger: "VibeBrowser server process successfully launched."
+          data:
+            "JSON payload with install_id, version, os, and arch. No PII."
+          destination: LOCAL
+        }
+        policy {
+          cookies_allowed: NO
+          setting: "This feature cannot be disabled by settings."
+          policy_exception_justification:
+            "Internal initialization for VibeBrowser server functionality."
+        })");
+
+  auto resource_request = std::make_unique<network::ResourceRequest>();
+  resource_request->url = init_url;
+  resource_request->method = "POST";
+  resource_request->credentials_mode = network::mojom::CredentialsMode::kOmit;
+  resource_request->headers.SetHeader("Content-Type", "application/json");
+
+  auto url_loader = network::SimpleURLLoader::Create(
+      std::move(resource_request), traffic_annotation);
+  url_loader->AttachStringForUpload(json_payload, "application/json");
+  url_loader->SetTimeoutDuration(base::Seconds(10));
+
+  auto* url_loader_factory =
+      g_browser_process->system_network_context_manager()
+          ->GetURLLoaderFactory();
+
+  auto* url_loader_ptr = url_loader.get();
+
+  url_loader_ptr->DownloadHeadersOnly(
+      url_loader_factory,
+      base::BindOnce(&VibeBrowserServerManager::OnInitRequestComplete,
+                     weak_factory_.GetWeakPtr(), std::move(url_loader)));
+
+  LOG(INFO) << "VibeBrowser: Sent /init request to MCP server";
+}
+
+void VibeBrowserServerManager::OnInitRequestComplete(
+    std::unique_ptr<network::SimpleURLLoader> url_loader,
+    scoped_refptr<net::HttpResponseHeaders> headers) {
+  if (!is_running_) {
+    return;
+  }
+
+  int response_code = 0;
+  if (headers) {
+    response_code = headers->response_code();
+  }
+
+  if (response_code == 200) {
+    LOG(INFO) << "VibeBrowser: /init request succeeded";
+    return;
+  }
+
+  int net_error = url_loader->NetError();
+  LOG(WARNING) << "VibeBrowser: /init request failed - HTTP " << response_code
+               << ", net error: " << net::ErrorToString(net_error);
+}
+
+int VibeBrowserServerManager::FindAvailablePort(int starting_port) {
+  const int kMaxPortAttempts = 100;
+  const int kMaxPort = 65535;
+
+  LOG(INFO) << "VibeBrowser: Finding port starting from "
+            << starting_port;
+
+  for (int i = 0; i < kMaxPortAttempts; i++) {
+    int port_to_try = starting_port + i;
+
+    // Don't exceed max valid port number
+    if (port_to_try > kMaxPort) {
+      break;
+    }
+
+    if (IsPortAvailable(port_to_try)) {
+      if (port_to_try != starting_port) {
+        LOG(INFO) << "VibeBrowser: Port " << starting_port
+                  << " was in use, using " << port_to_try << " instead";
+      } else {
+        LOG(INFO) << "VibeBrowser: Using port " << port_to_try;
+      }
+      return port_to_try;
+    }
+  }
+
+  // Fallback to starting port if we couldn't find anything
+  LOG(WARNING) << "VibeBrowser: Could not find available port after "
+               << kMaxPortAttempts
+               << " attempts, using " << starting_port << " anyway";
+  return starting_port;
+}
+
+bool VibeBrowserServerManager::IsPortAvailable(int port) {
+  // Check port is in valid range
+  if (!net::IsPortValid(port) || port == 0) {
+    return false;
+  }
+
+  // Avoid well-known ports (0-1023, require elevated privileges)
+  if (net::IsWellKnownPort(port)) {
+    return false;
+  }
+
+  // Avoid restricted ports (could interfere with system services)
+  if (!net::IsPortAllowedForScheme(port, "http")) {
+    return false;
+  }
+
+  // Try to bind to both IPv4 and IPv6 localhost
+  // If EITHER is in use, the port is NOT available
+  std::unique_ptr<net::TCPServerSocket> socket(
+      new net::TCPServerSocket(nullptr, net::NetLogSource()));
+
+  // Try binding to IPv4 localhost
+  int result = socket->ListenWithAddressAndPort("127.0.0.1", port, 1);
+  if (result != net::OK) {
+    return false;  // IPv4 port is in use
+  }
+
+  // Try binding to IPv6 localhost
+  std::unique_ptr<net::TCPServerSocket> socket6(
+      new net::TCPServerSocket(nullptr, net::NetLogSource()));
+  int result6 = socket6->ListenWithAddressAndPort("::1", port, 1);
+  if (result6 != net::OK) {
+    return false;  // IPv6 port is in use
+  }
+
+  return true;
+}
+
+base::FilePath VibeBrowserServerManager::GetVibeBrowserServerExecutablePath() const {
+  // Check for command-line override first
+  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
+  if (command_line->HasSwitch("VibeBrowser-server-binary")) {
+    base::FilePath custom_path =
+        command_line->GetSwitchValuePath("VibeBrowser-server-binary");
+    LOG(INFO) << "VibeBrowser: Using custom server binary from command line: "
+              << custom_path;
+    return custom_path;
+  }
+
+  base::FilePath exe_dir;
+
+#if BUILDFLAG(IS_MAC)
+  // On macOS, the binary will be in the app bundle
+  if (!base::PathService::Get(base::DIR_EXE, &exe_dir)) {
+    LOG(ERROR) << "VibeBrowser: Failed to get executable directory";
+    return base::FilePath();
+  }
+
+  // Navigate to Resources folder in the app bundle
+  // Chrome.app/Contents/MacOS -> Chrome.app/Contents/Resources
+  exe_dir = exe_dir.DirName().Append("Resources");
+
+#elif BUILDFLAG(IS_WIN)
+  // On Windows, installer places VibeBrowser Server under the versioned directory
+  if (!base::PathService::Get(base::DIR_EXE, &exe_dir)) {
+    LOG(ERROR) << "VibeBrowser: Failed to get executable directory";
+    return base::FilePath();
+  }
+  // Append version directory (chrome.release places VibeBrowserServer under versioned dir)
+  // chrome/installer/mini_installer/chrome.release
+  exe_dir = exe_dir.AppendASCII(version_info::GetVersionNumber());
+
+#elif BUILDFLAG(IS_LINUX)
+  // On Linux, binary is in the same directory as chrome
+  if (!base::PathService::Get(base::DIR_EXE, &exe_dir)) {
+    LOG(ERROR) << "VibeBrowser: Failed to get executable directory";
+    return base::FilePath();
+  }
+#endif
+
+  // Navigate to VibeBrowserServer/default/ subdirectory
+  // This structure allows future updates to install to versioned directories
+  base::FilePath VibeBrowser_exe = exe_dir.Append(FILE_PATH_LITERAL("VibeBrowserServer"))
+                                  .Append(FILE_PATH_LITERAL("default"))
+                                  .Append(FILE_PATH_LITERAL("VibeBrowser_server"));
+
+#if BUILDFLAG(IS_WIN)
+  VibeBrowser_exe = VibeBrowser_exe.AddExtension(FILE_PATH_LITERAL(".exe"));
+#endif
+
+  return VibeBrowser_exe;
+}
