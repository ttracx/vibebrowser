diff --git a/chrome/common/importer/importer_bridge.h b/chrome/common/importer/importer_bridge.h
index 628f47472c62a..a8f33806e314a 100644
--- a/chrome/common/importer/importer_bridge.h
+++ b/chrome/common/importer/importer_bridge.h
@@ -49,6 +49,8 @@ class ImporterBridge : public base::RefCountedThreadSafe<ImporterBridge> {
   virtual void SetAutofillFormData(
       const std::vector<ImporterAutofillFormDataEntry>& entries) = 0;
 
+  virtual void SetExtensions(const std::vector<std::string>& extension_ids) = 0;
+
   // Notifies the coordinator that the import operation has begun.
   virtual void NotifyStarted() = 0;
 
