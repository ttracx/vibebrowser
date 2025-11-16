diff --git a/components/metrics/VibeBrowser_metrics/VibeBrowser_metrics_service_factory.cc b/components/metrics/VibeBrowser_metrics/VibeBrowser_metrics_service_factory.cc
new file mode 100644
index 0000000000000..8f549533da577
--- /dev/null
+++ b/components/metrics/VibeBrowser_metrics/VibeBrowser_metrics_service_factory.cc
@@ -0,0 +1,58 @@
+// Copyright 2025 The Chromium Authors
+// Use of this source code is governed by a BSD-style license that can be
+// found in the LICENSE file.
+
+#include "components/metrics/VibeBrowser_metrics/VibeBrowser_metrics_service_factory.h"
+
+#include <memory>
+
+#include "base/no_destructor.h"
+#include "chrome/browser/browser_process.h"
+#include "chrome/browser/profiles/profile.h"
+#include "components/keyed_service/content/browser_context_dependency_manager.h"
+#include "components/metrics/VibeBrowser_metrics/VibeBrowser_metrics_service.h"
+#include "components/prefs/pref_service.h"
+#include "content/public/browser/browser_context.h"
+#include "content/public/browser/storage_partition.h"
+
+namespace VibeBrowser_metrics {
+
+// static
+VibeBrowserMetricsService* VibeBrowserMetricsServiceFactory::GetForBrowserContext(
+    content::BrowserContext* context) {
+  return static_cast<VibeBrowserMetricsService*>(
+      GetInstance()->GetServiceForBrowserContext(context, true));
+}
+
+// static
+VibeBrowserMetricsServiceFactory*
+VibeBrowserMetricsServiceFactory::GetInstance() {
+  static base::NoDestructor<VibeBrowserMetricsServiceFactory> instance;
+  return instance.get();
+}
+
+VibeBrowserMetricsServiceFactory::VibeBrowserMetricsServiceFactory()
+    : BrowserContextKeyedServiceFactory(
+          "VibeBrowserMetricsService",
+          BrowserContextDependencyManager::GetInstance()) {}
+
+VibeBrowserMetricsServiceFactory::~VibeBrowserMetricsServiceFactory() = default;
+
+std::unique_ptr<KeyedService>
+VibeBrowserMetricsServiceFactory::BuildServiceInstanceForBrowserContext(
+    content::BrowserContext* context) const {
+  Profile* profile = Profile::FromBrowserContext(context);
+
+  // Don't create service for incognito profiles
+  if (profile->IsOffTheRecord()) {
+    return nullptr;
+  }
+
+  return std::make_unique<VibeBrowserMetricsService>(
+      profile->GetPrefs(),
+      g_browser_process->local_state(),
+      profile->GetDefaultStoragePartition()
+          ->GetURLLoaderFactoryForBrowserProcess());
+}
+
+}  // namespace VibeBrowser_metrics
\ No newline at end of file
