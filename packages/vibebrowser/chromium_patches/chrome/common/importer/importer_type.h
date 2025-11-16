diff --git a/chrome/common/importer/importer_type.h b/chrome/common/importer/importer_type.h
index 4d14f7ac9f7a2..4fb09d902e0e2 100644
--- a/chrome/common/importer/importer_type.h
+++ b/chrome/common/importer/importer_type.h
@@ -28,6 +28,7 @@ enum ImporterType {
 #if BUILDFLAG(IS_WIN)
   TYPE_EDGE = 6,
 #endif
+  TYPE_CHROME = 7,
 };
 
 }  // namespace importer
