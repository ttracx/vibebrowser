diff --git a/chrome/browser/extensions/api/browser_os/browser_os_api.h b/chrome/browser/extensions/api/browser_os/browser_os_api.h
new file mode 100644
index 0000000000000..e4b1c5f821342
--- /dev/null
+++ b/chrome/browser/extensions/api/browser_os/browser_os_api.h
@@ -0,0 +1,331 @@
+// Copyright 2024 The Chromium Authors
+// Use of this source code is governed by a BSD-style license that can be
+// found in the LICENSE file.
+
+#ifndef CHROME_BROWSER_EXTENSIONS_API_BROWSER_OS_BROWSER_OS_API_H_
+#define CHROME_BROWSER_EXTENSIONS_API_BROWSER_OS_BROWSER_OS_API_H_
+
+#include <cstdint>
+
+#include "base/memory/raw_ptr.h"
+#include "base/values.h"
+#include "chrome/browser/extensions/api/browser_os/browser_os_api_utils.h"
+#include "chrome/browser/extensions/api/browser_os/browser_os_content_processor.h"
+#include "chrome/browser/extensions/api/browser_os/browser_os_snapshot_processor.h"
+#include "extensions/browser/extension_function.h"
+#include "third_party/skia/include/core/SkBitmap.h"
+
+namespace content {
+class WebContents;
+}
+
+namespace ui {
+struct AXTreeUpdate;
+}
+
+namespace extensions {
+namespace api {
+
+
+class VibeBrowserGetAccessibilityTreeFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.getAccessibilityTree",
+                             BROWSER_OS_GETACCESSIBILITYTREE)
+
+  VibeBrowserGetAccessibilityTreeFunction() = default;
+
+ protected:
+  ~VibeBrowserGetAccessibilityTreeFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+
+ private:
+  void OnAccessibilityTreeReceived(ui::AXTreeUpdate& tree_update);
+};
+
+class VibeBrowserGetInteractiveSnapshotFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.getInteractiveSnapshot",
+                             BROWSER_OS_GETINTERACTIVESNAPSHOT)
+
+  VibeBrowserGetInteractiveSnapshotFunction();
+
+ protected:
+  ~VibeBrowserGetInteractiveSnapshotFunction() override;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+
+ private:
+  void OnAccessibilityTreeReceived(ui::AXTreeUpdate& tree_update);
+  void OnSnapshotProcessed(SnapshotProcessingResult result);
+  
+  // Counter for snapshot IDs
+  static uint32_t next_snapshot_id_;
+  
+  // Tab ID for storing mappings
+  int tab_id_ = -1;
+  
+  // Web contents for processing and drawing
+  raw_ptr<content::WebContents> web_contents_ = nullptr;
+};
+
+class VibeBrowserClickFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.click", BROWSER_OS_CLICK)
+
+  VibeBrowserClickFunction() = default;
+
+ protected:
+  ~VibeBrowserClickFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class VibeBrowserInputTextFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.inputText", BROWSER_OS_INPUTTEXT)
+
+  VibeBrowserInputTextFunction() = default;
+
+ protected:
+  ~VibeBrowserInputTextFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class VibeBrowserClearFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.clear", BROWSER_OS_CLEAR)
+
+  VibeBrowserClearFunction() = default;
+
+ protected:
+  ~VibeBrowserClearFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class VibeBrowserGetPageLoadStatusFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.getPageLoadStatus", 
+                             BROWSER_OS_GETPAGELOADSTATUS)
+
+  VibeBrowserGetPageLoadStatusFunction() = default;
+
+ protected:
+  ~VibeBrowserGetPageLoadStatusFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class VibeBrowserScrollUpFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.scrollUp", BROWSER_OS_SCROLLUP)
+
+  VibeBrowserScrollUpFunction() = default;
+
+ protected:
+  ~VibeBrowserScrollUpFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class VibeBrowserScrollDownFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.scrollDown", BROWSER_OS_SCROLLDOWN)
+
+  VibeBrowserScrollDownFunction() = default;
+
+ protected:
+  ~VibeBrowserScrollDownFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class VibeBrowserScrollToNodeFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.scrollToNode", BROWSER_OS_SCROLLTONODE)
+
+  VibeBrowserScrollToNodeFunction() = default;
+
+ protected:
+  ~VibeBrowserScrollToNodeFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class VibeBrowserSendKeysFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.sendKeys", BROWSER_OS_SENDKEYS)
+
+  VibeBrowserSendKeysFunction() = default;
+
+ protected:
+  ~VibeBrowserSendKeysFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class VibeBrowserCaptureScreenshotFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.captureScreenshot", BROWSER_OS_CAPTURESCREENSHOT)
+
+  VibeBrowserCaptureScreenshotFunction();
+
+ protected:
+  ~VibeBrowserCaptureScreenshotFunction() override;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+  
+ private:
+  void DrawHighlightsAndCapture();
+  void CaptureScreenshotNow();
+  void OnScreenshotCaptured(const SkBitmap& bitmap);
+  
+  // Store web contents and tab id for highlight operations
+  raw_ptr<content::WebContents> web_contents_ = nullptr;
+  int tab_id_ = -1;
+  gfx::Size target_size_;
+  bool show_highlights_ = false;
+  bool use_exact_dimensions_ = false;
+};
+
+class VibeBrowserGetSnapshotFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.getSnapshot", BROWSER_OS_GETSNAPSHOT)
+
+  VibeBrowserGetSnapshotFunction() = default;
+
+ protected:
+  ~VibeBrowserGetSnapshotFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+
+ private:
+  void OnAccessibilityTreeReceived(ui::AXTreeUpdate& tree_update);
+};
+
+// Settings API functions
+class VibeBrowserGetPrefFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.getPref", BROWSER_OS_GETPREF)
+
+  VibeBrowserGetPrefFunction() = default;
+
+ protected:
+  ~VibeBrowserGetPrefFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class VibeBrowserSetPrefFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.setPref", BROWSER_OS_SETPREF)
+
+  VibeBrowserSetPrefFunction() = default;
+
+ protected:
+  ~VibeBrowserSetPrefFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class VibeBrowserGetAllPrefsFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.getAllPrefs", BROWSER_OS_GETALLPREFS)
+
+  VibeBrowserGetAllPrefsFunction() = default;
+
+ protected:
+  ~VibeBrowserGetAllPrefsFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class VibeBrowserLogMetricFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.logMetric", BROWSER_OS_LOGMETRIC)
+
+  VibeBrowserLogMetricFunction() = default;
+
+ protected:
+  ~VibeBrowserLogMetricFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class VibeBrowserGetVersionNumberFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.getVersionNumber", BROWSER_OS_GETVERSIONNUMBER)
+
+  VibeBrowserGetVersionNumberFunction() = default;
+
+ protected:
+  ~VibeBrowserGetVersionNumberFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class VibeBrowserExecuteJavaScriptFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.executeJavaScript", BROWSER_OS_EXECUTEJAVASCRIPT)
+
+  VibeBrowserExecuteJavaScriptFunction() = default;
+
+ protected:
+  ~VibeBrowserExecuteJavaScriptFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+  
+ private:
+  void OnJavaScriptExecuted(base::Value result);
+};
+
+class VibeBrowserClickCoordinatesFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.clickCoordinates", BROWSER_OS_CLICKCOORDINATES)
+
+  VibeBrowserClickCoordinatesFunction() = default;
+
+ protected:
+  ~VibeBrowserClickCoordinatesFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+class VibeBrowserTypeAtCoordinatesFunction : public ExtensionFunction {
+ public:
+  DECLARE_EXTENSION_FUNCTION("VibeBrowser.typeAtCoordinates", BROWSER_OS_TYPEATCOORDINATES)
+
+  VibeBrowserTypeAtCoordinatesFunction() = default;
+
+ protected:
+  ~VibeBrowserTypeAtCoordinatesFunction() override = default;
+
+  // ExtensionFunction:
+  ResponseAction Run() override;
+};
+
+}  // namespace api
+}  // namespace extensions
+
+#endif  // CHROME_BROWSER_EXTENSIONS_API_BROWSER_OS_BROWSER_OS_API_H_
\ No newline at end of file
