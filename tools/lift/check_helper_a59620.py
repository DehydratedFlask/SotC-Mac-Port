#!/usr/bin/env python3
"""ELF-tie checker for the id + string entry constructor 0xa59620 (CPU-01-AOT-CFG-SCALE-01-BATCH19).

Pins instruction by instruction: 0xa59620 (249 B, FDE 249): the 0xf180 empty-string initialiser on entry + 8, the id store, the alias skip
(`cmp r15, rbx`), the 0xf590 release, the 32 B view copy with `lock inc qword [ctl]`, and the inline doubling push onto the process vector V1
(begin 0x32f8dc8, end 0x32f8dd0, cap 0x32f8dd8: 0x17090 allocate, memmove, 0x170c0 free). memmove is NID-proven. The whole image is scanned for
E8 callers: exactly 37, all inside startup slot 428 (0x52ec10, hash-pinned).

NOT established: what the id / string pairs mean; the exit-time teardown of V1; that the native body is behaviourally equal (the fixture test checks the
modelled semantics).

Usage: python3 -I tools/lift/check_helper_a59620.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402
import helper_callee_lib as lib  # noqa: E402
import helper_callee_pins_g as pins_g  # noqa: E402
from slot_pin_util import require  # noqa: E402

lib.pins.PINS.update(pins_g.PINS)

SLOT_428 = (0x52EC10, 5375, "0cab3c7a2c7d45e3570f51adc3bdb5c898b030ce8f998699380fce0c56f3e828")
TARGET = 0xA59620


def callers(e, target: int) -> list[int]:
    image = e.raw(0, 0xF90000)
    found, i = [], 0
    while True:
        i = image.find(b"\xe8", i)
        if i < 0:
            return found
        if i + 5 <= len(image) and i + 5 + struct.unpack("<i", image[i + 1:i + 5])[0] == target:
            found.append(i)
        i += 1


def extra(e, fams) -> None:
    bp._bp_check_imports(e, ((0xF8F480, 0x1600030, "memmove"),))
    base, size, sha = SLOT_428
    fde = lib.fde_ranges()
    if fde is not None:
        require(fde.get(hex(base)) == size, "slot 428: FDE length")
    e.body_hash("slot 428 (0x52ec10)", base, size, sha, always=True)
    found = callers(e, TARGET)
    require(len(found) == 37, f"0xa59620 has exactly 37 E8 callers in the image, got {len(found)}")
    require(all(base <= c < base + size for c in found), "every caller of 0xa59620 lies inside slot 428")


SPEC = {
    "name": "check_helper_a59620",
    "summary": ("0xa59620 (249 B) pinned per instruction (f180 / id store / alias skip / f590 / 32 B copy with lock inc / doubling push on V1 0x32f8dc8); "
                "memmove NID-proven; 37 callers, all inside the hash-pinned slot 428"),
    "families": [],
    "bodies": [
        {"name": "0xa59620 id entry ctor", "rva": 0xA59620, "size": 249, "fde": 249, "pins": "id_entry_a59620",
         "sha": "82dd6511409f2b520222f6e177858e67965de5074bb1ffee3e10bd0a0c59195f"},
    ],
    "rips": [
        ("push: end load", 0xA59677, 7, 3, 0x32F8DD0), ("push: cap compare", 0xA5967E, 7, 3, 0x32F8DD8),
        ("push: end store", 0xA5968B, 7, 3, 0x32F8DD0), ("grow: begin load", 0xA59697, 7, 3, 0x32F8DC8),
        ("grow: begin store", 0xA596F5, 7, 3, 0x32F8DC8), ("grow: end store", 0xA596FC, 7, 3, 0x32F8DD0),
        ("grow: cap store", 0xA59703, 7, 3, 0x32F8DD8),
    ],
    "calls": [
        ("empty-string init 0xf180", 0xA5963E, 0xF180), ("inert release 0xf590", 0xA5964E, 0xF590),
        ("grow allocate 0x17090", 0xA596C5, 0x17090), ("grow memmove", 0xA596D6, 0xF8F480), ("grow free 0x170c0", 0xA596EC, 0x170C0),
    ],
    "jmps": [],
    "relocs": [],
    "bytes": [
        ("id store", 0xA59643, "458926"), ("alias compare r15, rbx", 0xA59646, "4c39fb"),
        ("lock inc qword [ctl]", 0xA59673, "f048ff00"),
        ("grow size >> 2 (sar rax, 2)", 0xA596AA, "48c1f802"),
        ("push fast path: jmp short +0x73 to the epilogue 0xa5970a", 0xA59695, "eb73"),
    ],
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_a59620_id_entry.hpp", "helper_c": "native/core/helper_a59620_id_entry.cpp",
        "test": "native/tests/test_helper_a59620.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {"kIdEntryCtorRva": 0xA59620, "kIdEntryCtorSize": 249, "kIdEntryVecBeginRva": 0x32F8DC8, "kIdEntryStride": 0x28},
    },
    "tokens": {
        "helper_h": ["static_assert(sizeof(PtrVec24) == 0x18);", "std::uint8_t gap04[4];        // never written",
                     "static_assert(sizeof(IdEntry) == kIdEntryStride"],
        "helper_c": ["(void)std::atomic_ref<std::int64_t>(*dst.ctl).fetch_add(1, std::memory_order_seq_cst);",
                     "if (vec.end < vec.cap) {", "std::size_t n = bytes != 0 ? static_cast<std::size_t>(static_cast<std::int64_t>(bytes) >> 2) : 1;",
                     "alloc_17090(n * sizeof(void*), alloc)", "free_170c0(vec.begin, alloc).ok", "vec.cap = buffer + n;",
                     "init_slot_f180(&entry.str);", "entry.id = id;", "if (&entry.str == &src) {",
                     "release_inert_view_f590(entry.str)", "copy_shared_str_inc(entry.str, src);",
                     "std::memmove(static_cast<void*>(buffer), static_cast<const void*>(vec.begin), bytes)"],
        "test": ["CHECK(ctl == 6);", "CHECK(grows == 6);", "CHECK(g.frees == 5);", "refusal == PtrVecRefusal::no_allocator",
                 "CHECK(entry.gap04[0] == kPoison && entry.gap04[3] == kPoison);"],
        "cmake": ["core/helper_a59620_id_entry.cpp", "add_test(NAME helper_a59620 COMMAND test_helper_a59620)"],
    },
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
