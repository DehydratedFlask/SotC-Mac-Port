#!/usr/bin/env python3
"""ELF-tie for CPU-01-STARTUP-SLOT-306-ASSEMBLE-01 (native/core/ctor_slot_306_a633b0).

Offline only: reads the pinned private ELF, never loads/relocates/executes an original RVA.
Pins: ELF SHA-256; constructor-table word at file offset 0x2dda750 (b033a60000000000 ->
0xa633b0); body [0xa633b0,0xa654bd) 0x210d B SHA-256 (E676/E697); the two direct calls the
analyze_slot_struct.py walker omits at 0x400-byte window boundaries (E690/E695): 0xa637ae ->
0xf8f390 (__cxa_atexit) and 0xa643ad -> 0xc300 (198 direct + 11 indirect is authoritative);
all 22 sndx_ console names exactly once, menu captions/paths and "MenuSeparator" present as
NUL-terminated strings; the 46 object globals strictly increasing, mapped, not file-backed
(BSS); the native kSlot306Table row-by-row against an independent copy of the semantics
checkpoint table plus module constants and per-kind counts; the production row
{0xa633b0, 306, ...} with fail-visible wiring, StartupState members, and CMake registration.
Private: needs local/re/eboot.elf.
"""
from __future__ import annotations

import hashlib
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
TABLE_WORD_FILE_OFFSET = 0x2DDA750
TABLE_WORD_BYTES = "b033a60000000000"
BODY_START = 0xA633B0
BODY_END = 0xA654BD
BODY_SIZE = 0x210D
BODY_SHA256 = "f8f707c7f80129cb79d2d54a61b6fd53caa62bf920c36f18a5b93152ff9933c8"
WALKER_OMITTED_CALLS = ((0xA637AE, 0xF8F390), (0xA643AD, 0xC300))

CONSOLE_NAMES = (
    "sndx_listenerOrientation", "sndx_listenerPosition", "sndx_listenerPlayerOffset",
    "sndx_drawListenerPosition", "sndx_showEnvironmentInfo", "sndx_showWorldAmbientInfo",
    "sndx_showTouchVolumes", "sndx_showDuckingInfo", "sndx_showGlobalInstVars",
    "sndx_onlyUseLastCore", "sndx_masterVolume", "sndx_attachSndxDebugger", "sndx_showBanks",
    "sndx_showActive", "sndx_showActive3d", "sndx_showSystemInfo", "sndx_debugGlobalAlpha",
    "sndx_debugLingerTime", "sndx_debugLingerFadeTime", "sndx_reloadData",
    "sndx_categoryMuteAll", "sndx_categoryFilterAll",
)
MENU_TEXTS = (
    "Sound", "Master Volume", "Category Mute", "Show System Info", "Show Banks",
    "Show Environment Info", "Show World Ambient Info", "Draw Touch Volumes", "Show Ducking Info",
    "Show Global Inst Vars", "Show Active", "Show Active 3d", "Category Filters",
    "Inactive Linger Time", "Draw Listener Position", "Listener Orientation", "Listener Position",
    "Player Position Offset", "Reload All Sound Data", "MenuSeparator",
)

