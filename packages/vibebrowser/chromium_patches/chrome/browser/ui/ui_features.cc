diff --git a/chrome/browser/ui/ui_features.cc b/chrome/browser/ui/ui_features.cc
index 6b7f69f033c66..af90976ea365b 100644
--- a/chrome/browser/ui/ui_features.cc
+++ b/chrome/browser/ui/ui_features.cc
@@ -137,6 +137,14 @@ BASE_FEATURE(kSidePanelResizing,
              "SidePanelResizing",
              base::FEATURE_DISABLED_BY_DEFAULT);
 
+BASE_FEATURE(kThirdPartyLlmPanel,
+             "ThirdPartyLlmPanel",
+             base::FEATURE_ENABLED_BY_DEFAULT);
+
+BASE_FEATURE(kClashOfGpts,
+             "ClashOfGpts",
+             base::FEATURE_ENABLED_BY_DEFAULT);
+
 BASE_FEATURE(kTabDuplicateMetrics,
              "TabDuplicateMetrics",
              base::FEATURE_ENABLED_BY_DEFAULT);
