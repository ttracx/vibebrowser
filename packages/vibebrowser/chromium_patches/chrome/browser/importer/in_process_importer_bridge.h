diff --git a/chrome/browser/importer/in_process_importer_bridge.h b/chrome/browser/importer/in_process_importer_bridge.h
index 49700b2f8a384..1a04f789e0fcc 100644
--- a/chrome/browser/importer/in_process_importer_bridge.h
+++ b/chrome/browser/importer/in_process_importer_bridge.h
@@ -50,6 +50,8 @@ class InProcessImporterBridge : public ImporterBridge {
   void SetAutofillFormData(
       const std::vector<ImporterAutofillFormDataEntry>& entries) override;
 
+  void SetExtensions(const std::vector<std::string>& extension_ids) override;
+
   void NotifyStarted() override;
   void NotifyItemStarted(importer::ImportItem item) override;
   void NotifyItemEnded(importer::ImportItem item) override;
