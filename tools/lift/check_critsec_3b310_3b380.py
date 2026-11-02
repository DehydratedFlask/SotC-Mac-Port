#!/usr/bin/env python3
"""Byte-oracle for the CritSec shim `0x3b310` (FDE 107 B) and destroy `0x3b380` (FDE 5 B).

RE-02-CRITSEC-3B310-3B380-NATIVE-01. Offline only: reads `local/re/eboot.elf`, never loads, relocates
or executes an original RVA.

What this checker pins, and why:

* **Body identity**: both bodies by sha256 and by FDE length (107 / 5). The 0x3b310 FDE ends at its
  `__stack_chk_fail` call; the trailing nops belong to the next alignment, and 0x3b380 is its own FDE.
* **Mutex kind**: the only attribute set is `scePthreadMutexattrSettype(&attr, 2)`, with the literal 2.
  Type 2 is PTHREAD_MUTEX_RECURSIVE under the FreeBSD-derived PS4 numbering - that mapping is an
  inference, not something these bytes state.
* **No priority inheritance**: the *complete* call census of the shim is exactly five relative calls
  {attrInit, attrSettype, mutexInit, attrDestroy, __stack_chk_fail}. There is no
  `scePthreadMutexattrSetprotocol` and no `Setprioceiling` call, so no PRIO_INHERIT / PRIO_PROTECT
  attribute can be requested. Confirmed against the native source, which must stay free of both.
* **8-byte opaque handle**: the shim performs no store to the caller buffer (it forwards rdi to
  `scePthreadMutexInit`); the width comes from two independent constructors. Slot 535 places the handle
  at 0x2ee14f0 and the next game field at 0x2ee14f8; slot 414 writes its first post-init field at
  base+8. Both are exactly 8 bytes apart.
* **Native wiring**: handle size, mutex kind, the "no priority inherit" flag, the registered-identity
  teardown helpers and their CMake target.
"""

from __future__ import annotations

import hashlib
import importlib.util
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools/lift"))
from slot_pin_util import Elf, digest, require  # noqa: E402
import lane_m5_check as m5  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
CTOR_LEN = 107
DTOR_LEN = 5
CTOR_SHA = "7ff74ae0dae217b10156b1c59b719a95ce6e529f0f794227f173c6aba3df255f"
DTOR_SHA = "de79f6373ce5a48f02b6ea56b3fb54c88d603d639900604b1b51ee6f78e3e565"
FDE = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"

# The complete relative-call census of 0x3b310, in address order.
CTOR_CALLS = (
    (0x3B335, 0xF8F920),   # scePthreadMutexattrInit
    (0x3B342, 0xF8F930),   # scePthreadMutexattrSettype(attr, 2)
    (0x3B354, 0xF8F8D0),   # scePthreadMutexInit(obj, attr, "CritSec")
    (0x3B35C, 0xF8F910),   # scePthreadMutexattrDestroy
    (0x3B375, 0xF8F3A0),   # __stack_chk_fail (canary path only)
)

HDR = ROOT / "native/core/critsec_3b310.hpp"
SRC = ROOT / "native/core/critsec_3b310.cpp"
SLOTS_HDR = ROOT / "native/core/ctor_tiny_misc_slots.hpp"
SLOTS_SRC = ROOT / "native/core/ctor_tiny_misc_slots.cpp"
CMAKE = ROOT / "native/CMakeLists.txt"


def load_fdes():
    spec = importlib.util.spec_from_file_location("cfg_census", ROOT / "tools/lift/cfg_census.py")
    census = importlib.util.module_from_spec(spec)
    sys.modules["cfg_census"] = census
    spec.loader.exec_module(census)
    ranges, _duplicates = census.load_fde_ranges(FDE)
    return ranges


def rel_lea(e: Elf, site: int) -> int:
    """Target of a rip-relative `lea` at `site` (checked opcode shape, then the 4-byte displacement)."""
    return e.lea(site)