# Independent copy of kSlot306Table / docs/checkpoints/2026-10-25-slot306-semantics.md.
# Compact form: kind,slot,rva,name,dtor,lo,hi,default,target,target_rva,parent,command,handler
EXPECTED_TEXT = """
root_cell,0,32f8ff0,,a5be70,0,0,0,-1,0,,,0
int_var,0,32f8ff8,sndx_listenerOrientation,a5bea0,0,0,0,-1,0,,,0
int_var,1,32f90d0,sndx_listenerPosition,a5bea0,0,0,0,-1,0,,,0
float_var,0,32f91a8,sndx_listenerPlayerOffset,20f0,0,42c80000,0,-1,0,,,0
bool_var,0,32f9280,sndx_drawListenerPosition,20b0,0,0,0,-1,0,,,0
bool_var,1,32f9348,sndx_showEnvironmentInfo,20b0,0,0,0,-1,0,,,0
bool_var,2,32f9410,sndx_showWorldAmbientInfo,20b0,0,0,0,-1,0,,,0
bool_var,3,32f94d8,sndx_showTouchVolumes,20b0,0,0,0,-1,0,,,0
bool_var,4,32f95a0,sndx_showDuckingInfo,20b0,0,0,0,-1,0,,,0
bool_var,5,32f9668,sndx_showGlobalInstVars,20b0,0,0,0,-1,0,,,0
bool_var,6,32f9730,sndx_onlyUseLastCore,20b0,0,0,0,-1,0,,,0
float_var,1,32f9810,sndx_masterVolume,20f0,0,3f800000,3f800000,-1,0,,,0
bool_var,7,32f98e8,sndx_attachSndxDebugger,20b0,0,0,0,-1,0,,,0
bool_var,8,32f99b0,sndx_showBanks,20b0,0,0,0,-1,0,,,0
int_var,2,32f9a78,sndx_showActive,a5bee0,0,0,0,-1,0,,,0
bool_var,9,32f9b50,sndx_showActive3d,20b0,0,0,0,-1,0,,,0
bool_var,10,32f9c18,sndx_showSystemInfo,20b0,0,0,0,-1,0,,,0
float_var,2,32f9ce0,sndx_debugGlobalAlpha,20f0,0,3f800000,3f800000,-1,0,,,0
float_var,3,32f9db8,sndx_debugLingerTime,20f0,0,41200000,0,-1,0,,,0
float_var,4,32f9e90,sndx_debugLingerFadeTime,20f0,0,41200000,3f000000,-1,0,,,0
command,0,32f9f68,sndx_reloadData,4b0,0,0,0,-1,0,,,a5bf20
command,1,32fa008,sndx_categoryMuteAll,4b0,0,0,0,-1,0,,,a5bf50
command,2,32fa0a8,sndx_categoryFilterAll,4b0,0,0,0,-1,0,,,a5c020
menu_group,0,32fa148,Sound,272d0,0,0,0,-1,0,,,0
menu_slider,1,32fa1f0,Master Volume,272d0,3c23d70a,3dcccccd,0,11,0,Sound,,0
menu_group,2,32fa298,Category Mute,272d0,0,0,0,-1,0,Sound,,0
menu_separator,3,32fa340,,272d0,0,0,0,-1,0,Sound,,0
menu_toggle,4,32fa3e8,Show System Info,272d0,0,0,0,16,0,Sound,,0
menu_toggle,5,32fa490,Show Banks,272d0,0,0,0,13,0,Sound,,0
menu_toggle,6,32fa538,Show Environment Info,272d0,0,0,0,5,0,Sound,,0
menu_toggle,7,32fa5e0,Show World Ambient Info,272d0,0,0,0,6,0,Sound,,0
menu_toggle,8,32fa688,Draw Touch Volumes,272d0,0,0,0,7,0,Sound,,0
menu_toggle,9,32fa730,Show Ducking Info,272d0,0,0,0,8,0,Sound,,0
menu_toggle,10,32fa7d8,Show Global Inst Vars,272d0,0,0,0,9,0,Sound,,0
menu_separator,11,32fa880,,272d0,0,0,0,-1,0,Sound,,0
menu_toggle,12,32fa928,Show Active,272d0,0,0,0,14,0,Sound,,0
menu_toggle,13,32fa9d0,Show Active 3d,272d0,0,0,0,15,0,Sound,,0
menu_group,14,32faa78,Category Filters,272d0,0,0,0,-1,0,Sound,,0
menu_slider,15,32fab20,Inactive Linger Time,272d0,3dcccccd,3f000000,0,18,0,Sound,,0
menu_separator,16,32fabc8,,272d0,0,0,0,-1,0,Sound,,0
menu_toggle,17,32fac70,Draw Listener Position,272d0,0,0,0,4,0,Sound,,0
menu_toggle,18,32fad18,Listener Orientation,272d0,0,0,0,1,0,Sound,,0
menu_toggle,19,32fadc0,Listener Position,272d0,0,0,0,2,0,Sound,,0
menu_slider,20,32fae68,Player Position Offset,272d0,3dcccccd,3f800000,0,3,0,Sound,,0
menu_separator,21,32faf10,,272d0,0,0,0,-1,0,Sound,,0
menu_command,22,32fafb8,Reload All Sound Data,272d0,0,0,0,-1,2dfcbd0,Sound,sndx_reloadData,0
"""

