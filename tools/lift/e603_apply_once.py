#!/usr/bin/env python3
"""One-shot E603 apply from git HEAD (E602 base)."""
from __future__ import annotations

import glob
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def git(path: str) -> str:
    return subprocess.check_output(["git", "show", f"HEAD:{path}"], text=True)


def write(path: str, text: str) -> None:
    (ROOT / path).write_text(text)


def main() -> None:
    t = git("native/core/init_531d60_post_base_registry_body.hpp")
    t = t.replace("0x532b19..0x532be1` (52 units)", "0x532b19..0x532be6` (53 units)")
    t = t.replace(
        '#include "init_531d60_post_base_mov_rax_rdi_mem_532bde_u1.hpp"\n#include "init_531d60_post_base_mov_r12_rbx_532b41_u1.hpp"',
        '#include "init_531d60_post_base_mov_rax_rdi_mem_532bde_u1.hpp"\n#include "init_531d60_post_base_sub_eax_010cb932_532be1_u1.hpp"\n#include "init_531d60_post_base_mov_r12_rbx_532b41_u1.hpp"',
    )
    t = t.replace("kInit531d60PostBaseRegistryBodySize = 200;", "kInit531d60PostBaseRegistryBodySize = 205;")
    t = t.replace("kInit531d60PostBaseRegistryBodyEndRva = 0x532be1;", "kInit531d60PostBaseRegistryBodyEndRva = 0x532be6;")
    t = t.replace(
        """static_assert(kInit531d60PostBaseMovRaxRdiMem_532bde_U1EndRva ==
                  kInit531d60PostBaseRegistryBodyEndRva);""",
        """static_assert(kInit531d60PostBaseMovRaxRdiMem_532bde_U1EndRva ==
                  kInit531d60PostBaseSubEax_010cb932_532be1_U1Rva);
static_assert(kInit531d60PostBaseSubEax_010cb932_532be1_U1EndRva ==
                  kInit531d60PostBaseRegistryBodyEndRva);""",
    )
    t = t.replace(
        "    mov_rax_rdi_mem_532bde_u1,\n    count",
        "    mov_rax_rdi_mem_532bde_u1,\n    sub_eax_010cb932_532be1_u1,\n    count",
    )
    t = t.replace(
        "    bool merge_mov_rax_rdi_mem_532bde_u1_ran = false;\n    const void* stack_rsp",
        "    bool merge_mov_rax_rdi_mem_532bde_u1_ran = false;\n    bool merge_sub_eax_010cb932_532be1_u1_ran = false;\n    const void* stack_rsp",
    )
    t = t.replace(
        "    Init531d60PostBaseMovRaxRdiMem_532bde_U1Cells mov_rax_rdi_mem_532bde_cells{};\n};",
        "    Init531d60PostBaseMovRaxRdiMem_532bde_U1Cells mov_rax_rdi_mem_532bde_cells{};\n    Init531d60PostBaseSubEax_010cb932_532be1_U1Cells sub_eax_010cb932_532be1_cells{};\n};",
    )
    t = t.replace("std::array<int, 52> unit_counts{};", "std::array<int, 53> unit_counts{};")
    t = t.replace(
        """    int mov_rax_rdi_mem_532bde_count = 0;
    void* rax_from_rdi_mem_532bde = nullptr;
    bool je_532bdb_branch_taken = false;""",
        """    int mov_rax_rdi_mem_532bde_count = 0;
    void* rax_from_rdi_mem_532bde = nullptr;
    int sub_eax_010cb932_532be1_count = 0;
    std::uint32_t eax_after_sub_532be1 = 0;
    bool je_532bdb_branch_taken = false;""",
    )
    write("native/core/init_531d60_post_base_registry_body.hpp", t)

    sub_block = """
    if (!hooks.merge_sub_eax_010cb932_532be1_u1_ran) {
        Init531d60PostBaseSubEax_010cb932_532be1_U1Cells cells =
            hooks.sub_eax_010cb932_532be1_cells;
        if (out.rax_from_rdi_mem_532bde != nullptr) {
            cells.eax_in = static_cast<std::uint32_t>(
                reinterpret_cast<std::uintptr_t>(out.rax_from_rdi_mem_532bde));
        }
        const Init531d60PostBaseSubEax_010cb932_532be1_U1Result sub_eax =
            init_531d60_post_base_sub_eax_010cb932_532be1_u1(cells);
        if (!sub_eax.ok) {
            return out;
        }
        out.sub_eax_010cb932_532be1_count = sub_eax.sub_eax_010cb932_532be1_count;
        out.eax_after_sub_532be1 = sub_eax.eax;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::sub_eax_010cb932_532be1_u1)] =
            1;
        ++out.unit_executed;
    }

"""
    cpp = git("native/core/init_531d60_post_base_registry_body.cpp")
    needle = "        ++out.unit_executed;\n    }\n\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;"
    cpp = cpp.replace(needle, "        ++out.unit_executed;\n    }\n" + sub_block + "\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;")
    cpp = cpp.replace(
        "    const int expected_mov_rax_rdi_mem_532bde =\n        hooks.merge_mov_rax_rdi_mem_532bde_u1_ran ? 0 : 1;\n    const int expected_units =",
        "    const int expected_mov_rax_rdi_mem_532bde =\n        hooks.merge_mov_rax_rdi_mem_532bde_u1_ran ? 0 : 1;\n    const int expected_sub_eax_010cb932_532be1 =\n        hooks.merge_sub_eax_010cb932_532be1_u1_ran ? 0 : 1;\n    const int expected_units =",
    )
    cpp = cpp.replace(
        "expected_mov_rax_rdi_mem_532bd6 + expected_mov_r13_8_rax_532bd9 +\n                               expected_mov_rax_rdi_mem_532bde;",
        "expected_mov_rax_rdi_mem_532bd6 + expected_mov_r13_8_rax_532bd9 +\n                               expected_mov_rax_rdi_mem_532bde + expected_sub_eax_010cb932_532be1;",
    )
    cpp = cpp.replace(
        "(out.mov_rax_rdi_mem_532bde_count == expected_mov_rax_rdi_mem_532bde);\n    return out;",
        "(out.mov_rax_rdi_mem_532bde_count == expected_mov_rax_rdi_mem_532bde) &&\n             (out.sub_eax_010cb932_532be1_count == expected_sub_eax_010cb932_532be1);\n    return out;",
    )
    write("native/core/init_531d60_post_base_registry_body.cpp", cpp)

    reg = git("native/core/registry_init_531d60.hpp")
    reg = reg.replace("kRegistryInit531d60FragmentPinnedRva = 0x532be1;", "kRegistryInit531d60FragmentPinnedRva = 0x532be6;")
    reg = reg.replace(
        "    bool merge_mov_rax_rdi_mem_532bde_u1_ran = false;\n};",
        "    bool merge_mov_rax_rdi_mem_532bde_u1_ran = false;\n    bool merge_sub_eax_010cb932_532be1_u1_ran = false;\n};",
    )
    write("native/core/registry_init_531d60.hpp", reg)

    cmake = git("native/CMakeLists.txt")
    if "sub_eax_010cb932_532be1" not in cmake:
        cmake = cmake.replace(
            "  core/init_531d60_post_base_mov_rax_rdi_mem_532bde_u1.cpp\n",
            "  core/init_531d60_post_base_mov_rax_rdi_mem_532bde_u1.cpp\n  core/init_531d60_post_base_sub_eax_010cb932_532be1_u1.cpp\n",
        )
    write("native/CMakeLists.txt", cmake)

    sc = git("native/core/appmain_init_ctor_98a90_slice.cpp")
    sc = sc.replace(
        """    if (out.post_base_registry_body.mov_rax_rdi_mem_532bde_count == 1) {
        registry_hooks.merge_mov_rax_rdi_mem_532bde_u1_ran = true;
    }
    out.remainder = run_appmain_init_remainder_slice""",
        """    if (out.post_base_registry_body.mov_rax_rdi_mem_532bde_count == 1) {
        registry_hooks.merge_mov_rax_rdi_mem_532bde_u1_ran = true;
    }
    if (out.post_base_registry_body.sub_eax_010cb932_532be1_count == 1) {
        registry_hooks.merge_sub_eax_010cb932_532be1_u1_ran = true;
    }
    out.remainder = run_appmain_init_remainder_slice""",
    )
    write("native/core/appmain_init_ctor_98a90_slice.cpp", sc)

    test = git("native/tests/test_appmain_init_ctor_98a90_slice.cpp")
    test = test.replace(
        "CHECK(kInit531d60PostBaseRegistryBodyUnitCount == 52);",
        "CHECK(kInit531d60PostBaseRegistryBodyUnitCount == 53);",
    )
    test = test.replace(
        "    pbregbody.merge_mov_rax_rdi_mem_532bde_u1_ran = false;\n",
        "    pbregbody.merge_mov_rax_rdi_mem_532bde_u1_ran = false;\n    pbregbody.merge_sub_eax_010cb932_532be1_u1_ran = false;\n",
    )
    test = test.replace(
        "    CHECK(r.post_base_registry_body.mov_rax_rdi_mem_532bde_count == 1);\n    CHECK(r.post_base_registry_body.end_rva == 0x532be1);",
        "    CHECK(r.post_base_registry_body.mov_rax_rdi_mem_532bde_count == 1);\n    CHECK(r.post_base_registry_body.sub_eax_010cb932_532be1_count == 1);\n    CHECK(r.post_base_registry_body.eax_after_sub_532be1 == 0xFEF346CEu);\n    CHECK(r.post_base_registry_body.end_rva == 0x532be6);",
    )
    write("native/tests/test_appmain_init_ctor_98a90_slice.cpp", test)

    chk = git("tools/lift/check_f531d60_post_base_registry_body.py")
    chk = chk.replace("START, SIZE, END = 0x532B19, 200, 0x532BE1", "START, SIZE, END = 0x532B19, 205, 0x532BE6")
    chk = chk.replace(
        'REGION_SHA256 = "7daba30407f1981826eee21e9a3d22c528f1bc79248c3e3d9c12fa5aa9fdab0d"',
        'REGION_SHA256 = "0b96df070c9022b86b4af8d8a844a7033e7b690c750dd9814e29d190861d4dc2"',
    )
    chk = chk.replace(
        '    (0x532BDE,3,"81c7e25bd38dde9dde06d8a46d8bb57256fcbc780400d7e09baf1a8585b5b409"),\n]',
        '    (0x532BDE,3,"81c7e25bd38dde9dde06d8a46d8bb57256fcbc780400d7e09baf1a8585b5b409"),\n    (0x532BE1,5,"7f367300693a91508777eb984165a5ee775ab703f8ddd95389d20f185269b165"),\n]',
    )
    if "SUBEAX=" not in chk:
        chk = chk.replace(
            'MOVRAXBDE=ROOT/"native/core/init_531d60_post_base_mov_rax_rdi_mem_532bde_u1.hpp"\n',
            'MOVRAXBDE=ROOT/"native/core/init_531d60_post_base_mov_rax_rdi_mem_532bde_u1.hpp"\nSUBEAX=ROOT/"native/core/init_531d60_post_base_sub_eax_010cb932_532be1_u1.hpp"\n',
        )
    pairs = [
        ("require(len(UNITS)==52,'units')", "require(len(UNITS)==53,'units')"),
        ("kInit531d60PostBaseRegistryBodyEndRva = 0x532be1", "kInit531d60PostBaseRegistryBodyEndRva = 0x532be6"),
        (
            "require('merge_mov_rax_rdi_mem_532bde_u1_ran' in nh,'merge mov rax rdi mem 532bde')\n    require('mov_rax_rdi_mem_532bd6_u1' in nh",
            "require('merge_mov_rax_rdi_mem_532bde_u1_ran' in nh,'merge mov rax rdi mem 532bde')\n    require('merge_sub_eax_010cb932_532be1_u1_ran' in nh,'merge sub eax 532be1')\n    require('mov_rax_rdi_mem_532bd6_u1' in nh",
        ),
        (
            "require('mov_rax_rdi_mem_532bde_u1' in nh,'unit enum mov rax rdi mem 532bde')\n    require('init_531d60_post_base_call_54dc10_532b19_u1' in NCPP.read_text(),'cpp call')",
            "require('mov_rax_rdi_mem_532bde_u1' in nh,'unit enum mov rax rdi mem 532bde')\n    require('sub_eax_010cb932_532be1_u1' in nh,'unit enum sub eax 532be1')\n    require('init_531d60_post_base_call_54dc10_532b19_u1' in NCPP.read_text(),'cpp call')",
        ),
        (
            "require('kInit531d60PostBaseRegistryBodyUnitCount == 52' in TEST.read_text(),'test unit count 52')",
            "require('kInit531d60PostBaseRegistryBodyUnitCount == 53' in TEST.read_text(),'test unit count 53')",
        ),
        (
            "require('kRegistryInit531d60FragmentPinnedRva = 0x532be1' in REG.read_text(),'reg pin')",
            "require('kRegistryInit531d60FragmentPinnedRva = 0x532be6' in REG.read_text(),'reg pin')",
        ),
        (
            "require('merge_mov_rax_rdi_mem_532bde_u1_ran' in REG.read_text(),'reg merge mov rax rdi mem 532bde')\n    require('merge_mov_rax_rdi_mem_532bd6_u1_ran' in REG.read_text(),'reg merge mov rax rdi mem 532bd6')",
            "require('merge_mov_rax_rdi_mem_532bde_u1_ran' in REG.read_text(),'reg merge mov rax rdi mem 532bde')\n    require('merge_sub_eax_010cb932_532be1_u1_ran' in REG.read_text(),'reg merge sub eax 532be1')\n    require('merge_mov_rax_rdi_mem_532bd6_u1_ran' in REG.read_text(),'reg merge mov rax rdi mem 532bd6')",
        ),
        (
            "require('merge_mov_rax_rdi_mem_532bde_u1_ran' in SC.read_text(),'slice merge mov rax rdi mem 532bde')\n    require('merge_mov_rax_rdi_mem_532bd6_u1_ran' in TEST.read_text(),'test merge mov rax rdi mem 532bd6')",
            "require('merge_mov_rax_rdi_mem_532bde_u1_ran' in SC.read_text(),'slice merge mov rax rdi mem 532bde')\n    require('merge_sub_eax_010cb932_532be1_u1_ran' in SC.read_text(),'slice merge sub eax 532be1')\n    require('merge_mov_rax_rdi_mem_532bd6_u1_ran' in TEST.read_text(),'test merge mov rax rdi mem 532bd6')",
        ),
        (
            "require('end_rva == 0x532be1' in TEST.read_text(),'test end rva 532be1')",
            "require('end_rva == 0x532be6' in TEST.read_text(),'test end rva 532be6')",
        ),
        (
            "require('merge_mov_rax_rdi_mem_532bde_u1_ran' in TEST.read_text(),'test merge mov rax rdi mem 532bde')\n    require('init_531d60_post_base_mov_r13_8_rax_532bd9_u1' in MOVR138.read_text(),'mov r13 8 rax hpp')",
            "require('merge_mov_rax_rdi_mem_532bde_u1_ran' in TEST.read_text(),'test merge mov rax rdi mem 532bde')\n    require('merge_sub_eax_010cb932_532be1_u1_ran' in TEST.read_text(),'test merge sub eax 532be1')\n    require('sub_eax_010cb932_532be1_count' in TEST.read_text(),'test sub eax 532be1 field')\n    require('init_531d60_post_base_mov_r13_8_rax_532bd9_u1' in MOVR138.read_text(),'mov r13 8 rax hpp')",
        ),
        (
            "require('init_531d60_post_base_mov_rax_rdi_mem_532bde_u1.cpp' in CMAKE.read_text(),'cmake mov rax rdi mem 532bde')\n    print('OK check_f531d60_post_base_registry_body end=0x532be1 units=52')",
            "require('init_531d60_post_base_mov_rax_rdi_mem_532bde_u1.cpp' in CMAKE.read_text(),'cmake mov rax rdi mem 532bde')\n    require('init_531d60_post_base_sub_eax_010cb932_532be1_u1' in SUBEAX.read_text(),'sub eax 532be1 hpp')\n    require('init_531d60_post_base_sub_eax_010cb932_532be1_u1.cpp' in CMAKE.read_text(),'cmake sub eax 532be1')\n    require('init_531d60_post_base_sub_eax_010cb932_532be1_u1' in NCPP.read_text(),'cpp sub eax 532be1')\n    print('OK check_f531d60_post_base_registry_body end=0x532be6 units=53')",
        ),
    ]
    for a, b in pairs:
        chk = chk.replace(a, b)
    write("tools/lift/check_f531d60_post_base_registry_body.py", chk)

    for fp in glob.glob(str(ROOT / "tools/lift/check_f531d60_post_base_*.py")):
        p = Path(fp)
        txt = p.read_text()
        txt = txt.replace("kRegistryInit531d60FragmentPinnedRva = 0x532be1", "kRegistryInit531d60FragmentPinnedRva = 0x532be6")
        txt = txt.replace("kRegistryInit531d60FragmentPinnedRva = 0x532bde", "kRegistryInit531d60FragmentPinnedRva = 0x532be6")
        p.write_text(txt)

    print("e603_apply_once: OK")


if __name__ == "__main__":
    main()
