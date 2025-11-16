diff --git a/chrome/utility/importer/external_process_importer_bridge.cc b/chrome/utility/importer/external_process_importer_bridge.cc
index 0f98b3d1da6e7..9b169aab54fd9 100644
--- a/chrome/utility/importer/external_process_importer_bridge.cc
+++ b/chrome/utility/importer/external_process_importer_bridge.cc
@@ -135,6 +135,13 @@ void ExternalProcessImporterBridge::SetAutofillFormData(
   DCHECK_EQ(0, autofill_form_data_entries_left);
 }
 
+void ExternalProcessImporterBridge::SetExtensions(
+    const std::vector<std::string>& extension_ids) {
+  // Since extension installations need to be handled by the browser process,
+  // we'll just pass this information through
+  observer_->OnExtensionsImportReady(extension_ids);
+}
+
 void ExternalProcessImporterBridge::NotifyStarted() {
   observer_->OnImportStart();
 }
