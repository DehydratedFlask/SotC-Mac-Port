#!/usr/bin/env python3
"""One-shot E610 apply from git HEAD (E609 base)."""
from __future__ import annotations

import glob
import hashlib
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402


def git(path: str) -> str:
    return subprocess.check_output(["git", "show", f"HEAD:{path}"], text=True)


def write(path: str, text: str) -> None:
    (ROOT / path).write_text(text)


def region_sha() -> str:
    START, SIZE = 0x532B19, 238
    PEEL_MOV_RAX_RDI = bytes.fromhex("488b07")
    PEEL_MOV_R13_8_RAX = bytes.fromhex("48894508")
    PEEL_LEA = bytes.fromhex("488d742410")
    PEEL_MOV = bytes.fromhex("498b5d10")
    PEEL_MOV15 = bytes.fromhex("4d8b7d18")
    PEEL_CMP15 = bytes.fromhex("4c39fb")
    PEEL_JE = bytes.fromhex("741b")
    PEEL_NOP = bytes.fromhex("0f1f00")
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)
    data = bytearray(img.read(START, SIZE))
    data[0x532B6A - START : 0x532B6F - START] = PEEL_LEA
    data[0x532B6F - START] = 0
    data[0x532B70 - START : 0x532B74 - START] = PEEL_MOV
    data[0x532B74 - START : 0x532B78 - START] = PEEL_MOV15
    data[0x532B78 - START : 0x532B7B - START] = PEEL_CMP15
    data[0x532B7B - START : 0x532B7D - START] = PEEL_JE
    data[0x532B7D - START : 0x532B80 - START] = PEEL_NOP
    data[0x532BD6 - START : 0x532BD9 - START] = PEEL_MOV_RAX_RDI
    data[0x532BD9 - START : 0x532BDE - START] = PEEL_MOV_R13_8_RAX
    data[0x532BDE - START : 0x532BE1 - START] = PEEL_MOV_RAX_RDI
    return hashlib.sha256(bytes(data)).hexdigest()


MOV_HPP = """// Init `0x531d60` registry mov r14,[rsp+0x20] OBS `0x532c02..0x532c07` (5 B):
//   mov r14,[rsp+0x20]   ; reload saved r14 after lea rax @ `0x532bfb`
//
// After E609 lea @ `0x532bfb`. NOT modelled: tail @ `0x532c07`.. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseMovR14MemRsp20_532c02_U1Rva = 0x532c02;
inline constexpr std::size_t kInit531d60PostBaseMovR14MemRsp20_532c02_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseMovR14MemRsp20_532c02_U1EndRva = 0x532c07;

struct Init531d60PostBaseMovR14MemRsp20_532c02_U1Cells {
    const void* const* rsp20_in = nullptr;
    void** r14_out = nullptr;
};

struct Init531d60PostBaseMovR14MemRsp20_532c02_U1Result {
    bool ok = false;
    int mov_r14_mem_rsp20_532c02_count = 0;
    void* r14_loaded = nullptr;
};

[[nodiscard]] Init531d60PostBaseMovR14MemRsp20_532c02_U1Result
init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1(
    const Init531d60PostBaseMovR14MemRsp20_532c02_U1Cells& cells = {}) noexcept;

}  // namespace sotc
"""

MOV_CPP = """#include "init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1.hpp"

namespace sotc {

Init531d60PostBaseMovR14MemRsp20_532c02_U1Result init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1(
    const Init531d60PostBaseMovR14MemRsp20_532c02_U1Cells& cells) noexcept {
    Init531d60PostBaseMovR14MemRsp20_532c02_U1Result out{};

    void* r14 = nullptr;
    if (cells.rsp20_in != nullptr) {
        r14 = const_cast<void*>(*cells.rsp20_in);
    }
    out.r14_loaded = r14;
    if (cells.r14_out != nullptr) {
        *cells.r14_out = r14;
    }
    ++out.mov_r14_mem_rsp20_532c02_count;

    // Next: mov edx, imm32 @ 0x532c07 — not modelled here.
    out.ok = (out.mov_r14_mem_rsp20_532c02_count == 1);
    return out;
}

}  // namespace sotc
"""