def main() -> int:
    raw = (ROOT / "local/re/eboot.elf").read_bytes()
    require(digest(raw) == ELF_SHA256, "unexpected ELF SHA256")

    e = Elf()
    e.recording = True

    # ---- body identity + FDE lengths -------------------------------------------------------
    ctor = e.read(0x3B310, CTOR_LEN)
    dtor = e.read(0x3B380, DTOR_LEN)
    require(digest(ctor) == CTOR_SHA, "0x3b310 body sha256 (107 B)")
    require(digest(dtor) == DTOR_SHA, "0x3b380 body sha256 (5 B)")
    fdes = load_fdes()
    for start, length, what in ((0x3B310, CTOR_LEN, "0x3b310"), (0x3B380, DTOR_LEN, "0x3b380")):
        found = fdes.find(start)
        require(found is not None and found[0] == start, f"{what} is not an FDE start")
        require(found[1] == length, f"{what} FDE length {found[1]:#x} != {length:#x}")

    # ---- imports, settype constant, debug name, destroy tail ---------------------------------
    m5.critsec_extra(e)
    for site, target in CTOR_CALLS:
        require(e.call(site) == target, f"0x{site:x} call target")
    require(e.jmp(0x3B380) == 0xF8F8C0, "0x3b380 tail-jumps to scePthreadMutexDestroy")

    # ---- no priority-inherit attribute can be requested --------------------------------------
    # The census above is complete: a raw scan finds exactly those five `e8` opcodes in the body, so a
    # sixth call (Setprotocol / Setprioceiling) cannot hide anywhere.
    raw_e8 = [0x3B310 + i for i, byte in enumerate(ctor) if byte == 0xE8]
    require(raw_e8 == [site for site, _ in CTOR_CALLS],
            f"0x3b310 call census changed: {[hex(x) for x in raw_e8]}")
    attr_calls = {target for _site, target in CTOR_CALLS}
    require(0xF8F940 not in attr_calls and 0xF8F950 not in attr_calls,
            "an attribute-protocol import appeared in the call census")

    # ---- 8-byte handle: two independent constructors agree ------------------------------------
    # slot 535 (0x138e20): handle cell 0x2ee14f0, next game field 0x2ee14f8 -> 8 bytes.
    require(rel_lea(e, 0x138E27) == 0x2EE14F0, "slot 535 passes the handle cell 0x2ee14f0 to 0x3b310")
    require(rel_lea(e, 0x138E53) == 0x2EE14F8, "slot 535's object starts at 0x2ee14f8")
    require(0x2EE14F8 - 0x2EE14F0 == 8, "slot 535 handle width")
    # slot 414 (0x57b9b0): handle at 0x2f35898, first zeroed field at base+8.
    require(rel_lea(e, 0x57B9B6) == 0x2F35898, "slot 414 passes 0x2f35898 to 0x3b310")
    require(rel_lea(e, 0x57B9DB) == 0x2F358A0, "slot 414's first post-init field is base+8")
    require(0x2F358A0 - 0x2F35898 == 8, "slot 414 handle width")
    e.bytes_at(0x57B9E6, "c70000000000", "slot 414 movl $0, (rax) at base+8")
    e.bytes_at(0x57B9EC, "c5f8114008", "slot 414 vmovups xmm0, 8(rax) at base+0x10")

    # ---- native contract ---------------------------------------------------------------------
    hdr = HDR.read_text()
    src = SRC.read_text()
    slots_hdr = SLOTS_HDR.read_text()
    slots_src = SLOTS_SRC.read_text()
    cmake = CMAKE.read_text()

    require("0x3b310" in hdr and "0x3b380" in hdr, "header missing RVA constants")
    require(re.search(r"kCritSecHandleBytes\s*=\s*8", hdr), "handle size must be 8")
    require(re.search(r"kCritSecCtorFdeLength\s*=\s*107", hdr), "ctor FDE length pin")
    require(re.search(r"kCritSecDtorFdeLength\s*=\s*5", hdr), "dtor FDE length pin")
    require(re.search(r"kCritSecSettypeValue\s*=\s*2", hdr), "settype constant pin")
    require(re.search(r"kCritSecUsesPriorityInheritance\s*=\s*false", hdr),
            "header must state that CritSec uses no priority inheritance")
    require("PTHREAD_MUTEX_RECURSIVE" in src, "critsec must use a plain recursive mutex")
    for forbidden in ("PTHREAD_PRIO_INHERIT", "PTHREAD_PRIO_PROTECT", "pthread_mutexattr_setprotocol",
                      "pthread_mutexattr_setprioceiling"):
        require(forbidden not in src, f"critsec must not request {forbidden}")
    require("sidecar" in hdr.lower() and "sidecar" in src.lower(), "sidecar layout documented")

    # Registered-destroy teardown for slots 414 / 535.
    for needle in ("teardown_slot414_57b9b0", "teardown_slot535_138e20"):
        require(needle in slots_hdr and needle in slots_src, f"missing teardown {needle}")
    require("default_critsec_slot414_hooks" in src, "default Slot414 hooks missing")
    require('"mutex_destroy_hook"' in slots_src and '"atexit_identity"' in slots_src and
            '"mutex_destroy"' in slots_src, "teardown stages must stay named")
    for needle in ("core/critsec_3b310.cpp", "tests/test_critsec_slots_teardown.cpp",
                   "test_critsec_slots_teardown"):
        require(needle in cmake, f"cmake missing {needle}")

    print("OK check_critsec_3b310_3b380: 0x3b310 (107 B) / 0x3b380 (5 B) bodies, five-call census, "
          "settype 2, no attribute-protocol import, 8-byte handle from both slots, teardown wiring")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
