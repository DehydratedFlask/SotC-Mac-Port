#!/usr/bin/env python3
"""ELF-tie the E604 SaveData metadata slice.

Pins the save body, the mount2 body, the raw-result jump table at 0x12c3370 and the native
`0x530` block constants. No guest code runs.
"""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
SAVE = 0xA4AB90
SAVE_SIZE = 6125
SAVE_SHA256 = "217f8559bb3611f83767ee2876828d6dbdacca64190aad90015e239922e231e5"
MOUNT = 0xA4A940
MOUNT_SIZE = 514
MOUNT_SHA256 = "3cdeeb68e9b81dc7ad92a15d94fcd1bfab17a644cdf129896c457fb13ce300f3"
RESULT_TABLE = 0x12C3370
RESULT_TABLE_BYTES = 48
# 12 entry pointers: index 0, 6, 10, 11 are unique; the rest share the generic handler.
TABLE_UNIQUE = {0: 0xFF789465, 6: 0xFF787F9A, 10: 0xFF789459, 11: 0xFF789460}
META_H = ROOT / "native/core/save_meta.hpp"
TRANSACTION_H = ROOT / "native/core/save_transaction.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(hashlib.sha256(img.read(SAVE, SAVE_SIZE)).hexdigest() == SAVE_SHA256, "save body hash")
    require(hashlib.sha256(img.read(MOUNT, MOUNT_SIZE)).hexdigest() == MOUNT_SHA256,
            "mount2 body hash")

    table = struct.unpack("<12I", img.read(RESULT_TABLE, RESULT_TABLE_BYTES))
    generic = table[1]
    require(len(set(table)) == 5, "result table shape (five distinct targets)")
    for index, target in TABLE_UNIQUE.items():
        require(table[index] == target, f"result table index {index}")
    require(table[1] == table[2] == table[3] == table[4] == table[5], "generic handler run")

    meta = META_H.read_text()
    for token in ("kSaveParamBytes = 0x530", "kSaveMetaUserParamOffset = 0x500",
                  "kSaveMetaReservedBytes = 0x02C", "SOTCMETA", "SaveIcon",
                  "kSaveIconMaxBytes = 1u << 20"):
        require(token in meta, f"meta token {token}")
    transaction = TRANSACTION_H.read_text()
    for token in ("SaveAttachments", "save_delete", "map_sce_result", "classify_dialog_status",
                  "write_meta", "write_icon", "delete_slot", "data_committed"):
        require(token in transaction, f"transaction token {token}")

    print(f"OK check_fsave_metadata: save {SAVE_SHA256[:12]}…; mount2 {MOUNT_SHA256[:12]}…; "
          f"result table {generic:#x} generic + indices 0/6/10/11; 0x530 block and delete API")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
