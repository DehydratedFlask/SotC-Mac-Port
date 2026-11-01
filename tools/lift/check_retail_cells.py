#!/usr/bin/env python3
"""Cross-check the guest-cell adapters against the bodies they marshal.

Every offset the adapters use must be the constant its model header declares, so an adapter cannot
drift from the pinned body it feeds.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CELLS_HPP = ROOT / "native/core/retail_cells.hpp"
CELLS_CPP = ROOT / "native/core/retail_cells.cpp"
CMAKE = ROOT / "native/CMakeLists.txt"

MODEL_HEADERS = {
    "kVtable1594dc0Slot20_20d60OnceFlagOffset": "native/core/vtable_1594dc0_slot20_20d60.hpp",
    "kVtable15955c0Slot08_358c0TableFieldOffset": "native/core/vtable_15955c0_slot08_358c0.hpp",
    "kVtable15955c0Slot08_358c0CopyFieldOffset": "native/core/vtable_15955c0_slot08_358c0.hpp",
    "kVtable15955c0Slot08_358c0SourceOffset": "native/core/vtable_15955c0_slot08_358c0.hpp",
    "kVtable1594ae8Slot28_13d20FieldOffset": "native/core/vtable_1594ae8_slot28_13d20.hpp",
    "kVtable15d1678SlotE8_c2e0CopyFieldOffset": "native/core/vtable_15d1678_slot_e8_c2e0.hpp",
    "kVtable15d1678SlotE8_c2e0SourceOffset": "native/core/vtable_15d1678_slot_e8_c2e0.hpp",
    "kVtable15d1d80Slot08_c740TableFieldOffset": "native/core/vtable_15d1d80_slot08_c740.hpp",
    "kVtable15d1d80Slot08_c740CopyFieldOffset": "native/core/vtable_15d1d80_slot08_c740.hpp",
    "kVtable15d1d80Slot08_c740SourceOffset": "native/core/vtable_15d1d80_slot08_c740.hpp",
}

EXPECTED_ADAPTERS = ("marshal_20d60", "marshal_358c0", "marshal_13d20", "marshal_187c0",
                     "marshal_2687b0", "marshal_1bf710", "marshal_c2e0", "marshal_c740",
                     "marshal_e16a00", "marshal_ddf4b0")


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    hpp = CELLS_HPP.read_text()
    cpp = CELLS_CPP.read_text()
    cmake = CMAKE.read_text()

    for adapter in EXPECTED_ADAPTERS:
        require(adapter in hpp, f"missing declaration {adapter}")
        require(adapter in cpp, f"missing definition {adapter}")

    for constant, header in MODEL_HEADERS.items():
        require(constant in cpp, f"adapter file does not use {constant}")
        header_text = (ROOT / header).read_text()
        require(constant in header_text, f"{header} does not declare {constant}")

    # The adapters must not invent offsets: every `self + <literal>` / `source + <literal>` must be a
    # named constant from a model header, not a bare number.
    bare = re.findall(r"(?:self|source)\s*\+\s*0x[0-9a-f]+", cpp)
    require(not bare, f"bare offsets in adapters: {bare}")

    require("core/guest_memory.cpp" in cmake, "cmake missing guest_memory.cpp")
    require("core/retail_cells.cpp" in cmake, "cmake missing retail_cells.cpp")
    require("test_guest_memory" in cmake, "cmake missing test_guest_memory")
    require("test_retail_cells" in cmake, "cmake missing test_retail_cells")

    print(f"OK check_retail_cells: {len(EXPECTED_ADAPTERS)} adapters use {len(MODEL_HEADERS)} "
          "model-declared offsets and no bare numbers")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
