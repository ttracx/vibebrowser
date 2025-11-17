"""
Extract module - Extract patches from git commits

This module provides commands to extract patches from git commits in a Chromium
repository, storing them as individual file diffs that can be re-applied.
"""

import click
import sys
from pathlib import Path
from typing import Optional, List, Dict
from context import BuildContext
from modules.dev_cli.utils import (
    FilePatch,
    FileOperation,
    GitError,
    run_git_command,
    validate_git_repository,
    validate_commit_exists,
    parse_diff_output,
    write_patch_file,
    create_deletion_marker,
    create_binary_marker,
    log_extraction_summary,
    get_commit_info,
    get_commit_changed_files,
)
from utils import log_info, log_error, log_success, log_warning


@click.group(name="extract")
def extract_group():
    """Extract patches from git commits"""
    pass


@extract_group.command(name="commit")
@click.argument("commit")
@click.option("--verbose", "-v", is_flag=True, help="Show detailed output")
@click.option("--force", "-f", is_flag=True, help="Overwrite existing patches")
@click.option("--include-binary", is_flag=True, help="Include binary files")
@click.option("--base", help="Extract full diff from base commit for files in COMMIT")
@click.pass_context
def extract_commit(ctx, commit, verbose, force, include_binary, base):
    """Extract patches from a single commit

    \b
    Examples:
      dev extract commit HEAD
      dev extract commit abc123
      dev extract commit HEAD~1 --verbose
      dev extract commit HEAD --base chromium/main

    With --base, extracts files changed in COMMIT but shows
    the full diff from base..COMMIT for those files.
    """
    # Get chromium source from parent context
    chromium_src = ctx.parent.obj.get("chromium_src")

    # Create build context
    from dev import create_build_context

    build_ctx = create_build_context(chromium_src)

    if not build_ctx:
        return

    # Validate it's a git repository
    if not validate_git_repository(build_ctx.chromium_src):
        log_error(f"Not a git repository: {build_ctx.chromium_src}")
        ctx.exit(1)

    if base:
        log_info(f"Extracting patches from commit: {commit} (base: {base})")
        # Validate base commit exists
        if not validate_commit_exists(base, build_ctx.chromium_src):
            log_error(f"Base commit not found: {base}")
            ctx.exit(1)
    else:
        log_info(f"Extracting patches from commit: {commit}")

    try:
        extracted = extract_single_commit(
            build_ctx, commit, verbose, force, include_binary, base
        )

        if extracted > 0:
            log_success(f"Successfully extracted {extracted} patches from {commit}")
        else:
            log_warning(f"No patches extracted from {commit}")

    except GitError as e:
        log_error(f"Git error: {e}")
        ctx.exit(1)
    except Exception as e:
        log_error(f"Unexpected error: {e}")
        if verbose:
            import traceback

            traceback.print_exc()
        ctx.exit(1)


