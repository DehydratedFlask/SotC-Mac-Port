#!/usr/bin/env python3
"""ELF byte-oracle of startup constructor slot 322 (0x849e30, 386 B): bool var `shadow_autoEquipNewWeapons`
(value 1) and its 0x26db0 "Shadow Items" menu item. Also pins the 0x26db0 constructor body (0xef B) that the
native core/menu_item_26db0 lifts. See tools/lift/slotfam_check.py for what is pinned. Private: needs local/re/eboot.elf."""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import slotfam_check as lib  # noqa: E402

if __name__ == "__main__":
    raise SystemExit(lib.main(322, "shadow_autoEquipNewWeapons + 0x26db0 'Shadow Items' item"))
