diff --git a/chrome/utility/importer/external_process_importer_bridge.h b/chrome/utility/importer/external_process_importer_bridge.h
index 57362d09f0d38..9340b44b5ac2e 100644
--- a/chrome/utility/importer/external_process_importer_bridge.h
+++ b/chrome/utility/importer/external_process_importer_bridge.h
@@ -62,6 +62,8 @@ class ExternalProcessImporterBridge : public ImporterBridge {
   void SetAutofillFormData(
       const std::vector<ImporterAutofillFormDataEntry>& entries) override;
 
+  void SetExtensions(const std::vector<std::string>& extension_ids) override;
+
   void NotifyStarted() override;
   void NotifyItemStarted(importer::ImportItem item) override;
   void NotifyItemEnded(importer::ImportItem item) override;