@extract_group.command(name="range")
@click.argument("base_commit")
@click.argument("head_commit")
@click.option("--verbose", "-v", is_flag=True, help="Show detailed output")
@click.option("--force", "-f", is_flag=True, help="Overwrite existing patches")
@click.option("--include-binary", is_flag=True, help="Include binary files")
@click.option("--squash", is_flag=True, help="Squash all commits into single patches")
@click.option(
    "--base",
    help="Use different base for diff (gets full diff from base for files in range)",
)
@click.pass_context
def extract_range(
    ctx, base_commit, head_commit, verbose, force, include_binary, squash, base
):
    """Extract patches from a range of commits

    \b
    Examples:
      dev extract range main HEAD
      dev extract range HEAD~5 HEAD
      dev extract range chromium-base HEAD --squash
      dev extract range HEAD~5 HEAD --base upstream/main
    """
    # Get chromium source from parent context
    chromium_src = ctx.parent.obj.get("chromium_src")

    # Create build context
    from dev import create_build_context

    build_ctx = create_build_context(chromium_src)

    if not build_ctx:
        return

    # Validate it's a git repository
    if not validate_git_repository(build_ctx.chromium_src):
        log_error(f"Not a git repository: {build_ctx.chromium_src}")
        ctx.exit(1)

    if base:
        log_info(
            f"Extracting patches from range: {base_commit}..{head_commit} (with base: {base})"
        )
    else:
        log_info(f"Extracting patches from range: {base_commit}..{head_commit}")

    try:
        if squash:
            # Extract as single cumulative diff
            extracted = extract_commit_range(
                build_ctx,
                base_commit,
                head_commit,
                verbose,
                force,
                include_binary,
                base,
            )
        else:
            # Extract each commit separately
            extracted = extract_commits_individually(
                build_ctx,
                base_commit,
                head_commit,
                verbose,
                force,
                include_binary,
                base,
            )

        if extracted > 0:
            log_success(f"Successfully extracted {extracted} patches from range")
        else:
            log_warning(f"No patches extracted from range")

    except GitError as e:
        log_error(f"Git error: {e}")
        ctx.exit(1)
    except Exception as e:
        log_error(f"Unexpected error: {e}")
        if verbose:
            import traceback

            traceback.print_exc()
        ctx.exit(1)


def extract_single_commit(
    ctx: BuildContext,
    commit_hash: str,
    verbose: bool = False,
    force: bool = False,
    include_binary: bool = False,
    base: Optional[str] = None,
) -> int:
    """Extract patches from a single commit

    Args:
        ctx: Build context
        commit_hash: Commit to extract
        verbose: Show detailed output
        force: Overwrite existing patches
        include_binary: Include binary files
        base: If provided, extract full diff from base for files in commit

    Returns:
        Number of patches successfully extracted
    """
    # Step 1: Validate commit
    if not validate_commit_exists(commit_hash, ctx.chromium_src):
        raise GitError(f"Commit not found: {commit_hash}")

    # Get commit info for logging
    commit_info = get_commit_info(commit_hash, ctx.chromium_src)
    if commit_info and verbose:
        log_info(
            f"  Author: {commit_info['author_name']} <{commit_info['author_email']}>"
        )
        log_info(f"  Subject: {commit_info['subject']}")

    if base:
        # With --base: Get files from commit, but diff from base
        return extract_with_base(ctx, commit_hash, base, verbose, force, include_binary)
    else:
        # Normal behavior: diff against parent
        return extract_normal(ctx, commit_hash, verbose, force, include_binary)


def extract_normal(
    ctx: BuildContext,
    commit_hash: str,
    verbose: bool,
    force: bool,
    include_binary: bool,
) -> int:
    """Extract patches normally (diff against parent)"""

    # Get diff against parent
    diff_cmd = ["git", "diff", f"{commit_hash}^..{commit_hash}"]
    if include_binary:
        diff_cmd.append("--binary")

    result = run_git_command(diff_cmd, cwd=ctx.chromium_src)

    if result.returncode != 0:
        raise GitError(f"Failed to get diff for commit {commit_hash}: {result.stderr}")

    # Parse diff into file patches
    file_patches = parse_diff_output(result.stdout)

    if not file_patches:
        log_warning("No changes found in commit")
        return 0

    # Check for existing patches
    if not force and not check_overwrite(ctx, file_patches, verbose):
        return 0

    # Write patches
    return write_patches(ctx, file_patches, verbose, include_binary)


