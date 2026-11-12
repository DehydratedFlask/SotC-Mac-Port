"""Batch-6 extension of the bool-page checker driver (startup slots 430 504 555 366 411).

Re-exports `bool_page_lib.main` (lane-M6 driver + real `--tamper native`) and registers one more named byte check:

  b6_string_class   the string console-variable class 0x1594ef0 (slot 0 = dtor 0x23c90, 2 = 0x20d60, 3 = 0x246a0
                    returning 1, 6 = 0x24780 returning 4), its functor class 0x1595068 with the four 0x25a50.. slot
                    bodies, and the two string-constructor vtable stores. Used by slot 555 (0x257a0 twice).

Offline only: reads the pinned private ELF; nothing is loaded, relocated or executed.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402
from slot_pin_util import require  # noqa: E402


def x_b6_string_class(e, spec) -> None:
    require(e.reloc_target(0x1594EF0) == 0x23C90, "string class slot 0 = 0x23c90")
    require(e.reloc_target(0x1594EF0 + 0x10) == 0x20D60, "string class slot 2 = 0x20d60")
    require(e.reloc_target(0x1594EF0 + 0x18) == 0x246A0, "string class slot 3 = 0x246a0 (returns 1)")
    require(e.reloc_target(0x1594EF0 + 0x30) == 0x24780, "string class slot 6 = 0x24780 (returns 4)")
    e.bytes_at(0x246A0, "b801000000c3", "string slot 3: mov eax,1; ret")
    e.bytes_at(0x24780, "b804000000c3", "string slot 6: mov eax,4; ret")
    for i, body in enumerate((0x25A50, 0x25A70, 0x25A90, 0x25AA0)):
        require(e.reloc_target(0x1595068 + 8 * i) == body, f"string functor class 0x1595068 slot {i}")
    require(e.rip(0x25A54, 7, 3) == 0x1595068 and e.rip(0x25A70, 7, 3) == 0x1595068, "functor stores its class tag")
    require(e.rip(0x257BF, 7, 3) == 0x1594EE0 and e.rip(0x25921, 7, 3) == 0x1594F58, "string constructor vtables")


bp.lib.EXTRAS["b6_string_class"] = x_b6_string_class
main = bp.main
