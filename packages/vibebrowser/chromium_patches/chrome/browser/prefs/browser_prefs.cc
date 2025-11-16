diff --git a/chrome/browser/prefs/browser_prefs.cc b/chrome/browser/prefs/browser_prefs.cc
index 9a00400829ae1..ea209084bb09b 100644
--- a/chrome/browser/prefs/browser_prefs.cc
+++ b/chrome/browser/prefs/browser_prefs.cc
@@ -21,6 +21,7 @@
 #include "chrome/browser/accessibility/page_colors.h"
 #include "chrome/browser/accessibility/prefers_default_scrollbar_styles_prefs.h"
 #include "chrome/browser/browser_process_impl.h"
+#include "chrome/browser/VibeBrowser_server/VibeBrowser_server_prefs.h"
 #include "chrome/browser/chrome_content_browser_client.h"
 #include "chrome/browser/component_updater/component_updater_prefs.h"
 #include "chrome/browser/download/download_prefs.h"
@@ -102,6 +103,7 @@
 #include "components/breadcrumbs/core/breadcrumbs_status.h"
 #include "components/browsing_data/core/pref_names.h"
 #include "components/certificate_transparency/pref_names.h"
+#include "components/metrics/VibeBrowser_metrics/VibeBrowser_metrics_prefs.h"
 #include "components/collaboration/public/pref_names.h"
 #include "components/commerce/core/pref_names.h"
 #include "components/content_settings/core/browser/host_content_settings_map.h"
@@ -1568,6 +1570,8 @@ void RegisterLocalState(PrefRegistrySimple* registry) {
   breadcrumbs::RegisterPrefs(registry);
   browser_shutdown::RegisterPrefs(registry);
   BrowserProcessImpl::RegisterPrefs(registry);
+  VibeBrowser_server::RegisterLocalStatePrefs(registry);
+  VibeBrowser_metrics::RegisterLocalStatePrefs(registry);
   ChromeContentBrowserClient::RegisterLocalStatePrefs(registry);
   chrome_labs_prefs::RegisterLocalStatePrefs(registry);
   chrome_urls::RegisterPrefs(registry);
@@ -1868,6 +1872,7 @@ void RegisterProfilePrefs(user_prefs::PrefRegistrySyncable* registry,
   AnnouncementNotificationService::RegisterProfilePrefs(registry);
   autofill::prefs::RegisterProfilePrefs(registry);
   browsing_data::prefs::RegisterBrowserUserPrefs(registry);
+  VibeBrowser_metrics::RegisterProfilePrefs(registry);
   capture_policy::RegisterProfilePrefs(registry);
   certificate_transparency::prefs::RegisterPrefs(registry);
   ChromeContentBrowserClient::RegisterProfilePrefs(registry);
@@ -1939,6 +1944,7 @@ void RegisterProfilePrefs(user_prefs::PrefRegistrySyncable* registry,
   regional_capabilities::prefs::RegisterProfilePrefs(registry);
   RegisterBrowserUserPrefs(registry);
   RegisterGeminiSettingsPrefs(registry);
+  RegisterNxtscapePrefs(registry);
   RegisterPrefersDefaultScrollbarStylesPrefs(registry);
   RegisterSafetyHubProfilePrefs(registry);
 #if BUILDFLAG(IS_CHROMEOS)
@@ -2322,6 +2328,46 @@ void RegisterGeminiSettingsPrefs(user_prefs::PrefRegistrySyncable* registry) {
   registry->RegisterIntegerPref(prefs::kGeminiSettings, 0);
 }
 
+void RegisterNxtscapePrefs(user_prefs::PrefRegistrySyncable* registry) {
+  // AI Provider configurations stored as JSON
+  // This will store the entire provider configuration including:
+  // - defaultProviderId
+  // - providers array with all configured providers
+  registry->RegisterStringPref(prefs::kVibeBrowserProviders, "");
+  
+  // Legacy preferences (kept for backward compatibility)
+  registry->RegisterStringPref("nxtscape.default_provider", "VibeBrowser");
+  
+  // Nxtscape provider settings
+  registry->RegisterStringPref("nxtscape.nxtscape_model", "");
+
+  // OpenAI provider settings
+  registry->RegisterStringPref("nxtscape.openai_api_key", "");
+  registry->RegisterStringPref("nxtscape.openai_model", "gpt-4o");
+  registry->RegisterStringPref("nxtscape.openai_base_url", "");
+
+  // Anthropic provider settings
+  registry->RegisterStringPref("nxtscape.anthropic_api_key", "");
+  registry->RegisterStringPref("nxtscape.anthropic_model", "claude-3-5-sonnet-latest");
+  registry->RegisterStringPref("nxtscape.anthropic_base_url", "");
+
+  // Gemini provider settings
+  registry->RegisterStringPref("nxtscape.gemini_api_key", "");
+  registry->RegisterStringPref("nxtscape.gemini_model", "gemini-1.5-pro");
+  registry->RegisterStringPref("nxtscape.gemini_base_url", "");
+
+  // Ollama provider settings
+  registry->RegisterStringPref("nxtscape.ollama_api_key", "");
+  registry->RegisterStringPref("nxtscape.ollama_base_url", "http://localhost:11434");
+  registry->RegisterStringPref("nxtscape.ollama_model", "");
+  
+  // VibeBrowser toolbar settings
+  registry->RegisterBooleanPref(prefs::kVibeBrowserShowToolbarLabels, true);
+  
+  // Custom providers list - stored as a JSON string
+  registry->RegisterStringPref(prefs::kVibeBrowserCustomProviders, "[]");
+}
+
 #if BUILDFLAG(IS_CHROMEOS)
 void RegisterSigninProfilePrefs(user_prefs::PrefRegistrySyncable* registry,
                                 std::string_view country) {
