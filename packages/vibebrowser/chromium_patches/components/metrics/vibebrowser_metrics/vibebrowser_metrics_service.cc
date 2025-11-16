diff --git a/components/metrics/VibeBrowser_metrics/VibeBrowser_metrics_service.cc b/components/metrics/VibeBrowser_metrics/VibeBrowser_metrics_service.cc
new file mode 100644
index 0000000000000..4f592a2c7b95b
--- /dev/null
+++ b/components/metrics/VibeBrowser_metrics/VibeBrowser_metrics_service.cc
@@ -0,0 +1,231 @@
+// Copyright 2025 The Chromium Authors
+// Use of this source code is governed by a BSD-style license that can be
+// found in the LICENSE file.
+
+#include "components/metrics/VibeBrowser_metrics/VibeBrowser_metrics_service.h"
+
+#include <memory>
+#include <string>
+
+#include "base/uuid.h"
+#include "base/json/json_writer.h"
+#include "base/logging.h"
+#include "base/strings/string_number_conversions.h"
+#include "base/system/sys_info.h"
+#include "base/time/time.h"
+#include "chrome/common/pref_names.h"
+#include "components/prefs/pref_service.h"
+#include "components/version_info/version_info.h"
+#include "net/base/load_flags.h"
+#include "net/http/http_status_code.h"
+#include "net/traffic_annotation/network_traffic_annotation.h"
+#include "services/network/public/cpp/resource_request.h"
+#include "services/network/public/cpp/shared_url_loader_factory.h"
+#include "services/network/public/cpp/simple_url_loader.h"
+#include "services/network/public/mojom/url_response_head.mojom.h"
+
+namespace VibeBrowser_metrics {
+
+namespace {
+
+// Event naming convention:
+// All events from C++ code are prefixed with "VibeBrowser.native." to distinguish
+// them from extension events which would use "VibeBrowser.extension." prefix.
+// This helps with analytics filtering and understanding event sources.
+
+// PostHog API configuration
+constexpr char kPostHogApiKey[] = "phc_PRrpVnBMVJgUumvaXzUnwKZ1dDs3L8MSICLhTdnc8jC";
+constexpr char kPostHogEndpoint[] = "https://us.i.posthog.com/i/v0/e/";
+constexpr size_t kMaxUploadSize = 256 * 1024;  // 256KB max upload size
+
+constexpr net::NetworkTrafficAnnotationTag kVibeBrowserMetricsTrafficAnnotation =
+    net::DefineNetworkTrafficAnnotation("VibeBrowser_metrics", R"(
+        semantics {
+          sender: "VibeBrowser Metrics"
+          description:
+            "Sends anonymous usage metrics to PostHog for VibeBrowser features. "
+            "This helps improve the browser by understanding how features are "
+            "used. No personally identifiable information is collected."
+          trigger:
+            "Triggered when VibeBrowser features are used, such as extension "
+            "actions or settings changes."
+          data:
+            "Event name, timestamp, anonymous client ID, browser version, "
+            "OS information, and feature-specific properties without PII."
+          destination: OTHER
+          destination_other:
+            "PostHog analytics service at us.i.posthog.com"
+        }
+        policy {
+          cookies_allowed: NO
+          setting:
+            "This feature cannot be disabled through settings. Events are "
+            "sent anonymously without user identification."
+          policy_exception_justification:
+            "Not implemented. Analytics are anonymous and help improve "
+            "the browser experience."
+        })");
+
+}  // namespace
+
+VibeBrowserMetricsService::VibeBrowserMetricsService(
+    PrefService* pref_service,
+    PrefService* local_state_prefs,
+    scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory)
+    : pref_service_(pref_service),
+      local_state_prefs_(local_state_prefs),
+      url_loader_factory_(std::move(url_loader_factory)) {
+  CHECK(pref_service_);
+  CHECK(local_state_prefs_);
+  CHECK(url_loader_factory_);
+  InitializeClientId();
+  InitializeInstallId();
+}
+
+VibeBrowserMetricsService::~VibeBrowserMetricsService() = default;
+
+void VibeBrowserMetricsService::CaptureEvent(const std::string& event_name,
+                                            base::Value::Dict properties) {
+  if (event_name.empty()) {
+    LOG(WARNING) << "VibeBrowser: Attempted to capture event with empty name";
+    return;
+  }
+
+  VLOG(1) << "VibeBrowser: Capturing event: " << event_name;
+  
+  // Add default properties
+  AddDefaultProperties(properties);
+  
+  // Send to PostHog
+  SendEventToPostHog(event_name, std::move(properties));
+}
+
+std::string VibeBrowserMetricsService::GetClientId() const {
+  return client_id_;
+}
+
+std::string VibeBrowserMetricsService::GetInstallId() const {
+  return install_id_;
+}
+
+void VibeBrowserMetricsService::Shutdown() {
+  // Cancel any pending network requests
+  weak_factory_.InvalidateWeakPtrs();
+}
+
+void VibeBrowserMetricsService::InitializeClientId() {
+  CHECK(pref_service_);
+
+  // Check if we have an existing client ID
+  const std::string& stored_id =
+      pref_service_->GetString(prefs::kVibeBrowserMetricsClientId);
+
+  if (!stored_id.empty() && base::Uuid::ParseCaseInsensitive(stored_id).is_valid()) {
+    client_id_ = stored_id;
+    VLOG(1) << "VibeBrowser: Using existing metrics client ID";
+  } else {
+    // Generate a new UUID
+    client_id_ = base::Uuid::GenerateRandomV4().AsLowercaseString();
+    pref_service_->SetString(prefs::kVibeBrowserMetricsClientId, client_id_);
+    LOG(INFO) << "VibeBrowser: Generated new metrics client ID";
+  }
+  VLOG(1) << "VibeBrowser: Metrics client ID: " << client_id_;
+}
+
+void VibeBrowserMetricsService::InitializeInstallId() {
+  CHECK(local_state_prefs_);
+
+  // Check if we have an existing install ID
+  const std::string& stored_id =
+      local_state_prefs_->GetString(prefs::kVibeBrowserMetricsInstallId);
+
+  if (!stored_id.empty() && base::Uuid::ParseCaseInsensitive(stored_id).is_valid()) {
+    install_id_ = stored_id;
+    VLOG(1) << "VibeBrowser: Using existing metrics install ID";
+  } else {
+    // Generate a new UUID
+    install_id_ = base::Uuid::GenerateRandomV4().AsLowercaseString();
+    local_state_prefs_->SetString(prefs::kVibeBrowserMetricsInstallId, install_id_);
+    LOG(INFO) << "VibeBrowser: Generated new metrics install ID";
+  }
+  VLOG(1) << "VibeBrowser: Metrics install ID: " << install_id_;
+}
+
+void VibeBrowserMetricsService::SendEventToPostHog(
+    const std::string& event_name,
+    base::Value::Dict properties) {
+  // Build the request payload
+  base::Value::Dict payload;
+  payload.Set("api_key", kPostHogApiKey);
+  payload.Set("event", "VibeBrowser.native." + event_name);
+  payload.Set("distinct_id", client_id_);
+  payload.Set("properties", std::move(properties));
+  
+  // Convert to JSON
+  std::string json_payload;
+  if (!base::JSONWriter::Write(payload, &json_payload)) {
+    LOG(ERROR) << "VibeBrowser: Failed to serialize metrics payload";
+    return;
+  }
+
+  // Create the request
+  auto resource_request = std::make_unique<network::ResourceRequest>();
+  resource_request->url = GURL(kPostHogEndpoint);
+  resource_request->method = "POST";
+  resource_request->load_flags = net::LOAD_DISABLE_CACHE;
+  resource_request->credentials_mode = network::mojom::CredentialsMode::kOmit;
+  
+  // Create the URL loader
+  auto url_loader = network::SimpleURLLoader::Create(
+      std::move(resource_request), kVibeBrowserMetricsTrafficAnnotation);
+  url_loader->SetAllowHttpErrorResults(true);
+  url_loader->AttachStringForUpload(json_payload, "application/json");
+  
+  // Send the request
+  network::SimpleURLLoader* loader_ptr = url_loader.get();
+  loader_ptr->DownloadToString(
+      url_loader_factory_.get(),
+      base::BindOnce(&VibeBrowserMetricsService::OnPostHogResponse,
+                     weak_factory_.GetWeakPtr(), std::move(url_loader)),
+      kMaxUploadSize);
+}
+
+void VibeBrowserMetricsService::OnPostHogResponse(
+    std::unique_ptr<network::SimpleURLLoader> loader,
+    std::unique_ptr<std::string> response_body) {
+  int response_code = 0;
+  if (loader->ResponseInfo() && loader->ResponseInfo()->headers) {
+    response_code = loader->ResponseInfo()->headers->response_code();
+  }
+  
+  if (response_code == net::HTTP_OK) {
+    VLOG(2) << "VibeBrowser: Metrics event sent successfully";
+  } else {
+    LOG(WARNING) << "VibeBrowser: Failed to send metrics event. Response code: "
+                 << response_code;
+    if (response_body && !response_body->empty()) {
+      LOG(WARNING) << "VibeBrowser: Error response: " << *response_body;
+    }
+  }
+}
+
+void VibeBrowserMetricsService::AddDefaultProperties(
+    base::Value::Dict& properties) {
+  // Add browser version
+  properties.Set("$browser_version", version_info::GetVersionNumber());
+
+  // Add OS information
+  properties.Set("$os", base::SysInfo::OperatingSystemName());
+  properties.Set("$os_version", base::SysInfo::OperatingSystemVersion());
+
+  // Ensure anonymous tracking
+  properties.Set("$process_person_profile", false);
+
+  // Add platform architecture
+  properties.Set("$arch", base::SysInfo::OperatingSystemArchitecture());
+
+  // Add install ID for correlating events across profiles
+  properties.Set("install_id", install_id_);
+}
+
+}  // namespace VibeBrowser_metrics