MOV_CHK = '''#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 registry mov r14,[rsp+0x20] @ 0x532c02 after E609 lea peel."""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532C02
FRAG = 0x532C02
FRAG_SIZE = 5
FRAG_BYTES = bytes.fromhex("4c8b742420")
FRAG_SHA256 = "184ba607e7d1a562ba442f32f4da06acbd9c6337fa5558847f6ac30effbb756f"
END = 0x532C07
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(
        "kInit531d60PostBaseLeaRax2ee2cf8_532bfb_U1EndRva = 0x532c02" in PREV_NATIVE.read_text(),
        "lea end == mov start",
    )
    require(PREV_END == FRAG, "contiguous")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag == FRAG_BYTES, "mov r14,[rsp+0x20]")
    require(img.read(END, 5) == bytes.fromhex("ba72647363"), "next mov edx @ 0x532c07")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1" in nh, "API")
    require("NOT modelled" in nh, "omit tail")
    require("merge_mov_r14_mem_rsp20_532c02_u1_ran" in BODY.read_text(), "body merge")
    require("merge_mov_r14_mem_rsp20_532c02_u1_ran" in REG.read_text(), "reg merge")
    require("kRegistryInit531d60FragmentPinnedRva = 0x532c07" in REG.read_text(), "pin")
    require("init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_mov_r14_mem_rsp20_532c02_u1: frag={FRAG_SHA256[:12]}…; "
        f"end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
'''


