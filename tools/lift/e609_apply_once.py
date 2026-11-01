#!/usr/bin/env python3
"""One-shot E609 apply from git HEAD (E608 base)."""
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
    START, SIZE = 0x532B19, 233
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


LEA_HPP = """// Init `0x531d60` registry lea rax OBS `0x532bfb..0x532c02` (7 B):
//   lea rax,[0x2ee2cf8]   ; ResourcePool loader cell after mov [rbx],r14 @ `0x532bf8`
//
// After E608 mov @ `0x532bf8`. NOT modelled: tail @ `0x532c02`.. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseLeaRax2ee2cf8_532bfb_U1Rva = 0x532bfb;
inline constexpr std::size_t kInit531d60PostBaseLeaRax2ee2cf8_532bfb_U1Size = 7;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRax2ee2cf8_532bfb_U1EndRva = 0x532c02;
inline constexpr std::uint64_t kInit531d60PostBaseLeaRax2ee2cf8_532bfb_U1TargetRva = 0x2ee2cf8;

struct Init531d60PostBaseLeaRax2ee2cf8_532bfb_U1Cells {
    void** rax_out = nullptr;
};

struct Init531d60PostBaseLeaRax2ee2cf8_532bfb_U1Result {
    bool ok = false;
    int lea_rax_2ee2cf8_532bfb_count = 0;
    void* rax = nullptr;
};

[[nodiscard]] Init531d60PostBaseLeaRax2ee2cf8_532bfb_U1Result
init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1(
    const Init531d60PostBaseLeaRax2ee2cf8_532bfb_U1Cells& cells = {}) noexcept;

}  // namespace sotc
"""

LEA_CPP = """#include "init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.hpp"

namespace sotc {

Init531d60PostBaseLeaRax2ee2cf8_532bfb_U1Result init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1(
    const Init531d60PostBaseLeaRax2ee2cf8_532bfb_U1Cells& cells) noexcept {
    Init531d60PostBaseLeaRax2ee2cf8_532bfb_U1Result out{};

    out.rax = reinterpret_cast<void*>(
        static_cast<std::uintptr_t>(kInit531d60PostBaseLeaRax2ee2cf8_532bfb_U1TargetRva));
    if (cells.rax_out != nullptr) {
        *cells.rax_out = out.rax;
    }
    ++out.lea_rax_2ee2cf8_532bfb_count;

    // Next: mov r14,[rsp+0x20] @ 0x532c02 — not modelled here.
    out.ok = (out.lea_rax_2ee2cf8_532bfb_count == 1);
    return out;
}

}  // namespace sotc
"""

LEA_CHK = '''#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 registry lea rax @ 0x532bfb after E608 mov peel."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532BFB
FRAG = 0x532BFB
FRAG_SIZE = 7
FRAG_BYTES = bytes.fromhex("488d05f6009b02")
FRAG_SHA256 = "e3c7b3ff7c67bc2dbe7f8acc1b9e244947b103e692e3824baac8a17bf07f669b"
END = 0x532C02
TARGET = 0x2EE2CF8
NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.hpp"
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
        "kInit531d60PostBaseMovR14MemRbx_532bf8_U1EndRva = 0x532bfb" in PREV_NATIVE.read_text(),
        "mov end == lea start",
    )
    require(PREV_END == FRAG, "contiguous")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag == FRAG_BYTES, "lea rax,[0x2ee2cf8]")
    rel = struct.unpack("<i", frag[3:7])[0]
    require(FRAG + 7 + rel == TARGET, "lea target 0x2ee2cf8")
    require(img.read(END, 5) == bytes.fromhex("4c8b742420"), "next mov r14,[rsp+0x20]")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1" in nh, "API")
    require("NOT modelled" in nh, "omit tail")
    require("merge_lea_rax_2ee2cf8_532bfb_u1_ran" in BODY.read_text(), "body merge")
    require("merge_lea_rax_2ee2cf8_532bfb_u1_ran" in REG.read_text(), "reg merge")
    require("kRegistryInit531d60FragmentPinnedRva = 0x532c02" in REG.read_text(), "pin")
    require("init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_lea_rax_2ee2cf8_532bfb_u1: frag={FRAG_SHA256[:12]}…; "
        f"lea→{TARGET:#x}; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
'''


