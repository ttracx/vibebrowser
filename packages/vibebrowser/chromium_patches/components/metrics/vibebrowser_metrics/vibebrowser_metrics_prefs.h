diff --git a/components/metrics/VibeBrowser_metrics/VibeBrowser_metrics_prefs.h b/components/metrics/VibeBrowser_metrics/VibeBrowser_metrics_prefs.h
new file mode 100644
index 0000000000000..a417e8d8af351
--- /dev/null
+++ b/components/metrics/VibeBrowser_metrics/VibeBrowser_metrics_prefs.h
@@ -0,0 +1,24 @@
+// Copyright 2025 The Chromium Authors
+// Use of this source code is governed by a BSD-style license that can be
+// found in the LICENSE file.
+
+#ifndef COMPONENTS_METRICS_VibeBrowser_METRICS_VibeBrowser_METRICS_PREFS_H_
+#define COMPONENTS_METRICS_VibeBrowser_METRICS_VibeBrowser_METRICS_PREFS_H_
+
+class PrefRegistrySimple;
+
+namespace user_prefs {
+class PrefRegistrySyncable;
+}  // namespace user_prefs
+
+namespace VibeBrowser_metrics {
+
+// Registers VibeBrowser metrics preferences for the profile.
+void RegisterProfilePrefs(user_prefs::PrefRegistrySyncable* registry);
+
+// Registers VibeBrowser metrics preferences for local state.
+void RegisterLocalStatePrefs(PrefRegistrySimple* registry);
+
+}  // namespace VibeBrowser_metrics
+
+#endif  // COMPONENTS_METRICS_VibeBrowser_METRICS_VibeBrowser_METRICS_PREFS_H_
\ No newline at end of file
