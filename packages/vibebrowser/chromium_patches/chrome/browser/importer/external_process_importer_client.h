diff --git a/chrome/browser/importer/external_process_importer_client.h b/chrome/browser/importer/external_process_importer_client.h
index 6d38b92400bbf..c6f6cfac5049c 100644
--- a/chrome/browser/importer/external_process_importer_client.h
+++ b/chrome/browser/importer/external_process_importer_client.h
@@ -83,6 +83,8 @@ class ExternalProcessImporterClient
   void OnAutofillFormDataImportGroup(
       const std::vector<ImporterAutofillFormDataEntry>&
           autofill_form_data_entry_group) override;
+  void OnExtensionsImportReady(
+      const std::vector<std::string>& extension_ids) override;
 
  protected:
   ~ExternalProcessImporterClient() override;
