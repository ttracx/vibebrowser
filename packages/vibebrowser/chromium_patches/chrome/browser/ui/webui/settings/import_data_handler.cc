diff --git a/chrome/browser/ui/webui/settings/import_data_handler.cc b/chrome/browser/ui/webui/settings/import_data_handler.cc
index cecce41ac08ae..934a8a7f2f7c2 100644
--- a/chrome/browser/ui/webui/settings/import_data_handler.cc
+++ b/chrome/browser/ui/webui/settings/import_data_handler.cc
@@ -146,6 +146,9 @@ void ImportDataHandler::HandleImportData(const base::Value::List& args) {
   if (*type_dict.FindBool(prefs::kImportDialogSearchEngine)) {
     selected_items |= importer::SEARCH_ENGINES;
   }
+  if (*type_dict.FindBool(prefs::kImportDialogExtensions)) {
+    selected_items |= importer::EXTENSIONS;
+  }
 
   const importer::SourceProfile& source_profile =
       importer_list_->GetSourceProfileAt(browser_index);
@@ -223,6 +226,8 @@ void ImportDataHandler::SendBrowserProfileData(const std::string& callback_id) {
                         (browser_services & importer::SEARCH_ENGINES) != 0);
     browser_profile.Set("autofillFormData",
                         (browser_services & importer::AUTOFILL_FORM_DATA) != 0);
+    browser_profile.Set("extensions",
+                        (browser_services & importer::EXTENSIONS) != 0);
 
     browser_profiles.Append(std::move(browser_profile));
   }
