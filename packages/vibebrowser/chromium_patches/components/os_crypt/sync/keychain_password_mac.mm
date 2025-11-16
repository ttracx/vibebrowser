diff --git a/components/os_crypt/sync/keychain_password_mac.mm b/components/os_crypt/sync/keychain_password_mac.mm
index 6b936d228cf71..164325ce05d78 100644
--- a/components/os_crypt/sync/keychain_password_mac.mm
+++ b/components/os_crypt/sync/keychain_password_mac.mm
@@ -33,8 +33,8 @@ namespace {
 const char kDefaultServiceName[] = "Chrome Safe Storage";
 const char kDefaultAccountName[] = "Chrome";
 #else
-const char kDefaultServiceName[] = "Chromium Safe Storage";
-const char kDefaultAccountName[] = "Chromium";
+const char kDefaultServiceName[] = "VibeBrowser Safe Storage";
+const char kDefaultAccountName[] = "VibeBrowser";
 #endif
 
 // Generates a random password and adds it to the Keychain.  The added password
