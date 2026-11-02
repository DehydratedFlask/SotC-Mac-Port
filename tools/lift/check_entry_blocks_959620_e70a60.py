#!/usr/bin/env python3
"""Pin the two bounded entry blocks that two BLOCKED tasks were waiting on.

`AUD-02-NGS2-RACKQUERY-CALLEE-959620-01` asked for the entry block at `0x959620`, capped at 32
instructions / 128 bytes, stopping at the first control transfer. `CPU-01-SLOT23-DIRECT-CALLEE-
0E70A60-01` asked for the same at `0xe70a60`. Both stop earlier than the cap, so both are complete
rather than truncated.

Neither block inspects a transfer target: the checks below are about the entry block only, and no
callee semantics are claimed.
"""

from __future__ import annotations

import hashlib
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF = ROOT / "local/re/eboot.elf"
DIS = ROOT / "local/validation/registry-analysis-agent/all.dis"
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"

# entry, byte length of the block, instruction count, first control transfer target.
BLOCKS = {
    0x959620: (0x1D, 11, 0x9596C7),   # 0x959620..0x95963d (the `je` is a 6-byte 0f 84 form)
    0xE70A60: (0x07, 3, 0xE70A79),    # 0xe70a60..0xe70a67 (the `ja` is a 2-byte 77 form)
}
CAP_INSTRUCTIONS = 32
CAP_BYTES = 128


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    if not (ELF.is_file() and DIS.is_file()):
        print("SKIP check_entry_blocks_959620_e70a60: private inputs not present")
        return 0

    elf = ELF.read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "pinned ELF digest changed")
    _, program_headers = d.program_headers(elf)
    image = d.Image(elf, program_headers)

    # Parse the pinned disassembly into (address, text) so the block bounds are read, not assumed.
    text = DIS.read_text(errors="replace")
    rows = [(int(m.group(1), 16), m.group(2).strip())
            for m in re.finditer(r"^\s+([0-9a-f]+):\s*(.*)$", text, re.MULTILINE)]
    starts = [a for a, _ in rows]
    after = {}
    for i, (addr, body) in enumerate(rows):
        after[addr] = rows[i + 1][0] if i + 1 < len(rows) else None

    for entry, (length, count, transfer) in BLOCKS.items():
        require(entry in after, f"0x{entry:x} is not an instruction boundary")
        block = [(a, b) for a, b in rows if entry <= a < entry + length]
        require(len(block) == count, f"0x{entry:x} block is {len(block)} instructions, expected {count}")
        require(len(block) <= CAP_INSTRUCTIONS, f"0x{entry:x} exceeds the instruction cap")
        require(length <= CAP_BYTES, f"0x{entry:x} exceeds the byte cap")
        require(block[0][0] == entry, f"0x{entry:x} does not start the block")
        # The block ends exactly where the first control transfer is, and nothing else in it transfers.
        transfers = [(a, b) for a, b in block
                     if re.match(r"^(retq|jmpq?|j[a-z]+|callq)\b", b)]
        require(len(transfers) == 1, f"0x{entry:x} has {len(transfers)} control transfers, expected 1")
        require(transfers[0][0] == block[-1][0],
                f"0x{entry:x} first transfer is at 0x{transfers[0][0]:x}, not the block end")
        require(re.search(rf"\b0x{transfer:x}\b", transfers[0][1]),
                f"0x{entry:x} transfer target changed: {transfers[0][1]}")
        # The next instruction after the block must be the boundary we pinned.
        require(after[block[-1][0]] == entry + length,
                f"0x{entry:x} block length no longer lands on the next instruction")
        body = image.read(entry, length)
        require(body is not None and len(body) == length, f"0x{entry:x} unreadable")
        print(f"  0x{entry:x}: {count} instructions / 0x{length:x} bytes, sha256 "
              f"{hashlib.sha256(body).hexdigest()[:16]}…, first transfer -> 0x{transfer:x}")

    print("OK check_entry_blocks_959620_e70a60: both entry blocks decode completely inside the caps "
          "and each ends at its single control transfer; no transfer target was inspected")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
