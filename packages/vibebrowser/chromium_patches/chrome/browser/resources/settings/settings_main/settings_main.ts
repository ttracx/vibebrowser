diff --git a/chrome/browser/resources/settings/settings_main/settings_main.ts b/chrome/browser/resources/settings/settings_main/settings_main.ts
index 43fd55ea0b83c..2da0ef8a114fa 100644
--- a/chrome/browser/resources/settings/settings_main/settings_main.ts
+++ b/chrome/browser/resources/settings/settings_main/settings_main.ts
@@ -14,6 +14,8 @@ import 'chrome://resources/cr_elements/icons.html.js';
 import 'chrome://resources/js/search_highlight_utils.js';
 import 'chrome://resources/cr_elements/cr_icon/cr_icon.js';
 import '../about_page/about_page.js';
+import '../vibebrowser_page/vibebrowser_page.js';
+import '../VibeBrowser_prefs_page/VibeBrowser_prefs_page.js';
 import '../basic_page/basic_page.js';
 import '../search_settings.js';
 import '../settings_shared.css.js';
@@ -32,6 +34,8 @@ import {getTemplate} from './settings_main.html.js';
 interface MainPageVisibility {
   about: boolean;
   settings: boolean;
+  vibebrowser: boolean;
+  VibeBrowserPrefs: boolean;
 }
 
 export interface SettingsMainElement {
@@ -68,7 +72,7 @@ export class SettingsMainElement extends SettingsMainElementBase {
       showPages_: {
         type: Object,
         value() {
-          return {about: false, settings: false};
+          return {about: false, settings: false, vibebrowser: false, VibeBrowserPrefs: false};
         },
       },
 
@@ -114,9 +118,16 @@ export class SettingsMainElement extends SettingsMainElementBase {
    * current route.
    */
   override currentRouteChanged() {
-    const inAbout =
-        routes.ABOUT.contains(Router.getInstance().getCurrentRoute());
-    this.showPages_ = {about: inAbout, settings: !inAbout};
+    const currentRoute = Router.getInstance().getCurrentRoute();
+    const inAbout = routes.ABOUT.contains(currentRoute);
+    const inVibeBrowser = routes.VIBEBROWSER.contains(currentRoute);
+    const inVibeBrowserPrefs = routes.VibeBrowser_PREFS.contains(currentRoute);
+    this.showPages_ = {
+      about: inAbout,
+      settings: !inAbout && !inVibeBrowser && !inVibeBrowserPrefs,
+      vibebrowser: inVibeBrowser,
+      VibeBrowserPrefs: inVibeBrowserPrefs
+    };
   }
 
   private onShowingSubpage_() {
