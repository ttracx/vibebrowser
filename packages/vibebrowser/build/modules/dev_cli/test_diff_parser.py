#!/usr/bin/env python3
"""
Test script for diff parser functionality

This script tests various edge cases for the diff parser to ensure
it handles all types of git diff outputs correctly.
"""

import sys
from pathlib import Path

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from modules.dev_cli.utils import parse_diff_output, FilePatch, FileOperation


def test_regular_modify():
    """Test regular file modification"""
    diff = """diff --git a/file.txt b/file.txt
index abc123..def456 100644
--- a/file.txt
+++ b/file.txt
@@ -1,3 +1,3 @@
 line1
-old line2
+new line2
 line3"""

    result = parse_diff_output(diff)
    assert len(result) == 1
    assert "file.txt" in result
    patch = result["file.txt"]
    assert patch.operation == FileOperation.MODIFY
    assert not patch.is_binary
    assert patch.patch_content is not None
    print("✓ Regular modify test passed")


def test_new_file():
    """Test new file addition"""
    diff = """diff --git a/newfile.txt b/newfile.txt
new file mode 100644
index 0000000..abc123
--- /dev/null
+++ b/newfile.txt
@@ -0,0 +1,3 @@
+line1
+line2
+line3"""

    result = parse_diff_output(diff)
    assert len(result) == 1
    assert "newfile.txt" in result
    patch = result["newfile.txt"]
    assert patch.operation == FileOperation.ADD
    assert patch.patch_content is not None
    print("✓ New file test passed")


def test_deleted_file():
    """Test file deletion"""
    diff = """diff --git a/deleted.txt b/deleted.txt
deleted file mode 100644
index abc123..0000000
--- a/deleted.txt
+++ /dev/null
@@ -1,3 +0,0 @@
-line1
-line2
-line3"""

    result = parse_diff_output(diff)
    assert len(result) == 1
    assert "deleted.txt" in result
    patch = result["deleted.txt"]
    assert patch.operation == FileOperation.DELETE
    print("✓ Deleted file test passed")


def test_renamed_file():
    """Test file rename"""
    diff = """diff --git a/old_name.txt b/new_name.txt
similarity index 100%
rename from old_name.txt
rename to new_name.txt"""

    result = parse_diff_output(diff)
    assert len(result) == 1
    assert "new_name.txt" in result
    patch = result["new_name.txt"]
    assert patch.operation == FileOperation.RENAME
    assert patch.old_path == "old_name.txt"
    assert patch.similarity == 100
    print("✓ Renamed file test passed")


def test_renamed_with_changes():
    """Test file rename with content changes"""
    diff = """diff --git a/old_name.txt b/new_name.txt
similarity index 85%
rename from old_name.txt
rename to new_name.txt
index abc123..def456 100644
--- a/old_name.txt
+++ b/new_name.txt
@@ -1,3 +1,4 @@
 line1
 line2
-line3
+modified line3
+new line4"""

    result = parse_diff_output(diff)
    assert len(result) == 1
    assert "new_name.txt" in result
    patch = result["new_name.txt"]
    assert patch.operation == FileOperation.RENAME
    assert patch.old_path == "old_name.txt"
    assert patch.similarity == 85
    assert patch.patch_content is not None
    print("✓ Renamed with changes test passed")


def test_binary_file():
    """Test binary file handling"""
    diff = """diff --git a/image.png b/image.png
index abc123..def456 100644
Binary files a/image.png and b/image.png differ"""

    result = parse_diff_output(diff)
    assert len(result) == 1
    assert "image.png" in result
    patch = result["image.png"]
    assert patch.is_binary
    assert patch.patch_content is None  # Binary content not stored
    print("✓ Binary file test passed")


