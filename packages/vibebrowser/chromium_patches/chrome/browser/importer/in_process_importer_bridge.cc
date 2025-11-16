diff --git a/chrome/browser/importer/in_process_importer_bridge.cc b/chrome/browser/importer/in_process_importer_bridge.cc
index ce876a21da88a..bc687cd5f9f4e 100644
--- a/chrome/browser/importer/in_process_importer_bridge.cc
+++ b/chrome/browser/importer/in_process_importer_bridge.cc
@@ -26,6 +26,10 @@
 
 namespace {
 
+// Temporary definition, should be added to the history module
+const history::VisitSource SOURCE_CHROME_IMPORTED =
+    static_cast<history::VisitSource>(4);
+
 history::URLRows ConvertImporterURLRowsToHistoryURLRows(
     const std::vector<ImporterURLRow>& rows) {
   history::URLRows converted;
@@ -53,6 +57,8 @@ history::VisitSource ConvertImporterVisitSourceToHistoryVisitSource(
       return history::SOURCE_IE_IMPORTED;
     case importer::VISIT_SOURCE_SAFARI_IMPORTED:
       return history::SOURCE_SAFARI_IMPORTED;
+    case importer::VISIT_SOURCE_CHROME_IMPORTED:
+      return SOURCE_CHROME_IMPORTED;
   }
   NOTREACHED();
 }
@@ -168,6 +174,16 @@ void InProcessImporterBridge::SetAutofillFormData(
   writer_->AddAutocompleteFormDataEntries(autocomplete_entries);
 }
 
+void InProcessImporterBridge::SetExtensions(
+    const std::vector<std::string>& extension_ids) {
+  LOG(INFO) << "InProcessImporterBridge: Received " << extension_ids.size()
+            << " extensions to import";
+
+  // Pass the extension IDs to the profile writer to handle installation
+  // This will be implemented in ProfileWriter
+  writer_->AddExtensions(extension_ids);
+}
+
 void InProcessImporterBridge::NotifyStarted() {
   host_->NotifyImportStarted();
 }
