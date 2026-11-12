#!/bin/sh
# Install TokenEconomizer for Freebuff in this workspace.
#
# Clones the pinned upstream checkout into the private, gitignored `local/tools/`, links the
# Freebuff adapter into it, and sets this repository's plugin settings. Safe to re-run: an
# existing checkout is kept, and its commit is checked rather than replaced.
#
#   tools/tokeneconomizer/install.sh          install (or refresh the links/settings)
#   tools/tokeneconomizer/install.sh --check  report state, change nothing
set -eu
PIN=264e7a7f32f0112a06aa83a97b9c2c4681563968
REPO=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
ADAPTER_DIR="$REPO/tools/tokeneconomizer"
DEST="$REPO/local/tools/TokenEconomizer"
CHECK=${1:-}

note() { printf '%s\n' "$*"; }

if [ ! -d "$DEST/.git" ]; then
  if [ "$CHECK" = "--check" ]; then
    note "checkout: MISSING ($DEST)"
    exit 1
  fi
  mkdir -p "$REPO/local/tools"
  note "cloning microsoft/TokenEconomizer into $DEST"
  git clone --quiet https://github.com/microsoft/TokenEconomizer.git "$DEST"
  git -C "$DEST" checkout --quiet "$PIN"
else
  have=$(git -C "$DEST" rev-parse HEAD)
  note "checkout: $DEST at $have"
  if [ "$have" != "$PIN" ] && [ "$CHECK" != "--check" ]; then
    note "note: this checkout is not the pinned commit ($PIN); leaving it as it is"
  fi
fi

for f in freebuff_host.py sitecustomize.py; do
  if [ -e "$DEST/$f" ] && [ ! -L "$DEST/$f" ]; then
    note "link: $DEST/$f exists and is not a link -- left alone"
    continue
  fi
  [ "$CHECK" = "--check" ] && { note "link: $DEST/$f $( [ -L "$DEST/$f" ] && echo ok || echo missing )"; continue; }
  ln -sfn "$ADAPTER_DIR/$f" "$DEST/$f"
  note "link: $DEST/$f -> $ADAPTER_DIR/$f"
done

if [ "$CHECK" = "--check" ]; then
  note "settings: $("$ADAPTER_DIR/te-freebuff" config 2>&1 | head -20)"
  exit 0
fi

# This repository: the model-free fact learner, and the mechanisms that need a host CLI off.
# (Freebuff's tool set is not configurable, and the loop guard's judge is a model call.)
for setting in microskills=envfacts toolprune=off loopguard=off; do
  "$ADAPTER_DIR/te-freebuff" config set "$setting" >/dev/null 2>&1 \
    || note "warning: could not set $setting (run tools/tokeneconomizer/te-freebuff config)"
done

note ""
note "done. In a Freebuff session, from the repository root:"
note "  tools/tokeneconomizer/te-freebuff start"
note "  tools/tokeneconomizer/te-freebuff run -- <command>"
note "  tools/tokeneconomizer/te-freebuff end"
