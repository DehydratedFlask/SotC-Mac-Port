#!/usr/bin/env python3
"""Cross-check the retail call entry point.

Every dispatch arm must name a target that (a) has a retail ABI row and (b) has a cell adapter, so
`call_resolved` cannot silently claim support for a target it cannot marshal.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CALL_CPP = ROOT / "native/core/retail_call.cpp"
ABI_HPP = ROOT / "native/core/retail_abi.hpp"
CELLS_CPP = ROOT / "native/core/retail_cells.cpp"
CMAKE = ROOT / "native/CMakeLists.txt"

EXPECTED = {
    0x20D60: "marshal_20d60",
    0x358C0: "marshal_358c0",
    0x13D20: "marshal_13d20",
    0x187C0: "marshal_187c0",
    0x2687B0: "marshal_2687b0",
    0x1BF710: "marshal_1bf710",
    0xC2E0: "marshal_c2e0",
    0xC740: "marshal_c740",
    0xE16A00: "marshal_e16a00",
    0xDDF4B0: "marshal_ddf4b0",
    0xA460F0: "marshal_a460f0",
    0xE24900: "marshal_e24900",
    0xE26F90: "marshal_e26f90",
    0xE1A5C0: "marshal_e1a5c0",
    0xE19980: "marshal_e19980",
    0xE21420: "marshal_e21420",
    0xA40E20: None,  # pure no-op: no cells to marshal
}


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    cpp = CALL_CPP.read_text()
    abi = ABI_HPP.read_text()
    cells = CELLS_CPP.read_text()
    cmake = CMAKE.read_text()

    arms = re.findall(r"case\s+kVtable[A-Za-z0-9_]+Rva:\s*\{", cpp)
    require(len(arms) == len(EXPECTED), f"expected {len(EXPECTED)} dispatch arms, found {len(arms)}")

    for rva, adapter in EXPECTED.items():
        require(f"0x{rva:x}" in abi, f"0x{rva:x} missing from the retail ABI table")
        if adapter is None:
            require("vtable_15d1678_slot_68_a40e20()" in cpp,
                    "no-op dispatch does not call the model directly")
            continue
        require(adapter in cpp, f"dispatch for 0x{rva:x} does not call {adapter}")
        require(adapter in cells, f"{adapter} missing from retail_cells.cpp")

    require("retail_arg_spec_for(rva) == nullptr" in cpp, "missing unknown-target guard")
    require("retail_stack_alloc(" in cpp, "missing guest stack allocator")
    require("RetailCallStatus::stack_unavailable" in cpp, "missing stack_unavailable status")
    require("retail_stack_alloc(" in (ROOT / "native/core/retail_call.hpp").read_text(),
            "stack allocator not declared")
    require("RetailCallStatus::unknown_target" in cpp, "missing unknown-target status")
    require("out.rax = 0" in cpp, "dispatch does not write rax")
    require("core/retail_call.cpp" in cmake, "cmake missing retail_call.cpp")
    require("test_retail_call" in cmake, "cmake missing test_retail_call")

    print(f"OK check_retail_call: {len(EXPECTED)} dispatch arms all have ABI rows and adapters")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
