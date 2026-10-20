#!/bin/sh
# Offline x86-64 disassembly of a raw byte dump (analysis aid only; output never executed).
# usage: disasm_range.sh dump.bin 0xVADDR   (work files go next to the dump)
set -eu
bin=$1; addr=$2
OD=${LLVM_OBJDUMP:-$(xcrun --find llvm-objdump)}
base=${bin%.bin}
printf '.text\n.incbin "%s"\n' "$(basename "$bin")" > "$base.s"
(cd "$(dirname "$bin")" && clang -target x86_64-unknown-linux-gnu -c "$(basename "$base").s" -o "$(basename "$base").o")
"$OD" -d -M intel --adjust-vma="$addr" "$base.o" | tail -n +7
