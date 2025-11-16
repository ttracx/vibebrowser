diff --git a/chrome/browser/ui/startup/google_api_keys_infobar_delegate.cc b/chrome/browser/ui/startup/google_api_keys_infobar_delegate.cc
index 78ba37b940137..5e11f058c5537 100644
--- a/chrome/browser/ui/startup/google_api_keys_infobar_delegate.cc
+++ b/chrome/browser/ui/startup/google_api_keys_infobar_delegate.cc
@@ -30,15 +30,18 @@ GoogleApiKeysInfoBarDelegate::GetIdentifier() const {
 }
 
 std::u16string GoogleApiKeysInfoBarDelegate::GetLinkText() const {
-  return l10n_util::GetStringUTF16(IDS_LEARN_MORE);
+  // return l10n_util::GetStringUTF16(IDS_LEARN_MORE);
+  return std::u16string();
 }
 
 GURL GoogleApiKeysInfoBarDelegate::GetLinkURL() const {
-  return GURL(google_apis::kAPIKeysDevelopersHowToURL);
+  // return GURL(google_apis::kAPIKeysDevelopersHowToURL);
+  return GURL();
 }
 
 std::u16string GoogleApiKeysInfoBarDelegate::GetMessageText() const {
-  return l10n_util::GetStringUTF16(IDS_MISSING_GOOGLE_API_KEYS);
+  // return l10n_util::GetStringUTF16(IDS_MISSING_GOOGLE_API_KEYS);
+  return std::u16string();
 }
 
 int GoogleApiKeysInfoBarDelegate::GetButtons() const {
