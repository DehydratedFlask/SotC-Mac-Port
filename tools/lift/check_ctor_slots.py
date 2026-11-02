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

prefix = [row for row in rows if row[1] <= 23]
assert [row[1] for row in prefix] == list(range(24)), "indices 0–23 must be represented contiguously"
assert all(not row[2] for row in prefix), "indices 0–23 must have callbacks (slot 23 uses private name fixture)"
assert prefix[10][:3] == (0xf09e60, 10, False), "index 10 RVA 0xf09e60 must be recovered"
assert prefix[11][:3] == (0xf09160, 11, False), "index 11 RVA 0xf09160 must be recovered"
assert prefix[23][:3] == (0xe71990, 23, False), "index 23 RVA 0xe71990 must be recovered"
# Production table must not carry explicit null rows (fail-visible is via init_failed on missing fixture).
assert not [row for row in rows if row[2]], "recovered_constructors() must not contain nullptr callbacks"
# Indices 24-47 (descriptor + mutex + atexit family) are on the production path after Phase B.
family_24_47 = [row for row in rows if 24 <= row[1] <= 47]
assert [row[1] for row in family_24_47] == list(range(24, 48)), "indices 24-47 must be registered contiguously"
assert all(not row[2] for row in family_24_47), "indices 24-47 must have callbacks"
# Row 11 must be the real slot-11 callback, not an empty lambda.
slot11_lines = [l for l in array_match.group(1).splitlines() if re.match(r"^\s*\{\s*0xf09160\s*,\s*11\s*,", l)]
assert len(slot11_lines) == 1 and "init_clothing_physical_mesh_descriptor(s)" in slot11_lines[0], (
    "slot 11 must call init_clothing_physical_mesh_descriptor")
# Slots 12-19 must call the registration helper with their own index; slot 21 the const-table lift.
source_lines = array_match.group(1).splitlines()
THUNKS = {12: 0xf06340, 13: 0xf058f0, 14: 0xf02ff0, 15: 0xf01100, 16: 0xf00090, 17: 0xefe320,
          18: 0xefd020, 19: 0xefacd0}
for thunk_index, thunk_rva in THUNKS.items():
    lines = [l for l in source_lines if re.match(rf"^\s*\{{\s*{thunk_rva:#x}\s*,\s*{thunk_index}\s*,", l)]
    assert len(lines) == 1 and f"init_registration_thunk(s, {thunk_index})" in lines[0], (
        f"slot {thunk_index} must call init_registration_thunk(s, {thunk_index})")
slot21_lines = [l for l in source_lines if re.match(r"^\s*\{\s*0xee1130\s*,\s*21\s*,", l)]
assert len(slot21_lines) == 1 and "init_const_table_ee1130(s.const_table_ee1130)" in slot21_lines[0], (
    "slot 21 must call init_const_table_ee1130")
slot23_lines = [l for l in source_lines if re.match(r"^\s*\{\s*0xe71990\s*,\s*23\s*,", l)]
assert len(slot23_lines) == 1 and "init_slot23_leaf_e71990(s)" in slot23_lines[0], (
    "slot 23 must call init_slot23_leaf_e71990")
# CPU-01-CONCMD-298-299-301-01: the three ConCmd slots, wired to the shared registry state.
SLOT_298_299_301 = {
    298: (0xb0e250, "init_slot298_b0e250(s.slot298_shader_type, s.concmd)"),
    299: (0xb0c7a0, "init_slot299_b0c7a0(s.slot299, s.concmd, s.debug_menu)"),
    301: (0xac9550, "init_slot301_ac9550(s.slot301_rumble, s.concmd)"),
    # CPU-01-CONCMD-303-01: ProfileCPU bool variable + "Profile CPU" debug-menu toggle.
    302: (0xaaeee0, "init_slot302_aaeee0(s.slot302, s.concmd.atexit_log)"),
    305: (0xa77b60, "init_slot305_a77b60(s.slot305)"),
    303: (0xa82170, "init_slot303_a82170(s.slot303, s.concmd, s.debug_menu)"),
}
for slot_index, (slot_rva, slot_api) in SLOT_298_299_301.items():
    lines = [l for l in source_lines if re.match(rf"^\s*\{{\s*{slot_rva:#x}\s*,\s*{slot_index}\s*,", l)]
    assert len(lines) == 1, f"slot {slot_index} ({slot_rva:#x}) must be registered exactly once"
    assert slot_api in array_match.group(1), f"slot {slot_index} must call {slot_api}"
