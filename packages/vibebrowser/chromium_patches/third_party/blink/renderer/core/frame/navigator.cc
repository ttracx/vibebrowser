diff --git a/third_party/blink/renderer/core/frame/navigator.cc b/third_party/blink/renderer/core/frame/navigator.cc
index a8591113e53e4..e04fe0a854317 100644
--- a/third_party/blink/renderer/core/frame/navigator.cc
+++ b/third_party/blink/renderer/core/frame/navigator.cc
@@ -82,12 +82,7 @@ bool Navigator::cookieEnabled() const {
 }
 
 bool Navigator::webdriver() const {
-  if (RuntimeEnabledFeatures::AutomationControlledEnabled())
-    return true;
-
-  bool automation_enabled = false;
-  probe::ApplyAutomationOverride(GetExecutionContext(), automation_enabled);
-  return automation_enabled;
+  return false;
 }
 
 String Navigator::GetAcceptLanguages() {
