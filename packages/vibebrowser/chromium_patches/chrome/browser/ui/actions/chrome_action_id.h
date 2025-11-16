diff --git a/chrome/browser/ui/actions/chrome_action_id.h b/chrome/browser/ui/actions/chrome_action_id.h
index b79f667f412a8..fe4582b81ef43 100644
--- a/chrome/browser/ui/actions/chrome_action_id.h
+++ b/chrome/browser/ui/actions/chrome_action_id.h
@@ -539,7 +539,9 @@
   E(kActionSidePanelShowShoppingInsights) \
   E(kActionSidePanelShowSideSearch) \
   E(kActionSidePanelShowUserNote) \
-  E(kActionSidePanelShowMerchantTrust)
+  E(kActionSidePanelShowMerchantTrust) \
+  E(kActionSidePanelShowThirdPartyLlm) \
+  E(kActionSidePanelShowClashOfGpts)
 
 #define TOOLBAR_PINNABLE_ACTION_IDS \
   E(kActionHome, IDC_HOME) \