# CPU-01-STARTUP-SLOTS-296-297-300-01: three slot-specific modules with real callbacks.
SLOT_296_297_300 = {
    296: (0xb2b6b0, "init_slot296_b2b6b0(s.slot296_stamp)"),
    297: (0xb25470, "init_slot297_b25470(s.slot297_table, default_slot297_hooks())"),
    300: (0xacc880, "init_slot300_acc880(s.slot300_cleanup)"),
}
for slot_index, (slot_rva, slot_api) in SLOT_296_297_300.items():
    lines = [l for l in source_lines if re.match(rf"^\s*\{{\s*{slot_rva:#x}\s*,\s*{slot_index}\s*,", l)]
    assert len(lines) == 1, f"slot {slot_index} ({slot_rva:#x}) must be registered exactly once"
    assert slot_api in array_match.group(1), (
        f"slot {slot_index} must call {slot_api}")
AHEAD = {24: 0xe56b70, 25: 0xe55f10, 26: 0xe55240, 27: 0xe54a30, 28: 0xe53a10, 29: 0xe52a20, 30: 0xe52150,
         31: 0xe518e0, 32: 0xe510e0, 33: 0xe50980, 34: 0xe500b0, 35: 0xe4f840, 36: 0xe4f040, 37: 0xe4e8e0,
         38: 0xe4e010, 39: 0xe4d7a0, 40: 0xe4cfa0, 41: 0xe4c840, 42: 0xe4bf10, 43: 0xe4b7b0, 44: 0xe4afb0,
         45: 0xe47670, 46: 0xe467e0, 47: 0xe459e0}
for ahead_index, ahead_rva in AHEAD.items():
    lines = [l for l in source_lines if re.match(rf"^\s*\{{\s*{ahead_rva:#x}\s*,\s*{ahead_index}\s*,", l)]
    assert len(lines) == 1 and f"init_registration_thunk_ahead(s, {ahead_index})" in lines[0], (
        f"slot {ahead_index} must call init_registration_thunk_ahead(s, {ahead_index})")
assert [(rva, index) for rva, index, _, _ in rows if rva in (0xd32720, 0xd2ffe0)] == [
    (0xd32720, 119),
    (0xd2ffe0, 120),
], "registered indices for 0xd32720 and 0xd2ffe0 must be 119 and 120"

# ---- computed rule: module coverage (replaces the hard-coded "lifted but not registered" list) ----
# Every `vec_<rva>.hpp` / `leaf_<rva>.hpp` module is an already-lifted constructor: its file name
# and its first comment line both name the original RVA. The pinned table is the identity authority;
# a module RVA that is not a unique table row is a hard failure. Every module index must then be
# registered with the pinned RVA, in original order, exactly once. (Prose index claims inside old
# headers use a legacy survey numbering and are not used for identity.)
core_dir = ROOT / "native/core"


def module_index(path):
    match = re.fullmatch(r"(?:vec|leaf)_([0-9a-f]+)\.hpp", path.name)
    if not match:
        return None
    rva = int(match.group(1), 16)
    matches = [index for index, value in enumerate(original_rvas) if value == rva]
    assert len(matches) == 1, f"{path.name}: RVA {rva:#x} is not a unique constructor-table row"
    first_line = path.read_text().splitlines()[0]
    claimed = re.search(r"RVA\s*0x([0-9a-fA-F]+)", first_line)
    assert claimed and int(claimed.group(1), 16) == rva, (
        f"{path.name}: first-line RVA claim disagrees with the file name")
    return matches[0]


