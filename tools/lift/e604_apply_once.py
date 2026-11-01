#!/usr/bin/env python3
"""One-shot E604 apply from git HEAD (E603 base)."""
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
    START, SIZE = 0x532B19, 210
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
    t = t.replace("0x532b19..0x532be6` (53 units)", "0x532b19..0x532beb` (54 units)")
    t = t.replace(
        '#include "init_531d60_post_base_sub_eax_010cb932_532be1_u1.hpp"\n#include "init_531d60_post_base_mov_r12_rbx_532b41_u1.hpp"',
        '#include "init_531d60_post_base_sub_eax_010cb932_532be1_u1.hpp"\n#include "init_531d60_post_base_je_532bf0_532be6_u1.hpp"\n#include "init_531d60_post_base_mov_r12_rbx_532b41_u1.hpp"',
    )
    t = t.replace("kInit531d60PostBaseRegistryBodySize = 205;", "kInit531d60PostBaseRegistryBodySize = 210;")
    t = t.replace("kInit531d60PostBaseRegistryBodyEndRva = 0x532be6;", "kInit531d60PostBaseRegistryBodyEndRva = 0x532beb;")
    t = t.replace(
        """static_assert(kInit531d60PostBaseSubEax_010cb932_532be1_U1EndRva ==
                  kInit531d60PostBaseRegistryBodyEndRva);""",
        """static_assert(kInit531d60PostBaseSubEax_010cb932_532be1_U1EndRva ==
                  kInit531d60PostBaseJe532bf0_532be6_U1Rva);
static_assert(kInit531d60PostBaseJe532bf0_532be6_U1EndRva ==
                  kInit531d60PostBaseRegistryBodyEndRva);""",
    )
    t = t.replace(
        "    sub_eax_010cb932_532be1_u1,\n    count",
        "    sub_eax_010cb932_532be1_u1,\n    je_532bf0_532be6_u1,\n    count",
    )
    t = t.replace(
        "    bool merge_sub_eax_010cb932_532be1_u1_ran = false;\n    const void* stack_rsp",
        "    bool merge_sub_eax_010cb932_532be1_u1_ran = false;\n    bool merge_je_532bf0_532be6_u1_ran = false;\n    const void* stack_rsp",
    )
    t = t.replace(
        "    Init531d60PostBaseSubEax_010cb932_532be1_U1Cells sub_eax_010cb932_532be1_cells{};\n};",
        "    Init531d60PostBaseSubEax_010cb932_532be1_U1Cells sub_eax_010cb932_532be1_cells{};\n    Init531d60PostBaseJe532bf0_532be6_U1Cells je_532bf0_532be6_cells{};\n};",
    )
    t = t.replace("std::array<int, 53> unit_counts{};", "std::array<int, 54> unit_counts{};")
    t = t.replace(
        """    std::uint32_t eax_after_sub_532be1 = 0;
    bool je_532bdb_branch_taken = false;""",
        """    std::uint32_t eax_after_sub_532be1 = 0;
    int je_532bf0_532be6_count = 0;
    bool je_532bf0_branch_taken = false;
    bool je_532bf0_branch_outcome_modelled = false;
    bool je_532bf0_rdi_is_zero = false;
    bool je_532bdb_branch_taken = false;""",
    )
    write("native/core/init_531d60_post_base_registry_body.hpp", t)

    je_block = """
    if (!hooks.merge_je_532bf0_532be6_u1_ran) {
        Init531d60PostBaseJe532bf0_532be6_U1Cells cells = hooks.je_532bf0_532be6_cells;
        if (cells.rdi_in == nullptr) {
            cells.rdi_in = out.rdi_from_r12_20;
        }
        const Init531d60PostBaseJe532bf0_532be6_U1Result je_532bf0 =
            init_531d60_post_base_je_532bf0_532be6_u1(cells);
        if (!je_532bf0.ok) {
            return out;
        }
        out.je_532bf0_532be6_count = je_532bf0.je_532bf0_532be6_count;
        out.je_532bf0_branch_taken = je_532bf0.branch_taken;
        out.je_532bf0_branch_outcome_modelled = je_532bf0.branch_outcome_modelled;
        out.je_532bf0_rdi_is_zero = je_532bf0.rdi_is_zero;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::je_532bf0_532be6_u1)] = 1;
        ++out.unit_executed;
    }

"""
    cpp = git("native/core/init_531d60_post_base_registry_body.cpp")
    needle = "        ++out.unit_executed;\n    }\n\n\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;"
    if needle not in cpp:
        needle = "        ++out.unit_executed;\n    }\n\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;"
    cpp = cpp.replace(needle, "        ++out.unit_executed;\n    }\n" + je_block + "\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;")
    cpp = cpp.replace(
        "    const int expected_sub_eax_010cb932_532be1 =\n        hooks.merge_sub_eax_010cb932_532be1_u1_ran ? 0 : 1;\n    const int expected_units =",
        "    const int expected_sub_eax_010cb932_532be1 =\n        hooks.merge_sub_eax_010cb932_532be1_u1_ran ? 0 : 1;\n    const int expected_je_532bf0_532be6 =\n        hooks.merge_je_532bf0_532be6_u1_ran ? 0 : 1;\n    const int expected_units =",
    )
    cpp = cpp.replace(
        "expected_mov_rax_rdi_mem_532bde + expected_sub_eax_010cb932_532be1;",
        "expected_mov_rax_rdi_mem_532bde + expected_sub_eax_010cb932_532be1 + expected_je_532bf0_532be6;",
    )
    cpp = cpp.replace(
        "(out.sub_eax_010cb932_532be1_count == expected_sub_eax_010cb932_532be1);\n    return out;",
        "(out.sub_eax_010cb932_532be1_count == expected_sub_eax_010cb932_532be1) &&\n             (out.je_532bf0_532be6_count == expected_je_532bf0_532be6);\n    return out;",
    )
    write("native/core/init_531d60_post_base_registry_body.cpp", cpp)

    reg = git("native/core/registry_init_531d60.hpp")
    reg = reg.replace("kRegistryInit531d60FragmentPinnedRva = 0x532be6;", "kRegistryInit531d60FragmentPinnedRva = 0x532beb;")
    reg = reg.replace(
        "    bool merge_sub_eax_010cb932_532be1_u1_ran = false;\n};",
        "    bool merge_sub_eax_010cb932_532be1_u1_ran = false;\n    bool merge_je_532bf0_532be6_u1_ran = false;\n};",
    )
    write("native/core/registry_init_531d60.hpp", reg)

    cmake = git("native/CMakeLists.txt")
    if "je_532bf0_532be6" not in cmake:
        cmake = cmake.replace(
            "  core/init_531d60_post_base_sub_eax_010cb932_532be1_u1.cpp\n",
            "  core/init_531d60_post_base_sub_eax_010cb932_532be1_u1.cpp\n  core/init_531d60_post_base_je_532bf0_532be6_u1.cpp\n",
        )
    write("native/CMakeLists.txt", cmake)

    sc = git("native/core/appmain_init_ctor_98a90_slice.cpp")
    sc = sc.replace(
        """    if (out.post_base_registry_body.sub_eax_010cb932_532be1_count == 1) {
        registry_hooks.merge_sub_eax_010cb932_532be1_u1_ran = true;
    }
    out.remainder = run_appmain_init_remainder_slice""",
        """    if (out.post_base_registry_body.sub_eax_010cb932_532be1_count == 1) {
        registry_hooks.merge_sub_eax_010cb932_532be1_u1_ran = true;
    }
    if (out.post_base_registry_body.je_532bf0_532be6_count == 1) {
        registry_hooks.merge_je_532bf0_532be6_u1_ran = true;
    }
    out.remainder = run_appmain_init_remainder_slice""",
    )
    write("native/core/appmain_init_ctor_98a90_slice.cpp", sc)

    test = git("native/tests/test_appmain_init_ctor_98a90_slice.cpp")
    test = test.replace(
        "CHECK(kInit531d60PostBaseRegistryBodyUnitCount == 53);",
        "CHECK(kInit531d60PostBaseRegistryBodyUnitCount == 54);",
    )
    test = test.replace(
        "    pbregbody.merge_sub_eax_010cb932_532be1_u1_ran = false;\n",
        "    pbregbody.merge_sub_eax_010cb932_532be1_u1_ran = false;\n    pbregbody.merge_je_532bf0_532be6_u1_ran = false;\n",
    )
    test = test.replace(
        "    CHECK(r.post_base_registry_body.eax_after_sub_532be1 == 0xFEF346CEu);\n    CHECK(r.post_base_registry_body.end_rva == 0x532be6);",
        "    CHECK(r.post_base_registry_body.eax_after_sub_532be1 == 0xFEF346CEu);\n    CHECK(r.post_base_registry_body.je_532bf0_532be6_count == 1);\n    CHECK(r.post_base_registry_body.end_rva == 0x532beb);",
    )
    write("native/tests/test_appmain_init_ctor_98a90_slice.cpp", test)

    rsha = region_sha()
    chk = git("tools/lift/check_f531d60_post_base_registry_body.py")
    chk = chk.replace("START, SIZE, END = 0x532B19, 205, 0x532BE6", "START, SIZE, END = 0x532B19, 210, 0x532BEB")
    chk = chk.replace(
        'REGION_SHA256 = "8d7ae0a3b88b2866b19901d629064a7644889c30ca1f3987bd27c0e8c887b4ec"',
        f'REGION_SHA256 = "{rsha}"',
    )
    chk = chk.replace(
        '    (0x532BE1,5,"7f367300693a91508777eb984165a5ee775ab703f8ddd95389d20f185269b165"),\n]',
        '    (0x532BE1,5,"7f367300693a91508777eb984165a5ee775ab703f8ddd95389d20f185269b165"),\n    (0x532BE6,5,"201be5646f38f1b81ff370448d95d65ff330b608b531be7015cc5d3764f5e6b7"),\n]',
    )
    if "JE532BF0=" not in chk:
        chk = chk.replace(
            'SUBEAX=ROOT/"native/core/init_531d60_post_base_sub_eax_010cb932_532be1_u1.hpp"\n',
            'SUBEAX=ROOT/"native/core/init_531d60_post_base_sub_eax_010cb932_532be1_u1.hpp"\nJE532BF0=ROOT/"native/core/init_531d60_post_base_je_532bf0_532be6_u1.hpp"\n',
        )
    pairs = [
        ("require(len(UNITS)==53,'units')", "require(len(UNITS)==54,'units')"),
        ("kInit531d60PostBaseRegistryBodyEndRva = 0x532be6", "kInit531d60PostBaseRegistryBodyEndRva = 0x532beb"),
        (
            "require('sub_eax_010cb932_532be1_u1' in nh,'unit enum sub eax 532be1')\n    require('init_531d60_post_base_call_54dc10_532b19_u1' in NCPP.read_text(),'cpp call')",
            "require('sub_eax_010cb932_532be1_u1' in nh,'unit enum sub eax 532be1')\n    require('je_532bf0_532be6_u1' in nh,'unit enum je 532bf0 532be6')\n    require('init_531d60_post_base_call_54dc10_532b19_u1' in NCPP.read_text(),'cpp call')",
        ),
        (
            "require('merge_sub_eax_010cb932_532be1_u1_ran' in nh,'merge sub eax 532be1')\n    require('mov_rax_rdi_mem_532bd6_u1' in nh",
            "require('merge_sub_eax_010cb932_532be1_u1_ran' in nh,'merge sub eax 532be1')\n    require('merge_je_532bf0_532be6_u1_ran' in nh,'merge je 532bf0 532be6')\n    require('mov_rax_rdi_mem_532bd6_u1' in nh",
        ),
        (
            "require('kInit531d60PostBaseRegistryBodyUnitCount == 53' in TEST.read_text(),'test unit count 53')",
            "require('kInit531d60PostBaseRegistryBodyUnitCount == 54' in TEST.read_text(),'test unit count 54')",
        ),
        (
            "require('kRegistryInit531d60FragmentPinnedRva = 0x532be6' in REG.read_text(),'reg pin')",
            "require('kRegistryInit531d60FragmentPinnedRva = 0x532beb' in REG.read_text(),'reg pin')",
        ),
        (
            "require('merge_sub_eax_010cb932_532be1_u1_ran' in REG.read_text(),'reg merge sub eax 532be1')\n    require('merge_mov_rax_rdi_mem_532bd6_u1_ran' in REG.read_text(),'reg merge mov rax rdi mem 532bd6')",
            "require('merge_sub_eax_010cb932_532be1_u1_ran' in REG.read_text(),'reg merge sub eax 532be1')\n    require('merge_je_532bf0_532be6_u1_ran' in REG.read_text(),'reg merge je 532bf0 532be6')\n    require('merge_mov_rax_rdi_mem_532bd6_u1_ran' in REG.read_text(),'reg merge mov rax rdi mem 532bd6')",
        ),
        (
            "require('merge_sub_eax_010cb932_532be1_u1_ran' in SC.read_text(),'slice merge sub eax 532be1')\n    require('merge_mov_rax_rdi_mem_532bd6_u1_ran' in TEST.read_text(),'test merge mov rax rdi mem 532bd6')",
            "require('merge_sub_eax_010cb932_532be1_u1_ran' in SC.read_text(),'slice merge sub eax 532be1')\n    require('merge_je_532bf0_532be6_u1_ran' in SC.read_text(),'slice merge je 532bf0 532be6')\n    require('merge_mov_rax_rdi_mem_532bd6_u1_ran' in TEST.read_text(),'test merge mov rax rdi mem 532bd6')",
        ),
        (
            "require('end_rva == 0x532be6' in TEST.read_text(),'test end rva 532be6')",
            "require('end_rva == 0x532beb' in TEST.read_text(),'test end rva 532beb')",
        ),
        (
            "require('merge_sub_eax_010cb932_532be1_u1_ran' in TEST.read_text(),'test merge sub eax 532be1')\n    require('sub_eax_010cb932_532be1_count' in TEST.read_text(),'test sub eax 532be1 field')",
            "require('merge_sub_eax_010cb932_532be1_u1_ran' in TEST.read_text(),'test merge sub eax 532be1')\n    require('merge_je_532bf0_532be6_u1_ran' in TEST.read_text(),'test merge je 532bf0 532be6')\n    require('sub_eax_010cb932_532be1_count' in TEST.read_text(),'test sub eax 532be1 field')\n    require('je_532bf0_532be6_count' in TEST.read_text(),'test je 532bf0 532be6 field')",
        ),
        (
            "require('init_531d60_post_base_sub_eax_010cb932_532be1_u1' in NCPP.read_text(),'cpp sub eax 532be1')\n    print('OK check_f531d60_post_base_registry_body end=0x532be6 units=53')",
            "require('init_531d60_post_base_sub_eax_010cb932_532be1_u1' in NCPP.read_text(),'cpp sub eax 532be1')\n    require('init_531d60_post_base_je_532bf0_532be6_u1' in NCPP.read_text(),'cpp je 532bf0 532be6')\n    require('init_531d60_post_base_je_532bf0_532be6_u1' in JE532BF0.read_text(),'je 532bf0 532be6 hpp')\n    require('init_531d60_post_base_je_532bf0_532be6_u1.cpp' in CMAKE.read_text(),'cmake je 532bf0 532be6')\n    print('OK check_f531d60_post_base_registry_body end=0x532beb units=54')",
        ),
    ]
    for a, b in pairs:
        chk = chk.replace(a, b)
    write("tools/lift/check_f531d60_post_base_registry_body.py", chk)

    sub = (ROOT / "native/core/init_531d60_post_base_sub_eax_010cb932_532be1_u1.hpp").read_text()
    sub = sub.replace(
        "`0x532be6`.. G1 PARTIAL.",
        "`je_532bf0_532be6_u1` @ `0x532be6` / tail @ `0x532beb`.. G1 PARTIAL.",
    )
    write("native/core/init_531d60_post_base_sub_eax_010cb932_532be1_u1.hpp", sub)

    subcpp = (ROOT / "native/core/init_531d60_post_base_sub_eax_010cb932_532be1_u1.cpp").read_text()
    subcpp = subcpp.replace(
        "    // Next: tail @ 0x532be6 — not modelled here.",
        "    // Next: E604 test/je @ 0x532be6 — not modelled here.",
    )
    write("native/core/init_531d60_post_base_sub_eax_010cb932_532be1_u1.cpp", subcpp)

    for fp in glob.glob(str(ROOT / "tools/lift/check_f531d60_post_base_*.py")):
        p = Path(fp)
        if p.name == "check_f531d60_post_base_registry_body.py":
            continue
        txt = p.read_text()
        txt = txt.replace("kRegistryInit531d60FragmentPinnedRva = 0x532be6", "kRegistryInit531d60FragmentPinnedRva = 0x532beb")
        p.write_text(txt)

    print(f"e604_apply_once: OK region={rsha}")


if __name__ == "__main__":
    main()
