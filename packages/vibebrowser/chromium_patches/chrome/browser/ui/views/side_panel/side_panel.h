diff --git a/chrome/browser/ui/views/side_panel/side_panel.h b/chrome/browser/ui/views/side_panel/side_panel.h
index 617d8674a4ead..e711565394814 100644
--- a/chrome/browser/ui/views/side_panel/side_panel.h
+++ b/chrome/browser/ui/views/side_panel/side_panel.h
@@ -128,6 +128,8 @@ class SidePanel : public views::AccessiblePaneView,
 
   bool animations_disabled_ = false;
 
+  bool animations_disabled_VibeBrowser_ = true;
+
   // Animation controlling showing and hiding of the side panel.
   gfx::SlideAnimation animation_{this};
 