modules = {}
for module_path in sorted(core_dir.glob("vec_*.hpp")) + sorted(core_dir.glob("leaf_*.hpp")):
    index = module_index(module_path)
    if index is None:
        continue
    assert index not in modules, f"two modules claim constructor index {index}"
    modules[index] = int(module_path.stem.split("_", 1)[1], 16)

rows_by_index = {index: rva for rva, index, _, _ in rows}
for index in sorted(modules):
    assert index in rows_by_index, (
        f"constructor index {index} has module RVA {modules[index]:#x} but is not registered")
    assert rows_by_index[index] == original_rvas[index] == modules[index], (
        f"constructor index {index} registered at {rows_by_index[index]:#x}, "
        f"pinned table says {original_rvas[index]:#x}, module says {modules[index]:#x}")

# Registered set = recovered prefix 0-151 + every byte-verified module + the sparse late rows that
# have no vec_/leaf_ module + the slot-specific modules 296-301 and 302/303/305/306. Extra rows would be
# unverified registrations; a missing module row means a recovered constructor is silently skipped.
# Slots recovered by the 2026-10-26 lanes (L1-L5), each with its own tools/lift/check_ctor_slot*/check_ctor_*_slots.py ELF checker.
LANE_SLOTS_20261026 = {
    307, 308, 310, 311, 313, 315, 317, 318, 322, 323, 325, 327, 328, 330, 331, 332, 333, 334,
    341, 343, 344, 345, 347, 348, 350, 353, 355, 356, 357, 371, 372, 378, 394, 404, 405, 406,
    412, 416, 425, 431, 432, 433, 434, 435, 438, 447, 470, 471, 472, 481, 483, 484, 485, 487,
    496, 500, 515, 517, 519, 522, 523, 525, 527, 528, 529, 531, 533, 541, 542, 546, 547, 549,
    551, 553, 560, 562, 563, 564, 565,
}
LANE_SLOTS_20261028 = {
    359, 364, 365, 376, 377, 383, 384, 385, 386, 387,
    389, 393, 403, 407, 413, 415, 419, 421,
    426, 436, 439, 440, 441, 456, 457, 458, 459, 461,
    463, 465, 475, 476, 478, 479, 482, 486, 489, 490,
    491, 492, 499, 502, 509, 516, 535, 540, 543,
    544, 557, 558, 559, 569,
}
# Slot 309 (0xa505e0, the Save System debug block) was registered by CPU-01-RANGED-BOUND-VAR-A4F4D0-01
# once its three console-variable constructors 0xa4f4d0 / 0xa4fec0 / 0xa50280 were lifted as the shared
# model core/ranged_bound_var_a4f4d0 (pinned by tools/lift/check_console_var_ctors.py).
expected_registered = (set(range(152)) | set(modules) | {282, 290, 520, 538, 296, 297, 300, 298, 299, 301, 302, 303, 305, 306, 309}
                       | LANE_SLOTS_20261026 | LANE_SLOTS_20261028)
assert set(rows_by_index) == expected_registered, (
    "registered set differs from the computed expectation: "
    f"extra={sorted(set(rows_by_index) - expected_registered)} "
    f"missing={sorted(expected_registered - set(rows_by_index))}")
registered_digest = hashlib.sha256(
    ",".join(str(index) for index in sorted(rows_by_index)).encode()).hexdigest()
assert registered_digest == "4d7af7037f84854b9300a4c8f92cc0babc52a6d7f4a9f9dba04b1a732fd57571", (
    f"registered index-set sha256 changed: {registered_digest}")

# The remaining 123 original slots have no registered native callback; the first is the explicit
# startup cliff after slot 309 was registered by CPU-01-RANGED-BOUND-VAR-A4F4D0-01.
unregistered = sorted(set(range(TABLE_COUNT)) - set(rows_by_index))
assert len(unregistered) == 123, f"expected 123 unregistered slots, found {len(unregistered)}"
assert unregistered[0] == 312 and original_rvas[312] == 0xa46130, (
    f"first unregistered slot changed: {unregistered[0]}/{original_rvas[unregistered[0]]:#x}")

