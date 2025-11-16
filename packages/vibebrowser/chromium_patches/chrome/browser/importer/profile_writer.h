diff --git a/chrome/browser/importer/profile_writer.h b/chrome/browser/importer/profile_writer.h
index 7bccdf2099ae9..8c2c44972981f 100644
--- a/chrome/browser/importer/profile_writer.h
+++ b/chrome/browser/importer/profile_writer.h
@@ -88,6 +88,10 @@ class ProfileWriter : public base::RefCountedThreadSafe<ProfileWriter> {
   virtual void AddAutocompleteFormDataEntries(
       const std::vector<autofill::AutocompleteEntry>& autocomplete_entries);
 
+  // Installs the extensions with the given extension IDs from the Chrome Web Store.
+  // This is used when importing extensions from another browser profile.
+  virtual void AddExtensions(const std::vector<std::string>& extension_ids);
+
  protected:
   friend class base::RefCountedThreadSafe<ProfileWriter>;
 
