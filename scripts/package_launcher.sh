#!/bin/bash
# Assemble "SotC Launcher.app" (launcher shell; the native core is optional).
# Usage: scripts/package_launcher.sh <out-dir> [core-executable]
#
# With a core path, the Mach-O arm64 `SoCGameCore` is copied to Contents/MacOS/SoCGameCore so the
# launcher's CoreLocator can find it. Without one, the bundle stays a launcher-only shell and
# reports NOT BUILT. Packaging only: this does not demonstrate G4, boot, audio or save. The core
# still reports engine_implemented=false and refuses start until a real engine exists.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="${1:?usage: package_launcher.sh <out-dir> [core-executable]}"
CORE="${2:-}"
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
if [ -n "$CORE" ]; then
  [ -f "$CORE" ] || { echo "missing core executable: $CORE" >&2; exit 1; }
  [ -x "$CORE" ] || { echo "core is not executable: $CORE" >&2; exit 1; }
  CORE_ARCHS="$(lipo -archs "$CORE" 2>/dev/null || true)"
  [ "$CORE_ARCHS" = "arm64" ] || { echo "core architectures must be exactly arm64, got: ${CORE_ARCHS:-<none>}" >&2; exit 1; }
  CORE_MAGIC="$(head -c 4 "$CORE" | xxd -p)"
  [ "$CORE_MAGIC" = "cffaedfe" ] || [ "$CORE_MAGIC" = "feedfacf" ] || {
    echo "core is not a thin Mach-O executable (magic $CORE_MAGIC)" >&2; exit 1; }
  cp "$CORE" "$APP/Contents/MacOS/SoCGameCore"
  chmod +x "$APP/Contents/MacOS/SoCGameCore"
fi
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
if [ -n "$CORE" ]; then
  codesign --force --sign - "$APP/Contents/MacOS/SoCGameCore"
  codesign --verify --strict --verbose=2 "$APP/Contents/MacOS/SoCGameCore"
fi
codesign --force --sign - "$APP"
codesign --verify --strict --verbose=2 "$APP"
echo "packaged: $APP"
if [ -n "$CORE" ]; then
  echo "core bundled: $APP/Contents/MacOS/SoCGameCore (engine_implemented=false; Play stays disabled)"
else
  echo "core not bundled: the launcher will report NOT BUILT"
fi