def extract_with_base(
    ctx: BuildContext,
    commit_hash: str,
    base: str,
    verbose: bool,
    force: bool,
    include_binary: bool,
) -> int:
    """Extract patches with custom base (full diff from base for files in commit)"""

    # Step 1: Get list of files changed in the commit
    changed_files = get_commit_changed_files(commit_hash, ctx.chromium_src)

    if not changed_files:
        log_warning(f"No files changed in commit {commit_hash}")
        return 0

    if verbose:
        log_info(f"Files changed in {commit_hash}: {len(changed_files)}")

    # Step 2: For each file, get diff from base to commit
    file_patches = {}

    for file_path in changed_files:
        if verbose:
            log_info(f"  Getting diff for: {file_path}")

        # Get diff for this specific file from base to commit
        diff_cmd = ["git", "diff", f"{base}..{commit_hash}", "--", file_path]
        if include_binary:
            diff_cmd.append("--binary")

        result = run_git_command(diff_cmd, cwd=ctx.chromium_src)

        if result.returncode != 0:
            log_warning(f"Failed to get diff for {file_path}")
            continue

        if result.stdout.strip():
            # Parse this single file's diff
            patches = parse_diff_output(result.stdout)
            # Should only have one file in the result
            if patches:
                file_patches.update(patches)
        else:
            # File might have been added/deleted
            # Check if file exists in base and commit
            base_exists = (
                run_git_command(
                    ["git", "cat-file", "-e", f"{base}:{file_path}"],
                    cwd=ctx.chromium_src,
                ).returncode
                == 0
            )

            commit_exists = (
                run_git_command(
                    ["git", "cat-file", "-e", f"{commit_hash}:{file_path}"],
                    cwd=ctx.chromium_src,
                ).returncode
                == 0
            )

            if not base_exists and commit_exists:
                # File was added - get full content
                diff_cmd = ["git", "diff", f"{base}..{commit_hash}", "--", file_path]
                if include_binary:
                    diff_cmd.append("--binary")
                result = run_git_command(diff_cmd, cwd=ctx.chromium_src)
                if result.stdout.strip():
                    patches = parse_diff_output(result.stdout)
                    if patches:
                        file_patches.update(patches)
            elif base_exists and not commit_exists:
                # File was deleted
                file_patches[file_path] = FilePatch(
                    file_path=file_path,
                    operation=FileOperation.DELETE,
                    patch_content=None,
                    is_binary=False,
                )

    if not file_patches:
        log_warning("No patches to extract")
        return 0

    log_info(f"Extracting {len(file_patches)} patches with base {base}")

    # Check for existing patches
    if not force and not check_overwrite(ctx, file_patches, verbose):
        return 0

    # Write patches
    return write_patches(ctx, file_patches, verbose, include_binary)


def check_overwrite(ctx: BuildContext, file_patches: Dict, verbose: bool) -> bool:
    """Check for existing patches and prompt for overwrite"""
    existing_patches = []
    for file_path in file_patches.keys():
        patch_path = ctx.get_patch_path_for_file(file_path)
        if patch_path.exists():
            existing_patches.append(file_path)

    if existing_patches:
        log_warning(f"Found {len(existing_patches)} existing patches")
        if verbose:
            for path in existing_patches[:5]:
                log_warning(f"  - {path}")
            if len(existing_patches) > 5:
                log_warning(f"  ... and {len(existing_patches) - 5} more")

        if not click.confirm("Overwrite existing patches?", default=False):
            log_info("Extraction cancelled")
            return False
    return True


