diff --git a/chrome/browser/extensions/api/settings_private/prefs_util.cc b/chrome/browser/extensions/api/settings_private/prefs_util.cc
index 97bb4be60af93..1bff654e3ac42 100644
--- a/chrome/browser/extensions/api/settings_private/prefs_util.cc
+++ b/chrome/browser/extensions/api/settings_private/prefs_util.cc
@@ -580,6 +580,37 @@ const PrefsUtil::TypedPrefMap& PrefsUtil::GetAllowlistedKeys() {
   (*s_allowlist)[::prefs::kCaretBrowsingEnabled] =
       settings_api::PrefType::kBoolean;
 
+  // Nxtscape AI provider preferences
+  (*s_allowlist)[prefs::kVibeBrowserProviders] = settings_api::PrefType::kString;
+  (*s_allowlist)["nxtscape.default_provider"] = settings_api::PrefType::kString;
+  
+  // Nxtscape provider settings
+  (*s_allowlist)["nxtscape.nxtscape_model"] = settings_api::PrefType::kString;
+  
+  // OpenAI provider settings
+  (*s_allowlist)["nxtscape.openai_api_key"] = settings_api::PrefType::kString;
+  (*s_allowlist)["nxtscape.openai_model"] = settings_api::PrefType::kString;
+  (*s_allowlist)["nxtscape.openai_base_url"] = settings_api::PrefType::kString;
+
+  // Anthropic provider settings
+  (*s_allowlist)["nxtscape.anthropic_api_key"] = settings_api::PrefType::kString;
+  (*s_allowlist)["nxtscape.anthropic_model"] = settings_api::PrefType::kString;
+  (*s_allowlist)["nxtscape.anthropic_base_url"] = settings_api::PrefType::kString;
+
+  // Gemini provider settings
+  (*s_allowlist)["nxtscape.gemini_api_key"] = settings_api::PrefType::kString;
+  (*s_allowlist)["nxtscape.gemini_model"] = settings_api::PrefType::kString;
+  (*s_allowlist)["nxtscape.gemini_base_url"] = settings_api::PrefType::kString;
+
+  // Ollama provider settings
+  (*s_allowlist)["nxtscape.ollama_api_key"] = settings_api::PrefType::kString;
+  (*s_allowlist)["nxtscape.ollama_base_url"] = settings_api::PrefType::kString;
+  (*s_allowlist)["nxtscape.ollama_model"] = settings_api::PrefType::kString;
+
+  // VibeBrowser preferences
+  (*s_allowlist)[prefs::kVibeBrowserShowToolbarLabels] = settings_api::PrefType::kBoolean;
+  (*s_allowlist)[prefs::kVibeBrowserCustomProviders] = settings_api::PrefType::kString;
+
 #if BUILDFLAG(IS_CHROMEOS)
   // Accounts / Users / People.
   (*s_allowlist)[ash::kAccountsPrefAllowGuest] =
@@ -1164,6 +1195,8 @@ const PrefsUtil::TypedPrefMap& PrefsUtil::GetAllowlistedKeys() {
       settings_api::PrefType::kBoolean;
   (*s_allowlist)[::prefs::kImportDialogSearchEngine] =
       settings_api::PrefType::kBoolean;
+  (*s_allowlist)[::prefs::kImportDialogExtensions] =
+      settings_api::PrefType::kBoolean;
 #endif  // BUILDFLAG(IS_CHROMEOS)
 
   // Supervised Users.  This setting is queried in our Tast tests (b/241943380).
