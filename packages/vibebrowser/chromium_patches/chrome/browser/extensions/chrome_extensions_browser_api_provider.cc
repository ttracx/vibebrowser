diff --git a/chrome/browser/extensions/chrome_extensions_browser_api_provider.cc b/chrome/browser/extensions/chrome_extensions_browser_api_provider.cc
index 9c73fc6067b2f..3666bf5a0d2c8 100644
--- a/chrome/browser/extensions/chrome_extensions_browser_api_provider.cc
+++ b/chrome/browser/extensions/chrome_extensions_browser_api_provider.cc
@@ -4,6 +4,7 @@
 
 #include "chrome/browser/extensions/chrome_extensions_browser_api_provider.h"
 
+#include "chrome/browser/extensions/api/browser_os/browser_os_api.h"
 #include "chrome/browser/extensions/api/generated_api_registration.h"
 #include "extensions/browser/extension_function_registry.h"
 #include "extensions/buildflags/buildflags.h"
@@ -21,6 +22,14 @@ void ChromeExtensionsBrowserAPIProvider::RegisterExtensionFunctions(
   // Commands
   registry->RegisterFunction<GetAllCommandsFunction>();
 
+  // Browser OS API
+  registry->RegisterFunction<api::VibeBrowserGetAccessibilityTreeFunction>();
+  registry->RegisterFunction<api::VibeBrowserGetInteractiveSnapshotFunction>();
+  registry->RegisterFunction<api::VibeBrowserClickFunction>();
+  registry->RegisterFunction<api::VibeBrowserInputTextFunction>();
+  registry->RegisterFunction<api::VibeBrowserClearFunction>();
+  registry->RegisterFunction<api::VibeBrowserExecuteJavaScriptFunction>();
+
   // Generated APIs from Chrome.
   api::ChromeGeneratedFunctionRegistry::RegisterAll(registry);
 }
