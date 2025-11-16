diff --git a/chrome/common/importer/profile_import_process_param_traits_macros.h b/chrome/common/importer/profile_import_process_param_traits_macros.h
index 4c37c6c749616..753fadff0f94a 100644
--- a/chrome/common/importer/profile_import_process_param_traits_macros.h
+++ b/chrome/common/importer/profile_import_process_param_traits_macros.h
@@ -20,11 +20,11 @@
 #if BUILDFLAG(IS_WIN)
 IPC_ENUM_TRAITS_MIN_MAX_VALUE(importer::ImporterType,
                               importer::TYPE_UNKNOWN,
-                              importer::TYPE_EDGE)
+                              importer::TYPE_CHROME)
 #else
 IPC_ENUM_TRAITS_MIN_MAX_VALUE(importer::ImporterType,
                               importer::TYPE_UNKNOWN,
-                              importer::TYPE_BOOKMARKS_FILE)
+                              importer::TYPE_CHROME)
 #endif
 
 IPC_ENUM_TRAITS_MIN_MAX_VALUE(importer::ImportItem,
