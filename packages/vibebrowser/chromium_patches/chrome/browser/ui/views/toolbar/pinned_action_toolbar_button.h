diff --git a/chrome/browser/ui/views/toolbar/pinned_action_toolbar_button.h b/chrome/browser/ui/views/toolbar/pinned_action_toolbar_button.h
index e1557abfda184..df7f5c078211a 100644
--- a/chrome/browser/ui/views/toolbar/pinned_action_toolbar_button.h
+++ b/chrome/browser/ui/views/toolbar/pinned_action_toolbar_button.h
@@ -56,6 +56,7 @@ class PinnedActionToolbarButton : public ToolbarButton {
   bool IsPinned() { return pinned_; }
   bool IsPermanent() { return permanent_; }
   views::View* GetImageContainerView() { return image_container_view(); }
+  Browser* GetBrowser() { return browser_; }
 
   bool ShouldSkipExecutionForTesting() { return skip_execution_; }
 
