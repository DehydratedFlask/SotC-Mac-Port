#!/usr/bin/env python3
"""Pin the TLS allocation-site guard chain inside `0x11fb0` and `0x12820`.

Verifies against the pinned image that both bodies keep the exact sequence the model encodes:
`cmpl $0x1` guard, the thread-pointer load, the `0x400` count guard, `count-1` scaled by 0x20, the
fallback to the per-index table at `0x2dfaab0`, and the final `movq (%rax),%rax` / `movq (%rax),%rbx`
load of the chosen allocator. Skips cleanly without the private image.
"""

from __future__ import annotations

import hashlib
import importlib.util
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF = ROOT / "local/re/eboot.elf"
FDE = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"
HEADER = ROOT / "native/core/alloc_site_context.hpp"
SOURCE = ROOT / "native/core/alloc_site_context.cpp"
CMAKE = ROOT / "native/CMakeLists.txt"

# (function, length, body sha, guard window start, guard window bytes, table window start, table bytes)
UNITS = (
    (0x11FB0, 0x81,
     "62ab8ac07db0f2d3587c48e2fe891ed69bbeb00954e189cab5621c23cbb91a24",
     0x11FF5, "83fb0175224584f6741d488b004885c074158b880004000085c9740bffc948c1e1054801c8eb0d89d8",
     0x1201C, "89d8488d0d8b8ade02488d04c148"),
    (0x12820, 0x125,
     "e53019b12d35291400c52ec66081aab45e06a171179676deffae45e6f0fe7f38",
     0x12883, "83fb017520488b004885c074188b880004000085c9740effc94489ea",
     0x128A8, "89d8488d0dff81de024489ea488d04c148"),
)


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    header = HEADER.read_text()
    source = SOURCE.read_text()
    cmake = CMAKE.read_text()

    for needle in ("kAllocatorTableAddress = 0x2dfaab0", "kAllocatorTlsIndex = 1",
                   "enum class AllocatorSource", "tls_site"):
        require(needle in header, f"header missing {needle}")
    require("index == kAllocatorTlsIndex && flag" in source,
            "source does not encode the `index == 1 && flag` guard")
    require("tls_alloc_site_peek_top" in source, "source does not peek the TLS site")
    require("tls_stack_empty" in source and "out.allocator = fallback" in source,
            "source does not fall back on an empty stack")
    for needle in ("core/alloc_site_context.cpp", "tests/test_alloc_site_context.cpp",
                   "test_alloc_site_context"):
        require(needle in cmake, f"cmake missing {needle}")

    if not ELF.is_file() or not FDE.is_file():
        print("SKIP check_alloc_site_context: private image not present (module wiring checked)")
        return 0

    elf = ELF.read_bytes()
    _, program_headers = d.program_headers(elf)
    image = d.Image(elf, program_headers)

    spec = importlib.util.spec_from_file_location("cfg_census", ROOT / "tools/lift/cfg_census.py")
    census = importlib.util.module_from_spec(spec)
    sys.modules["cfg_census"] = census
    spec.loader.exec_module(census)
    fdes, _duplicates = census.load_fde_ranges(FDE)

    for function, size, sha, guard_start, guard_hex, table_start, table_hex in UNITS:
        body = image.read(function, size)
        require(hashlib.sha256(body).hexdigest() == sha, f"0x{function:x} body sha")
        found = fdes.find(function)
        require(found is not None and found[0] == function, f"0x{function:x} is not an FDE start")
        require(found[1] == size, f"0x{function:x} FDE length {found[1]:#x} != {size:#x}")

        guard = image.read(guard_start, len(bytes.fromhex(guard_hex)))
        require(guard.hex() == guard_hex, f"0x{guard_start:x} guard window changed")
        require(guard.startswith(bytes.fromhex("83fb01")), "guard no longer compares the index to 1")
        require(bytes.fromhex("8b8800040000") in guard, "guard lost the 0x400 count load")
        require(bytes.fromhex("48c1e105") in guard or bytes.fromhex("48c1e105") in body,
                "guard lost the 0x20 scaling")

        table_slot = image.read(table_start, len(bytes.fromhex(table_hex)))
        require(table_slot.hex() == table_hex, f"0x{table_start:x} table fallback changed")
        require(bytes.fromhex("8d0d") in table_slot, "fallback no longer leas a table")

        # The chosen pointer is loaded once at the shared exit: `movq (%rax),%rax` in 0x11fb0,
        # `movq (%rax),%rbx` in 0x12820 (its body continues into the allocation path afterwards).
        expected = b"\x48\x8b\x00" if function == 0x11FB0 else b"\x48\x8b\x18"
        require(expected in body, f"0x{function:x} no longer loads the chosen allocator pointer")

    print("OK check_alloc_site_context: both bodies keep the index==1 guard, the 0x400 count "
          "check, the 0x20 record stride and the 0x2dfaab0 table fallback")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