NATIVE_CONSTANTS = {
    "kSlot306Rva": 0xA633B0,
    "kSlot306EndRva": 0xA654BD,
    "kSlot306BodySize": 0x210D,
    "kSlot306ObjectCount": 46,
    "kSlot306FirstMenuIndex": 23,
    "kSlot306MenuItemCount": 23,
    "kSlot306IntVarCount": 3,
    "kSlot306FloatVarCount": 5,
    "kSlot306BoolVarCount": 11,
    "kSlot306CommandCount": 3,
    "kSlot306RootCellRva": 0x32F8FF0,
    "kSlot306RootCellDtorRva": 0xA5BE70,
    "kSlot306BoolFnVtbl": 0x15D2060,
    "kSlot306VarFlags": 1,
    "kSlot306ItemFlags": 0x30,
    "kSlot306CommandTargetRva": 0x2DFCBD0,
}
KIND_COUNTS = {
    "root_cell": 1, "int_var": 3, "float_var": 5, "bool_var": 11, "command": 3,
    "menu_group": 3, "menu_slider": 3, "menu_toggle": 12, "menu_separator": 4, "menu_command": 1,
}

ROW_RE = re.compile(
    r"\{Slot306Kind::(\w+),\s*(\d+),\s*(0x[0-9a-fA-F]+),\s*\"([^\"]*)\",\s*(0x[0-9a-fA-F]+),"
    r"\s*(0x[0-9a-fA-F]+),\s*(0x[0-9a-fA-F]+),\s*(0x[0-9a-fA-F]+),\s*(-?\d+),\s*(0x[0-9a-fA-F]+),"
    r"\s*\"([^\"]*)\",\s*\"([^\"]*)\",\s*(0x[0-9a-fA-F]+)\}"
)


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(f"FAIL {message}")


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def const_value(text: str, name: str) -> int:
    match = re.search(rf"\b{re.escape(name)}\s*=\s*(0x[0-9a-fA-F]+|\d+)\s*;", text)
    require(match is not None, f"missing native constant {name}")
    return int(match.group(1), 0)


