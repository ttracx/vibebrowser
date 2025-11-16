diff --git a/chrome/browser/importer/profile_writer.cc b/chrome/browser/importer/profile_writer.cc
index 7bb741920d9af..3e64195089866 100644
--- a/chrome/browser/importer/profile_writer.cc
+++ b/chrome/browser/importer/profile_writer.cc
@@ -36,6 +36,18 @@
 #include "components/prefs/pref_service.h"
 #include "components/search_engines/template_url.h"
 #include "components/search_engines/template_url_service.h"
+#include "chrome/browser/extensions/extension_service.h"
+#include "extensions/browser/extension_system.h"
+#include "extensions/common/extension.h"
+#include "chrome/browser/extensions/webstore_installer.h"
+#include "chrome/browser/extensions/install_approval.h"
+#include "extensions/browser/extension_registry.h"
+#include "chrome/browser/extensions/extension_install_prompt.h"
+#include "chrome/browser/extensions/webstore_install_with_prompt.h"
+#include "chrome/browser/ui/browser.h"
+#include "chrome/browser/ui/browser_finder.h"
+#include "content/public/browser/web_contents.h"
+#include "base/memory/raw_ptr.h"
 
 using bookmarks::BookmarkModel;
 using bookmarks::BookmarkNode;
@@ -74,6 +86,22 @@ void ShowBookmarkBar(Profile* profile) {
   profile->GetPrefs()->SetBoolean(bookmarks::prefs::kShowBookmarkBar, true);
 }
 
+// Silent installer via webstore without any prompt or bubble.
+class SilentWebstoreInstaller
+    : public extensions::WebstoreInstallWithPrompt {
+ public:
+  using WebstoreInstallWithPrompt::WebstoreInstallWithPrompt;
+
+ private:
+  ~SilentWebstoreInstaller() override = default;
+
+  std::unique_ptr<ExtensionInstallPrompt::Prompt> CreateInstallPrompt()
+      const override {
+    return nullptr;
+  }
+  bool ShouldShowPostInstallUI() const override { return false; }
+};
+
 }  // namespace
 
 ProfileWriter::ProfileWriter(Profile* profile) : profile_(profile) {}
@@ -337,3 +365,119 @@ void ProfileWriter::AddAutocompleteFormDataEntries(
 }
 
 ProfileWriter::~ProfileWriter() = default;
+
+void ProfileWriter::AddExtensions(
+    const std::vector<std::string>& extension_ids) {
+  if (extension_ids.empty())
+    return;
+
+  LOG(INFO) << "ProfileWriter: Installing " << extension_ids.size()
+            << " extensions from Chrome import";
+
+  // Get the ExtensionService for the profile
+  extensions::ExtensionSystem* extension_system =
+      extensions::ExtensionSystem::Get(profile_);
+  if (!extension_system) {
+    LOG(ERROR) << "Failed to get extension system for profile";
+    return;
+  }
+
+  extensions::ExtensionService* extension_service =
+      extension_system->extension_service();
+  if (!extension_service) {
+    LOG(ERROR) << "Failed to get extension service for profile";
+    return;
+  }
+
+  // Check which extensions are already installed
+  extensions::ExtensionRegistry* registry =
+      extensions::ExtensionRegistry::Get(profile_);
+
+  // Find an active WebContents to use (required by WebstoreInstallWithPrompt)
+  content::WebContents* web_contents = nullptr;
+
+  // Try to get a web contents from the active browser
+  Browser* browser = chrome::FindBrowserWithProfile(profile_);
+  if (browser && browser->tab_strip_model()) {
+    web_contents = browser->tab_strip_model()->GetActiveWebContents();
+  }
+
+  if (!web_contents) {
+    LOG(ERROR) << "Could not find an active WebContents. Extension import aborted.";
+    return;
+  }
+
+  // Filter out already installed extensions
+  std::vector<std::string> extensions_to_install;
+  for (const auto& extension_id : extension_ids) {
+    // Skip already installed extensions
+    if (registry && registry->GetInstalledExtension(extension_id)) {
+      LOG(INFO) << "Extension already installed: " << extension_id;
+      continue;
+    }
+    extensions_to_install.push_back(extension_id);
+  }
+
+  if (extensions_to_install.empty()) {
+    LOG(INFO) << "No new extensions to install.";
+    return;
+  }
+
+  // The window/tab could be closed before all extensions are installed
+  // Keep a reference to it in a new class
+  class ExtensionInstallHelper : public base::RefCounted<ExtensionInstallHelper> {
+   public:
+    ExtensionInstallHelper(Profile* profile, content::WebContents* web_contents)
+        : profile_(profile), web_contents_(web_contents) {}
+
+    void InstallExtension(const std::string& extension_id) {
+      // Create the callback that handles installation results
+      auto callback = base::BindOnce(
+          &ExtensionInstallHelper::OnExtensionInstalled,
+          // Need to capture this to ensure the object lives until callback is called
+          base::WrapRefCounted(this),
+          extension_id);
+
+      installer_ = base::MakeRefCounted<SilentWebstoreInstaller>(
+          extension_id,
+          profile_,
+          web_contents_->GetTopLevelNativeWindow(),
+          std::move(callback));
+
+      installer_->BeginInstall();
+      LOG(INFO) << "Started installation for extension: " << extension_id;
+    }
+
+   private:
+    friend class base::RefCounted<ExtensionInstallHelper>;
+
+    // This callback matches the signature expected by WebstoreInstallWithPrompt
+    void OnExtensionInstalled(
+        const std::string& extension_id,
+        bool success,
+        const std::string& error,
+        extensions::webstore_install::Result result) {
+      if (success) {
+        LOG(INFO) << "Successfully installed extension: " << extension_id;
+      } else {
+        LOG(ERROR) << "Failed to install extension " << extension_id
+                  << ": " << error << " (reason: " << result << ")";
+      }
+      // Clear installer to avoid memory leaks
+      installer_ = nullptr;
+    }
+
+    ~ExtensionInstallHelper() = default;
+
+    raw_ptr<Profile> profile_;
+    raw_ptr<content::WebContents> web_contents_;
+    scoped_refptr<SilentWebstoreInstaller> installer_;
+  };
+
+  scoped_refptr<ExtensionInstallHelper> helper =
+      base::MakeRefCounted<ExtensionInstallHelper>(profile_, web_contents);
+
+  for (const auto& extension_id : extensions_to_install) {
+    helper->InstallExtension(extension_id);
+  }
+}
