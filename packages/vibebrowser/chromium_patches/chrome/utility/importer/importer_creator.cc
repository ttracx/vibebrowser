diff --git a/chrome/utility/importer/importer_creator.cc b/chrome/utility/importer/importer_creator.cc
index bca57d332db1f..dc0e8c3f4e756 100644
--- a/chrome/utility/importer/importer_creator.cc
+++ b/chrome/utility/importer/importer_creator.cc
@@ -8,6 +8,7 @@
 #include "build/build_config.h"
 #include "chrome/utility/importer/bookmarks_file_importer.h"
 #include "chrome/utility/importer/firefox_importer.h"
+#include "chrome/utility/importer/chrome_importer.h"
 
 #if BUILDFLAG(IS_WIN)
 #include "chrome/common/importer/edge_importer_utils_win.h"
@@ -38,6 +39,8 @@ scoped_refptr<Importer> CreateImporterByType(ImporterType type) {
 #if !BUILDFLAG(IS_CHROMEOS)
     case TYPE_FIREFOX:
       return new FirefoxImporter();
+    case TYPE_CHROME:
+      return new ChromeImporter();
 #endif
 #if BUILDFLAG(IS_MAC)
     case TYPE_SAFARI:
