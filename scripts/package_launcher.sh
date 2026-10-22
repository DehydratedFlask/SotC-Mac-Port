#!/bin/bash
# Assemble "SotC Launcher.app" (launcher shell only; no game core, no game assets).
# Usage: scripts/package_launcher.sh <out-dir>
# Packaging only: this does not demonstrate G4 or any game boot/audio/save.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="${1:?usage: package_launcher.sh <out-dir>}"
APP="$OUT/SotC Launcher.app"
PLIST="$ROOT/launcher/Packaging/Info.plist"
PRODUCT=SoCLauncher

plutil -lint "$PLIST" >/dev/null

swift build --package-path "$ROOT/launcher" -c release --arch arm64 --product "$PRODUCT"
BIN_DIR="$(swift build --package-path "$ROOT/launcher" -c release --arch arm64 --show-bin-path)"
BIN="$BIN_DIR/$PRODUCT"
[ -f "$BIN" ] || { echo "missing built product: $BIN" >&2; exit 1; }

ARCHS="$(lipo -archs "$BIN")"
[ "$ARCHS" = "arm64" ] || { echo "binary architectures must be exactly arm64, got: $ARCHS" >&2; exit 1; }

rm -rf "$APP"
mkdir -p "$APP/Contents/MacOS"
cp "$PLIST" "$APP/Contents/Info.plist"
cp "$BIN" "$APP/Contents/MacOS/$PRODUCT"
printf 'APPL????' > "$APP/Contents/PkgInfo"

# Refuse original-game artifacts anywhere in the bundle (by name and by magic).
bad=0
while IFS= read -r -d '' f; do
  base="$(basename "$f")"; lower="$(printf '%s' "$base" | tr '[:upper:]' '[:lower:]')"
  case "$lower" in
    *eboot*|*.sprx|*.self|*.elf|*.psarc|*.pkg|*.pfs|*.prx|*.bin|*.at9) echo "forbidden file in bundle: $f" >&2; bad=1 ;;
  esac
  magic="$(head -c 4 "$f" | xxd -p)"
  case "$magic" in
    7f454c46|4f153d1d|50534152|7f504b47) echo "forbidden magic ($magic) in bundle: $f" >&2; bad=1 ;; # ELF, SELF, PSAR, PKG
  esac
done < <(find "$APP" -type f -print0)
[ "$bad" = 0 ] || { rm -rf "$APP"; exit 1; }

# Ad-hoc signature, no entitlements.
codesign --force --sign - "$APP"
codesign --verify --strict --verbose=2 "$APP"
echo "packaged: $APP"
