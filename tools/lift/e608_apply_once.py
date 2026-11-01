#!/usr/bin/env python3
"""One-shot E608 apply from git HEAD (E607 base)."""
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
    START, SIZE = 0x532B19, 226
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


def main() -> None:
    t = git("native/core/init_531d60_post_base_registry_body.hpp")
    t = t.replace("0x532b19..0x532bf8` (57 units)", "0x532b19..0x532bfb` (58 units)")
    t = t.replace(
        '#include "init_531d60_post_base_call_170b0_532bf3_u1.hpp"\n#include "init_531d60_post_base_mov_r12_rbx_532b41_u1.hpp"',
        '#include "init_531d60_post_base_call_170b0_532bf3_u1.hpp"\n#include "init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.hpp"\n#include "init_531d60_post_base_mov_r12_rbx_532b41_u1.hpp"',
    )
    t = t.replace("kInit531d60PostBaseRegistryBodySize = 223;", "kInit531d60PostBaseRegistryBodySize = 226;")
    t = t.replace("kInit531d60PostBaseRegistryBodyEndRva = 0x532bf8;", "kInit531d60PostBaseRegistryBodyEndRva = 0x532bfb;")
    t = t.replace(
        """static_assert(kInit531d60PostBaseCall170b0_532bf3_U1EndRva ==
                  kInit531d60PostBaseRegistryBodyEndRva);""",
        """static_assert(kInit531d60PostBaseCall170b0_532bf3_U1EndRva ==
                  kInit531d60PostBaseMovR14MemRbx_532bf8_U1Rva);
static_assert(kInit531d60PostBaseMovR14MemRbx_532bf8_U1EndRva ==
                  kInit531d60PostBaseRegistryBodyEndRva);""",
    )
    t = t.replace(
        "    call_170b0_532bf3_u1,\n    count",
        "    call_170b0_532bf3_u1,\n    mov_r14_mem_rbx_532bf8_u1,\n    count",
    )
    t = t.replace(
        "    bool merge_call_170b0_532bf3_u1_ran = false;\n    const void* stack_rsp",
        "    bool merge_call_170b0_532bf3_u1_ran = false;\n    bool merge_mov_r14_mem_rbx_532bf8_u1_ran = false;\n    const void* stack_rsp",
    )
    t = t.replace(
        "    Init531d60PostBaseCall170b0_532bf3_U1Cells call_170b0_532bf3_cells{};\n};",
        "    Init531d60PostBaseCall170b0_532bf3_U1Cells call_170b0_532bf3_cells{};\n    Init531d60PostBaseMovR14MemRbx_532bf8_U1Cells mov_r14_mem_rbx_532bf8_cells{};\n};",
    )
    t = t.replace("std::array<int, 57> unit_counts{};", "std::array<int, 58> unit_counts{};")
    t = t.replace(
        """    int call_170b0_532bf3_count = 0;
    bool je_532bdb_branch_taken = false;""",
        """    int call_170b0_532bf3_count = 0;
    int mov_r14_mem_rbx_532bf8_count = 0;
    void* stored_r14_cell_532bf8 = nullptr;
    bool je_532bdb_branch_taken = false;""",
    )
    write("native/core/init_531d60_post_base_registry_body.hpp", t)

    mov_block = """
    if (!hooks.merge_mov_r14_mem_rbx_532bf8_u1_ran) {
        Init531d60PostBaseMovR14MemRbx_532bf8_U1Cells cells = hooks.mov_r14_mem_rbx_532bf8_cells;
        if (cells.r14_in == nullptr) {
            cells.r14_in = hooks.r14_in;
        }
        const Init531d60PostBaseMovR14MemRbx_532bf8_U1Result mov_store =
            init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1(cells);
        if (!mov_store.ok) {
            return out;
        }
        out.mov_r14_mem_rbx_532bf8_count = mov_store.mov_r14_mem_rbx_532bf8_count;
        out.stored_r14_cell_532bf8 = mov_store.stored;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::mov_r14_mem_rbx_532bf8_u1)] = 1;
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
        "    const int expected_call_170b0_532bf3 =\n        hooks.merge_call_170b0_532bf3_u1_ran ? 0 : 1;\n    const int expected_units =",
        "    const int expected_call_170b0_532bf3 =\n        hooks.merge_call_170b0_532bf3_u1_ran ? 0 : 1;\n    const int expected_mov_r14_mem_rbx_532bf8 =\n        hooks.merge_mov_r14_mem_rbx_532bf8_u1_ran ? 0 : 1;\n    const int expected_units =",
    )
    cpp = cpp.replace(
        "expected_mov_rdi_r12_532bf0 + expected_call_170b0_532bf3;",
        "expected_mov_rdi_r12_532bf0 + expected_call_170b0_532bf3 + expected_mov_r14_mem_rbx_532bf8;",
    )
    cpp = cpp.replace(
        "(out.call_170b0_532bf3_count == expected_call_170b0_532bf3);\n    return out;",
        "(out.call_170b0_532bf3_count == expected_call_170b0_532bf3) &&\n             (out.mov_r14_mem_rbx_532bf8_count == expected_mov_r14_mem_rbx_532bf8);\n    return out;",
    )
    write("native/core/init_531d60_post_base_registry_body.cpp", cpp)

    reg = git("native/core/registry_init_531d60.hpp")
    reg = reg.replace("kRegistryInit531d60FragmentPinnedRva = 0x532bf8;", "kRegistryInit531d60FragmentPinnedRva = 0x532bfb;")
    reg = reg.replace(
        "    bool merge_call_170b0_532bf3_u1_ran = false;\n};",
        "    bool merge_call_170b0_532bf3_u1_ran = false;\n    bool merge_mov_r14_mem_rbx_532bf8_u1_ran = false;\n};",
    )
    write("native/core/registry_init_531d60.hpp", reg)

    cmake = git("native/CMakeLists.txt")
    if "mov_r14_mem_rbx_532bf8" not in cmake:
        cmake = cmake.replace(
            "  core/init_531d60_post_base_call_170b0_532bf3_u1.cpp\n",
            "  core/init_531d60_post_base_call_170b0_532bf3_u1.cpp\n  core/init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.cpp\n",
        )
    write("native/CMakeLists.txt", cmake)

    sc = git("native/core/appmain_init_ctor_98a90_slice.cpp")
    sc = sc.replace(
        """    if (out.post_base_registry_body.call_170b0_532bf3_count == 1) {
        registry_hooks.merge_call_170b0_532bf3_u1_ran = true;
    }
    out.remainder = run_appmain_init_remainder_slice""",
        """    if (out.post_base_registry_body.call_170b0_532bf3_count == 1) {
        registry_hooks.merge_call_170b0_532bf3_u1_ran = true;
    }
    if (out.post_base_registry_body.mov_r14_mem_rbx_532bf8_count == 1) {
        registry_hooks.merge_mov_r14_mem_rbx_532bf8_u1_ran = true;
    }
    out.remainder = run_appmain_init_remainder_slice""",
    )
    write("native/core/appmain_init_ctor_98a90_slice.cpp", sc)

    test = git("native/tests/test_appmain_init_ctor_98a90_slice.cpp")
    test = test.replace(
        "CHECK(kInit531d60PostBaseRegistryBodyUnitCount == 57);",
        "CHECK(kInit531d60PostBaseRegistryBodyUnitCount == 58);",
    )
    test = test.replace(
        "    pbregbody.merge_call_170b0_532bf3_u1_ran = false;\n",
        "    pbregbody.merge_call_170b0_532bf3_u1_ran = false;\n    pbregbody.merge_mov_r14_mem_rbx_532bf8_u1_ran = false;\n",
    )
    test = test.replace(
        "    CHECK(r.post_base_registry_body.call_170b0_532bf3_count == 1);\n    CHECK(r.post_base_registry_body.end_rva == 0x532bf8);",
        "    CHECK(r.post_base_registry_body.call_170b0_532bf3_count == 1);\n    CHECK(r.post_base_registry_body.mov_r14_mem_rbx_532bf8_count == 1);\n    CHECK(r.post_base_registry_body.end_rva == 0x532bfb);",
    )
    write("native/tests/test_appmain_init_ctor_98a90_slice.cpp", test)

    call_hpp = (ROOT / "native/core/init_531d60_post_base_call_170b0_532bf3_u1.hpp").read_text()
    call_hpp = call_hpp.replace(
        "After E606 mov @ `0x532bf0`. NOT modelled: tail @ `0x532bf8`.. G1 PARTIAL.",
        "After E606 mov @ `0x532bf0`. NOT modelled: E608 mov [rbx],r14 @ `0x532bf8` / tail @ `0x532bfb`.. G1 PARTIAL.",
    )
    write("native/core/init_531d60_post_base_call_170b0_532bf3_u1.hpp", call_hpp)

    mov_rdi_hpp = (ROOT / "native/core/init_531d60_post_base_mov_rdi_r12_532bf0_u1.hpp").read_text()
    mov_rdi_hpp = mov_rdi_hpp.replace(
        "NOT modelled: E607 call 0x170b0 @ `0x532bf3` / tail @ `0x532bf8`.. G1 PARTIAL.",
        "NOT modelled: E607 call @ `0x532bf3` / E608 mov @ `0x532bf8` / tail @ `0x532bfb`.. G1 PARTIAL.",
    )
    write("native/core/init_531d60_post_base_mov_rdi_r12_532bf0_u1.hpp", mov_rdi_hpp)

    je_hpp = (ROOT / "native/core/init_531d60_post_base_je_532bf0_532be6_u1.hpp").read_text()
    je_hpp = je_hpp.replace(
        "NOT modelled: tail @ `0x532bf8`.. G1 PARTIAL.",
        "NOT modelled: E608 mov @ `0x532bf8` / tail @ `0x532bfb`.. G1 PARTIAL.",
    )
    write("native/core/init_531d60_post_base_je_532bf0_532be6_u1.hpp", je_hpp)

    je_null_hpp = (ROOT / "native/core/init_531d60_post_base_je_532bf8_532b50_u1.hpp").read_text()
    je_null_hpp = je_null_hpp.replace(
        "NOT modelled: `0x532bf8` store epilogue or fall-through destroy @",
        "NOT modelled: E608 `mov [rbx],r14` @ `0x532bf8` or fall-through destroy @",
    )
    write("native/core/init_531d60_post_base_je_532bf8_532b50_u1.hpp", je_null_hpp)

    rsha = region_sha()
    chk = git("tools/lift/check_f531d60_post_base_registry_body.py")
    chk = chk.replace("START, SIZE, END = 0x532B19, 223, 0x532BF8", "START, SIZE, END = 0x532B19, 226, 0x532BFB")
    chk = chk.replace(
        'REGION_SHA256 = "dd2f0d6dc8beda7a87193790fedc997ce80e7dd60b2f64df4731cbfda9638b70"',
        f'REGION_SHA256 = "{rsha}"',
    )
    chk = chk.replace(
        '    (0x532BF3,5,"1a3a644d7ebd3edc4663188ea24d56f8717d7fff774dfa1950d768403af9e477"),\n]',
        '    (0x532BF3,5,"1a3a644d7ebd3edc4663188ea24d56f8717d7fff774dfa1950d768403af9e477"),\n    (0x532BF8,3,"7241225d5359852087b3106b7249779b8871e6d8576b01678db3873f1ea38309"),\n]',
    )
    if "MOVR14MEMRBX=" not in chk:
        chk = chk.replace(
            'CALL170B0=ROOT/"native/core/init_531d60_post_base_call_170b0_532bf3_u1.hpp"\n',
            'CALL170B0=ROOT/"native/core/init_531d60_post_base_call_170b0_532bf3_u1.hpp"\nMOVR14MEMRBX=ROOT/"native/core/init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.hpp"\n',
        )
    pairs = [
        ("require(len(UNITS)==57,'units')", "require(len(UNITS)==58,'units')"),
        ("kInit531d60PostBaseRegistryBodyEndRva = 0x532bf8", "kInit531d60PostBaseRegistryBodyEndRva = 0x532bfb"),
        (
            "require('call_170b0_532bf3_u1' in nh,'unit enum call 170b0 532bf3')",
            "require('call_170b0_532bf3_u1' in nh,'unit enum call 170b0 532bf3')\n    require('mov_r14_mem_rbx_532bf8_u1' in nh,'unit enum mov r14 mem rbx 532bf8')",
        ),
        (
            "'call_170c0_532beb_u1' in nh and 'mov_rdi_r12_532bf0_u1' in nh and 'call_170b0_532bf3_u1' in nh,'unit enum')",
            "'call_170c0_532beb_u1' in nh and 'mov_rdi_r12_532bf0_u1' in nh and 'call_170b0_532bf3_u1' in nh and 'mov_r14_mem_rbx_532bf8_u1' in nh,'unit enum')",
        ),
        (
            "require('merge_call_170b0_532bf3_u1_ran' in nh,'merge call 170b0 532bf3')",
            "require('merge_call_170b0_532bf3_u1_ran' in nh,'merge call 170b0 532bf3')\n    require('merge_mov_r14_mem_rbx_532bf8_u1_ran' in nh,'merge mov r14 mem rbx 532bf8')",
        ),
        (
            "require('kInit531d60PostBaseRegistryBodyUnitCount == 57' in TEST.read_text(),'test unit count 57')",
            "require('kInit531d60PostBaseRegistryBodyUnitCount == 58' in TEST.read_text(),'test unit count 58')",
        ),
        (
            "require('kRegistryInit531d60FragmentPinnedRva = 0x532bf8' in REG.read_text(),'reg pin')",
            "require('kRegistryInit531d60FragmentPinnedRva = 0x532bfb' in REG.read_text(),'reg pin')",
        ),
        (
            "require('merge_call_170b0_532bf3_u1_ran' in REG.read_text(),'reg merge call 170b0 532bf3')",
            "require('merge_call_170b0_532bf3_u1_ran' in REG.read_text(),'reg merge call 170b0 532bf3')\n    require('merge_mov_r14_mem_rbx_532bf8_u1_ran' in REG.read_text(),'reg merge mov r14 mem rbx 532bf8')",
        ),
        (
            "require('merge_call_170b0_532bf3_u1_ran' in SC.read_text(),'slice merge call 170b0 532bf3')",
            "require('merge_call_170b0_532bf3_u1_ran' in SC.read_text(),'slice merge call 170b0 532bf3')\n    require('merge_mov_r14_mem_rbx_532bf8_u1_ran' in SC.read_text(),'slice merge mov r14 mem rbx 532bf8')",
        ),
        (
            "require('end_rva == 0x532bf8' in TEST.read_text(),'test end rva 532bf8')",
            "require('end_rva == 0x532bfb' in TEST.read_text(),'test end rva 532bfb')",
        ),
        (
            "require('call_170b0_532bf3_count' in TEST.read_text(),'test call 170b0 532bf3 field')",
            "require('call_170b0_532bf3_count' in TEST.read_text(),'test call 170b0 532bf3 field')\n    require('mov_r14_mem_rbx_532bf8_count' in TEST.read_text(),'test mov r14 mem rbx 532bf8 field')",
        ),
        (
            "require('merge_call_170b0_532bf3_u1_ran' in TEST.read_text(),'test merge call 170b0 532bf3')",
            "require('merge_call_170b0_532bf3_u1_ran' in TEST.read_text(),'test merge call 170b0 532bf3')\n    require('merge_mov_r14_mem_rbx_532bf8_u1_ran' in TEST.read_text(),'test merge mov r14 mem rbx 532bf8')",
        ),
        (
            "require('init_531d60_post_base_call_170b0_532bf3_u1.cpp' in CMAKE.read_text(),'cmake call 170b0 532bf3')\n    print('OK check_f531d60_post_base_registry_body end=0x532bf8 units=57')",
            "require('init_531d60_post_base_call_170b0_532bf3_u1.cpp' in CMAKE.read_text(),'cmake call 170b0 532bf3')\n    require('init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1' in NCPP.read_text(),'cpp mov r14 mem rbx 532bf8')\n    require('init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1' in MOVR14MEMRBX.read_text(),'mov r14 mem rbx 532bf8 hpp')\n    require('init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.cpp' in CMAKE.read_text(),'cmake mov r14 mem rbx 532bf8')\n    print('OK check_f531d60_post_base_registry_body end=0x532bfb units=58')",
        ),
    ]
    for a, b in pairs:
        chk = chk.replace(a, b)
    write("tools/lift/check_f531d60_post_base_registry_body.py", chk)

    call170b0_chk = (ROOT / "tools/lift/check_f531d60_post_base_call_170b0_532bf3_u1.py").read_text()
    call170b0_chk = call170b0_chk.replace(
        'require("kRegistryInit531d60FragmentPinnedRva = 0x532bf8" in REG.read_text(), "pin")',
        'require("kRegistryInit531d60FragmentPinnedRva = 0x532bfb" in REG.read_text(), "pin")',
    )
    write("tools/lift/check_f531d60_post_base_call_170b0_532bf3_u1.py", call170b0_chk)

    mov_chk = (ROOT / "tools/lift/check_f531d60_post_base_mov_rdi_r12_532bf0_u1.py").read_text()
    mov_chk = mov_chk.replace(
        'require("kRegistryInit531d60FragmentPinnedRva = 0x532bf8" in REG.read_text(), "pin")',
        'require("kRegistryInit531d60FragmentPinnedRva = 0x532bfb" in REG.read_text(), "pin")',
    )
    write("tools/lift/check_f531d60_post_base_mov_rdi_r12_532bf0_u1.py", mov_chk)

    for fp in glob.glob(str(ROOT / "tools/lift/check_f531d60_post_base_*.py")):
        p = Path(fp)
        if p.name in (
            "check_f531d60_post_base_registry_body.py",
            "check_f531d60_post_base_call_170b0_532bf3_u1.py",
            "check_f531d60_post_base_mov_rdi_r12_532bf0_u1.py",
            "check_f531d60_post_base_mov_r14_mem_rbx_532bf8_u1.py",
        ):
            continue
        txt = p.read_text()
        if "kRegistryInit531d60FragmentPinnedRva = 0x532bf8" in txt:
            txt = txt.replace(
                "kRegistryInit531d60FragmentPinnedRva = 0x532bf8",
                "kRegistryInit531d60FragmentPinnedRva = 0x532bfb",
            )
            p.write_text(txt)

    print(f"e608_apply_once: OK region={rsha}")


if __name__ == "__main__":
    main()
