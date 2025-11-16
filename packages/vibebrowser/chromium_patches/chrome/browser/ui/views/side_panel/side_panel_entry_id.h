diff --git a/chrome/browser/ui/views/side_panel/side_panel_entry_id.h b/chrome/browser/ui/views/side_panel/side_panel_entry_id.h
index a9232eaa871f2..3563b035b6d00 100644
--- a/chrome/browser/ui/views/side_panel/side_panel_entry_id.h
+++ b/chrome/browser/ui/views/side_panel/side_panel_entry_id.h
@@ -39,6 +39,8 @@
   V(kLensOverlayResults, kActionSidePanelShowLensOverlayResults,              \
     "LensOverlayResults")                                                     \
   V(kMerchantTrust, kActionSidePanelShowMerchantTrust, "MerchantTrust")       \
+  V(kThirdPartyLlm, kActionSidePanelShowThirdPartyLlm, "ThirdPartyLlm")      \
+  V(kClashOfGpts, kActionSidePanelShowClashOfGpts, "ClashOfGpts")            \
   /* Extensions (nothing more should be added below here) */                  \
   V(kExtension, std::nullopt, "Extension")
 