def expected_rows():
    rows = []
    for line in EXPECTED_TEXT.strip().splitlines():
        f = line.split(",")
        rows.append((f[0], int(f[1]), int(f[2], 16), f[3], int(f[4], 16), int(f[5], 16),
                     int(f[6], 16), int(f[7], 16), int(f[8]), int(f[9], 16), f[10], f[11],
                     int(f[12], 16)))
    require(len(rows) == 46, "expected table must hold 46 rows")
    return rows


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(digest(elf) == ELF_SHA256, "unexpected ELF SHA256")
    _, program_headers = d.program_headers(elf)
    img = d.Image(elf, program_headers)
    expected = expected_rows()

    # original table word, body extent, walker-omitted calls
    require(elf[TABLE_WORD_FILE_OFFSET:TABLE_WORD_FILE_OFFSET + 8].hex() == TABLE_WORD_BYTES,
            "constructor-table word at file offset 0x2dda750")
    require(BODY_START + BODY_SIZE == BODY_END, "body extent arithmetic")
    body = img.read(BODY_START, BODY_SIZE)
    require(digest(body) == BODY_SHA256, "slot 306 body sha256")
    for site, target in WALKER_OMITTED_CALLS:
        opcode = img.read(site, 5)
        resolved = site + 5 + struct.unpack("<i", opcode[1:5])[0]
        require(opcode[0] == 0xE8 and resolved == target,
                f"walker-omitted call at {site:#x} must reach {target:#x}")

    # strings in the raw image
    for name in CONSOLE_NAMES:
        require(elf.count(name.encode() + b"\0") == 1,
                f"console name {name!r} must occur exactly once")
    for text in MENU_TEXTS:
        require(elf.count(text.encode() + b"\0") >= 1, f"menu text {text!r} missing")

    # the 46 object globals: ordered, mapped, BSS
    globals_ = [row[2] for row in expected]
    require(len(globals_) == 46 and globals_ == sorted(globals_) and len(set(globals_)) == 46,
            "expected globals must be 46 strictly increasing unique RVAs")
    for rva in globals_:
        require(img.mapped(rva, 8), f"global {rva:#x} must be inside a loadable segment")
        try:
            img.read(rva, 8)
            require(False, f"global {rva:#x} must be BSS (not file-backed)")
        except ValueError:
            pass

    # native module: constants and the 46-row table
    header = (ROOT / "native/core/ctor_slot_306_a633b0.hpp").read_text()
    source = (ROOT / "native/core/ctor_slot_306_a633b0.cpp").read_text()
    for name, value in NATIVE_CONSTANTS.items():
        require(const_value(header, name) == value, f"native constant {name}")
    require('kSlot306Sound[] = "Sound"' in header, "kSlot306Sound literal")
    require(re.search(r"inline constexpr std::array<std::size_t, 0> kSlot306Gaps\{\};",
                      header) is not None,
            "kSlot306Gaps must be empty: all 46 objects are built")
    table_match = re.search(r"kSlot306Table\{\{(.*?)\n\}\};", header, re.DOTALL)
    require(table_match is not None, "cannot find kSlot306Table")
    rows = ROW_RE.findall(table_match.group(1))
    require(len(rows) == 46, f"expected 46 table rows, found {len(rows)}")
    for i, row in enumerate(rows):
        parsed = (row[0], int(row[1]), int(row[2], 16), row[3], int(row[4], 16),
                  int(row[5], 16), int(row[6], 16), int(row[7], 16), int(row[8]),
                  int(row[9], 16), row[10], row[11], int(row[12], 16))
        require(parsed == expected[i],
                f"table row #{i} diverges from the pinned construction table")
    counts: dict[str, int] = {}
    for row in expected:
        counts[row[0]] = counts.get(row[0], 0) + 1
    require(counts == KIND_COUNTS, "per-kind object counts")
    for i, row in enumerate(expected):
        if row[8] >= 0:
            require(expected[row[8]][2] != 0 and expected[row[8]][8] == -1,
                    f"menu row #{i} target must be a variable object")
    require("init_slot306_a633b0" in source and "slot306_stage_name" in source,
            "assembly implementation entry points")

    # production wiring
    startup_source = (ROOT / "native/core/startup_ctors.cpp").read_text()
    startup_header = (ROOT / "native/core/startup_ctors.hpp").read_text()
    require("{0xa633b0, 306, [](StartupState& s) noexcept {" in startup_source,
            "missing production row {0xa633b0, 306}")
    require("s.slot306_result = init_slot306_a633b0(s.slot306, s.concmd, s.debug_menu, "
            "s.slot306_hooks);" in startup_source, "slot 306 row callback")
    require("if (!s.slot306_result.ok) s.init_failed = true;" in startup_source,
            "slot 306 row must be fail-visible")
    for member in ("Slot306Objects slot306{};",
                   "Slot306Hooks slot306_hooks = default_slot306_hooks();",
                   "Slot306Result slot306_result{};"):
        require(member in startup_header, f"missing StartupState member: {member}")
    cmake = (ROOT / "native/CMakeLists.txt").read_text()
    require("core/ctor_slot_306_a633b0.cpp" in cmake, "CMake source registration")
    require("add_test(NAME ctor_slot_306 COMMAND test_ctor_slot_306)" in cmake,
            "CMake test registration")
    require((ROOT / "native/tests/test_ctor_slot_306.cpp").is_file(), "missing test fixture")

    print(
        "OK check_ctor_slot_306: table word 0x2dda750 -> 0xa633b0; body "
        f"{BODY_SHA256[:12]}... ({BODY_SIZE:#x} B); walker-omitted calls pinned; 22 console "
        f"names + {len(MENU_TEXTS)} menu texts + MenuSeparator present; 46 BSS globals "
        "ordered; 46-row native table + constants + production row + fail-visible wiring"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
