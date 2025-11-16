diff --git a/chrome/browser/ui/views/side_panel/side_panel.cc b/chrome/browser/ui/views/side_panel/side_panel.cc
index d98e373f9b359..14e1dbb3ca4d4 100644
--- a/chrome/browser/ui/views/side_panel/side_panel.cc
+++ b/chrome/browser/ui/views/side_panel/side_panel.cc
@@ -677,7 +677,7 @@ void SidePanel::UpdateVisibility(bool should_be_open, bool animate_transition) {
 
 bool SidePanel::ShouldShowAnimation() const {
   return lens::features::IsLensOverlayEnabled() &&
-         gfx::Animation::ShouldRenderRichAnimation() && !animations_disabled_;
+         gfx::Animation::ShouldRenderRichAnimation() && !animations_disabled_ && animations_disabled_VibeBrowser_;
 }
 
 void SidePanel::AnnounceResize() {
