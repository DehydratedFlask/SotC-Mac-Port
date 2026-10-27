#!/usr/bin/env python3
"""ELF-tie for CPU-01-SLOT306-CATEGORY-VARS-01: the native sndxCategory_* model
(native/core/engine_category_vars.{hpp,cpp} + the init_bool_var_vector sibling in
native/core/concmd_vars.{hpp,cpp}).

Offline only: reads the pinned private ELF, never loads, relocates or executes an original RVA.
It pins the engine-init body hash, the 0xa5d990 builder's instruction sites (vector leas, end
advances, formatter/ctor/register/toggle/dtor calls, loop bound and back-edge), the seven name
strings, the native module constants, the vector-owned construction sibling, and the build files.
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
ENGINE_INIT_SHA256 = "460f5e5226e6e090e8af001f2df159bfd55dd60b2d4e01b0f59a279eb0686e92"

INSTRUCTION_PINS = {
    "mute vector lea": (0xA5D9C7, 7, "488d83d08b0000"),
    "mute format lea": (0xA5DA30, 7, "488d35af758600"),
    "mute format call": (0xA5DA3A, 5, "e851305bff"),
    "mute ctor call": (0xA5DAAC, 5, "e83f305cff"),
    "mute register call": (0xA5DB24, 5, "e837325cff"),
    "mute end advance": (0xA5DB29, 8, "488183d88b0000c8"),
    "filter vector lea": (0xA5D9DC, 7, "488d83a8720000"),
    "filter format lea": (0xA5DB68, 7, "488d358c748600"),
    "filter format call": (0xA5DB72, 5, "e8192f5bff"),
    "filter ctor call": (0xA5DBE0, 5, "e80b2f5cff"),
    "filter register call": (0xA5DC4E, 5, "e80d315cff"),
    "filter end advance": (0xA5DC53, 9, "49818424b0720000c8"),
    "caption format lea": (0xA5DCBC, 7, "488d354f738600"),
    "caption format call": (0xA5DCC9, 5, "e8c22d5bff"),
    "toggle ctor call 1": (0xA5DD2E, 5, "e86d915cff"),
    "toggle ctor call 2": (0xA5DD91, 5, "e80a915cff"),
    "stack dtor call 1": (0xA5DDA8, 5, "e823955cff"),
    "stack dtor call 2": (0xA5DDB4, 5, "e817955cff"),
    "loop bound": (0xA5DDDA, 4, "4983fe20"),
    "loop back-edge": (0xA5DDDE, 6, "0f853cfcffff"),
}

CALL_TARGETS = {
    "mute format call": 0x10A90,
    "mute ctor call": 0x20AF0,
    "mute register call": 0x20D60,
    "filter format call": 0x10A90,
    "filter ctor call": 0x20AF0,
    "filter register call": 0x20D60,
    "caption format call": 0x10A90,
    "toggle ctor call 1": 0x26EA0,
    "toggle ctor call 2": 0x26EA0,
    "stack dtor call 1": 0x272D0,
    "stack dtor call 2": 0x272D0,
}

LEA_TARGETS = {
    "mute format lea": 0x12C4FE6,
    "filter format lea": 0x12C4FFB,
    "caption format lea": 0x12C5012,
}

DISPLACEMENTS = {  # (site, disp32 offset inside the instruction, expected value)
    "mute vector lea": (0xA5D9C7, 3, 0x8BD0),
    "filter vector lea": (0xA5D9DC, 3, 0x72A8),
    "mute end advance": (0xA5DB29, 3, 0x8BD8),
    "filter end advance": (0xA5DC53, 4, 0x72B0),  # modrm 0x84: SIB byte shifts disp32 by one
}

STRINGS = {
    0x12C4FE6: "sndxCategory_Mute_%d",
    0x12C4FFB: "sndxCategory_Filter_%d",
    0x12C5012: "%d - %s",
    0x12C4D2F: "Category Mute",
    0x12C4DDD: "Category Filters",
    0x12C501A: "Unnamed Categories",
    0x12C4D1A: "",
}

NATIVE_CONSTANTS = {
    "kSndxCategoriesBuilderRva": 0xA5D990,
    "kSndxCategoriesEngineInitRva": 0xA5CD90,
    "kSndxNameFormatRva": 0x10A90,
    "kSndxVectorGrowRva": 0xA62BC0,
    "kSndxCategoryVarFnVtbl": 0x15D2060,
    "kSndxMuteFormatRva": 0x12C4FE6,
    "kSndxFilterFormatRva": 0x12C4FFB,
    "kSndxCaptionFormatRva": 0x12C5012,
    "kSndxMuteParentRva": 0x12C4D2F,
    "kSndxFilterParentRva": 0x12C4DDD,
    "kSndxUnnamedGroupRva": 0x12C501A,
    "kSndxEmptyDescRva": 0x12C4D1A,
    "kSndxCategoryCount": 32,
    "kSndxCategoryNameCapacity": 32,
    "kSndxMuteVectorCapOffset": 0x8BE0,
    "kSndxFilterVectorCapOffset": 0x72B8,
}
SHARED_CONSTANTS = {  # sndx_command_handlers.hpp (the shared vector/stride facts)
    "kSndxMuteVectorOffset": 0x8BD0,
    "kSndxFilterVectorOffset": 0x72A8,
    "kSndxCategoryVarCount": 32,
    "kSndxCategoryVarStride": 0xC8,
}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(f"FAIL {message}")


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def const_value(text: str, name: str) -> int:
    match = re.search(rf"\b{re.escape(name)}\s*=\s*(0x[0-9a-fA-F]+|\d+)\s*;", text)
    require(match is not None, f"missing native constant {name}")
    return int(match.group(1), 0)


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(digest(elf) == ELF_SHA256, "unexpected ELF SHA256")
    mod = d.parse(elf)
    img = d.Image(elf, mod["ph"])

    body = img.read(0xA5CD90, 0xA5E41F - 0xA5CD90)
    require(digest(body) == ENGINE_INIT_SHA256, "engine init 0xa5cd90 body sha256")

    for label, (site, size, hex_bytes) in INSTRUCTION_PINS.items():
        require(img.read(site, size).hex() == hex_bytes, f"{label} bytes at {site:#x}")
    for label, target in CALL_TARGETS.items():
        site = INSTRUCTION_PINS[label][0]
        opcode = img.read(site, 5)
        require(opcode[0] == 0xE8 and site + 5 + struct.unpack("<i", opcode[1:5])[0] == target,
                f"{label} must reach {target:#x}")
    for label, target in LEA_TARGETS.items():
        site = INSTRUCTION_PINS[label][0]
        raw = img.read(site, 7)
        require(raw[0:2] == b"\x48\x8d" and site + 7 + struct.unpack("<i", raw[3:7])[0] == target,
                f"{label} must address {target:#x}")
    for label, (site, disp_off, want) in DISPLACEMENTS.items():
        raw = img.read(site, disp_off + 4)
        require(struct.unpack("<I", raw[disp_off:disp_off + 4])[0] == want,
                f"{label} displacement must be {want:#x}")
    # the end advances also carry the 0xc8 stride immediate
    require(img.read(0xA5DB29 + 7, 1) == b"\xc8", "mute end advance stride immediate 0xc8")
    require(img.read(0xA5DC53 + 8, 1) == b"\xc8", "filter end advance stride immediate 0xc8")

    for addr, text in STRINGS.items():
        require(img.read(addr, len(text) + 1) == text.encode() + b"\0",
                f"string {text!r} at {addr:#x}")

    header = (ROOT / "native/core/engine_category_vars.hpp").read_text()
    source = (ROOT / "native/core/engine_category_vars.cpp").read_text()
    for name, value in NATIVE_CONSTANTS.items():
        require(const_value(header, name) == value, f"native constant {name}")
    shared = (ROOT / "native/core/sndx_command_handlers.hpp").read_text()
    for name, value in SHARED_CONSTANTS.items():
        require(const_value(shared, name) == value, f"shared constant {name}")
    require("build_sndx_category_vars" in header and "sndx_category_vectors_from_engine" in header,
            "module entry points")
    require("relink_build_block" in source, "the relocation fixup must exist")

    # the vector-owned construction sibling (no atexit) and its wiring
    vars_header = (ROOT / "native/core/concmd_vars.hpp").read_text()
    require("init_bool_var_vector" in vars_header, "vector-owned bool-var construction sibling")
    require("SharedStrDeallocatorF590* name_deallocator" in vars_header,
            "owned-name release contract")
    wiring = (ROOT / "native/core/fconcmd_object_slot306.hpp").read_text()
    require("kSndxCategoriesBuilderRva" not in wiring, "category lane stays decoupled from the "
            "command-object module")

    cmake = (ROOT / "native/CMakeLists.txt").read_text()
    require("core/engine_category_vars.cpp" in cmake, "CMake source registration")
    require("add_test(NAME sndx_category_vars COMMAND test_sndx_category_vars)" in cmake,
            "CMake test registration")
    require((ROOT / "native/tests/test_sndx_category_vars.cpp").is_file(), "missing test fixture")

    print(
        "OK check_sndx_category_vars: engine init body "
        f"{ENGINE_INIT_SHA256[:12]}...; builder sites pinned (vectors 0x8bd0/0x72a8, stride 0xc8, "
        "bound 32, formatter/ctor/register/toggle/dtor calls); 7 strings; native + shared "
        "constants; vector-owned sibling; build files verified"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
