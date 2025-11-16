diff --git a/components/metrics/VibeBrowser_metrics/VibeBrowser_metrics.h b/components/metrics/VibeBrowser_metrics/VibeBrowser_metrics.h
new file mode 100644
index 0000000000000..73e98afdaa28a
--- /dev/null
+++ b/components/metrics/VibeBrowser_metrics/VibeBrowser_metrics.h
@@ -0,0 +1,40 @@
+// Copyright 2025 The Chromium Authors
+// Use of this source code is governed by a BSD-style license that can be
+// found in the LICENSE file.
+
+#ifndef COMPONENTS_METRICS_VibeBrowser_METRICS_VibeBrowser_METRICS_H_
+#define COMPONENTS_METRICS_VibeBrowser_METRICS_VibeBrowser_METRICS_H_
+
+#include <string>
+#include <utility>
+
+#include "base/values.h"
+
+namespace VibeBrowser_metrics {
+
+// Simple static API for logging VibeBrowser metrics.
+// Usage: VibeBrowserMetrics::Log("event.name");
+class VibeBrowserMetrics {
+ public:
+  // Log an event with no properties
+  // sample_rate: 0.0 to 1.0, defaults to 1.0 (always log)
+  // For example, sample_rate=0.1 means log only 10% of the time
+  static void Log(const std::string& event_name, double sample_rate = 1.0);
+  
+  // Log an event with properties using initializer list
+  // Example: Log("event", {{"key1", "value1"}, {"key2", 123}})
+  static void Log(const std::string& event_name,
+                  std::initializer_list<std::pair<std::string, base::Value>> properties,
+                  double sample_rate = 1.0);
+  
+  // Log an event with pre-built properties dict
+  static void Log(const std::string& event_name, base::Value::Dict properties,
+                  double sample_rate = 1.0);
+
+ private:
+  VibeBrowserMetrics() = delete;
+};
+
+}  // namespace VibeBrowser_metrics
+
+#endif  // COMPONENTS_METRICS_VibeBrowser_METRICS_VibeBrowser_METRICS_H_
\ No newline at end of file
