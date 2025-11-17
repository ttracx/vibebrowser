"""
Feature module - Manage feature-to-file mappings

Simple feature management with YAML persistence.
"""

import click
import yaml
from pathlib import Path
from typing import Dict, List
from context import BuildContext
from modules.dev_cli.utils import get_commit_changed_files, run_git_command
from utils import log_info, log_error, log_success, log_warning


@click.group(name="feature")
def feature_group():
    """Manage feature-to-file mappings"""
    pass


@feature_group.command(name="add")
@click.argument("feature_name")
@click.argument("commit")
@click.option("--description", "-d", help="Description of the feature")
@click.pass_context
def add_feature(ctx, feature_name, commit, description):
    """Add files from a commit to a feature

    \b
    Examples:
      dev feature add llm-chat HEAD
      dev feature add my-feature abc123 -d "My new feature"
    """
    chromium_src = ctx.parent.obj.get("chromium_src")

    from dev import create_build_context

    build_ctx = create_build_context(chromium_src)
    if not build_ctx:
        return

    # Get changed files from commit
    changed_files = get_commit_changed_files(commit, build_ctx.chromium_src)

    if not changed_files:
        log_error(f"No files changed in commit {commit}")
        ctx.exit(1)

    # Load or create features.yaml
    features_path = build_ctx.get_features_yaml_path()

    if features_path.exists():
        with open(features_path) as f:
            data = yaml.safe_load(f) or {}
    else:
        data = {"version": "1.0", "features": {}}

    features = data.get("features", {})

    # Add or update feature
    if feature_name in features:
        existing_files = set(features[feature_name].get("files", []))
        all_files = list(existing_files | set(changed_files))
        features[feature_name]["files"] = sorted(all_files)
        log_info(f"Updated feature '{feature_name}' ({len(all_files)} files total)")
    else:
        features[feature_name] = {
            "description": description or f"Feature from commit {commit[:8]}",
            "files": sorted(changed_files),
        }
        log_info(f"Created feature '{feature_name}' with {len(changed_files)} files")

    # Save back
    data["features"] = features
    with open(features_path, "w") as f:
        yaml.dump(data, f, default_flow_style=False, sort_keys=False)

    log_success(f"Feature '{feature_name}' saved")


@feature_group.command(name="list")
@click.pass_context
def list_features(ctx):
    """List all features"""
    # Use current directory's features.yaml
    features_path = Path.cwd() / "features.yaml"

    if not features_path.exists():
        log_warning("No features defined (features.yaml not found)")
        return

    with open(features_path) as f:
        data = yaml.safe_load(f) or {}

    features = data.get("features", {})

    if not features:
        log_warning("No features defined")
        return

    log_info("Features:")
    for name, info in features.items():
        file_count = len(info.get("files", []))
        description = info.get("description", "No description")
        log_info(f"  {name} ({file_count} files) - {description}")


@feature_group.command(name="show")
@click.argument("feature_name")
@click.pass_context
def show_feature(ctx, feature_name):
    """Show details of a specific feature"""
    features_path = Path.cwd() / "features.yaml"

    if not features_path.exists():
        log_error("No features.yaml found")
        ctx.exit(1)

    with open(features_path) as f:
        data = yaml.safe_load(f)

    features = data.get("features", {})

    if feature_name not in features:
        log_error(f"Feature '{feature_name}' not found")
        ctx.exit(1)

    info = features[feature_name]
    files = info.get("files", [])

    log_info(f"Feature: {feature_name}")
    log_info(f"Description: {info.get('description', 'No description')}")
    log_info(f"Files ({len(files)}):")

    for file_path in files:
        log_info(f"  - {file_path}")


@feature_group.command(name="generate-patch")
@click.argument("feature_name")
@click.option("--output", "-o", type=click.Path(), help="Output file path")
@click.pass_context
def generate_patch(ctx, feature_name, output):
    """Generate combined patch for a feature

    \b
    Examples:
      dev feature generate-patch llm-chat
      dev feature generate-patch my-feature -o combined.patch
    """
    # Load feature
    features_path = Path.cwd() / "features.yaml"

    if not features_path.exists():
        log_error("No features.yaml found")
        ctx.exit(1)

    with open(features_path) as f:
        data = yaml.safe_load(f)

    features = data.get("features", {})

    if feature_name not in features:
        log_error(f"Feature '{feature_name}' not found")
        ctx.exit(1)

    file_list = features[feature_name].get("files", [])

    if not file_list:
        log_error(f"Feature '{feature_name}' has no files")
        ctx.exit(1)

    # Find patches directory
    patches_dir = Path.cwd() / "chromium_src"
    if not patches_dir.exists():
        log_error(f"Patches directory not found: {patches_dir}")
        ctx.exit(1)

    # Collect all patches
    combined_patches = []
    missing = []

    for file_path in file_list:
        patch_path = patches_dir / f"{file_path}.patch"

        if patch_path.exists():
            with open(patch_path) as f:
                combined_patches.append(f.read())
        else:
            missing.append(file_path)

    if missing:
        log_warning(f"Missing patches for {len(missing)} files:")
        for m in missing[:5]:
            log_warning(f"  - {m}")
        if len(missing) > 5:
            log_warning(f"  ... and {len(missing) - 5} more")

    if not combined_patches:
        log_error("No patches found to combine")
        ctx.exit(1)

    # Create combined patch with headers
    header = f"# Combined patch for feature: {feature_name}\n"
    header += f"# Files: {len(file_list)}\n"
    header += f"# Description: {features[feature_name].get('description', 'No description')}\n\n"

    combined = header + "\n".join(combined_patches)

    # Write output
    if output:
        output_path = Path(output)
        output_path.write_text(combined)
        log_success(f"Generated patch: {output_path}")
    else:
        # Output to stdout
        click.echo(combined)


@feature_group.command(name="remove")
@click.argument("feature_name")
@click.pass_context
def remove_feature(ctx, feature_name):
    """Remove a feature"""
    features_path = Path.cwd() / "features.yaml"

    if not features_path.exists():
        log_error("No features.yaml found")
        ctx.exit(1)

    with open(features_path) as f:
        data = yaml.safe_load(f)

    features = data.get("features", {})

    if feature_name not in features:
        log_error(f"Feature '{feature_name}' not found")
        ctx.exit(1)

    # Remove and save
    del features[feature_name]
    data["features"] = features

    with open(features_path, "w") as f:
        yaml.dump(data, f, default_flow_style=False, sort_keys=False)

    log_success(f"Removed feature '{feature_name}'")