def main() -> None:
    write("native/core/init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1.hpp", MOV_HPP)
    write("native/core/init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1.cpp", MOV_CPP)
    write("tools/lift/check_f531d60_post_base_mov_r14_mem_rsp20_532c02_u1.py", MOV_CHK)

    t = git("native/core/init_531d60_post_base_registry_body.hpp")
    t = t.replace("0x532b19..0x532c02` (59 units)", "0x532b19..0x532c07` (60 units)")
    t = t.replace(
        '#include "init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.hpp"\n#include "init_531d60_post_base_mov_r12_rbx_532b41_u1.hpp"',
        '#include "init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.hpp"\n#include "init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1.hpp"\n#include "init_531d60_post_base_mov_r12_rbx_532b41_u1.hpp"',
    )
    t = t.replace("kInit531d60PostBaseRegistryBodySize = 233;", "kInit531d60PostBaseRegistryBodySize = 238;")
    t = t.replace("kInit531d60PostBaseRegistryBodyEndRva = 0x532c02;", "kInit531d60PostBaseRegistryBodyEndRva = 0x532c07;")
    t = t.replace(
        """static_assert(kInit531d60PostBaseLeaRax2ee2cf8_532bfb_U1EndRva ==
                  kInit531d60PostBaseRegistryBodyEndRva);""",
        """static_assert(kInit531d60PostBaseLeaRax2ee2cf8_532bfb_U1EndRva ==
                  kInit531d60PostBaseMovR14MemRsp20_532c02_U1Rva);
static_assert(kInit531d60PostBaseMovR14MemRsp20_532c02_U1EndRva ==
                  kInit531d60PostBaseRegistryBodyEndRva);""",
    )
    t = t.replace(
        "    lea_rax_2ee2cf8_532bfb_u1,\n    count",
        "    lea_rax_2ee2cf8_532bfb_u1,\n    mov_r14_mem_rsp20_532c02_u1,\n    count",
    )
    t = t.replace(
        "    bool merge_lea_rax_2ee2cf8_532bfb_u1_ran = false;\n    const void* stack_rsp",
        "    bool merge_lea_rax_2ee2cf8_532bfb_u1_ran = false;\n    bool merge_mov_r14_mem_rsp20_532c02_u1_ran = false;\n    const void* stack_rsp",
    )
    t = t.replace(
        "    Init531d60PostBaseLeaRax2ee2cf8_532bfb_U1Cells lea_rax_2ee2cf8_532bfb_cells{};\n};",
        "    Init531d60PostBaseLeaRax2ee2cf8_532bfb_U1Cells lea_rax_2ee2cf8_532bfb_cells{};\n    Init531d60PostBaseMovR14MemRsp20_532c02_U1Cells mov_r14_mem_rsp20_532c02_cells{};\n};",
    )
    t = t.replace("std::array<int, 59> unit_counts{};", "std::array<int, 60> unit_counts{};")
    t = t.replace(
        """    int lea_rax_2ee2cf8_532bfb_count = 0;
    void* rax_loader_cell_532bfb = nullptr;
    bool je_532bdb_branch_taken = false;""",
        """    int lea_rax_2ee2cf8_532bfb_count = 0;
    void* rax_loader_cell_532bfb = nullptr;
    int mov_r14_mem_rsp20_532c02_count = 0;
    void* r14_from_rsp20_532c02 = nullptr;
    bool je_532bdb_branch_taken = false;""",
    )
    write("native/core/init_531d60_post_base_registry_body.hpp", t)

    mov_block = """
    if (!hooks.merge_mov_r14_mem_rsp20_532c02_u1_ran) {
        const Init531d60PostBaseMovR14MemRsp20_532c02_U1Result mov_r14_rsp20 =
            init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1(hooks.mov_r14_mem_rsp20_532c02_cells);
        if (!mov_r14_rsp20.ok) {
            return out;
        }
        out.mov_r14_mem_rsp20_532c02_count = mov_r14_rsp20.mov_r14_mem_rsp20_532c02_count;
        out.r14_from_rsp20_532c02 = mov_r14_rsp20.r14_loaded;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::mov_r14_mem_rsp20_532c02_u1)] = 1;
        ++out.unit_executed;
    }

"""
    cpp = git("native/core/init_531d60_post_base_registry_body.cpp")
    needle = "        ++out.unit_executed;\n    }\n\n\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;"
    if needle not in cpp:
        needle = "        ++out.unit_executed;\n    }\n\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;"
    cpp = cpp.replace(
        needle,
        "        ++out.unit_executed;\n    }\n" + mov_block + "\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;",
        1,
    )
    cpp = cpp.replace(
        "    const int expected_lea_rax_2ee2cf8_532bfb =\n        hooks.merge_lea_rax_2ee2cf8_532bfb_u1_ran ? 0 : 1;\n    const int expected_units =",
        "    const int expected_lea_rax_2ee2cf8_532bfb =\n        hooks.merge_lea_rax_2ee2cf8_532bfb_u1_ran ? 0 : 1;\n    const int expected_mov_r14_mem_rsp20_532c02 =\n        hooks.merge_mov_r14_mem_rsp20_532c02_u1_ran ? 0 : 1;\n    const int expected_units =",
    )
    cpp = cpp.replace(
        "expected_mov_r14_mem_rbx_532bf8 + expected_lea_rax_2ee2cf8_532bfb;",
        "expected_mov_r14_mem_rbx_532bf8 + expected_lea_rax_2ee2cf8_532bfb + expected_mov_r14_mem_rsp20_532c02;",
    )
    cpp = cpp.replace(
        "(out.lea_rax_2ee2cf8_532bfb_count == expected_lea_rax_2ee2cf8_532bfb);\n    return out;",
        "(out.lea_rax_2ee2cf8_532bfb_count == expected_lea_rax_2ee2cf8_532bfb) &&\n             (out.mov_r14_mem_rsp20_532c02_count == expected_mov_r14_mem_rsp20_532c02);\n    return out;",
    )
    write("native/core/init_531d60_post_base_registry_body.cpp", cpp)

    reg = git("native/core/registry_init_531d60.hpp")
    reg = reg.replace("kRegistryInit531d60FragmentPinnedRva = 0x532c02;", "kRegistryInit531d60FragmentPinnedRva = 0x532c07;")
    reg = reg.replace(
        "    bool merge_lea_rax_2ee2cf8_532bfb_u1_ran = false;\n};",
        "    bool merge_lea_rax_2ee2cf8_532bfb_u1_ran = false;\n    bool merge_mov_r14_mem_rsp20_532c02_u1_ran = false;\n};",
    )
    write("native/core/registry_init_531d60.hpp", reg)

    cmake = git("native/CMakeLists.txt")
    if "mov_r14_mem_rsp20_532c02" not in cmake:
        cmake = cmake.replace(
            "  core/init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.cpp\n",
            "  core/init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.cpp\n  core/init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1.cpp\n",
        )
    write("native/CMakeLists.txt", cmake)

    sc = git("native/core/appmain_init_ctor_98a90_slice.cpp")
    sc = sc.replace(
        """    if (out.post_base_registry_body.lea_rax_2ee2cf8_532bfb_count == 1) {
        registry_hooks.merge_lea_rax_2ee2cf8_532bfb_u1_ran = true;
    }
    out.remainder = run_appmain_init_remainder_slice""",
        """    if (out.post_base_registry_body.lea_rax_2ee2cf8_532bfb_count == 1) {
        registry_hooks.merge_lea_rax_2ee2cf8_532bfb_u1_ran = true;
    }
    if (out.post_base_registry_body.mov_r14_mem_rsp20_532c02_count == 1) {
        registry_hooks.merge_mov_r14_mem_rsp20_532c02_u1_ran = true;
    }
    out.remainder = run_appmain_init_remainder_slice""",
    )
    write("native/core/appmain_init_ctor_98a90_slice.cpp", sc)

    test = git("native/tests/test_appmain_init_ctor_98a90_slice.cpp")
    test = test.replace(
        "CHECK(kInit531d60PostBaseRegistryBodyUnitCount == 59);",
        "CHECK(kInit531d60PostBaseRegistryBodyUnitCount == 60);",
    )
    test = test.replace(
        "    pbregbody.merge_lea_rax_2ee2cf8_532bfb_u1_ran = false;\n",
        "    pbregbody.merge_lea_rax_2ee2cf8_532bfb_u1_ran = false;\n    pbregbody.merge_mov_r14_mem_rsp20_532c02_u1_ran = false;\n",
    )
    test = test.replace(
        "    CHECK(r.post_base_registry_body.lea_rax_2ee2cf8_532bfb_count == 1);\n    CHECK(r.post_base_registry_body.end_rva == 0x532c02);",
        "    CHECK(r.post_base_registry_body.lea_rax_2ee2cf8_532bfb_count == 1);\n    CHECK(r.post_base_registry_body.mov_r14_mem_rsp20_532c02_count == 1);\n    CHECK(r.post_base_registry_body.end_rva == 0x532c07);",
    )
    write("native/tests/test_appmain_init_ctor_98a90_slice.cpp", test)

    lea_hpp = (ROOT / "native/core/init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.hpp").read_text()
    lea_hpp = lea_hpp.replace(
        "NOT modelled: tail @ `0x532c02`.. G1 PARTIAL.",
        "NOT modelled: E610 mov r14,[rsp+0x20] @ `0x532c02` / tail @ `0x532c07`.. G1 PARTIAL.",
    )
    write("native/core/init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.hpp", lea_hpp)

    lea_cpp = (ROOT / "native/core/init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.cpp").read_text()
    lea_cpp = lea_cpp.replace(
        "    // Next: mov r14,[rsp+0x20] @ 0x532c02 — not modelled here.",
        "    // Next: E610 mov r14,[rsp+0x20] @ 0x532c02 — not modelled here.",
    )
    write("native/core/init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.cpp", lea_cpp)

    for path, old, new in [
        (
            "native/core/init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.hpp",
            "E609 lea rax @ `0x532bfb` / tail @ `0x532c02`.. G1 PARTIAL.",
            "E609 lea @ `0x532bfb` / E610 mov r14,[rsp+0x20] @ `0x532c02` / tail @ `0x532c07`.. G1 PARTIAL.",
        ),
        (
            "native/core/init_531d60_post_base_call_170b0_532bf3_u1.hpp",
            "E609 lea @ `0x532bfb` / tail @ `0x532c02`.. G1 PARTIAL.",
            "E609 lea @ `0x532bfb` / E610 mov @ `0x532c02` / tail @ `0x532c07`.. G1 PARTIAL.",
        ),
        (
            "native/core/init_531d60_post_base_mov_rdi_r12_532bf0_u1.hpp",
            "E609 lea @ `0x532bfb` / tail @ `0x532c02`.. G1 PARTIAL.",
            "E609 lea @ `0x532bfb` / E610 mov @ `0x532c02` / tail @ `0x532c07`.. G1 PARTIAL.",
        ),
        (
            "native/core/init_531d60_post_base_je_532bf0_532be6_u1.hpp",
            "E609 lea @ `0x532bfb` / tail @ `0x532c02`.. G1 PARTIAL.",
            "E609 lea @ `0x532bfb` / E610 mov @ `0x532c02` / tail @ `0x532c07`.. G1 PARTIAL.",
        ),
        (
            "native/core/init_531d60_post_base_je_532bf8_532b50_u1.hpp",
            "E609 lea @ `0x532bfb` or fall-through destroy @",
            "E609 lea @ `0x532bfb` / E610 mov @ `0x532c02` or fall-through destroy @",
        ),
    ]:
        txt = (ROOT / path).read_text()
        if old in txt:
            write(path, txt.replace(old, new))

    rsha = region_sha()
    chk = git("tools/lift/check_f531d60_post_base_registry_body.py")
    chk = chk.replace("START, SIZE, END = 0x532B19, 233, 0x532C02", "START, SIZE, END = 0x532B19, 238, 0x532C07")
    chk = chk.replace(
        'REGION_SHA256 = "a0f0987aad4981b823cb2048f48b92769d02c416c6402cf622c6744c26594de8"',
        f'REGION_SHA256 = "{rsha}"',
    )
    chk = chk.replace(
        '    (0x532BFB,7,"e3c7b3ff7c67bc2dbe7f8acc1b9e244947b103e692e3824baac8a17bf07f669b"),\n]',
        '    (0x532BFB,7,"e3c7b3ff7c67bc2dbe7f8acc1b9e244947b103e692e3824baac8a17bf07f669b"),\n    (0x532C02,5,"184ba607e7d1a562ba442f32f4da06acbd9c6337fa5558847f6ac30effbb756f"),\n]',
    )
    if "MOVR14MEMRSP20=" not in chk:
        chk = chk.replace(
            'LEARAX2EE2CF8=ROOT/"native/core/init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.hpp"\n',
            'LEARAX2EE2CF8=ROOT/"native/core/init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.hpp"\nMOVR14MEMRSP20=ROOT/"native/core/init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1.hpp"\n',
        )
    pairs = [
        ("require(len(UNITS)==59,'units')", "require(len(UNITS)==60,'units')"),
        ("kInit531d60PostBaseRegistryBodyEndRva = 0x532c02", "kInit531d60PostBaseRegistryBodyEndRva = 0x532c07"),
        (
            "require('lea_rax_2ee2cf8_532bfb_u1' in nh,'unit enum lea rax 2ee2cf8 532bfb')",
            "require('lea_rax_2ee2cf8_532bfb_u1' in nh,'unit enum lea rax 2ee2cf8 532bfb')\n    require('mov_r14_mem_rsp20_532c02_u1' in nh,'unit enum mov r14 mem rsp20 532c02')",
        ),
        (
            "'mov_r14_mem_rbx_532bf8_u1' in nh and 'lea_rax_2ee2cf8_532bfb_u1' in nh,'unit enum')",
            "'mov_r14_mem_rbx_532bf8_u1' in nh and 'lea_rax_2ee2cf8_532bfb_u1' in nh and 'mov_r14_mem_rsp20_532c02_u1' in nh,'unit enum')",
        ),
        (
            "require('merge_lea_rax_2ee2cf8_532bfb_u1_ran' in nh,'merge lea rax 2ee2cf8 532bfb')",
            "require('merge_lea_rax_2ee2cf8_532bfb_u1_ran' in nh,'merge lea rax 2ee2cf8 532bfb')\n    require('merge_mov_r14_mem_rsp20_532c02_u1_ran' in nh,'merge mov r14 mem rsp20 532c02')",
        ),
        (
            "require('kInit531d60PostBaseRegistryBodyUnitCount == 59' in TEST.read_text(),'test unit count 59')",
            "require('kInit531d60PostBaseRegistryBodyUnitCount == 60' in TEST.read_text(),'test unit count 60')",
        ),
        (
            "require('kRegistryInit531d60FragmentPinnedRva = 0x532c02' in REG.read_text(),'reg pin')",
            "require('kRegistryInit531d60FragmentPinnedRva = 0x532c07' in REG.read_text(),'reg pin')",
        ),
        (
            "require('merge_lea_rax_2ee2cf8_532bfb_u1_ran' in REG.read_text(),'reg merge lea rax 2ee2cf8 532bfb')",
            "require('merge_lea_rax_2ee2cf8_532bfb_u1_ran' in REG.read_text(),'reg merge lea rax 2ee2cf8 532bfb')\n    require('merge_mov_r14_mem_rsp20_532c02_u1_ran' in REG.read_text(),'reg merge mov r14 mem rsp20 532c02')",
        ),
        (
            "require('merge_lea_rax_2ee2cf8_532bfb_u1_ran' in SC.read_text(),'slice merge lea rax 2ee2cf8 532bfb')",
            "require('merge_lea_rax_2ee2cf8_532bfb_u1_ran' in SC.read_text(),'slice merge lea rax 2ee2cf8 532bfb')\n    require('merge_mov_r14_mem_rsp20_532c02_u1_ran' in SC.read_text(),'slice merge mov r14 mem rsp20 532c02')",
        ),
        (
            "require('end_rva == 0x532c02' in TEST.read_text(),'test end rva 532c02')",
            "require('end_rva == 0x532c07' in TEST.read_text(),'test end rva 532c07')",
        ),
        (
            "require('lea_rax_2ee2cf8_532bfb_count' in TEST.read_text(),'test lea rax 2ee2cf8 532bfb field')",
            "require('lea_rax_2ee2cf8_532bfb_count' in TEST.read_text(),'test lea rax 2ee2cf8 532bfb field')\n    require('mov_r14_mem_rsp20_532c02_count' in TEST.read_text(),'test mov r14 mem rsp20 532c02 field')",
        ),
        (
            "require('merge_lea_rax_2ee2cf8_532bfb_u1_ran' in TEST.read_text(),'test merge lea rax 2ee2cf8 532bfb')",
            "require('merge_lea_rax_2ee2cf8_532bfb_u1_ran' in TEST.read_text(),'test merge lea rax 2ee2cf8 532bfb')\n    require('merge_mov_r14_mem_rsp20_532c02_u1_ran' in TEST.read_text(),'test merge mov r14 mem rsp20 532c02')",
        ),
        (
            "require('init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.cpp' in CMAKE.read_text(),'cmake lea rax 2ee2cf8 532bfb')\n    print('OK check_f531d60_post_base_registry_body end=0x532c02 units=59')",
            "require('init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.cpp' in CMAKE.read_text(),'cmake lea rax 2ee2cf8 532bfb')\n    require('init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1' in NCPP.read_text(),'cpp mov r14 mem rsp20 532c02')\n    require('init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1' in MOVR14MEMRSP20.read_text(),'mov r14 mem rsp20 532c02 hpp')\n    require('init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1.cpp' in CMAKE.read_text(),'cmake mov r14 mem rsp20 532c02')\n    print('OK check_f531d60_post_base_registry_body end=0x532c07 units=60')",
        ),
    ]
    for a, b in pairs:
        chk = chk.replace(a, b)
    write("tools/lift/check_f531d60_post_base_registry_body.py", chk)

    lea_chk = (ROOT / "tools/lift/check_f531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.py").read_text()
    lea_chk = lea_chk.replace(
        'require("kRegistryInit531d60FragmentPinnedRva = 0x532c02" in REG.read_text(), "pin")',
        'require("kRegistryInit531d60FragmentPinnedRva = 0x532c07" in REG.read_text(), "pin")',
    )
    write("tools/lift/check_f531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.py", lea_chk)

    for name in (
        "check_f531d60_post_base_mov_r14_mem_rbx_532bf8_u1.py",
        "check_f531d60_post_base_call_170b0_532bf3_u1.py",
        "check_f531d60_post_base_mov_rdi_r12_532bf0_u1.py",
    ):
        p = ROOT / "tools/lift" / name
        txt = p.read_text()
        txt = txt.replace(
            'require("kRegistryInit531d60FragmentPinnedRva = 0x532c02" in REG.read_text(), "pin")',
            'require("kRegistryInit531d60FragmentPinnedRva = 0x532c07" in REG.read_text(), "pin")',
        )
        p.write_text(txt)

    for fp in glob.glob(str(ROOT / "tools/lift/check_f531d60_post_base_*.py")):
        p = Path(fp)
        if p.name in (
            "check_f531d60_post_base_registry_body.py",
            "check_f531d60_post_base_call_170b0_532bf3_u1.py",
            "check_f531d60_post_base_mov_rdi_r12_532bf0_u1.py",
            "check_f531d60_post_base_mov_r14_mem_rbx_532bf8_u1.py",
            "check_f531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.py",
            "check_f531d60_post_base_mov_r14_mem_rsp20_532c02_u1.py",
        ):
            continue
        txt = p.read_text()
        if "kRegistryInit531d60FragmentPinnedRva = 0x532c02" in txt:
            txt = txt.replace(
                "kRegistryInit531d60FragmentPinnedRva = 0x532c02",
                "kRegistryInit531d60FragmentPinnedRva = 0x532c07",
            )
            p.write_text(txt)

    print(f"e610_apply_once: OK region={rsha}")


if __name__ == "__main__":
    main()
