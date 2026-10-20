#!/usr/bin/env python3
"""Verify native constructor slot identities against the pinned private ELF table."""

import hashlib
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
TABLE_START = 0x2dd70e0
TABLE_COUNT = 573
TABLE_ENTRY_BYTES = 8
TABLE_SHA256 = "5917cb85db190e9ef93ce5e06d5b6596d40b6c3623166e37ed7c6c3c3cd3ab28"

elf_path = ROOT / "local/re/eboot.elf"
elf = elf_path.read_bytes()
elf_digest = hashlib.sha256(elf).hexdigest()
assert elf_digest == ELF_SHA256, f"unexpected ELF sha256: {elf_digest}"

_, program_headers = d.program_headers(elf)
image = d.Image(elf, program_headers)
table_words = [
    image.read(TABLE_START - TABLE_ENTRY_BYTES * index, TABLE_ENTRY_BYTES)
    for index in range(TABLE_COUNT)
]
table_bytes = b"".join(table_words)
table_digest = hashlib.sha256(table_bytes).hexdigest()
assert table_digest == TABLE_SHA256, f"unexpected constructor table sha256: {table_digest}"

original_rvas = []
for index, word in enumerate(table_words):
    target = struct.unpack("<Q", word)[0]
    assert target <= 0xffffffff, f"constructor index {index} is not an ELF-relative RVA: {target:#x}"
    original_rvas.append(target)

source = (ROOT / "native/core/startup_ctors.cpp").read_text()
array_match = re.search(
    r"static constexpr CtorSlot kSlots\[\]\s*=\s*\{(.*?)\n\s*\};",
    source,
    re.DOTALL,
)
assert array_match, "could not find recovered constructor slot array"

rows = []
row_pattern = re.compile(r"^\s*\{\s*(0x[0-9a-fA-F]+)\s*,\s*(\d+)\s*,\s*(nullptr|\[\])")
for line_number, line in enumerate(array_match.group(1).splitlines(), start=1):
    match = row_pattern.match(line)
    if match:
        rows.append((int(match.group(1), 16), int(match.group(2)), match.group(3) == "nullptr", line_number))

assert rows, "no constructor registrations found"
seen_indices = set()
seen_rvas = set()
previous_index = -1
for rva, index, is_blocker, line_number in rows:
    assert 0 <= index < TABLE_COUNT, f"source line {line_number}: index out of range: {index}"
    assert index > previous_index, f"source line {line_number}: constructor rows are not in original order"
    assert index not in seen_indices, f"duplicate constructor index {index}"
    assert rva not in seen_rvas, f"duplicate constructor RVA {rva:#x}"
    expected_rva = original_rvas[index]
    assert rva == expected_rva, (
        f"source line {line_number}: RVA/index pair {rva:#x}/{index} disagrees with "
        f"pinned table value {expected_rva:#x}"
    )
    seen_indices.add(index)
    seen_rvas.add(rva)
    previous_index = index

prefix = [row for row in rows if row[1] <= 10]
assert [row[1] for row in prefix] == list(range(11)), "indices 0–10 must be represented contiguously"
assert all(not row[2] for row in prefix[:10]), "the first ten constructor entries must have callbacks"
assert prefix[10][:3] == (0xf09e60, 10, True), "index 10 must be the explicit unresolved blocker"
assert [(rva, index) for rva, index, _, _ in rows if rva in (0xd32720, 0xd2ffe0)] == [
    (0xd32720, 119),
    (0xd2ffe0, 120),
], "registered indices for 0xd32720 and 0xd2ffe0 must be 119 and 120"

print(
    f"OK: ELF sha256={elf_digest}; constructor table start={TABLE_START:#x} "
    f"count={TABLE_COUNT} sha256={table_digest}; verified {len(rows)} registered RVA/index pairs; "
    "first unresolved slot is index 10 RVA 0xf09e60 (semantics unknown)"
)