# The 573-row data table must reproduce the ELF exactly, in index order.
startup_header = (core_dir / "startup_ctors.hpp").read_text()
assert "inline constexpr std::size_t kOriginalCtorCount = 573;" in startup_header, (
    "startup_ctors.hpp must pin kOriginalCtorCount = 573")
table_source = (core_dir / "ctor_original_table.cpp").read_text()
table_rows = [(int(index), int(rva, 16)) for index, rva in re.findall(
    r"^\s*\{\s*(\d+)\s*,\s*(0x[0-9a-fA-F]+)\s*\},", table_source, re.MULTILINE)]
assert len(table_rows) == TABLE_COUNT, (
    f"ctor_original_table.cpp must carry {TABLE_COUNT} rows, found {len(table_rows)}")
for index, rva in table_rows:
    assert index < TABLE_COUNT and rva == original_rvas[index], (
        f"ctor_original_table.cpp row {index}: {rva:#x} != ELF table {original_rvas[index]:#x}")


# ---- byte anchors for slot 11 (ClothingPhysicalMeshParameters) against the pinned ELF ----
BODY_SHA256 = {
    0xf09160: (281, "adb91b08884c8fc861cc37b1653e2496a0aaf8efa883222e2d602e8e6d444ef0"),   # slot 11
    0xf06410: (1021, "086c93ce9f3d9ae834aea2d1641bc6e791d5824427002a81176c8d46ee57a5d5"),  # class ctor
    0xf069f0: (44, "d2e9246d8ca47ea76b9b46e68c1e9942e0a0a9d9dccf6fd7666d6d5d0a894e3c"),   # slot-0 dtor
    # Slots 12-19 registration thunks (full traced-effect proof: tools/lift/check_ctor_thunks.py).
    0xf06340: (205, "a16f79f44e9fa082e8e7a25d300210dd6538aa4f5f1341d66a6e30769b3dcb53"),  # slot 12
    0xf058f0: (79, "d021c860c421a25ee64f18d6007daa897d963d4a9be62d51928061b0eee6675b"),   # slot 13
    0xf02ff0: (239, "0c803e076d6f24f81b7cb9cab7deca5e633c4e24f9041350455f87a88f83b7fa"),  # slot 14
    0xf01100: (226, "006d8ef333df1b54dde77fcd144deec4277711d05567fe36b20cc31804be7d24"),  # slot 15
    0xf00090: (284, "aed062395020b9ce82f5c04c640cd1b42ba83a844c84ac9bdbbfda9fbc2ca5a0"),  # slot 16
    0xefe320: (239, "d157e4bf9c9e7f3900051edb9659906cca7c45c454af68a29d05c73ec9b2a612"),  # slot 17
    0xefd020: (281, "a14f6aa8db166697a1cc3eb0ad2feb49c15c482e725864333c748335b9492638"),  # slot 18
    0xefacd0: (79, "2750b1530f9edd2b561c213b7c6be699443617bbc464e0769bfc1910bfa51f6e"),   # slot 19
    0xe71990: (337, "3932fb85b759ae53dd1675142ffda4b449be725f241cfeefdb40a07a2beaf774"),  # slot 23
}
for body_rva, (body_size, body_digest) in BODY_SHA256.items():
    got = hashlib.sha256(image.read(body_rva, body_size)).hexdigest()
    assert got == body_digest, f"body {body_rva:#x} sha256 {got} != {body_digest}"

# The native header's block offsets/strides must equal the original constructor stores.
header = (ROOT / "native/core/clothing_physical_mesh_parameters.hpp").read_text()


def header_array(name):
    match = re.search(name + r"\[kBlockCount\]\s*=\s*\{(.*?)\};", header, re.DOTALL)
    assert match, f"cannot find {name} in clothing_physical_mesh_parameters.hpp"
    return [int(tok, 0) for tok in re.findall(r"0x[0-9a-fA-F]+|\d+", match.group(1))]


EXPECTED_OFFSETS = [0x60, 0x78, 0x90, 0xa8, 0xc0, 0xd8, 0xf0, 0x110, 0x140, 0x158, 0x178, 0x190]
EXPECTED_STRIDES = [12, 12, 12, 12, 2, 4, 1, 4, 40, 52, 40, 52]
assert header_array("kBlockOffsets") == EXPECTED_OFFSETS, "native block offsets differ from the verified list"
assert header_array("kBlockStrides") == EXPECTED_STRIDES, "native block strides differ from the verified list"


