diff --git a/chrome/browser/ui/toolbar/toolbar_actions_model.cc b/chrome/browser/ui/toolbar/toolbar_actions_model.cc
index 8c748d7cc5fc1..2019fe0341327 100644
--- a/chrome/browser/ui/toolbar/toolbar_actions_model.cc
+++ b/chrome/browser/ui/toolbar/toolbar_actions_model.cc
@@ -20,6 +20,7 @@
 #include "base/one_shot_event.h"
 #include "base/strings/utf_string_conversions.h"
 #include "base/task/single_thread_task_runner.h"
+#include "chrome/browser/extensions/VibeBrowser_extension_constants.h"
 #include "chrome/browser/extensions/extension_management.h"
 #include "chrome/browser/extensions/extension_tab_util.h"
 #include "chrome/browser/extensions/profile_util.h"
@@ -289,6 +290,11 @@ bool ToolbarActionsModel::IsActionPinned(const ActionId& action_id) const {
 }
 
 bool ToolbarActionsModel::IsActionForcePinned(const ActionId& action_id) const {
+  // Check if it's a VibeBrowser extension
+  if (extensions::VibeBrowser::IsVibeBrowserExtension(action_id)) {
+    return true;
+  }
+  
   auto* management =
       extensions::ExtensionManagementFactory::GetForBrowserContext(profile_);
   return base::Contains(management->GetForcePinnedList(), action_id);
@@ -533,6 +539,13 @@ ToolbarActionsModel::GetFilteredPinnedActionIds() const {
   std::ranges::copy_if(
       management->GetForcePinnedList(), std::back_inserter(pinned),
       [&pinned](const std::string& id) { return !base::Contains(pinned, id); });
+      
+  // Add VibeBrowser extensions to the force-pinned list
+  for (const char* ext_id : extensions::VibeBrowser::kAllowedExtensions) {
+    if (!base::Contains(pinned, ext_id)) {
+      pinned.push_back(ext_id);
+    }
+  }
 
   // TODO(pbos): Make sure that the pinned IDs are pruned from ExtensionPrefs on
   // startup so that we don't keep saving stale IDs.
