diff --git a/chrome/browser/importer/importer_list.cc b/chrome/browser/importer/importer_list.cc
index 5898c273ff443..d709456441330 100644
--- a/chrome/browser/importer/importer_list.cc
+++ b/chrome/browser/importer/importer_list.cc
@@ -6,10 +6,15 @@
 
 #include <stdint.h>
 
+#include "base/files/file_util.h"
 #include "base/functional/bind.h"
+#include "base/json/json_reader.h"
+#include "base/path_service.h"
+#include "base/strings/utf_string_conversions.h"
 #include "base/task/task_traits.h"
 #include "base/task/thread_pool.h"
 #include "base/threading/scoped_blocking_call.h"
+#include "base/values.h"
 #include "build/build_config.h"
 #include "chrome/browser/shell_integration.h"
 #include "chrome/common/importer/firefox_importer_utils.h"
@@ -17,6 +22,7 @@
 #include "chrome/common/importer/importer_data_types.h"
 #include "chrome/grit/generated_resources.h"
 #include "ui/base/l10n/l10n_util.h"
+#include "base/logging.h"
 
 #if BUILDFLAG(IS_MAC)
 #include "base/apple/foundation_util.h"
@@ -29,6 +35,196 @@
 
 namespace {
 
+// Forward declaration for platform-specific Chrome user data folder getter
+base::FilePath GetChromeUserDataFolder();
+
+// Chrome importer helper functions (cross-platform)
+bool HasExtensionsToImport(const base::FilePath& preferences_path) {
+  LOG(INFO) << "VibeBrowser: Checking for extensions in: " << preferences_path.AsUTF8Unsafe();
+
+  std::string preferences_content;
+  if (!base::ReadFileToString(preferences_path, &preferences_content)) {
+    LOG(INFO) << "VibeBrowser: Failed to read preferences file: " << preferences_path.AsUTF8Unsafe();
+    return false;
+  }
+
+  std::optional<base::Value::Dict> preferences =
+      base::JSONReader::ReadDict(preferences_content);
+  if (!preferences) {
+    LOG(INFO) << "VibeBrowser: Failed to parse preferences file as JSON: " << preferences_path.AsUTF8Unsafe();
+    return false;
+  }
+
+  // Extensions are stored in extensions.settings in Chrome preferences
+  const base::Value::Dict* extensions_dict =
+      preferences->FindDictByDottedPath("extensions.settings");
+  if (!extensions_dict) {
+    LOG(INFO) << "VibeBrowser: No extensions.settings found in preferences file";
+    return false;
+  }
+
+  LOG(INFO) << "VibeBrowser: Found extensions.settings with " << extensions_dict->size() << " entries";
+
+  // Check for at least one valid extension
+  int examined_extensions = 0;
+  for (const auto [key, value] : *extensions_dict) {
+    examined_extensions++;
+    if (!value.is_dict()) {
+      continue;
+    }
+
+    const base::Value::Dict& dict = value.GetDict();
+
+    // Only count if:
+    // 1. It's from the Chrome Web Store
+    // 2. It's not installed by default
+    // 3. It's enabled (we're being lenient here, importing disabled ones too for now)
+
+    if (dict.FindBool("was_installed_by_default").value_or(true)) {
+      LOG(INFO) << "VibeBrowser: Extension " << key << " was installed by default, skipping";
+      continue;  // Skip default extensions
+    }
+
+    if (!dict.FindBool("from_webstore").value_or(false)) {
+      LOG(INFO) << "VibeBrowser: Extension " << key << " is not from the web store, skipping";
+      continue;  // Skip non-webstore extensions
+    }
+    return true;
+  }
+
+  LOG(INFO) << "VibeBrowser: Examined " << examined_extensions << " extensions, none qualified for import";
+  return false;
+}
+
+bool ChromeImporterCanImport(const base::FilePath& profile_path, uint16_t* services) {
+  DCHECK(services);
+  *services = importer::NONE;
+
+  if (!base::PathExists(profile_path))
+    return false;
+
+  base::FilePath bookmarks_path = profile_path.Append(FILE_PATH_LITERAL("Bookmarks"));
+  base::FilePath history_path = profile_path.Append(FILE_PATH_LITERAL("History"));
+  base::FilePath passwords_path = profile_path.Append(FILE_PATH_LITERAL("Login Data"));
+  base::FilePath preferences_path = profile_path.Append(FILE_PATH_LITERAL("Preferences"));
+  base::FilePath secure_preferences_path = profile_path.Append(FILE_PATH_LITERAL("Secure Preferences"));
+
+  if (base::PathExists(bookmarks_path))
+    *services |= importer::FAVORITES;
+
+  if (base::PathExists(history_path))
+    *services |= importer::HISTORY;
+
+  if (base::PathExists(passwords_path))
+    *services |= importer::PASSWORDS;
+
+  if (base::PathExists(preferences_path)) {
+    *services |= importer::AUTOFILL_FORM_DATA;
+    *services |= importer::SEARCH_ENGINES;
+
+    // Check for extensions in preferences
+    if (HasExtensionsToImport(preferences_path) ||
+        (base::PathExists(secure_preferences_path) &&
+         HasExtensionsToImport(secure_preferences_path))) {
+      *services |= importer::EXTENSIONS;
+    }
+  }
+
+  return *services != importer::NONE;
+}
+
+base::Value::List GetChromeSourceProfiles(const base::FilePath& local_state_path) {
+  base::Value::List profiles;
+
+  if (base::PathExists(local_state_path)) {
+    std::string local_state_content;
+    if (!base::ReadFileToString(local_state_path, &local_state_content)) {
+      return profiles;
+    }
+
+    std::optional<base::Value::Dict> local_state_dict =
+        base::JSONReader::ReadDict(local_state_content);
+
+    if (local_state_dict) {
+      const auto* profile_dict = local_state_dict->FindDict("profile");
+      if (profile_dict) {
+        const auto* info_cache = profile_dict->FindDict("info_cache");
+        if (info_cache) {
+          for (const auto value : *info_cache) {
+            const auto* profile = value.second.GetIfDict();
+            if (!profile)
+              continue;
+
+            auto* name = profile->FindString("name");
+            if (!name)
+              continue;
+
+            base::Value::Dict entry;
+            entry.Set("id", value.first);
+            entry.Set("name", *name);
+            profiles.Append(std::move(entry));
+          }
+        }
+      }
+    }
+  }
+
+  // If no profiles were found, add the default one
+  if (profiles.empty()) {
+    base::Value::Dict entry;
+    entry.Set("id", "Default");
+    entry.Set("name", "Default");
+    profiles.Append(std::move(entry));
+  }
+
+  return profiles;
+}
+
+void DetectChromeProfiles(std::vector<importer::SourceProfile>* profiles) {
+  base::ScopedBlockingCall scoped_blocking_call(FROM_HERE,
+                                               base::BlockingType::MAY_BLOCK);
+
+  base::FilePath chrome_path = GetChromeUserDataFolder();
+  if (!base::PathExists(chrome_path))
+    return;
+
+  // Get the list of profiles from Local State
+  base::FilePath local_state_path = chrome_path.Append(FILE_PATH_LITERAL("Local State"));
+  base::Value::List chrome_profiles = GetChromeSourceProfiles(local_state_path);
+
+  // Add each profile
+  for (const auto& value : chrome_profiles) {
+    const auto* dict = value.GetIfDict();
+    if (!dict)
+      continue;
+
+    const std::string* profile_id = dict->FindString("id");
+    const std::string* name = dict->FindString("name");
+
+    if (!profile_id || !name)
+      continue;
+
+    base::FilePath profile_folder = chrome_path.Append(
+        base::FilePath::StringType(profile_id->begin(), profile_id->end()));
+    uint16_t services = importer::NONE;
+
+    if (!ChromeImporterCanImport(profile_folder, &services))
+      continue;
+
+    importer::SourceProfile chrome;
+    if (*profile_id == "Default") {
+      chrome.importer_name = l10n_util::GetStringUTF16(IDS_IMPORT_FROM_CHROME);
+    } else {
+      chrome.importer_name = l10n_util::GetStringUTF16(IDS_IMPORT_FROM_CHROME) +
+                            u" - " + base::UTF8ToUTF16(*name);
+    }
+    chrome.importer_type = importer::TYPE_CHROME;
+    chrome.services_supported = services;
+    chrome.source_path = profile_folder;
+    profiles->push_back(chrome);
+  }
+}
+
 #if BUILDFLAG(IS_WIN)
 void DetectIEProfiles(std::vector<importer::SourceProfile>* profiles) {
   base::ScopedBlockingCall scoped_blocking_call(FROM_HERE,
@@ -67,6 +263,21 @@ void DetectBuiltinWindowsProfiles(
 
 #endif  // BUILDFLAG(IS_WIN)
 
+#if BUILDFLAG(IS_WIN)
+// Windows-specific Chrome user data folder getter
+base::FilePath GetChromeUserDataFolder() {
+  base::FilePath result;
+  if (!base::PathService::Get(base::DIR_LOCAL_APP_DATA, &result))
+    return base::FilePath();
+
+  result = result.Append(FILE_PATH_LITERAL("Google"));
+  result = result.Append(FILE_PATH_LITERAL("Chrome"));
+  result = result.Append(FILE_PATH_LITERAL("User Data"));
+
+  return result;
+}
+#endif  // BUILDFLAG(IS_WIN)
+
 #if BUILDFLAG(IS_MAC)
 void DetectSafariProfiles(std::vector<importer::SourceProfile>* profiles) {
   base::ScopedBlockingCall scoped_blocking_call(FROM_HERE,
@@ -83,8 +294,30 @@ void DetectSafariProfiles(std::vector<importer::SourceProfile>* profiles) {
   safari.services_supported = items;
   profiles->push_back(safari);
 }
+
+// macOS-specific Chrome user data folder getter
+base::FilePath GetChromeUserDataFolder() {
+  base::FilePath result = base::apple::GetUserLibraryPath();
+  return result.Append("Application Support/Google/Chrome");
+}
+
+// These functions have been moved outside the platform blocks above
 #endif  // BUILDFLAG(IS_MAC)
 
+#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS)
+// Linux-specific Chrome user data folder getter
+base::FilePath GetChromeUserDataFolder() {
+  const char* home = getenv("HOME");
+  if (!home)
+    return base::FilePath();
+
+  base::FilePath result(home);
+  result = result.Append(".config");
+  result = result.Append("google-chrome");
+  return result;
+}
+#endif  // BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS)
+
 // |locale|: The application locale used for lookups in Firefox's
 // locale-specific search engines feature (see firefox_importer.cc for
 // details).
@@ -163,8 +396,10 @@ std::vector<importer::SourceProfile> DetectSourceProfilesWorker(
 #if BUILDFLAG(IS_WIN)
   if (shell_integration::IsFirefoxDefaultBrowser()) {
     DetectFirefoxProfiles(locale, &profiles);
+    DetectChromeProfiles(&profiles);
     DetectBuiltinWindowsProfiles(&profiles);
   } else {
+    DetectChromeProfiles(&profiles);
     DetectBuiltinWindowsProfiles(&profiles);
     DetectFirefoxProfiles(locale, &profiles);
   }
@@ -172,11 +407,15 @@ std::vector<importer::SourceProfile> DetectSourceProfilesWorker(
   if (shell_integration::IsFirefoxDefaultBrowser()) {
     DetectFirefoxProfiles(locale, &profiles);
     DetectSafariProfiles(&profiles);
+    DetectChromeProfiles(&profiles);
   } else {
     DetectSafariProfiles(&profiles);
+    DetectChromeProfiles(&profiles);
     DetectFirefoxProfiles(locale, &profiles);
   }
 #else
+  // Linux and other platforms
+  DetectChromeProfiles(&profiles);
   DetectFirefoxProfiles(locale, &profiles);
 #endif
   if (include_interactive_profiles) {
