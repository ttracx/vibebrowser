diff --git a/chrome/browser/extensions/extension_context_menu_model.cc b/chrome/browser/extensions/extension_context_menu_model.cc
index d5e3f14b43b44..1a3f546260abb 100644
--- a/chrome/browser/extensions/extension_context_menu_model.cc
+++ b/chrome/browser/extensions/extension_context_menu_model.cc
@@ -7,6 +7,7 @@
 #include <memory>
 
 #include "base/containers/contains.h"
+#include "chrome/browser/extensions/VibeBrowser_extension_constants.h"
 #include "base/feature_list.h"
 #include "base/functional/bind.h"
 #include "base/metrics/histogram_macros.h"
@@ -693,7 +694,8 @@ void ExtensionContextMenuModel::InitMenuWithFeature(
 
   // Controls section.
   bool has_options_page = OptionsPageInfo::HasOptionsPage(extension);
-  bool can_uninstall_extension = !is_component_ && !is_required_by_policy;
+  bool can_uninstall_extension = !is_component_ && !is_required_by_policy &&
+                                  VibeBrowser::CanUninstallExtension(extension->id());
   if (can_show_icon_in_toolbar || has_options_page || can_uninstall_extension) {
     AddSeparator(ui::NORMAL_SEPARATOR);
   }