def main() -> None:
    write("native/core/init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.hpp", LEA_HPP)
    write("native/core/init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.cpp", LEA_CPP)
    write("tools/lift/check_f531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.py", LEA_CHK)

    t = git("native/core/init_531d60_post_base_registry_body.hpp")
    t = t.replace("0x532b19..0x532bfb` (58 units)", "0x532b19..0x532c02` (59 units)")
    t = t.replace(
        '#include "init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.hpp"\n#include "init_531d60_post_base_mov_r12_rbx_532b41_u1.hpp"',
        '#include "init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.hpp"\n#include "init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.hpp"\n#include "init_531d60_post_base_mov_r12_rbx_532b41_u1.hpp"',
    )
    t = t.replace("kInit531d60PostBaseRegistryBodySize = 226;", "kInit531d60PostBaseRegistryBodySize = 233;")
    t = t.replace("kInit531d60PostBaseRegistryBodyEndRva = 0x532bfb;", "kInit531d60PostBaseRegistryBodyEndRva = 0x532c02;")
    t = t.replace(
        """static_assert(kInit531d60PostBaseMovR14MemRbx_532bf8_U1EndRva ==
                  kInit531d60PostBaseRegistryBodyEndRva);""",
        """static_assert(kInit531d60PostBaseMovR14MemRbx_532bf8_U1EndRva ==
                  kInit531d60PostBaseLeaRax2ee2cf8_532bfb_U1Rva);
static_assert(kInit531d60PostBaseLeaRax2ee2cf8_532bfb_U1EndRva ==
                  kInit531d60PostBaseRegistryBodyEndRva);""",
    )
    t = t.replace(
        "    mov_r14_mem_rbx_532bf8_u1,\n    count",
        "    mov_r14_mem_rbx_532bf8_u1,\n    lea_rax_2ee2cf8_532bfb_u1,\n    count",
    )
    t = t.replace(
        "    bool merge_mov_r14_mem_rbx_532bf8_u1_ran = false;\n    const void* stack_rsp",
        "    bool merge_mov_r14_mem_rbx_532bf8_u1_ran = false;\n    bool merge_lea_rax_2ee2cf8_532bfb_u1_ran = false;\n    const void* stack_rsp",
    )
    t = t.replace(
        "    Init531d60PostBaseMovR14MemRbx_532bf8_U1Cells mov_r14_mem_rbx_532bf8_cells{};\n};",
        "    Init531d60PostBaseMovR14MemRbx_532bf8_U1Cells mov_r14_mem_rbx_532bf8_cells{};\n    Init531d60PostBaseLeaRax2ee2cf8_532bfb_U1Cells lea_rax_2ee2cf8_532bfb_cells{};\n};",
    )
    t = t.replace("std::array<int, 58> unit_counts{};", "std::array<int, 59> unit_counts{};")
    t = t.replace(
        """    int mov_r14_mem_rbx_532bf8_count = 0;
    void* stored_r14_cell_532bf8 = nullptr;
    bool je_532bdb_branch_taken = false;""",
        """    int mov_r14_mem_rbx_532bf8_count = 0;
    void* stored_r14_cell_532bf8 = nullptr;
    int lea_rax_2ee2cf8_532bfb_count = 0;
    void* rax_loader_cell_532bfb = nullptr;
    bool je_532bdb_branch_taken = false;""",
    )
    write("native/core/init_531d60_post_base_registry_body.hpp", t)

    lea_block = """
    if (!hooks.merge_lea_rax_2ee2cf8_532bfb_u1_ran) {
        const Init531d60PostBaseLeaRax2ee2cf8_532bfb_U1Result lea_loader =
            init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1(hooks.lea_rax_2ee2cf8_532bfb_cells);
        if (!lea_loader.ok) {
            return out;
        }
        out.lea_rax_2ee2cf8_532bfb_count = lea_loader.lea_rax_2ee2cf8_532bfb_count;
        out.rax_loader_cell_532bfb = lea_loader.rax;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::lea_rax_2ee2cf8_532bfb_u1)] = 1;
        ++out.unit_executed;
    }

"""
    cpp = git("native/core/init_531d60_post_base_registry_body.cpp")
    needle = "        ++out.unit_executed;\n    }\n\n\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;"
    if needle not in cpp:
        needle = "        ++out.unit_executed;\n    }\n\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;"
    cpp = cpp.replace(
        needle,
        "        ++out.unit_executed;\n    }\n" + lea_block + "\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;",
        1,
    )
    cpp = cpp.replace(
        "    const int expected_mov_r14_mem_rbx_532bf8 =\n        hooks.merge_mov_r14_mem_rbx_532bf8_u1_ran ? 0 : 1;\n    const int expected_units =",
        "    const int expected_mov_r14_mem_rbx_532bf8 =\n        hooks.merge_mov_r14_mem_rbx_532bf8_u1_ran ? 0 : 1;\n    const int expected_lea_rax_2ee2cf8_532bfb =\n        hooks.merge_lea_rax_2ee2cf8_532bfb_u1_ran ? 0 : 1;\n    const int expected_units =",
    )
    cpp = cpp.replace(
        "expected_call_170b0_532bf3 + expected_mov_r14_mem_rbx_532bf8;",
        "expected_call_170b0_532bf3 + expected_mov_r14_mem_rbx_532bf8 + expected_lea_rax_2ee2cf8_532bfb;",
    )
    cpp = cpp.replace(
        "(out.mov_r14_mem_rbx_532bf8_count == expected_mov_r14_mem_rbx_532bf8);\n    return out;",
        "(out.mov_r14_mem_rbx_532bf8_count == expected_mov_r14_mem_rbx_532bf8) &&\n             (out.lea_rax_2ee2cf8_532bfb_count == expected_lea_rax_2ee2cf8_532bfb);\n    return out;",
    )
    write("native/core/init_531d60_post_base_registry_body.cpp", cpp)

    reg = git("native/core/registry_init_531d60.hpp")
    reg = reg.replace("kRegistryInit531d60FragmentPinnedRva = 0x532bfb;", "kRegistryInit531d60FragmentPinnedRva = 0x532c02;")
    reg = reg.replace(
        "    bool merge_mov_r14_mem_rbx_532bf8_u1_ran = false;\n};",
        "    bool merge_mov_r14_mem_rbx_532bf8_u1_ran = false;\n    bool merge_lea_rax_2ee2cf8_532bfb_u1_ran = false;\n};",
    )
    write("native/core/registry_init_531d60.hpp", reg)

    cmake = git("native/CMakeLists.txt")
    if "lea_rax_2ee2cf8_532bfb" not in cmake:
        cmake = cmake.replace(
            "  core/init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.cpp\n",
            "  core/init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.cpp\n  core/init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.cpp\n",
        )
    write("native/CMakeLists.txt", cmake)

    sc = git("native/core/appmain_init_ctor_98a90_slice.cpp")
    sc = sc.replace(
        """    if (out.post_base_registry_body.mov_r14_mem_rbx_532bf8_count == 1) {
        registry_hooks.merge_mov_r14_mem_rbx_532bf8_u1_ran = true;
    }
    out.remainder = run_appmain_init_remainder_slice""",
        """    if (out.post_base_registry_body.mov_r14_mem_rbx_532bf8_count == 1) {
        registry_hooks.merge_mov_r14_mem_rbx_532bf8_u1_ran = true;
    }
    if (out.post_base_registry_body.lea_rax_2ee2cf8_532bfb_count == 1) {
        registry_hooks.merge_lea_rax_2ee2cf8_532bfb_u1_ran = true;
    }
    out.remainder = run_appmain_init_remainder_slice""",
    )
    write("native/core/appmain_init_ctor_98a90_slice.cpp", sc)

    test = git("native/tests/test_appmain_init_ctor_98a90_slice.cpp")
    test = test.replace(
        "CHECK(kInit531d60PostBaseRegistryBodyUnitCount == 58);",
        "CHECK(kInit531d60PostBaseRegistryBodyUnitCount == 59);",
    )
    test = test.replace(
        "    pbregbody.merge_mov_r14_mem_rbx_532bf8_u1_ran = false;\n",
        "    pbregbody.merge_mov_r14_mem_rbx_532bf8_u1_ran = false;\n    pbregbody.merge_lea_rax_2ee2cf8_532bfb_u1_ran = false;\n",
    )
    test = test.replace(
        "    CHECK(r.post_base_registry_body.mov_r14_mem_rbx_532bf8_count == 1);\n    CHECK(r.post_base_registry_body.end_rva == 0x532bfb);",
        "    CHECK(r.post_base_registry_body.mov_r14_mem_rbx_532bf8_count == 1);\n    CHECK(r.post_base_registry_body.lea_rax_2ee2cf8_532bfb_count == 1);\n    CHECK(r.post_base_registry_body.end_rva == 0x532c02);",
    )
    write("native/tests/test_appmain_init_ctor_98a90_slice.cpp", test)

    mov_r14_hpp = (ROOT / "native/core/init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.hpp").read_text()
    mov_r14_hpp = mov_r14_hpp.replace(
        "After E607 call @ `0x532bf3`. NOT modelled: tail @ `0x532bfb`.. G1 PARTIAL.",
        "After E607 call @ `0x532bf3`. NOT modelled: E609 lea rax @ `0x532bfb` / tail @ `0x532c02`.. G1 PARTIAL.",
    )
    write("native/core/init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.hpp", mov_r14_hpp)

    mov_r14_cpp = (ROOT / "native/core/init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.cpp").read_text()
    mov_r14_cpp = mov_r14_cpp.replace(
        "    // Next: lea rax,[rip+disp] @ 0x532bfb — not modelled here.",
        "    // Next: E609 lea rax,[0x2ee2cf8] @ 0x532bfb — not modelled here.",
    )
    write("native/core/init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.cpp", mov_r14_cpp)

    call_hpp = (ROOT / "native/core/init_531d60_post_base_call_170b0_532bf3_u1.hpp").read_text()
    call_hpp = call_hpp.replace(
        "E608 mov [rbx],r14 @ `0x532bf8` / tail @ `0x532bfb`.. G1 PARTIAL.",
        "E608 mov @ `0x532bf8` / E609 lea @ `0x532bfb` / tail @ `0x532c02`.. G1 PARTIAL.",
    )
    write("native/core/init_531d60_post_base_call_170b0_532bf3_u1.hpp", call_hpp)

    mov_rdi_hpp = (ROOT / "native/core/init_531d60_post_base_mov_rdi_r12_532bf0_u1.hpp").read_text()
    mov_rdi_hpp = mov_rdi_hpp.replace(
        "E608 mov @ `0x532bf8` / tail @ `0x532bfb`.. G1 PARTIAL.",
        "E608 mov @ `0x532bf8` / E609 lea @ `0x532bfb` / tail @ `0x532c02`.. G1 PARTIAL.",
    )
    write("native/core/init_531d60_post_base_mov_rdi_r12_532bf0_u1.hpp", mov_rdi_hpp)

    je_hpp = (ROOT / "native/core/init_531d60_post_base_je_532bf0_532be6_u1.hpp").read_text()
    je_hpp = je_hpp.replace(
        "NOT modelled: E608 mov @ `0x532bf8` / tail @ `0x532bfb`.. G1 PARTIAL.",
        "NOT modelled: E608 mov @ `0x532bf8` / E609 lea @ `0x532bfb` / tail @ `0x532c02`.. G1 PARTIAL.",
    )
    write("native/core/init_531d60_post_base_je_532bf0_532be6_u1.hpp", je_hpp)

    je_null_hpp = (ROOT / "native/core/init_531d60_post_base_je_532bf8_532b50_u1.hpp").read_text()
    je_null_hpp = je_null_hpp.replace(
        "NOT modelled: E608 `mov [rbx],r14` @ `0x532bf8` or fall-through destroy @",
        "NOT modelled: E608 mov @ `0x532bf8` / E609 lea @ `0x532bfb` or fall-through destroy @",
    )
    write("native/core/init_531d60_post_base_je_532bf8_532b50_u1.hpp", je_null_hpp)

    rsha = region_sha()
    chk = git("tools/lift/check_f531d60_post_base_registry_body.py")
    chk = chk.replace("START, SIZE, END = 0x532B19, 226, 0x532BFB", "START, SIZE, END = 0x532B19, 233, 0x532C02")
    chk = chk.replace(
        'REGION_SHA256 = "5ecc31a8fee03fe8869127ceb1804dade33f661cf33f72c7719573a6541359a7"',
        f'REGION_SHA256 = "{rsha}"',
    )
    chk = chk.replace(
        '    (0x532BF8,3,"7241225d5359852087b3106b7249779b8871e6d8576b01678db3873f1ea38309"),\n]',
        '    (0x532BF8,3,"7241225d5359852087b3106b7249779b8871e6d8576b01678db3873f1ea38309"),\n    (0x532BFB,7,"e3c7b3ff7c67bc2dbe7f8acc1b9e244947b103e692e3824baac8a17bf07f669b"),\n]',
    )
    if "LEARAX2EE2CF8=" not in chk:
        chk = chk.replace(
            'MOVR14MEMRBX=ROOT/"native/core/init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.hpp"\n',
            'MOVR14MEMRBX=ROOT/"native/core/init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.hpp"\nLEARAX2EE2CF8=ROOT/"native/core/init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.hpp"\n',
        )
    pairs = [
        ("require(len(UNITS)==58,'units')", "require(len(UNITS)==59,'units')"),
        ("kInit531d60PostBaseRegistryBodyEndRva = 0x532bfb", "kInit531d60PostBaseRegistryBodyEndRva = 0x532c02"),
        (
            "require('mov_r14_mem_rbx_532bf8_u1' in nh,'unit enum mov r14 mem rbx 532bf8')",
            "require('mov_r14_mem_rbx_532bf8_u1' in nh,'unit enum mov r14 mem rbx 532bf8')\n    require('lea_rax_2ee2cf8_532bfb_u1' in nh,'unit enum lea rax 2ee2cf8 532bfb')",
        ),
        (
            "'call_170c0_532beb_u1' in nh and 'mov_rdi_r12_532bf0_u1' in nh and 'call_170b0_532bf3_u1' in nh and 'mov_r14_mem_rbx_532bf8_u1' in nh,'unit enum')",
            "'call_170c0_532beb_u1' in nh and 'mov_rdi_r12_532bf0_u1' in nh and 'call_170b0_532bf3_u1' in nh and 'mov_r14_mem_rbx_532bf8_u1' in nh and 'lea_rax_2ee2cf8_532bfb_u1' in nh,'unit enum')",
        ),
        (
            "require('merge_mov_r14_mem_rbx_532bf8_u1_ran' in nh,'merge mov r14 mem rbx 532bf8')",
            "require('merge_mov_r14_mem_rbx_532bf8_u1_ran' in nh,'merge mov r14 mem rbx 532bf8')\n    require('merge_lea_rax_2ee2cf8_532bfb_u1_ran' in nh,'merge lea rax 2ee2cf8 532bfb')",
        ),
        (
            "require('kInit531d60PostBaseRegistryBodyUnitCount == 58' in TEST.read_text(),'test unit count 58')",
            "require('kInit531d60PostBaseRegistryBodyUnitCount == 59' in TEST.read_text(),'test unit count 59')",
        ),
        (
            "require('kRegistryInit531d60FragmentPinnedRva = 0x532bfb' in REG.read_text(),'reg pin')",
            "require('kRegistryInit531d60FragmentPinnedRva = 0x532c02' in REG.read_text(),'reg pin')",
        ),
        (
            "require('merge_mov_r14_mem_rbx_532bf8_u1_ran' in REG.read_text(),'reg merge mov r14 mem rbx 532bf8')",
            "require('merge_mov_r14_mem_rbx_532bf8_u1_ran' in REG.read_text(),'reg merge mov r14 mem rbx 532bf8')\n    require('merge_lea_rax_2ee2cf8_532bfb_u1_ran' in REG.read_text(),'reg merge lea rax 2ee2cf8 532bfb')",
        ),
        (
            "require('merge_mov_r14_mem_rbx_532bf8_u1_ran' in SC.read_text(),'slice merge mov r14 mem rbx 532bf8')",
            "require('merge_mov_r14_mem_rbx_532bf8_u1_ran' in SC.read_text(),'slice merge mov r14 mem rbx 532bf8')\n    require('merge_lea_rax_2ee2cf8_532bfb_u1_ran' in SC.read_text(),'slice merge lea rax 2ee2cf8 532bfb')",
        ),
        (
            "require('end_rva == 0x532bfb' in TEST.read_text(),'test end rva 532bfb')",
            "require('end_rva == 0x532c02' in TEST.read_text(),'test end rva 532c02')",
        ),
        (
            "require('mov_r14_mem_rbx_532bf8_count' in TEST.read_text(),'test mov r14 mem rbx 532bf8 field')",
            "require('mov_r14_mem_rbx_532bf8_count' in TEST.read_text(),'test mov r14 mem rbx 532bf8 field')\n    require('lea_rax_2ee2cf8_532bfb_count' in TEST.read_text(),'test lea rax 2ee2cf8 532bfb field')",
        ),
        (
            "require('merge_mov_r14_mem_rbx_532bf8_u1_ran' in TEST.read_text(),'test merge mov r14 mem rbx 532bf8')",
            "require('merge_mov_r14_mem_rbx_532bf8_u1_ran' in TEST.read_text(),'test merge mov r14 mem rbx 532bf8')\n    require('merge_lea_rax_2ee2cf8_532bfb_u1_ran' in TEST.read_text(),'test merge lea rax 2ee2cf8 532bfb')",
        ),
        (
            "require('init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.cpp' in CMAKE.read_text(),'cmake mov r14 mem rbx 532bf8')\n    print('OK check_f531d60_post_base_registry_body end=0x532bfb units=58')",
            "require('init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.cpp' in CMAKE.read_text(),'cmake mov r14 mem rbx 532bf8')\n    require('init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1' in NCPP.read_text(),'cpp lea rax 2ee2cf8 532bfb')\n    require('init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1' in LEARAX2EE2CF8.read_text(),'lea rax 2ee2cf8 532bfb hpp')\n    require('init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.cpp' in CMAKE.read_text(),'cmake lea rax 2ee2cf8 532bfb')\n    print('OK check_f531d60_post_base_registry_body end=0x532c02 units=59')",
        ),
    ]
    for a, b in pairs:
        chk = chk.replace(a, b)
    write("tools/lift/check_f531d60_post_base_registry_body.py", chk)

    mov_r14_chk = (ROOT / "tools/lift/check_f531d60_post_base_mov_r14_mem_rbx_532bf8_u1.py").read_text()
    mov_r14_chk = mov_r14_chk.replace(
        'require("kRegistryInit531d60FragmentPinnedRva = 0x532bfb" in REG.read_text(), "pin")',
        'require("kRegistryInit531d60FragmentPinnedRva = 0x532c02" in REG.read_text(), "pin")',
    )
    write("tools/lift/check_f531d60_post_base_mov_r14_mem_rbx_532bf8_u1.py", mov_r14_chk)

    call170b0_chk = (ROOT / "tools/lift/check_f531d60_post_base_call_170b0_532bf3_u1.py").read_text()
    call170b0_chk = call170b0_chk.replace(
        'require("kRegistryInit531d60FragmentPinnedRva = 0x532bfb" in REG.read_text(), "pin")',
        'require("kRegistryInit531d60FragmentPinnedRva = 0x532c02" in REG.read_text(), "pin")',
    )
    write("tools/lift/check_f531d60_post_base_call_170b0_532bf3_u1.py", call170b0_chk)

    mov_chk = (ROOT / "tools/lift/check_f531d60_post_base_mov_rdi_r12_532bf0_u1.py").read_text()
    mov_chk = mov_chk.replace(
        'require("kRegistryInit531d60FragmentPinnedRva = 0x532bfb" in REG.read_text(), "pin")',
        'require("kRegistryInit531d60FragmentPinnedRva = 0x532c02" in REG.read_text(), "pin")',
    )
    write("tools/lift/check_f531d60_post_base_mov_rdi_r12_532bf0_u1.py", mov_chk)

    for fp in glob.glob(str(ROOT / "tools/lift/check_f531d60_post_base_*.py")):
        p = Path(fp)
        if p.name in (
            "check_f531d60_post_base_registry_body.py",
            "check_f531d60_post_base_call_170b0_532bf3_u1.py",
            "check_f531d60_post_base_mov_rdi_r12_532bf0_u1.py",
            "check_f531d60_post_base_mov_r14_mem_rbx_532bf8_u1.py",
            "check_f531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.py",
        ):
            continue
        txt = p.read_text()
        if "kRegistryInit531d60FragmentPinnedRva = 0x532bfb" in txt:
            txt = txt.replace(
                "kRegistryInit531d60FragmentPinnedRva = 0x532bfb",
                "kRegistryInit531d60FragmentPinnedRva = 0x532c02",
            )
            p.write_text(txt)

    print(f"e609_apply_once: OK region={rsha}")


if __name__ == "__main__":
    main()
