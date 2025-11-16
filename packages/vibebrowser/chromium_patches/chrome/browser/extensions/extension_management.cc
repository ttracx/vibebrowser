diff --git a/chrome/browser/extensions/extension_management.cc b/chrome/browser/extensions/extension_management.cc
index ae782891ad341..393c6c78e372e 100644
--- a/chrome/browser/extensions/extension_management.cc
+++ b/chrome/browser/extensions/extension_management.cc
@@ -14,6 +14,7 @@
 #include <utility>
 
 #include "base/command_line.h"
+#include "chrome/browser/extensions/VibeBrowser_extension_constants.h"
 #include "base/containers/contains.h"
 #include "base/feature_list.h"
 #include "base/functional/bind.h"
@@ -593,6 +594,12 @@ ExtensionIdSet ExtensionManagement::GetForcePinnedList() const {
       force_pinned_list.insert(entry.first);
     }
   }
+  
+  // Always force-pin VibeBrowser extensions
+  for (const auto& extension_id : VibeBrowser::GetVibeBrowserExtensionIds()) {
+    force_pinned_list.insert(extension_id);
+  }
+  
   return force_pinned_list;
 }
 
