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

# Validate the optional input before building or replacing an existing app bundle.
if [ -n "$CORE" ]; then
  [ -f "$CORE" ] || { echo "missing core executable: $CORE" >&2; exit 1; }
  [ -x "$CORE" ] || { echo "core is not executable: $CORE" >&2; exit 1; }
  CORE_ARCHS="$(lipo -archs "$CORE" 2>/dev/null || true)"
  [ "$CORE_ARCHS" = "arm64" ] || { echo "core architectures must be exactly arm64, got: ${CORE_ARCHS:-<none>}" >&2; exit 1; }
  CORE_MAGIC="$(head -c 4 "$CORE" | xxd -p)"
  [ "$CORE_MAGIC" = "cffaedfe" ] || [ "$CORE_MAGIC" = "feedfacf" ] || {
    echo "core is not a thin Mach-O executable (magic $CORE_MAGIC)" >&2; exit 1; }
  CORE_HEADER="$(otool -hv "$CORE")"
  printf '%s\n' "$CORE_HEADER" | awk '$1 == "MH_MAGIC_64" && $2 == "ARM64" && $5 == "EXECUTE" { valid = 1 } END { exit !valid }' || {
    echo "core must be a thin ARM64 MH_EXECUTE Mach-O (refusing libraries/objects)" >&2; exit 1; }
  # Refuse a core that links anything outside the OS (e.g. the ASan runtime, found through an absolute
  # Xcode rpath): such a bundle only starts on the build machine and is not a distributable product.
  CORE_DEPS="$(otool -L "$CORE")"
  EXTRA_DEPS="$(printf '%s\n' "$CORE_DEPS" | awk 'NR > 1 && $1 !~ /^\/(usr\/lib\/|System\/Library\/)/ { print $1 }')"
  [ -z "$EXTRA_DEPS" ] || {
    echo "core links non-system libraries (sanitizer build?): $EXTRA_DEPS" >&2
    echo "build a release core: cmake -DSOTC_SANITIZE=OFF -DCMAKE_BUILD_TYPE=Release" >&2
    exit 1; }
fi

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
  echo "core bundled: $APP/Contents/MacOS/SoCGameCore (engine readiness is queried by the launcher)"
else
  echo "core not bundled: the launcher will report NOT BUILT"
fi
