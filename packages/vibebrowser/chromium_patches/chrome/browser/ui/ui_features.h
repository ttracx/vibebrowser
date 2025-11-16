diff --git a/chrome/browser/ui/ui_features.h b/chrome/browser/ui/ui_features.h
index 6ebeab8624481..e079cd62708d6 100644
--- a/chrome/browser/ui/ui_features.h
+++ b/chrome/browser/ui/ui_features.h
@@ -110,6 +110,8 @@ extern const char kTabScrollingButtonPositionParameterName[];
 
 BASE_DECLARE_FEATURE(kSidePanelResizing);
 BASE_DECLARE_FEATURE(kSidePanelSearchCompanion);
+BASE_DECLARE_FEATURE(kThirdPartyLlmPanel);
+BASE_DECLARE_FEATURE(kClashOfGpts);
 
 BASE_DECLARE_FEATURE(kTabGroupsCollapseFreezing);
 