def test_multiple_files():
    """Test multiple files in one diff"""
    diff = """diff --git a/file1.txt b/file1.txt
index abc123..def456 100644
--- a/file1.txt
+++ b/file1.txt
@@ -1 +1 @@
-old content
+new content
diff --git a/file2.txt b/file2.txt
new file mode 100644
index 0000000..xyz789
--- /dev/null
+++ b/file2.txt
@@ -0,0 +1 @@
+new file content
diff --git a/file3.txt b/file3.txt
deleted file mode 100644
index 111111..000000
--- a/file3.txt
+++ /dev/null
@@ -1 +0,0 @@
-deleted content"""

    result = parse_diff_output(diff)
    assert len(result) == 3
    assert "file1.txt" in result
    assert "file2.txt" in result
    assert "file3.txt" in result

    assert result["file1.txt"].operation == FileOperation.MODIFY
    assert result["file2.txt"].operation == FileOperation.ADD
    assert result["file3.txt"].operation == FileOperation.DELETE
    print("✓ Multiple files test passed")


def test_no_newline_marker():
    """Test handling of 'No newline at end of file' marker"""
    diff = """diff --git a/file.txt b/file.txt
index abc123..def456 100644
--- a/file.txt
+++ b/file.txt
@@ -1 +1 @@
-old content
\\ No newline at end of file
+new content
\\ No newline at end of file"""

    result = parse_diff_output(diff)
    assert len(result) == 1
    assert "file.txt" in result
    patch = result["file.txt"]
    assert patch.operation == FileOperation.MODIFY
    assert "\\ No newline at end of file" in patch.patch_content
    print("✓ No newline marker test passed")


def test_complex_path():
    """Test handling of complex file paths"""
    diff = """diff --git a/src/chrome/browser/ui/views/file.cc b/src/chrome/browser/ui/views/file.cc
index abc123..def456 100644
--- a/src/chrome/browser/ui/views/file.cc
+++ b/src/chrome/browser/ui/views/file.cc
@@ -100,7 +100,7 @@ void Function() {
   int x = 1;
-  int y = 2;
+  int y = 3;
   return x + y;
 }"""

    result = parse_diff_output(diff)
    assert len(result) == 1
    assert "src/chrome/browser/ui/views/file.cc" in result
    patch = result["src/chrome/browser/ui/views/file.cc"]
    assert patch.operation == FileOperation.MODIFY
    print("✓ Complex path test passed")


def test_empty_diff():
    """Test empty diff handling"""
    diff = ""
    result = parse_diff_output(diff)
    assert len(result) == 0
    print("✓ Empty diff test passed")


def test_mode_change():
    """Test file mode change"""
    diff = """diff --git a/script.sh b/script.sh
old mode 100644
new mode 100755
index abc123..abc123
--- a/script.sh
+++ b/script.sh
@@ -1 +1 @@
 #!/bin/bash"""

    result = parse_diff_output(diff)
    assert len(result) == 1
    assert "script.sh" in result
    patch = result["script.sh"]
    # Mode changes are captured in the patch content
    assert "old mode 100644" in patch.patch_content
    assert "new mode 100755" in patch.patch_content
    print("✓ Mode change test passed")


def test_copied_file():
    """Test file copy"""
    diff = """diff --git a/original.txt b/copy.txt
similarity index 100%
copy from original.txt
copy to copy.txt"""

    result = parse_diff_output(diff)
    assert len(result) == 1
    assert "copy.txt" in result
    patch = result["copy.txt"]
    assert patch.operation == FileOperation.COPY
    assert patch.old_path == "original.txt"
    assert patch.similarity == 100
    print("✓ Copied file test passed")


def run_all_tests():
    """Run all test cases"""
    tests = [
        test_regular_modify,
        test_new_file,
        test_deleted_file,
        test_renamed_file,
        test_renamed_with_changes,
        test_binary_file,
        test_multiple_files,
        test_no_newline_marker,
        test_complex_path,
        test_empty_diff,
        test_mode_change,
        test_copied_file,
    ]

    print("Running diff parser tests...")
    print("=" * 60)

    failed_tests = []
    for test in tests:
        try:
            test()
        except Exception as e:
            test_name = test.__name__
            print(f"✗ {test_name} failed: {e}")
            failed_tests.append((test_name, str(e)))

    print("=" * 60)
    if failed_tests:
        print(f"\n{len(failed_tests)} tests failed:")
        for name, error in failed_tests:
            print(f"  - {name}: {error}")
        return False
    else:
        print(f"\nAll {len(tests)} tests passed!")
        return True


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
