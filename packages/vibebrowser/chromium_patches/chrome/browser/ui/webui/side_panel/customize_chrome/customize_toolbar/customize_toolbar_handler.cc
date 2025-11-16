diff --git a/chrome/browser/ui/webui/side_panel/customize_chrome/customize_toolbar/customize_toolbar_handler.cc b/chrome/browser/ui/webui/side_panel/customize_chrome/customize_toolbar/customize_toolbar_handler.cc
index 52dddc0ddc518..5be29b2ed54dd 100644
--- a/chrome/browser/ui/webui/side_panel/customize_chrome/customize_toolbar/customize_toolbar_handler.cc
+++ b/chrome/browser/ui/webui/side_panel/customize_chrome/customize_toolbar/customize_toolbar_handler.cc
@@ -85,6 +85,10 @@ MojoActionForChromeAction(actions::ActionId action_id) {
       return side_panel::customize_chrome::mojom::ActionId::kTabSearch;
     case kActionSplitTab:
       return side_panel::customize_chrome::mojom::ActionId::kSplitTab;
+    case kActionSidePanelShowThirdPartyLlm:
+      return side_panel::customize_chrome::mojom::ActionId::kShowThirdPartyLlm;
+    case kActionSidePanelShowClashOfGpts:
+      return side_panel::customize_chrome::mojom::ActionId::kShowClashOfGpts;
     default:
       return std::nullopt;
   }
@@ -143,6 +147,10 @@ std::optional<actions::ActionId> ChromeActionForMojoAction(
       return kActionTabSearch;
     case side_panel::customize_chrome::mojom::ActionId::kSplitTab:
       return kActionSplitTab;
+    case side_panel::customize_chrome::mojom::ActionId::kShowThirdPartyLlm:
+      return kActionSidePanelShowThirdPartyLlm;
+    case side_panel::customize_chrome::mojom::ActionId::kShowClashOfGpts:
+      return kActionSidePanelShowClashOfGpts;
     default:
       return std::nullopt;
   }
@@ -290,6 +298,10 @@ void CustomizeToolbarHandler::ListActions(ListActionsCallback callback) {
              side_panel::customize_chrome::mojom::CategoryId::kYourChrome);
   add_action(kActionSidePanelShowReadingList,
              side_panel::customize_chrome::mojom::CategoryId::kYourChrome);
+  add_action(kActionSidePanelShowThirdPartyLlm,
+             side_panel::customize_chrome::mojom::CategoryId::kYourChrome);
+  add_action(kActionSidePanelShowClashOfGpts,
+             side_panel::customize_chrome::mojom::CategoryId::kYourChrome);
   add_action(kActionSidePanelShowHistoryCluster,
              side_panel::customize_chrome::mojom::CategoryId::kYourChrome);
   add_action(kActionShowDownloads,
