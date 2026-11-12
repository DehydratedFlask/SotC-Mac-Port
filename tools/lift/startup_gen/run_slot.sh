#!/bin/zsh
# usage: run_slot.sh <slot> <rva-hex> <menu|nomenu> <prev-stem> <prev-n>   (generate, wire, build, test, check, tamper)
set -e
cd "$(dirname "$0")/../../.."
n=$1; menu=$3; prev=$4; pn=$5
python3 -I tools/lift/startup_gen/mk_$n.py
rva=0x$(basename $(ls native/core/ctor_slot_${n}_*.cpp) .cpp | sed 's/.*_//')
stem=$(basename native/core/ctor_slot_${n}_*.cpp .cpp)
python3 -I - <<PY
import sys
sys.path.insert(0,'tools/lift/startup_gen')
import wire
wire.wire($n,'$stem',$rva,after_stem='$prev',after_n=$pn,cmake_after='$prev',menu=('$menu'=='menu'))
PY
(cmake -S native -B ${BUILD:-local/build/startup-b12} >/dev/null; cmake --build ${BUILD:-local/build/startup-b12} -j 12 --target test_ctor_slot_$n 2>&1 | grep -E "error|warning" -A6 | head -40) || true
./${BUILD:-local/build/startup-b12}/test_ctor_slot_$n | tail -3
python3 -I tools/lift/check_ctor_slot_$n.py | tail -2
for t in "--tamper elf" "--tamper native"; do python3 -I tools/lift/check_ctor_slot_$n.py ${=t} >/dev/null 2>&1 && echo "TAMPER $t NOT REJECTED" || echo "tamper $t rejected"; done