def write_patches(
    ctx: BuildContext,
    file_patches: Dict[str, FilePatch],
    verbose: bool,
    include_binary: bool,
) -> int:
    """Write patches to disk"""
    success_count = 0
    fail_count = 0
    skip_count = 0

    for file_path, patch in file_patches.items():
        if verbose:
            op_str = patch.operation.value.capitalize()
            log_info(f"Processing ({op_str}): {file_path}")

        # Handle different operations
        if patch.operation == FileOperation.DELETE:
            # Create deletion marker
            if create_deletion_marker(ctx, file_path):
                success_count += 1
            else:
                fail_count += 1

        elif patch.is_binary:
            if include_binary:
                # Create binary marker
                if create_binary_marker(ctx, file_path, patch.operation):
                    success_count += 1
                else:
                    fail_count += 1
            else:
                log_warning(f"  Skipping binary file: {file_path}")
                skip_count += 1

        elif patch.operation == FileOperation.RENAME:
            # Write patch with rename info
            if patch.patch_content:
                # If there are changes beyond the rename
                if write_patch_file(ctx, file_path, patch.patch_content):
                    success_count += 1
                else:
                    fail_count += 1
            else:
                # Pure rename - create marker
                marker_path = ctx.get_dev_patches_dir() / file_path
                marker_path = marker_path.with_suffix(marker_path.suffix + ".rename")
                marker_path.parent.mkdir(parents=True, exist_ok=True)
                try:
                    marker_content = f"Renamed from: {patch.old_path}\nSimilarity: {patch.similarity}%\n"
                    marker_path.write_text(marker_content)
                    log_info(f"  Rename marked: {file_path}")
                    success_count += 1
                except Exception as e:
                    log_error(f"  Failed to mark rename: {e}")
                    fail_count += 1

        else:
            # Normal patch (ADD, MODIFY, COPY)
            if patch.patch_content:
                if write_patch_file(ctx, file_path, patch.patch_content):
                    success_count += 1
                else:
                    fail_count += 1
            else:
                log_warning(f"  No patch content for: {file_path}")
                skip_count += 1

    # Log summary
    log_extraction_summary(file_patches)

    if fail_count > 0:
        log_warning(f"Failed to extract {fail_count} patches")
    if skip_count > 0:
        log_info(f"Skipped {skip_count} files")

    return success_count


def extract_commit_range(
    ctx: BuildContext,
    base_commit: str,
    head_commit: str,
    verbose: bool = False,
    force: bool = False,
    include_binary: bool = False,
    custom_base: Optional[str] = None,
) -> int:
    """Extract patches from a commit range as a single cumulative diff

    Returns:
        Number of patches successfully extracted
    """
    # Step 1: Validate commits
    if not validate_commit_exists(base_commit, ctx.chromium_src):
        raise GitError(f"Base commit not found: {base_commit}")
    if not validate_commit_exists(head_commit, ctx.chromium_src):
        raise GitError(f"Head commit not found: {head_commit}")
    if custom_base and not validate_commit_exists(custom_base, ctx.chromium_src):
        raise GitError(f"Custom base commit not found: {custom_base}")

    # Count commits in range for progress
    result = run_git_command(
        ["git", "rev-list", "--count", f"{base_commit}..{head_commit}"],
        cwd=ctx.chromium_src,
    )
    commit_count = int(result.stdout.strip()) if result.returncode == 0 else 0

    if commit_count == 0:
        log_warning(f"No commits between {base_commit} and {head_commit}")
        return 0

    log_info(f"Processing {commit_count} commits")

    # Step 2: Get diff based on whether we have a custom base
    if custom_base:
        # First get list of files changed in the range
        range_files_cmd = [
            "git",
            "diff",
            "--name-only",
            f"{base_commit}..{head_commit}",
        ]
        result = run_git_command(range_files_cmd, cwd=ctx.chromium_src)

        if result.returncode != 0:
            raise GitError(f"Failed to get changed files: {result.stderr}")

        changed_files = (
            result.stdout.strip().split("\n") if result.stdout.strip() else []
        )

        if not changed_files:
            log_warning("No files changed in range")
            return 0

        log_info(f"Found {len(changed_files)} files changed in range")

        # Now get diff from custom base to head for these files
        diff_cmd = ["git", "diff", f"{custom_base}..{head_commit}"]
        if include_binary:
            diff_cmd.append("--binary")
        # Add the specific files to diff command
        diff_cmd.append("--")
        diff_cmd.extend(changed_files)
    else:
        # Regular diff from base_commit to head_commit
        diff_cmd = ["git", "diff", f"{base_commit}..{head_commit}"]
        if include_binary:
            diff_cmd.append("--binary")

    result = run_git_command(diff_cmd, cwd=ctx.chromium_src, timeout=120)

    if result.returncode != 0:
        raise GitError(f"Failed to get diff for range: {result.stderr}")

    # Step 3-5: Process diff
    file_patches = parse_diff_output(result.stdout)

    if not file_patches:
        log_warning("No changes found in commit range")
        return 0

    # Check for existing patches
    if not force and not check_overwrite(ctx, file_patches, verbose):
        return 0

    success_count = 0
    fail_count = 0
    skip_count = 0

    # Process with progress indicator
    with click.progressbar(
        file_patches.items(),
        label="Extracting patches",
        show_pos=True,
        show_percent=True,
    ) as patches_bar:
        for file_path, patch in patches_bar:
            # Handle different operations
            if patch.operation == FileOperation.DELETE:
                if create_deletion_marker(ctx, file_path):
                    success_count += 1
                else:
                    fail_count += 1

            elif patch.is_binary:
                if include_binary:
                    if create_binary_marker(ctx, file_path, patch.operation):
                        success_count += 1
                    else:
                        fail_count += 1
                else:
                    skip_count += 1

            elif patch.patch_content:
                if write_patch_file(ctx, file_path, patch.patch_content):
                    success_count += 1
                else:
                    fail_count += 1
            else:
                skip_count += 1

    # Step 6: Log summary
    log_extraction_summary(file_patches)

    if fail_count > 0:
        log_warning(f"Failed to extract {fail_count} patches")
    if skip_count > 0:
        log_info(f"Skipped {skip_count} files")

    return success_count


