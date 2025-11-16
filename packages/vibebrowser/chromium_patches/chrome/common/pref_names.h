diff --git a/chrome/common/pref_names.h b/chrome/common/pref_names.h
index 0e898dc745b6e..c649ec745fbcd 100644
--- a/chrome/common/pref_names.h
+++ b/chrome/common/pref_names.h
@@ -1590,6 +1590,8 @@ inline constexpr char kImportDialogSavedPasswords[] =
     "import_dialog_saved_passwords";
 inline constexpr char kImportDialogSearchEngine[] =
     "import_dialog_search_engine";
+inline constexpr char kImportDialogExtensions[] =
+    "import_dialog_extensions";
 
 #if BUILDFLAG(IS_CHROMEOS)
 // Boolean controlling whether native client is force allowed by policy.
@@ -4271,6 +4273,29 @@ inline constexpr char kServiceWorkerToControlSrcdocIframeEnabled[] =
 // is set as a SharedWorker script URL.
 inline constexpr char kSharedWorkerBlobURLFixEnabled[] =
     "worker.shared_worker_blob_url_fix_enabled";
+
+// String containing the stable client ID for VibeBrowser metrics
+inline constexpr char kVibeBrowserMetricsClientId[] =
+    "VibeBrowser.metrics_client_id";
+
+// String containing the stable install ID for VibeBrowser metrics (Local State)
+inline constexpr char kVibeBrowserMetricsInstallId[] =
+    "VibeBrowser.metrics_install_id";
+
+// JSON string containing custom AI providers for VibeBrowser
+inline constexpr char kVibeBrowserCustomProviders[] = 
+    "VibeBrowser.custom_providers";
+
+// JSON string containing the list of AI providers and configuration
+inline constexpr char kVibeBrowserProviders[] = "VibeBrowser.providers";
+
+// String containing the default provider ID for VibeBrowser
+inline constexpr char kVibeBrowserDefaultProviderId[] = 
+    "VibeBrowser.default_provider_id";
+
+// Boolean that controls whether toolbar labels are shown for VibeBrowser actions
+inline constexpr char kVibeBrowserShowToolbarLabels[] =
+    "VibeBrowser.show_toolbar_labels";
 }  // namespace prefs
 
 #endif  // CHROME_COMMON_PREF_NAMES_H_
