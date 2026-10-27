#!/usr/bin/env python3
"""ELF-tie for CPU-01-SLOT306-HANDLERS-NATIVE-01: the native sndx_ command-handler model
(native/core/sndx_command_handlers.{hpp,cpp}).

Offline only: reads the pinned private ELF, never loads, relocates or executes an original RVA.
It pins:
  * the ELF SHA-256 and the body hashes/extents of the three handlers (0xa5bf20 / 0xa5bf50 /
    0xa5c020), the reload callee 0xa5f380, the bool setter 0xba40, the argument lookup 0x1e4c0,
    the execute dispatcher 0x21da0 and the atexit destructors 0xa5be70 / 0xa5bea0 / 0xa5bee0
    (digests from the private handlers manifest, independently re-read here);
  * the 0xa5bf20 instruction contract: `lea` of the engine cell 0x32f8ff0, the null test, the
    `mov eax,3` early-return and the single `call 0xa5f380`;
  * the 0xa5bf50/0xa5c020 guards and vectors: the engine-cell load, the args-count test
    ((end-begin)>>4 == 1), the Mute vector +0x8bd0/+0x8bd8 and Filter +0x72a8/+0x72b0 and the
    0xc8 stride immediates;
  * the 0xa5f380 walk: bank vector +0x28/+0x30, record size 0x38, handle field +0x10 with the
    -1 sentinel, loaded byte +0x08, reference flag +0x14, the load-table args +0x7270/+0x7250 and
    the 256 x 0x40 clear array at +0x1c50 (field +0x38, bound 0x4000);
  * the 0x1e4c0 token table: "true" @0xf95a1e, "yes" @0xf95a23, "false" @0xf95a27, "no" @0xf95a2d
    as NUL-terminated strings, plus the two import NIDs derived by scripts/ps4_dynlink.nid:
    strcasecmp = AV6ipCNa4Rw (case-insensitive compare) and _Stoul = zlfEH8FmyUA;
  * the native constants of the module (handler RVAs, engine offsets, strides, counts, var offsets,
    result codes) and the sndx_command_id_for_tag wiring.

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

# (name, start, size, sha256)
FUNCTIONS = {
    "reload_handler": (0xA5BF20, 33, "e77baf97d82167d59faf5de48529575bb78bfe4bafe6565880b1626fc4470444"),
    "mute_handler": (0xA5BF50, 192, "f0d56f563f92bfd0d8d15dd1274d1c4632e8c8c6defb7453ddb22d8edd3458b1"),
    "filter_handler": (0xA5C020, 192, "fa10b6c20693cecbb5a5852423653ccaec45797164428a7e73637f0f381ad196"),
    "reload_callee": (0xA5F380, 0x117, "2af0ccd9e83dac5578b59887f4a360de131db87f8bf57a64255c30da8e951dcf"),
    "bool_setter": (0xBA40, 0xD5, "d3c3ffcfe5be639bafac77508aebc54ffb13da38c7374acf7782c2cea127ee75"),
    "arg_lookup": (0x1E4C0, 0x118, "a4cb3cf28cf870c17ae1e75ef5af83a9b5b11ff1a9d6e295b12a3a6eb077194e"),
    "cmd_execute": (0x21DA0, 0x1D3, "792cb9b25bb7a37cdea45e9a0494961ad22d2c4bf1168b7884b61e5a6f80ea07"),
    "root_cell_dtor": (0xA5BE70, 46, "944d664bcd64f875f3bf167280ca1b0484b65898495be038b88b58d613c45221"),
    "intvar_dtor_listener": (0xA5BEA0, 54, "46c09fe5f28364f9c736490ba5f7a3e66e8f0bfc90eccc394756e6543ebbe0f5"),
    "intvar_dtor_active": (0xA5BEE0, 54, "4c21694d286e257ba5049c75423e5341816d018b7a87ba0258ff4f27c24fb4b4"),
    "cmd_dtor": (0x4B0, 113, "a15bae38e545531a8c5769a079333bd4e0385846160e10a4b768827ecb0d8c8c"),
    "invoke_thunk": (0x1990, 12, "c5eca1d65aea48ea6feaa7c41abef0c4f4d7bc1212e753f2d12da5b89285fd12"),
}

TOKENS = {0xF95A1E: "true", 0xF95A23: "yes", 0xF95A27: "false", 0xF95A2D: "no"}
ENGINE_CELL = 0x32F8FF0

NATIVE_CONSTANTS = {
    "kSndxReloadDataHandlerRva": 0xA5BF20,
    "kSndxCategoryMuteAllHandlerRva": 0xA5BF50,
    "kSndxCategoryFilterAllHandlerRva": 0xA5C020,
    "kSndxEngineCellRva": 0x32F8FF0,
    "kSndxReloadCalleeRva": 0xA5F380,
    "kSndxBoolArgLookupRva": 0x1E4C0,
    "kSndxBoolVarSetRva": 0xBA40,
    "kSndxCmdExecuteRva": 0x21DA0,
    "kSndxResourceUnregisterRva": 0x989450,
    "kSndxResourceRemoveRefRva": 0x14D2D0,
    "kSndxBankAddRefRva": 0x95580,
    "kSndxLoadSoundTableRva": 0xB096B0,
    "kSndxBankVectorBegin": 0x28,
    "kSndxBankVectorEnd": 0x30,
    "kSndxBankRecordSize": 0x38,
    "kSndxBankHandleOffset": 0x10,
    "kSndxBankLoadedOffset": 0x08,
    "kSndxBankWrapperOffset": 0x00,
    "kSndxBankRefFlagOffset": 0x14,
    "kSndxSoundTableOffset": 0x7270,
    "kSndxSoundTableNameOffset": 0x7250,
    "kSndxClearArrayOffset": 0x1C50,
    "kSndxClearArrayCount": 256,
    "kSndxClearRecordSize": 0x40,
    "kSndxClearFieldOffset": 0x38,
    "kSndxMuteVectorOffset": 0x8BD0,
    "kSndxFilterVectorOffset": 0x72A8,
    "kSndxCategoryVarCount": 32,
    "kSndxCategoryVarStride": 0xC8,
    "kSndxArgRecordSize": 0x10,
    "kSndxCmdResultDone": 0,
    "kSndxCmdResultRejected": 3,
    "kSndxVarFlagsOffset": 0x1C,
    "kSndxVarValueRefOffset": 0x60,
    "kSndxVarDefaultOffset": 0x68,
    "kSndxVarMinBoundOffset": 0x69,
    "kSndxVarClampEngagedOffset": 0x6A,
    "kSndxVarMaxBoundOffset": 0x6B,
    "kSndxVarCallbackOffset": 0x70,
    "kSndxVarAdjacentValueOffset": 0xC0,
}

NID_STRCASE = "AV6ipCNa4Rw"   # d.nid("strcasecmp")
NID_STOUL = "zlfEH8FmyUA"     # d.nid("_Stoul")


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

    # ---- body hashes/extents (re-read from the pinned bytes, matching the private manifest)
    for name, (start, size, sha) in FUNCTIONS.items():
        body = img.read(start, size)
        require(digest(body) == sha, f"{name} body sha256 at {start:#x}")

    # ---- 0xa5bf20 instruction contract
    body = img.read(FUNCTIONS["reload_handler"][0], FUNCTIONS["reload_handler"][1])
    require(body[0:3] == bytes.fromhex("488d05"), "0xa5bf20 must start with lea rax,[rip+..]")
    lea_target = 0xA5BF20 + 7 + struct.unpack("<i", body[3:7])[0]
    require(lea_target == ENGINE_CELL, "0xa5bf20 lea must address the engine cell 0x32f8ff0")
    require(body[7:10] == bytes.fromhex("488b38"), "0xa5bf20 must load the engine pointer into rdi")
    require(body[10:15] == bytes.fromhex("b803000000"), "0xa5bf20 pre-loads the return code 3")
    require(body[15:20] == bytes.fromhex("4885ff740c"), "0xa5bf20 null test + early return branch")
    call = img.read(0xA5BF38, 5)
    require(call[0] == 0xE8 and 0xA5BF38 + 5 + struct.unpack("<i", call[1:5])[0] == 0xA5F380,
            "0xa5bf38 must call the reload callee 0xa5f380")
    require(body[29:33] == bytes.fromhex("31c05dc3"), "0xa5bf20 must end xor eax,eax; pop rbp; ret (0)")

    # ---- instruction-level pins (exact bytes read from the pinned ELF at the observed sites)
    INSTRUCTION_PINS = {
        # 0xa5bf50 sndx_categoryMuteAll
        "mute args-count guard": (0xA5BF92, 3, "83f901"),
        "mute arg lookup call": (0xA5BFA3, 5, "e818255cff"),
        "mute vector load": (0xA5BFAE, 7, "498b9dd08b0000"),
        "mute vector end cmp": (0xA5BFB5, 7, "493b9dd88b0000"),
        "mute value-ref load": (0xA5BFD0, 4, "488b7360"),
        "mute setter call": (0xA5BFDA, 5, "e861fa5aff"),
        "mute stride add": (0xA5BFDF, 7, "4881c3c8000000"),
        # 0xa5c020 sndx_categoryFilterAll (same template, Filter vector)
        "filter args-count guard": (0xA5C062, 3, "83f901"),
        "filter arg lookup call": (0xA5C073, 5, "e848245cff"),
        "filter vector load": (0xA5C07E, 7, "498b9da8720000"),
        "filter vector end cmp": (0xA5C085, 7, "493b9db0720000"),
        "filter value-ref load": (0xA5C0A0, 4, "488b7360"),
        "filter setter call": (0xA5C0AA, 5, "e891f95aff"),
        "filter stride add": (0xA5C0AF, 7, "4881c3c8000000"),
        # 0xa5f380 reload callee
        "reload bank begin": (0xA5F39B, 4, "4d8b6628"),
        "reload bank end cmp": (0xA5F39F, 4, "4d396630"),
        "reload unregister call": (0xA5F3CD, 5, "e87ea0f2ff"),
        "reload handle clear": (0xA5F3D2, 7, "41c7041cffffff"),
        "reload loaded clear": (0xA5F3EC, 5, "41c6441cf8"),
        "reload stride add": (0xA5F3FD, 4, "4883c338"),
        "reload ref-flag test": (0xA5F420, 6, "41807c1c1400"),
        "reload addref call": (0xA5F42E, 5, "e84d6163ff"),
        "reload table lea": (0xA5F455, 7, "498dbe70720000"),
        "reload table name lea": (0xA5F45C, 7, "498db650720000"),
        "reload loadtable call": (0xA5F463, 5, "e848a20a00"),
        "reload clear store": (0xA5F470, 9, "49c78406881c000000"),
        "reload clear bound": (0xA5F480, 8, "483d0040000075e8"),
    }
    for label, (site, size, hex_bytes) in INSTRUCTION_PINS.items():
        require(img.read(site, size).hex() == hex_bytes, f"{label} bytes at {site:#x}")
    # decode the call targets and vector/table displacements the pins encode
    for label, site, target in (
        ("mute arg lookup call", 0xA5BFA3, 0x1E4C0), ("mute setter call", 0xA5BFDA, 0xBA40),
        ("filter arg lookup call", 0xA5C073, 0x1E4C0), ("filter setter call", 0xA5C0AA, 0xBA40),
        ("reload unregister call", 0xA5F3CD, 0x989450), ("reload addref call", 0xA5F42E, 0x95580),
        ("reload loadtable call", 0xA5F463, 0xB096B0),
    ):
        opcode = img.read(site, 5)
        require(opcode[0] == 0xE8 and site + 5 + struct.unpack("<i", opcode[1:5])[0] == target,
                f"{label} must reach {target:#x}")
    for label, site, size, disp_off, want in (
        ("mute vector load", 0xA5BFAE, 7, 3, 0x8BD0), ("mute vector end cmp", 0xA5BFB5, 7, 3, 0x8BD8),
        ("filter vector load", 0xA5C07E, 7, 3, 0x72A8),
        ("filter vector end cmp", 0xA5C085, 7, 3, 0x72B0),
        ("reload table lea", 0xA5F455, 7, 3, 0x7270), ("reload table name lea", 0xA5F45C, 7, 3, 0x7250),
    ):
        raw = img.read(site, size)
        require(struct.unpack("<I", raw[disp_off:disp_off + 4])[0] == want,
                f"{label} displacement must be {want:#x}")
    require(struct.unpack("<I", img.read(0xA5F470 + 4, 4))[0] == 0x1C88,
            "reload clear store disp must be 0x1c88 (array 0x1c50 + field 0x38)")

    # ---- 0x1e4c0 token table and the two import NIDs
    for addr, token in TOKENS.items():
        raw = img.read(addr, len(token) + 1)
        require(raw == token.encode() + b"\0", f"token {token!r} at {addr:#x}")
    require(d.nid("strcasecmp") == NID_STRCASE, "NID AV6ipCNa4Rw must be strcasecmp (case-insensitive)")
    require(d.nid("_Stoul") == NID_STOUL, "NID zlfEH8FmyUA must be _Stoul")
    jmprel = [r for r in mod["relocs"] if r["table"] == "JMPREL"]
    # The 0x1e4c0 compare/parse go through PLT thunks whose push-immediates index JMPREL:
    # strcasecmp at 0xf8f506 (push 0x17) and _Stoul at 0xf8f5d6 (push 0x24).
    require(mod["symbols"][int(jmprel[0x17]["symbol"])]["nid"] == NID_STRCASE,
            "PLT 0xf8f506 resolves to strcasecmp")
    require(mod["symbols"][int(jmprel[0x24]["symbol"])]["nid"] == NID_STOUL,
            "PLT 0xf8f5d6 resolves to _Stoul")

    # ---- native module constants and the tag wiring
    header = (ROOT / "native/core/sndx_command_handlers.hpp").read_text()
    source = (ROOT / "native/core/sndx_command_handlers.cpp").read_text()
    for name, value in NATIVE_CONSTANTS.items():
        require(const_value(header, name) == value, f"native constant {name}")
    for token in ("case kSndxReloadDataHandlerRva:", "case kSndxCategoryMuteAllHandlerRva:",
                  "case kSndxCategoryFilterAllHandlerRva:"):
        require(token in source, f"tag wiring missing {token}")
    require("sndx_command_id_for_tag" in header and "run_sndx_handler" in header,
            "module entry points")
    require((ROOT / "native/tests/test_sndx_command_handlers.cpp").is_file(), "missing test fixture")
    cmake = (ROOT / "native/CMakeLists.txt").read_text()
    require("core/sndx_command_handlers.cpp" in cmake, "CMake source registration")
    require("add_test(NAME sndx_command_handlers COMMAND test_sndx_command_handlers)" in cmake,
            "CMake test registration")

    print(
        f"OK check_fconcmd_handlers_slot306: 12 body hashes pinned (reload {FUNCTIONS['reload_handler'][2][:12]}…, "
        f"mute/filter {FUNCTIONS['mute_handler'][2][:12]}…); engine cell {ENGINE_CELL:#x}; "
        "vectors 0x8bd0/0x72a8 stride 0xc8; reload walk +0x28/+0x30/0x38/-1/+0x7270/+0x1c50; "
        "tokens true/yes/false/no; strcasecmp (AV6ipCNa4Rw) + _Stoul (zlfEH8FmyUA) NIDs; "
        "native constants + tag wiring verified"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
