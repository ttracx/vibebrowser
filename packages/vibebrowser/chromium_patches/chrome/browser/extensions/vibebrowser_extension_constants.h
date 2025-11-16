diff --git a/chrome/browser/extensions/VibeBrowser_extension_constants.h b/chrome/browser/extensions/VibeBrowser_extension_constants.h
new file mode 100644
index 0000000000000..6bb906bc7068f
--- /dev/null
+++ b/chrome/browser/extensions/VibeBrowser_extension_constants.h
@@ -0,0 +1,74 @@
+// Copyright 2024 The Chromium Authors
+// Use of this source code is governed by a BSD-style license that can be
+// found in the LICENSE file.
+
+#ifndef CHROME_BROWSER_EXTENSIONS_VibeBrowser_EXTENSION_CONSTANTS_H_
+#define CHROME_BROWSER_EXTENSIONS_VibeBrowser_EXTENSION_CONSTANTS_H_
+
+#include <optional>
+#include <string>
+#include <vector>
+
+namespace extensions {
+namespace VibeBrowser {
+
+// AI Agent Extension ID
+inline constexpr char kAISidePanelExtensionId[] =
+    "djhdjhlnljbjgejbndockeedocneiaei";
+
+// Bug Reporter Extension ID
+inline constexpr char kBugReporterExtensionId[] =
+    "adlpneommgkgeanpaekgoaolcpncohkf";
+
+// Controller Extension ID
+inline constexpr char kControllerExtensionId[] =
+    "nlnihljpboknmfagkikhkdblbedophja";
+
+// Allowlist of VibeBrowser extension IDs that are permitted to be installed
+// Only extensions with these IDs will be loaded from the config
+constexpr const char* kAllowedExtensions[] = {
+    kAISidePanelExtensionId,  // AI Side Panel extension
+    kBugReporterExtensionId,  // Bug Reporter extension
+    kControllerExtensionId,   // Controller extension
+};
+
+// Check if an extension is a VibeBrowser extension
+inline bool IsVibeBrowserExtension(const std::string& extension_id) {
+  return extension_id == kAISidePanelExtensionId ||
+         extension_id == kBugReporterExtensionId ||
+         extension_id == kControllerExtensionId;
+}
+
+// Check if an extension can be uninstalled (false for VibeBrowser extensions)
+inline bool CanUninstallExtension(const std::string& extension_id) {
+  return !IsVibeBrowserExtension(extension_id);
+}
+
+// Get all VibeBrowser extension IDs
+inline std::vector<std::string> GetVibeBrowserExtensionIds() {
+  return {
+    kAISidePanelExtensionId,
+    kBugReporterExtensionId,
+    kControllerExtensionId
+  };
+}
+
+// Get display name for VibeBrowser extensions in omnibox
+// Returns the display name if extension_id is a VibeBrowser extension,
+// otherwise returns std::nullopt
+inline std::optional<std::string> GetExtensionDisplayName(
+    const std::string& extension_id) {
+  if (extension_id == kAISidePanelExtensionId) {
+    return "VibeBrowser";
+  } else if (extension_id == kBugReporterExtensionId) {
+    return "VibeBrowser/bug-reporter";
+  } else if (extension_id == kControllerExtensionId) {
+    return "VibeBrowser/controller";
+  }
+  return std::nullopt;
+}
+
+}  // namespace VibeBrowser
+}  // namespace extensions
+
+#endif  // CHROME_BROWSER_EXTENSIONS_VibeBrowser_EXTENSION_CONSTANTS_H_
