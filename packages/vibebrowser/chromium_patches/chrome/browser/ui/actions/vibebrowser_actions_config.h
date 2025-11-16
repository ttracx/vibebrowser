diff --git a/chrome/browser/ui/actions/VibeBrowser_actions_config.h b/chrome/browser/ui/actions/VibeBrowser_actions_config.h
new file mode 100644
index 0000000000000..3900759f40883
--- /dev/null
+++ b/chrome/browser/ui/actions/VibeBrowser_actions_config.h
@@ -0,0 +1,69 @@
+// Copyright 2025 The Chromium Authors
+// Use of this source code is governed by a BSD-style license that can be
+// found in the LICENSE file.
+
+#ifndef CHROME_BROWSER_UI_ACTIONS_VibeBrowser_ACTIONS_CONFIG_H_
+#define CHROME_BROWSER_UI_ACTIONS_VibeBrowser_ACTIONS_CONFIG_H_
+
+#include <string>
+#include <string_view>
+
+#include "base/containers/contains.h"
+#include "base/containers/fixed_flat_set.h"
+#include "chrome/browser/ui/actions/chrome_action_id.h"
+#include "chrome/browser/ui/ui_features.h"
+#include "chrome/browser/ui/views/side_panel/side_panel_entry_key.h"
+#include "chrome/common/chrome_features.h"
+#include "chrome/browser/extensions/VibeBrowser_extension_constants.h"
+#include "ui/actions/actions.h"
+
+namespace VibeBrowser {
+
+// Native action IDs for VibeBrowser panels that need special treatment
+// These actions will:
+// - Always be pinned
+// - Show text labels
+// - Have high flex priority (always visible)
+constexpr auto kVibeBrowserNativeActionIds =
+    base::MakeFixedFlatSet<actions::ActionId>({
+        kActionSidePanelShowThirdPartyLlm,
+        kActionSidePanelShowClashOfGpts,
+    });
+
+// Check if an action ID is a VibeBrowser action (native or extension)
+inline bool IsVibeBrowserAction(actions::ActionId id) {
+  // Check native actions
+  if (kVibeBrowserNativeActionIds.contains(id)) {
+    return true;
+  }
+
+  // Check extension actions using the allowed extensions from VibeBrowser constants
+  for (const char* ext_id : extensions::VibeBrowser::kAllowedExtensions) {
+    auto ext_action_id = actions::ActionIdMap::StringToActionId(
+        SidePanelEntryKey(SidePanelEntryId::kExtension, std::string(ext_id))
+            .ToString());
+    if (ext_action_id && id == *ext_action_id) {
+      return true;
+    }
+  }
+
+  return false;
+}
+
+
+// Get the feature flag for a native VibeBrowser action
+inline const base::Feature* GetFeatureForVibeBrowserAction(actions::ActionId id) {
+  switch (id) {
+    case kActionSidePanelShowThirdPartyLlm:
+      return &features::kThirdPartyLlmPanel;
+    case kActionSidePanelShowClashOfGpts:
+      return &features::kClashOfGpts;
+    default:
+      return nullptr;
+  }
+}
+
+}  // namespace VibeBrowser
+
+#endif  // CHROME_BROWSER_UI_ACTIONS_VibeBrowser_ACTIONS_CONFIG_H_
+
