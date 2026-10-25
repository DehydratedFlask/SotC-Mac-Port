#!/bin/sh
# Run every ELF-tie checker under tools/lift/ and report a single pass/fail summary.
#
# Requires local/re/eboot.elf (private). Checkers that need a build directory read
# SOTC_NATIVE_BUILD; the ones we ship do not require it, so an unset value is fine.
#
# Usage:
#   tools/lift/run_all_checkers.sh            # all check_*.py
#   SOTC_NATIVE_BUILD=local/build/x tools/lift/run_all_checkers.sh
set -eu

ROOT=$(cd "$(dirname "$0")/../.." && pwd)
cd "$ROOT"

if [ ! -f local/re/eboot.elf ]; then
    echo "SKIP: local/re/eboot.elf is absent (private game image required)" >&2
    exit 2
fi

fail=0
ok=0
skip=0
for checker in tools/lift/check_*.py; do
    case "$checker" in
        *_all*|*util*) continue ;;
    esac
    if python3 -I "$checker" >/tmp/checker.$$ 2>&1; then
        ok=$((ok + 1))
    elif grep -q "not built in" /tmp/checker.$$; then
        # Environmental: the checker wants a build directory it was written against.
        # Run with SOTC_NATIVE_BUILD=<dir> and a matching build to execute it.
        skip=$((skip + 1))
    else
        echo "FAILED: $checker" >&2
        sed 's/^/    /' /tmp/checker.$$ >&2
        fail=$((fail + 1))
    fi
done
rm -f /tmp/checker.$$

echo "checkers: $ok passed, $skip skipped (need a build dir), $fail failed"
[ "$fail" -eq 0 ]
