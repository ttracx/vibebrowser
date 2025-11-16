diff --git a/chrome/browser/resources/settings/people_page/import_data_browser_proxy.ts b/chrome/browser/resources/settings/people_page/import_data_browser_proxy.ts
index c4e401c551fc5..c8503a94c0c93 100644
--- a/chrome/browser/resources/settings/people_page/import_data_browser_proxy.ts
+++ b/chrome/browser/resources/settings/people_page/import_data_browser_proxy.ts
@@ -19,6 +19,7 @@ export interface BrowserProfile {
   passwords: boolean;
   search: boolean;
   autofillFormData: boolean;
+  extensions: boolean;
 }
 
 /**
