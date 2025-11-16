diff --git a/chrome/browser/extensions/api/side_panel/side_panel_api.cc b/chrome/browser/extensions/api/side_panel/side_panel_api.cc
index 5586f29b403f0..3fd9c2ab31c7f 100644
--- a/chrome/browser/extensions/api/side_panel/side_panel_api.cc
+++ b/chrome/browser/extensions/api/side_panel/side_panel_api.cc
@@ -71,11 +71,11 @@ ExtensionFunction::ResponseAction SidePanelOpenFunction::RunFunction() {
   EXTENSION_FUNCTION_VALIDATE(extension());
 
   // `sidePanel.open()` requires a user gesture.
-  if (!user_gesture()) {
-    return RespondNow(
-        Error("`sidePanel.open()` may only be called in "
-              "response to a user gesture."));
-  }
+  // if (!user_gesture()) {
+  //   return RespondNow(
+  //       Error("`sidePanel.open()` may only be called in "
+  //             "response to a user gesture."));
+  // }
 
   std::optional<api::side_panel::Open::Params> params =
       api::side_panel::Open::Params::Create(args());
