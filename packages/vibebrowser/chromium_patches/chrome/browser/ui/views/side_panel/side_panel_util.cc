diff --git a/chrome/browser/ui/views/side_panel/side_panel_util.cc b/chrome/browser/ui/views/side_panel/side_panel_util.cc
index f93a373cd9e96..f48aaeb53210b 100644
--- a/chrome/browser/ui/views/side_panel/side_panel_util.cc
+++ b/chrome/browser/ui/views/side_panel/side_panel_util.cc
@@ -20,6 +20,7 @@
 #include "chrome/browser/ui/views/side_panel/history_clusters/history_clusters_side_panel_coordinator.h"
 #include "chrome/browser/ui/views/side_panel/reading_list/reading_list_side_panel_coordinator.h"
 #include "chrome/browser/ui/views/side_panel/side_panel_content_proxy.h"
+#include "chrome/browser/ui/views/side_panel/third_party_llm/third_party_llm_panel_coordinator.h"
 #include "chrome/browser/ui/views/side_panel/side_panel_coordinator.h"
 #include "chrome/browser/ui/views/side_panel/side_panel_registry.h"
 #include "chrome/browser/ui/views/side_panel/side_panel_ui.h"
@@ -54,6 +55,14 @@ void SidePanelUtil::PopulateGlobalEntries(Browser* browser,
         ->history_side_panel_coordinator()
         ->CreateAndRegisterEntry(window_registry);
   }
+
+  // Add third-party LLM panel.
+  if (base::FeatureList::IsEnabled(features::kThirdPartyLlmPanel)) {
+    ThirdPartyLlmPanelCoordinator::GetOrCreateForBrowser(browser)
+        ->CreateAndRegisterEntry(window_registry);
+  }
+
+  // Clash of GPTs doesn't need side panel registration as it opens in its own window
 }
 
 SidePanelContentProxy* SidePanelUtil::GetSidePanelContentProxy(
