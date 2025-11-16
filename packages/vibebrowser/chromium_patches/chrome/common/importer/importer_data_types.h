diff --git a/chrome/common/importer/importer_data_types.h b/chrome/common/importer/importer_data_types.h
index 183af47c26576..d0f67017eb876 100644
--- a/chrome/common/importer/importer_data_types.h
+++ b/chrome/common/importer/importer_data_types.h
@@ -29,7 +29,8 @@ enum ImportItem {
   SEARCH_ENGINES     = 1 << 4,
   HOME_PAGE          = 1 << 5,
   AUTOFILL_FORM_DATA = 1 << 6,
-  ALL                = (1 << 7) - 1  // All the bits should be 1, hence the -1.
+  EXTENSIONS         = 1 << 7,
+  ALL                = (1 << 8) - 1  // All the bits should be 1, hence the -1.
 };
 
 // Information about a profile needed by an importer to do import work.
@@ -109,6 +110,7 @@ enum VisitSource {
   VISIT_SOURCE_FIREFOX_IMPORTED = 1,
   VISIT_SOURCE_IE_IMPORTED = 2,
   VISIT_SOURCE_SAFARI_IMPORTED = 3,
+  VISIT_SOURCE_CHROME_IMPORTED = 4,
 };
 
 }  // namespace importer
