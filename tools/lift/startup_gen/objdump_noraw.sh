#!/bin/bash
args=()
for a in "$@"; do [ "$a" = "--show-raw-insn" ] || args+=("$a"); done
exec /usr/bin/objdump "${args[@]}"