def extract_commits_individually(
    ctx: BuildContext,
    base_commit: str,
    head_commit: str,
    verbose: bool = False,
    force: bool = False,
    include_binary: bool = False,
    custom_base: Optional[str] = None,
) -> int:
    """Extract patches from each commit in a range individually

    This preserves commit boundaries and can help with conflict resolution.

    Returns:
        Total number of patches successfully extracted
    """
    # Validate custom base if provided
    if custom_base and not validate_commit_exists(custom_base, ctx.chromium_src):
        raise GitError(f"Custom base commit not found: {custom_base}")

    # Get list of commits in range
    result = run_git_command(
        ["git", "rev-list", "--reverse", f"{base_commit}..{head_commit}"],
        cwd=ctx.chromium_src,
    )

    if result.returncode != 0:
        raise GitError(f"Failed to list commits: {result.stderr}")

    commits = [c.strip() for c in result.stdout.strip().split("\n") if c.strip()]

    if not commits:
        log_warning(f"No commits between {base_commit} and {head_commit}")
        return 0

    log_info(f"Extracting patches from {len(commits)} commits individually")
    if custom_base:
        log_info(f"Using custom base: {custom_base}")

    total_extracted = 0
    failed_commits = []

    with click.progressbar(
        commits, label="Processing commits", show_pos=True, show_percent=True
    ) as commits_bar:
        for commit in commits_bar:
            try:
                if custom_base:
                    # Use extract_with_base for full diff from custom base
                    extracted = extract_with_base(
                        ctx,
                        commit,
                        custom_base,
                        verbose=False,
                        force=force,
                        include_binary=include_binary,
                    )
                else:
                    # Normal extraction from parent
                    extracted = extract_single_commit(
                        ctx,
                        commit,
                        verbose=False,
                        force=force,
                        include_binary=include_binary,
                    )
                total_extracted += extracted
            except GitError as e:
                failed_commits.append((commit, str(e)))
                if verbose:
                    log_error(f"Failed to extract {commit}: {e}")

    if failed_commits:
        log_warning(f"Failed to extract {len(failed_commits)} commits:")
        for commit, error in failed_commits[:5]:
            log_warning(f"  - {commit[:8]}: {error}")
        if len(failed_commits) > 5:
            log_warning(f"  ... and {len(failed_commits) - 5} more")

    return total_extracted