def has_store(body, op, disp, imm, imm_len):
    """Find `mov [rbx+disp], imm` in either disp8 or disp32 form (rbx = this)."""
    encodings = []
    if disp < 0x80:
        encodings.append(bytes([op, 0x43, disp]) + imm.to_bytes(imm_len, "little"))
    encodings.append(bytes([op, 0x83]) + disp.to_bytes(4, "little") + imm.to_bytes(imm_len, "little"))
    return any(enc in body for enc in encodings)


def block_stores_ok(body, label):
    for index, (offset, stride) in enumerate(zip(EXPECTED_OFFSETS, EXPECTED_STRIDES)):
        assert has_store(body, 0xc6, offset + 8, 1, 1), f"{label}: block {index} owns=1 store missing"
        if index == len(EXPECTED_OFFSETS) - 1:
            # The last block writes stride and count with one REX.W qword store (count = high dword = 0).
            qword = b"\x48\xc7\x83" + (offset + 0x0c).to_bytes(4, "little") + stride.to_bytes(4, "little")
            assert qword in body, f"{label}: block {index} qword stride/count store missing"
        else:
            assert has_store(body, 0xc7, offset + 0x0c, stride, 4), f"{label}: block {index} stride {stride} store missing"
            assert has_store(body, 0xc7, offset + 0x10, 0, 4), f"{label}: block {index} count=0 store missing"


# The native production and test-only registration tables must carry the ELF constructor-table RVA
# for each of their indices. Parse them separately so ahead rows cannot satisfy or contaminate the
# 12-19 production helper assertion.
thunk_header = (ROOT / "native/core/registration_thunks.hpp").read_text()
row_pattern = (r"\{\s*(\d+)\s*,\s*(0x[0-9a-fA-F]+)\s*,\s*0x[0-9a-fA-F]+\s*,\s*0x[0-9a-fA-F]+\s*,"
               r"\s*RegistrationProbeShape::\w+\s*\}")


def thunk_rows_for(table_name):
    match = re.search(rf"{re.escape(table_name)}\s*=\s*\{{\{{(.*?)\}}\}};", thunk_header, re.DOTALL)
    assert match, f"cannot find {table_name} in registration_thunks.hpp"
    return re.findall(row_pattern, match.group(1))


thunk_rows = thunk_rows_for("kRegistrationThunkDescriptors")
assert [int(i) for i, _ in thunk_rows] == list(THUNKS), "production registration table must list indices 12-19 in order"
for i, rva in thunk_rows:
    assert int(rva, 16) == original_rvas[int(i)] == THUNKS[int(i)], (
        f"registration_thunks.hpp row {i}: {rva} != ELF table {original_rvas[int(i)]:#x}")

ahead_rows = thunk_rows_for("kAheadRegistrationDescriptors")
assert [int(i) for i, _ in ahead_rows] == list(range(24, 48)), (
    "test-only ahead registration table must list indices 24-47 in order")
for i, rva in ahead_rows:
    index = int(i)
    assert int(rva, 16) == original_rvas[index], (
        f"ahead registration row {index}: {rva} != ELF table {original_rvas[index]:#x}")

block_stores_ok(image.read(0xf06410, 1021), "ctor 0xf06410")
block_stores_ok(image.read(0xf08f00, 556), "reset 0xf08f00")


print(
    f"OK: ELF sha256={elf_digest}; constructor table start={TABLE_START:#x} "
    f"count={TABLE_COUNT} sha256={table_digest}; verified {len(rows)} registered RVA/index pairs "
    f"({len(modules)} vec_*/leaf_* modules); registered index-set sha256={registered_digest}; "
    f"slots 0-47 recovered; idx 95 registered; {len(unregistered)} unregistered `other` slots remain, first "
    f"{unregistered[0]} / {original_rvas[unregistered[0]]:#x}"
)
