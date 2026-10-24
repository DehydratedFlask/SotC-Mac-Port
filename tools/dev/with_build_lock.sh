#!/bin/sh
# Serialize access to the shared build tree and native/CMakeLists.txt between concurrent agents.
#
# Usage:
#   tools/dev/with_build_lock.sh <command> [args...]
#
# The lock is an flock on $SOTC_BUILD_LOCK (default /tmp/sotc-native-build.lock). It is held for the
# whole command and released automatically when the process exits, including on failure. Use it for
# any cmake configure/build, ctest run, or read-modify-write of native/CMakeLists.txt. This is a
# coordination tool only: it changes no source and claims nothing about the build.
set -eu
exec python3 -c '
import fcntl, os, subprocess, sys
path = os.environ.get("SOTC_BUILD_LOCK", "/tmp/sotc-native-build.lock")
with open(path, "a+") as handle:
    fcntl.flock(handle, fcntl.LOCK_EX)
    raise SystemExit(subprocess.call(sys.argv[1:]))
' "$@"
