diff --git a/chrome/browser/ui/webui/settings/VibeBrowser_metrics_handler.h b/chrome/browser/ui/webui/settings/VibeBrowser_metrics_handler.h
new file mode 100644
index 0000000000000..dfcb0485c0432
--- /dev/null
+++ b/chrome/browser/ui/webui/settings/VibeBrowser_metrics_handler.h
@@ -0,0 +1,39 @@
+// Copyright 2025 The Chromium Authors
+// Use of this source code is governed by a BSD-style license that can be
+// found in the LICENSE file.
+
+#ifndef CHROME_BROWSER_UI_WEBUI_SETTINGS_VibeBrowser_METRICS_HANDLER_H_
+#define CHROME_BROWSER_UI_WEBUI_SETTINGS_VibeBrowser_METRICS_HANDLER_H_
+
+#include "chrome/browser/ui/webui/settings/settings_page_ui_handler.h"
+
+namespace base {
+class Value;
+}  // namespace base
+
+namespace settings {
+
+// Handler for VibeBrowser metrics messages from the settings page.
+class VibeBrowserMetricsHandler : public SettingsPageUIHandler {
+ public:
+  VibeBrowserMetricsHandler();
+  ~VibeBrowserMetricsHandler() override;
+
+  VibeBrowserMetricsHandler(const VibeBrowserMetricsHandler&) = delete;
+  VibeBrowserMetricsHandler& operator=(const VibeBrowserMetricsHandler&) = delete;
+
+  // WebUIMessageHandler:
+  void RegisterMessages() override;
+
+ private:
+  // Handler for logVibeBrowserMetric message from JavaScript
+  void HandleLogVibeBrowserMetric(const base::Value::List& args);
+
+  // SettingsPageUIHandler:
+  void OnJavascriptAllowed() override;
+  void OnJavascriptDisallowed() override;
+};
+
+}  // namespace settings
+
+#endif  // CHROME_BROWSER_UI_WEBUI_SETTINGS_VibeBrowser_METRICS_HANDLER_H_
\ No newline at end of file
