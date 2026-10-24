#!/usr/bin/env python3
"""ELF-tie the native SaveData transaction (E598 / SAV-02).

Pins the GameData producer, the BSS blob address/size, the mount2 body hash, and the native
header/ordinal/backup rules. No guest code runs; bytes are data.
"""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PRODUCER = 0x7E5BB0
PRODUCER_SIZE = 13
PRODUCER_SHA256 = "ec99373fe9a5c1f422cc5ac7186b2059ca9cb41aa20870e27191cf8dcc08335e"
GAME_DATA_ADDR = 0x3260BB8
GAME_DATA_BYTES = 0x3CD8
MOUNT2 = 0xA4A940
MOUNT2_SIZE = 514
MOUNT2_SHA256 = "3cdeeb68e9b81dc7ad92a15d94fcd1bfab17a644cdf129896c457fb13ce300f3"

TRANSACTION_H = ROOT / "native/core/save_transaction.hpp"
PROGRESS_H = ROOT / "native/core/save_progress_object.hpp"
CODEC_H = ROOT / "native/core/save_chunk_codec.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(PRODUCER, PRODUCER_SIZE)
    require(hashlib.sha256(body).hexdigest() == PRODUCER_SHA256, "GameData producer hash")
    require(body[:3] == bytes.fromhex("488d05"), "producer is lea rax,[...]")
    require(body[7:10] == bytes.fromhex("c707d8"), "producer stores 0x3cd8")
    require(b"\xd8\x3c\x00\x00" in body, "size immediate 0x3cd8")
    require(hashlib.sha256(img.read(MOUNT2, MOUNT2_SIZE)).hexdigest() == MOUNT2_SHA256,
            "mount2 body hash")

    loads = [p for p in ph if p["type"] == 1]
    max_end = max(p["vaddr"] + p["filesz"] for p in loads)
    require(GAME_DATA_ADDR >= max_end, "0x3260bb8 is past every PT_LOAD file image")

    text = TRANSACTION_H.read_text()
    for token in ("save_load", "save_store", "save_check_backup", "save_restore_backup",
                  "save_enumerate", "backup_capture", "write_data", "failed_corrupt"):
        require(token in text, f"transaction token {token}")
    progress = PROGRESS_H.read_text()
    require(f"kprogressgamedatabytes = {GAME_DATA_BYTES:#x}" in progress.lower(),
            "0x3cd8 constant")
    for n, name in enumerate(("success", "canceled", "failed_no_space", "failed_file_not_found",
                              "failed_mount", "failed_corrupt", "failed_generic"), start=1):
        require(f"{name} = {n}" in progress, f"SaveResult ordinal {name}={n}")
    codec = CODEC_H.read_text()
    require("0x3cd8" in codec or "15576" in codec, "codec GameData fixed length")
    require("0x9c" in codec, "codec Options fixed length")

    print(f"OK check_fsave_transaction: producer {PRODUCER_SHA256[:12]}…; GameData "
          f"{GAME_DATA_ADDR:#x} {GAME_DATA_BYTES} B in BSS; mount2 {MOUNT2_SHA256[:12]}…; "
          f"ordinals 1-7 and backup-before-write present")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
