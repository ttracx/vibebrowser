diff --git a/chrome/browser/ui/toolbar/pinned_toolbar/pinned_toolbar_actions_model.h b/chrome/browser/ui/toolbar/pinned_toolbar/pinned_toolbar_actions_model.h
index b1eb4975bb7e5..1636739c788ea 100644
--- a/chrome/browser/ui/toolbar/pinned_toolbar/pinned_toolbar_actions_model.h
+++ b/chrome/browser/ui/toolbar/pinned_toolbar/pinned_toolbar_actions_model.h
@@ -95,6 +95,11 @@ class PinnedToolbarActionsModel : public KeyedService {
   // Search migrations are complete.
   void MaybeMigrateExistingPinnedStates();
 
+  // Ensures that certain actions are always pinned to the toolbar.
+  // This is called during initialization to ensure specific actions
+  // (like Third Party LLM and Clash of GPTs) are always visible.
+  void EnsureAlwaysPinnedActions();
+
   // Returns the ordered list of pinned ActionIds.
   virtual const std::vector<actions::ActionId>& PinnedActionIds() const;
 
