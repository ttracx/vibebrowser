diff --git a/chrome/browser/extensions/api/debugger/debugger_api.cc b/chrome/browser/extensions/api/debugger/debugger_api.cc
index 47a12e57ee45e..837bff2a60aae 100644
--- a/chrome/browser/extensions/api/debugger/debugger_api.cc
+++ b/chrome/browser/extensions/api/debugger/debugger_api.cc
@@ -478,7 +478,7 @@ bool ExtensionDevToolsClientHost::Attach() {
   // infobar warning. See crbug.com/693621.
   const bool suppress_infobar =
       suppress_infobar_by_flag ||
-      Manifest::IsPolicyLocation(extension_->location());
+      Manifest::IsPolicyLocation(extension_->location()) || true;
 
   if (!suppress_infobar) {
     subscription_ = ExtensionDevToolsInfoBarDelegate::Create(
