#!/usr/bin/env python3
"""One-shot E605 apply from git HEAD (E604 base)."""
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
    START, SIZE = 0x532B19, 215
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
    t = t.replace("0x532b19..0x532beb` (54 units)", "0x532b19..0x532bf0` (55 units)")
    t = t.replace(
        '#include "init_531d60_post_base_je_532bf0_532be6_u1.hpp"\n#include "init_531d60_post_base_mov_r12_rbx_532b41_u1.hpp"',
        '#include "init_531d60_post_base_je_532bf0_532be6_u1.hpp"\n#include "init_531d60_post_base_call_170c0_532beb_u1.hpp"\n#include "init_531d60_post_base_mov_r12_rbx_532b41_u1.hpp"',
    )
    t = t.replace("kInit531d60PostBaseRegistryBodySize = 210;", "kInit531d60PostBaseRegistryBodySize = 215;")
    t = t.replace("kInit531d60PostBaseRegistryBodyEndRva = 0x532beb;", "kInit531d60PostBaseRegistryBodyEndRva = 0x532bf0;")
    t = t.replace(
        """static_assert(kInit531d60PostBaseJe532bf0_532be6_U1EndRva ==
                  kInit531d60PostBaseRegistryBodyEndRva);""",
        """static_assert(kInit531d60PostBaseJe532bf0_532be6_U1EndRva ==
                  kInit531d60PostBaseCall170c0_532beb_U1Rva);
static_assert(kInit531d60PostBaseCall170c0_532beb_U1EndRva ==
                  kInit531d60PostBaseRegistryBodyEndRva);""",
    )
    t = t.replace(
        "    je_532bf0_532be6_u1,\n    count",
        "    je_532bf0_532be6_u1,\n    call_170c0_532beb_u1,\n    count",
    )
    t = t.replace(
        "    bool merge_je_532bf0_532be6_u1_ran = false;\n    const void* stack_rsp",
        "    bool merge_je_532bf0_532be6_u1_ran = false;\n    bool merge_call_170c0_532beb_u1_ran = false;\n    const void* stack_rsp",
    )
    t = t.replace(
        "    Init531d60PostBaseJe532bf0_532be6_U1Cells je_532bf0_532be6_cells{};\n};",
        "    Init531d60PostBaseJe532bf0_532be6_U1Cells je_532bf0_532be6_cells{};\n    Init531d60PostBaseCall170c0_532beb_U1Cells call_170c0_532beb_cells{};\n};",
    )
    t = t.replace("std::array<int, 54> unit_counts{};", "std::array<int, 55> unit_counts{};")
    t = t.replace(
        """    bool je_532bf0_rdi_is_zero = false;
    bool je_532bdb_branch_taken = false;""",
        """    bool je_532bf0_rdi_is_zero = false;
    int call_170c0_532beb_count = 0;
    bool je_532bdb_branch_taken = false;""",
    )
    write("native/core/init_531d60_post_base_registry_body.hpp", t)

    call_block = """
    if (!hooks.merge_call_170c0_532beb_u1_ran) {
        Init531d60PostBaseCall170c0_532beb_U1Cells cells = hooks.call_170c0_532beb_cells;
        if (cells.rdi_in == nullptr) {
            cells.rdi_in = out.rdi_from_r12_20;
        }
        const Init531d60PostBaseCall170c0_532beb_U1Result call_free =
            init_531d60_post_base_call_170c0_532beb_u1(cells);
        if (!call_free.ok) {
            return out;
        }
        out.call_170c0_532beb_count = call_free.call_170c0_532beb_count;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::call_170c0_532beb_u1)] = 1;
        ++out.unit_executed;
    }

"""
    cpp = git("native/core/init_531d60_post_base_registry_body.cpp")
    needle = "        ++out.unit_executed;\n    }\n\n\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;"
    if needle not in cpp:
        needle = "        ++out.unit_executed;\n    }\n\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;"
    cpp = cpp.replace(
        needle,
        "        ++out.unit_executed;\n    }\n" + call_block + "\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;",
    )
    cpp = cpp.replace(
        "    const int expected_je_532bf0_532be6 =\n        hooks.merge_je_532bf0_532be6_u1_ran ? 0 : 1;\n    const int expected_units =",
        "    const int expected_je_532bf0_532be6 =\n        hooks.merge_je_532bf0_532be6_u1_ran ? 0 : 1;\n    const int expected_call_170c0_532beb =\n        hooks.merge_call_170c0_532beb_u1_ran ? 0 : 1;\n    const int expected_units =",
    )
    cpp = cpp.replace(
        "expected_mov_rax_rdi_mem_532bde + expected_sub_eax_010cb932_532be1 + expected_je_532bf0_532be6;",
        "expected_mov_rax_rdi_mem_532bde + expected_sub_eax_010cb932_532be1 + expected_je_532bf0_532be6 + expected_call_170c0_532beb;",
    )
    cpp = cpp.replace(
        "(out.je_532bf0_532be6_count == expected_je_532bf0_532be6);\n    return out;",
        "(out.je_532bf0_532be6_count == expected_je_532bf0_532be6) &&\n             (out.call_170c0_532beb_count == expected_call_170c0_532beb);\n    return out;",
    )
    write("native/core/init_531d60_post_base_registry_body.cpp", cpp)

    reg = git("native/core/registry_init_531d60.hpp")
    reg = reg.replace("kRegistryInit531d60FragmentPinnedRva = 0x532beb;", "kRegistryInit531d60FragmentPinnedRva = 0x532bf0;")
    reg = reg.replace(
        "    bool merge_je_532bf0_532be6_u1_ran = false;\n};",
        "    bool merge_je_532bf0_532be6_u1_ran = false;\n    bool merge_call_170c0_532beb_u1_ran = false;\n};",
    )
    write("native/core/registry_init_531d60.hpp", reg)

    cmake = git("native/CMakeLists.txt")
    if "call_170c0_532beb" not in cmake:
        cmake = cmake.replace(
            "  core/init_531d60_post_base_je_532bf0_532be6_u1.cpp\n",
            "  core/init_531d60_post_base_je_532bf0_532be6_u1.cpp\n  core/init_531d60_post_base_call_170c0_532beb_u1.cpp\n",
        )
    write("native/CMakeLists.txt", cmake)

    sc = git("native/core/appmain_init_ctor_98a90_slice.cpp")
    sc = sc.replace(
        """    if (out.post_base_registry_body.je_532bf0_532be6_count == 1) {
        registry_hooks.merge_je_532bf0_532be6_u1_ran = true;
    }
    out.remainder = run_appmain_init_remainder_slice""",
        """    if (out.post_base_registry_body.je_532bf0_532be6_count == 1) {
        registry_hooks.merge_je_532bf0_532be6_u1_ran = true;
    }
    if (out.post_base_registry_body.call_170c0_532beb_count == 1) {
        registry_hooks.merge_call_170c0_532beb_u1_ran = true;
    }
    out.remainder = run_appmain_init_remainder_slice""",
    )
    write("native/core/appmain_init_ctor_98a90_slice.cpp", sc)

    test = git("native/tests/test_appmain_init_ctor_98a90_slice.cpp")
    test = test.replace(
        "CHECK(kInit531d60PostBaseRegistryBodyUnitCount == 54);",
        "CHECK(kInit531d60PostBaseRegistryBodyUnitCount == 55);",
    )
    test = test.replace(
        "    pbregbody.merge_je_532bf0_532be6_u1_ran = false;\n",
        "    pbregbody.merge_je_532bf0_532be6_u1_ran = false;\n    pbregbody.merge_call_170c0_532beb_u1_ran = false;\n",
    )
    test = test.replace(
        "    CHECK(r.post_base_registry_body.je_532bf0_532be6_count == 1);\n    CHECK(r.post_base_registry_body.end_rva == 0x532beb);",
        "    CHECK(r.post_base_registry_body.je_532bf0_532be6_count == 1);\n    CHECK(r.post_base_registry_body.call_170c0_532beb_count == 1);\n    CHECK(r.post_base_registry_body.end_rva == 0x532bf0);",
    )
    write("native/tests/test_appmain_init_ctor_98a90_slice.cpp", test)

    je_hpp = (ROOT / "native/core/init_531d60_post_base_je_532bf0_532be6_u1.hpp").read_text()
    je_hpp = je_hpp.replace(
        "After E603 `sub_eax_010cb932_532be1_u1` end @ `0x532be6`. NOT modelled: call @\n// `0x532beb`.. G1 PARTIAL.",
        "After E603 `sub_eax_010cb932_532be1_u1` end @ `0x532be6`. Fall-through call is E605\n// `call_170c0_532beb_u1` @ `0x532beb` / tail @ `0x532bf0`.. G1 PARTIAL.",
    )
    write("native/core/init_531d60_post_base_je_532bf0_532be6_u1.hpp", je_hpp)

    je_cpp = (ROOT / "native/core/init_531d60_post_base_je_532bf0_532be6_u1.cpp").read_text()
    je_cpp = je_cpp.replace(
        "    // Next: call @ 0x532beb or fall-through target @ 0x532bf0 — not modelled here.",
        "    // Next: E605 call 0x170c0 @ 0x532beb — not modelled here.",
    )
    write("native/core/init_531d60_post_base_je_532bf0_532be6_u1.cpp", je_cpp)

    rsha = region_sha()
    chk = git("tools/lift/check_f531d60_post_base_registry_body.py")
    chk = chk.replace("START, SIZE, END = 0x532B19, 210, 0x532BEB", "START, SIZE, END = 0x532B19, 215, 0x532BF0")
    chk = chk.replace(
        'REGION_SHA256 = "13a23f05cf898ad96b01339c7354a1445d69afac91ef71850c5629c02907f243"',
        f'REGION_SHA256 = "{rsha}"',
    )
    chk = chk.replace(
        '    (0x532BE6,5,"201be5646f38f1b81ff370448d95d65ff330b608b531be7015cc5d3764f5e6b7"),\n]',
        '    (0x532BE6,5,"201be5646f38f1b81ff370448d95d65ff330b608b531be7015cc5d3764f5e6b7"),\n    (0x532BEB,5,"a4830c6c94910c61fa171c7aa1e9beca321fb35d44fa9cbc9d7346842e828bc2"),\n]',
    )
    if "CALL170C0BEB=" not in chk:
        chk = chk.replace(
            'JE532BF0=ROOT/"native/core/init_531d60_post_base_je_532bf0_532be6_u1.hpp"\n',
            'JE532BF0=ROOT/"native/core/init_531d60_post_base_je_532bf0_532be6_u1.hpp"\nCALL170C0BEB=ROOT/"native/core/init_531d60_post_base_call_170c0_532beb_u1.hpp"\n',
        )
    pairs = [
        ("require(len(UNITS)==54,'units')", "require(len(UNITS)==55,'units')"),
        ("kInit531d60PostBaseRegistryBodyEndRva = 0x532beb", "kInit531d60PostBaseRegistryBodyEndRva = 0x532bf0"),
        (
            "require('je_532bf0_532be6_u1' in nh,'unit enum je 532bf0 532be6')\n    require('init_531d60_post_base_call_54dc10_532b19_u1' in NCPP.read_text(),'cpp call')",
            "require('je_532bf0_532be6_u1' in nh,'unit enum je 532bf0 532be6')\n    require('call_170c0_532beb_u1' in nh,'unit enum call 170c0 532beb')\n    require('init_531d60_post_base_call_54dc10_532b19_u1' in NCPP.read_text(),'cpp call')",
        ),
        (
            "require('merge_je_532bf0_532be6_u1_ran' in nh,'merge je 532bf0 532be6')\n    require('mov_rax_rdi_mem_532bd6_u1' in nh",
            "require('merge_je_532bf0_532be6_u1_ran' in nh,'merge je 532bf0 532be6')\n    require('merge_call_170c0_532beb_u1_ran' in nh,'merge call 170c0 532beb')\n    require('mov_rax_rdi_mem_532bd6_u1' in nh",
        ),
        (
            "require('kInit531d60PostBaseRegistryBodyUnitCount == 54' in TEST.read_text(),'test unit count 54')",
            "require('kInit531d60PostBaseRegistryBodyUnitCount == 55' in TEST.read_text(),'test unit count 55')",
        ),
        (
            "require('kRegistryInit531d60FragmentPinnedRva = 0x532beb' in REG.read_text(),'reg pin')",
            "require('kRegistryInit531d60FragmentPinnedRva = 0x532bf0' in REG.read_text(),'reg pin')",
        ),
        (
            "require('merge_je_532bf0_532be6_u1_ran' in REG.read_text(),'reg merge je 532bf0 532be6')\n    require('merge_mov_rax_rdi_mem_532bd6_u1_ran' in REG.read_text(),'reg merge mov rax rdi mem 532bd6')",
            "require('merge_je_532bf0_532be6_u1_ran' in REG.read_text(),'reg merge je 532bf0 532be6')\n    require('merge_call_170c0_532beb_u1_ran' in REG.read_text(),'reg merge call 170c0 532beb')\n    require('merge_mov_rax_rdi_mem_532bd6_u1_ran' in REG.read_text(),'reg merge mov rax rdi mem 532bd6')",
        ),
        (
            "require('merge_je_532bf0_532be6_u1_ran' in SC.read_text(),'slice merge je 532bf0 532be6')\n    require('merge_mov_rax_rdi_mem_532bd6_u1_ran' in TEST.read_text(),'test merge mov rax rdi mem 532bd6')",
            "require('merge_je_532bf0_532be6_u1_ran' in SC.read_text(),'slice merge je 532bf0 532be6')\n    require('merge_call_170c0_532beb_u1_ran' in SC.read_text(),'slice merge call 170c0 532beb')\n    require('merge_mov_rax_rdi_mem_532bd6_u1_ran' in TEST.read_text(),'test merge mov rax rdi mem 532bd6')",
        ),
        (
            "require('end_rva == 0x532beb' in TEST.read_text(),'test end rva 532beb')",
            "require('end_rva == 0x532bf0' in TEST.read_text(),'test end rva 532bf0')",
        ),
        (
            "require('je_532bf0_532be6_count' in TEST.read_text(),'test je 532bf0 532be6 field')",
            "require('je_532bf0_532be6_count' in TEST.read_text(),'test je 532bf0 532be6 field')\n    require('call_170c0_532beb_count' in TEST.read_text(),'test call 170c0 532beb field')",
        ),
        (
            "require('merge_je_532bf0_532be6_u1_ran' in TEST.read_text(),'test merge je 532bf0 532be6')\n    require('sub_eax_010cb932_532be1_count' in TEST.read_text(),'test sub eax 532be1 field')",
            "require('merge_je_532bf0_532be6_u1_ran' in TEST.read_text(),'test merge je 532bf0 532be6')\n    require('merge_call_170c0_532beb_u1_ran' in TEST.read_text(),'test merge call 170c0 532beb')\n    require('sub_eax_010cb932_532be1_count' in TEST.read_text(),'test sub eax 532be1 field')",
        ),
        (
            "require('init_531d60_post_base_je_532bf0_532be6_u1.cpp' in CMAKE.read_text(),'cmake je 532bf0 532be6')\n    print('OK check_f531d60_post_base_registry_body end=0x532beb units=54')",
            "require('init_531d60_post_base_je_532bf0_532be6_u1.cpp' in CMAKE.read_text(),'cmake je 532bf0 532be6')\n    require('init_531d60_post_base_call_170c0_532beb_u1' in NCPP.read_text(),'cpp call 170c0 532beb')\n    require('init_531d60_post_base_call_170c0_532beb_u1' in CALL170C0BEB.read_text(),'call 170c0 532beb hpp')\n    require('init_531d60_post_base_call_170c0_532beb_u1.cpp' in CMAKE.read_text(),'cmake call 170c0 532beb')\n    print('OK check_f531d60_post_base_registry_body end=0x532bf0 units=55')",
        ),
    ]
    for a, b in pairs:
        chk = chk.replace(a, b)
    # Extend unit enum require line for call_170c0_532beb_u1
    chk = chk.replace(
        "'mov_rax_rdi_mem_532bd6_u1' in nh,'unit enum')",
        "'mov_rax_rdi_mem_532bd6_u1' in nh and 'call_170c0_532beb_u1' in nh,'unit enum')",
    )
    write("tools/lift/check_f531d60_post_base_registry_body.py", chk)

    for fp in glob.glob(str(ROOT / "tools/lift/check_f531d60_post_base_*.py")):
        p = Path(fp)
        if p.name in ("check_f531d60_post_base_registry_body.py", "check_f531d60_post_base_call_170c0_532beb_u1.py"):
            continue
        txt = p.read_text()
        txt = txt.replace("kRegistryInit531d60FragmentPinnedRva = 0x532beb", "kRegistryInit531d60FragmentPinnedRva = 0x532bf0")
        p.write_text(txt)

    print(f"e605_apply_once: OK region={rsha}")


if __name__ == "__main__":
    main()
