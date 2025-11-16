#!/usr/bin/env bash
set -euo pipefail

DIR="packages/VibeBrowser-agent"
BRANCH="${1:-main}"

git -C "$DIR" fetch origin "$BRANCH" --tags
git -C "$DIR" checkout -q "$BRANCH"
git -C "$DIR" pull -q --ff-only origin "$BRANCH"

NEW_SHA=$(git -C "$DIR" rev-parse --short HEAD)
git add "$DIR"
git commit -m "chore(agent): bump submodule to $NEW_SHA" || { echo "No changes"; exit 0; }
echo "Bumped $DIR to $NEW_SHA"
EOF

chmod +x scripts/submodule-update.sh
