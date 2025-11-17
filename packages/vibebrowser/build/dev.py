#!/usr/bin/env python3
"""
Dev CLI - Chromium patch management tool

A git-like patch management system for maintaining patches against Chromium.
Enables extracting, applying, and managing patches across Chromium upgrades.
"""

import click
import os
import sys
import yaml
from pathlib import Path
from typing import Optional, Dict, Any
from dataclasses import dataclass

# Add build directory to path for imports
sys.path.insert(0, str(Path(__file__).parent))

from context import BuildContext
from utils import log_info, log_error, log_success, log_warning, join_paths


@dataclass
class DevCliConfig:
    """Configuration for Dev CLI from various sources"""

    chromium_src: Optional[Path] = None
    auto_commit: bool = False
    interactive: bool = True

    @classmethod
    def load(cls, cli_chromium_src: Optional[Path] = None) -> "DevCliConfig":
        """Load configuration from various sources with precedence:
        1. CLI arguments (highest priority)
        2. Environment variables
        3. Config file
        4. Defaults (lowest priority)
        """
        config = cls()

        # Load from config file if exists
        config_file = Path.cwd() / ".dev-cli.yaml"
        if config_file.exists():
            try:
                with open(config_file, "r") as f:
                    file_config = yaml.safe_load(f)
                    if file_config and "defaults" in file_config:
                        defaults = file_config["defaults"]
                        if "chromium_src" in defaults:
                            config.chromium_src = Path(defaults["chromium_src"])
                        config.auto_commit = defaults.get("auto_commit", False)
                        config.interactive = defaults.get("interactive", True)
            except Exception as e:
                log_warning(f"Failed to load config file: {e}")

        # Override with environment variables
        if "DEV_CLI_CHROMIUM_SRC" in os.environ:
            config.chromium_src = Path(os.environ["DEV_CLI_CHROMIUM_SRC"])
        if "DEV_CLI_AUTO_COMMIT" in os.environ:
            config.auto_commit = os.environ["DEV_CLI_AUTO_COMMIT"].lower() in (
                "true",
                "1",
                "yes",
            )
        if "DEV_CLI_INTERACTIVE" in os.environ:
            config.interactive = os.environ["DEV_CLI_INTERACTIVE"].lower() in (
                "true",
                "1",
                "yes",
            )

        # Override with CLI arguments (highest priority)
        if cli_chromium_src:
            config.chromium_src = cli_chromium_src

        return config


def create_build_context(chromium_src: Optional[Path] = None) -> Optional[BuildContext]:
    """Create BuildContext with dev CLI extensions"""
    config = DevCliConfig.load(chromium_src)

    if not config.chromium_src:
        log_error("Chromium source directory not specified")
        log_info("Use --chromium-src or set DEV_CLI_CHROMIUM_SRC environment variable")
        return None

    if not config.chromium_src.exists():
        log_error(f"Chromium source directory does not exist: {config.chromium_src}")
        return None

    # For dev CLI, we just need it to be a git repository
    # Don't enforce strict Chromium structure
    if not (config.chromium_src / ".git").exists():
        log_warning(f"Warning: Not a git repository: {config.chromium_src}")
        # Continue anyway - patches might still work

    try:
        ctx = BuildContext(
            root_dir=Path.cwd(),
            chromium_src=config.chromium_src,
            architecture="",  # Not needed for patch operations
            build_type="debug",  # Not needed for patch operations
        )

        # Store config in context for access by commands
        ctx.dev_config = config

        return ctx
    except Exception as e:
        log_error(f"Failed to create build context: {e}")
        return None


@click.group()
@click.option(
    "--chromium-src",
    "-S",
    type=click.Path(exists=True, path_type=Path),
    help="Path to Chromium source directory",
)
@click.option("--verbose", "-v", is_flag=True, help="Enable verbose output")
@click.option("--quiet", "-q", is_flag=True, help="Suppress non-essential output")
@click.pass_context
def cli(ctx, chromium_src, verbose, quiet):
    """Dev CLI - Chromium patch management tool

    This tool provides git-like commands for managing patches against Chromium:

    \b
    Extract patches from commits:
      dev extract commit HEAD
      dev extract range HEAD~5 HEAD

    \b
    Apply patches:
      dev apply all
      dev apply feature llm-chat

    \b
    Manage features:
      dev feature list
      dev feature add my-feature HEAD
      dev feature show my-feature
    """
    # Store options in context for subcommands
    ctx.ensure_object(dict)
    ctx.obj["chromium_src"] = chromium_src
    ctx.obj["verbose"] = verbose
    ctx.obj["quiet"] = quiet


# Import and register subcommand groups
# These will be created in the next step
try:
    from modules.dev_cli import extract, apply, feature

    cli.add_command(extract.extract_group)
    cli.add_command(apply.apply_group)
    cli.add_command(feature.feature_group)
except ImportError as e:
    # During initial setup, modules might not exist yet
    log_warning(f"Some modules not yet available: {e}")

    # Add placeholder commands for testing
    @cli.command()
    @click.pass_context
    def status(ctx):
        """Show dev CLI status"""
        log_info("Dev CLI Status")
        log_info("-" * 40)

        build_ctx = create_build_context(ctx.obj.get("chromium_src"))
        if build_ctx:
            log_success(f"Chromium source: {build_ctx.chromium_src}")

            # Check for patches directory
            patches_dir = build_ctx.root_dir / "chromium_src"
            if patches_dir.exists():
                patch_count = len(list(patches_dir.rglob("*.patch")))
                log_info(f"Individual patches: {patch_count}")
            else:
                log_warning("No patches directory found")

            # Check for features.yaml
            features_file = build_ctx.root_dir / "features.yaml"
            if features_file.exists():
                with open(features_file) as f:
                    features = yaml.safe_load(f)
                    feature_count = len(features.get("features", {}))
                    log_info(f"Features defined: {feature_count}")
            else:
                log_warning("No features.yaml found")
        else:
            log_error("Failed to create build context")


def main():
    """Main entry point"""
    try:
        cli()
    except KeyboardInterrupt:
        log_warning("\nInterrupted by user")
        sys.exit(1)
    except Exception as e:
        if "--verbose" in sys.argv or "-v" in sys.argv:
            import traceback

            traceback.print_exc()
        else:
            log_error(f"Error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
