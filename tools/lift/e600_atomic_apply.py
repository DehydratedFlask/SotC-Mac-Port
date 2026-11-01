#!/usr/bin/env python3
"""Apply E600 mov_rax_rdi_mem_532bd6 wiring atomically (50 units, end 0x532bd9)."""
from __future__ import annotations

import hashlib
import re
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

HPP = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CPP = ROOT / "native/core/init_531d60_post_base_registry_body.cpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
SC = ROOT / "native/core/appmain_init_ctor_98a90_slice.cpp"
TEST = ROOT / "native/tests/test_appmain_init_ctor_98a90_slice.cpp"
BODY_CHK = ROOT / "tools/lift/check_f531d60_post_base_registry_body.py"
MOV_CHK = ROOT / "tools/lift/check_f531d60_post_base_mov_rax_rdi_mem_532bd6_u1.py"


def patch_hpp(text: str) -> str:
    text = re.sub(
        r"// Init `0x531d60` registry fragment body — OBS @ `0x532b19\.\.0x532b[a-f0-9]+` \([^)]+\)\.",
        "// Init `0x531d60` registry fragment body — OBS @ `0x532b19..0x532bd9` (50 units after E600 peel).",
        text,
        count=1,
    )
    text = text.replace('#include "init_531d60_post_base_mov_r13_8_rax_532bd9_u1.hpp"\n', "")
    if "init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1.hpp" not in text:
        text = text.replace(
            '#include "init_531d60_post_base_je_532bdb_532bd4_u1.hpp"\n',
            '#include "init_531d60_post_base_je_532bdb_532bd4_u1.hpp"\n'
            '#include "init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1.hpp"\n',
        )
    text = re.sub(
        r"inline constexpr std::size_t kInit531d60PostBaseRegistryBodySize = \d+;",
        "inline constexpr std::size_t kInit531d60PostBaseRegistryBodySize = 192;",
        text,
        count=1,
    )
    text = re.sub(
        r"inline constexpr std::uint64_t kInit531d60PostBaseRegistryBodyEndRva = 0x532b[a-f0-9]+;",
        "inline constexpr std::uint64_t kInit531d60PostBaseRegistryBodyEndRva = 0x532bd9;",
        text,
        count=1,
    )
    text = re.sub(
        r"static_assert\(kInit531d60PostBaseJe532bdb_532bd4_U1EndRva ==\s*\n\s*kInit531d60PostBaseMovRaxRdiMem_532bd6_U1Rva\);\s*\n"
        r"static_assert\(kInit531d60PostBaseMovRaxRdiMem_532bd6_U1EndRva ==\s*\n\s*kInit531d60PostBase(?:MovR13_8_Rax_532bd9_U1Rva|RegistryBodyEndRva)\);\s*\n"
        r"(?:static_assert\(kInit531d60PostBaseMovR13_8_Rax_532bd9_U1EndRva ==\s*\n\s*kInit531d60PostBaseRegistryBodyEndRva\);\s*\n)?",
        "static_assert(kInit531d60PostBaseJe532bdb_532bd4_U1EndRva ==\n"
        "                  kInit531d60PostBaseMovRaxRdiMem_532bd6_U1Rva);\n"
        "static_assert(kInit531d60PostBaseMovRaxRdiMem_532bd6_U1EndRva ==\n"
        "                  kInit531d60PostBaseRegistryBodyEndRva);\n",
        text,
        count=1,
    )
    text = re.sub(
        r"    je_532bdb_532bd4_u1,\n(?:    mov_rax_rdi_mem_532bd6_u1,\n)?(?:    mov_r13_8_rax_532bd9_u1,\n)?    count",
        "    je_532bdb_532bd4_u1,\n    mov_rax_rdi_mem_532bd6_u1,\n    count",
        text,
        count=1,
    )
    if "merge_mov_rax_rdi_mem_532bd6_u1_ran" not in text:
        text = text.replace(
            "    bool merge_je_532bdb_532bd4_u1_ran = false;\n",
            "    bool merge_je_532bdb_532bd4_u1_ran = false;\n"
            "    bool merge_mov_rax_rdi_mem_532bd6_u1_ran = false;\n",
        )
    text = text.replace("    bool merge_mov_r13_8_rax_532bd9_u1_ran = false;\n", "")
    text = re.sub(
        r"    Init531d60PostBaseJe532bdb_532bd4_U1Cells je_532bdb_532bd4_cells\{\};\n"
        r"(?:    Init531d60PostBaseMovRaxRdiMem_532bd6_U1Cells mov_rax_rdi_mem_532bd6_cells\{\};\n)?"
        r"(?:    Init531d60PostBaseMovR13_8_Rax_532bd9_U1Cells mov_r13_8_rax_532bd9_cells\{\};\n)?",
        "    Init531d60PostBaseJe532bdb_532bd4_U1Cells je_532bdb_532bd4_cells{};\n"
        "    Init531d60PostBaseMovRaxRdiMem_532bd6_U1Cells mov_rax_rdi_mem_532bd6_cells{};\n",
        text,
        count=1,
    )
    text = re.sub(r"std::array<int, \d+> unit_counts\{\};", "std::array<int, 50> unit_counts{};", text, count=1)
    text = re.sub(r"    int mov_r13_8_rax_532bd9_count = 0;\n", "", text)
    text = re.sub(r"    void\* rax_stored_r13_8 = nullptr;\n", "", text)
    if "mov_rax_rdi_mem_532bd6_count" not in text:
        text = text.replace(
            "    bool je_532bdb_branch_outcome_modelled = false;\n",
            "    bool je_532bdb_branch_outcome_modelled = false;\n"
            "    int mov_rax_rdi_mem_532bd6_count = 0;\n"
            "    void* rax_from_rdi_mem_532bd6 = nullptr;\n",
        )
    text = text.replace('#include "init_531d60_post_base_mov_r13_8_rax_532bd9_u1.hpp"\n', "")
    return text


MOV_BLOCK = """
    if (!hooks.merge_mov_rax_rdi_mem_532bd6_u1_ran) {
        Init531d60PostBaseMovRaxRdiMem_532bd6_U1Cells cells = hooks.mov_rax_rdi_mem_532bd6_cells;
        if (cells.cell_ptr == nullptr && out.rdi_from_r12_20 != nullptr) {
            cells.cell_ptr = reinterpret_cast<void* const*>(out.rdi_from_r12_20);
        }
        const Init531d60PostBaseMovRaxRdiMem_532bd6_U1Result mov_rax =
            init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1(cells);
        if (!mov_rax.ok) {
            return out;
        }
        out.mov_rax_rdi_mem_532bd6_count = mov_rax.mov_rax_rdi_mem_532bd6_count;
        out.rax_from_rdi_mem_532bd6 = mov_rax.rax;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::mov_rax_rdi_mem_532bd6_u1)] = 1;
        ++out.unit_executed;
    }
"""


def patch_cpp(text: str) -> str:
    text = re.sub(
        r"\n    if \(!hooks\.merge_mov_r13_8_rax_532bd9_u1_ran\) \{.*?\n        \+\+out\.unit_executed;\n    \}\n",
        "\n",
        text,
        flags=re.S,
    )
    if "merge_mov_rax_rdi_mem_532bd6_u1_ran" not in text.split("out.end_rva")[0]:
        text = text.replace(
            "        ++out.unit_executed;\n    }\n\n\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;",
            "        ++out.unit_executed;\n    }\n" + MOV_BLOCK + "\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;",
            1,
        )
        text = text.replace(
            "        ++out.unit_executed;\n    }\n\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;",
            "        ++out.unit_executed;\n    }\n" + MOV_BLOCK + "\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;",
            1,
        )
    if "expected_mov_rax_rdi_mem_532bd6" not in text:
        text = text.replace(
            "    const int expected_je_532bdb =\n        hooks.merge_je_532bdb_532bd4_u1_ran ? 0 : 1;\n",
            "    const int expected_je_532bdb =\n        hooks.merge_je_532bdb_532bd4_u1_ran ? 0 : 1;\n"
            "    const int expected_mov_rax_rdi_mem_532bd6 =\n"
            "        hooks.merge_mov_rax_rdi_mem_532bd6_u1_ran ? 0 : 1;\n",
        )
    text = re.sub(
        r"    const int expected_mov_r13_8_rax_532bd9 =\n        hooks\.merge_mov_r13_8_rax_532bd9_u1_ran \? 0 : 1;\n",
        "",
        text,
    )
    text = re.sub(
        r"(expected_test_rdi_rdi \+ expected_je_532bdb)(?: \+ expected_mov_rax_rdi_mem_532bd6(?: \+ expected_mov_r13_8_rax_532bd9)?)?;\s*\n\s+out\.ok",
        r"\1 + expected_mov_rax_rdi_mem_532bd6;\n    out.ok",
        text,
        count=1,
    )
    text = text.replace(
        "expected_mov_rax_rdi_mem_532bd6 + expected_mov_r13_8_rax_532bd9;",
        "expected_mov_rax_rdi_mem_532bd6;",
    )
    text = re.sub(
        r"             \(out\.je_532bdb_count == expected_je_532bdb\)(?: &&\n             \(out\.mov_rax_rdi_mem_532bd6_count == expected_mov_rax_rdi_mem_532bd6\))?(?: &&\n             \(out\.mov_r13_8_rax_532bd9_count == expected_mov_r13_8_rax_532bd9\))?;",
        "             (out.je_532bdb_count == expected_je_532bdb) &&\n"
        "             (out.mov_rax_rdi_mem_532bd6_count == expected_mov_rax_rdi_mem_532bd6);",
        text,
        count=1,
    )
    return text


def patch_registry(text: str) -> str:
    text = re.sub(
        r"inline constexpr std::uint64_t kRegistryInit531d60FragmentPinnedRva = 0x532b[a-f0-9]+;",
        "inline constexpr std::uint64_t kRegistryInit531d60FragmentPinnedRva = 0x532bd9;",
        text,
        count=1,
    )
    if "merge_mov_rax_rdi_mem_532bd6_u1_ran" not in text:
        text = text.replace(
            "    bool merge_je_532bdb_532bd4_u1_ran = false;\n",
            "    bool merge_je_532bdb_532bd4_u1_ran = false;\n"
            "    bool merge_mov_rax_rdi_mem_532bd6_u1_ran = false;\n",
        )
    text = text.replace("    bool merge_mov_r13_8_rax_532bd9_u1_ran = false;\n", "")
    return text


def patch_slice_and_test() -> None:
    sc = SC.read_text()
    if "merge_mov_rax_rdi_mem_532bd6_u1_ran" not in sc:
        sc = sc.replace(
            "        registry_hooks.merge_je_532bdb_532bd4_u1_ran = true;\n",
            "        registry_hooks.merge_je_532bdb_532bd4_u1_ran = true;\n"
            "        registry_hooks.merge_mov_rax_rdi_mem_532bd6_u1_ran = true;\n",
        )
    sc = re.sub(
        r"\n    if \(out\.post_base_registry_body\.mov_r13_8_rax_532bd9_count == 1\) \{.*?\n    \}\n",
        "\n",
        sc,
        flags=re.S,
    )
    SC.write_text(sc)

    tt = TEST.read_text()
    tt = tt.replace("    pbregbody.merge_mov_r13_8_rax_532bd9_u1_ran = false;\n", "")
    tt = tt.replace("    CHECK(r.post_base_registry_body.mov_r13_8_rax_532bd9_count == 1);\n", "")
    if "pbregbody.merge_mov_rax_rdi_mem_532bd6_u1_ran" not in tt:
        tt = tt.replace(
            "    pbregbody.merge_je_532bdb_532bd4_u1_ran = false;\n",
            "    pbregbody.merge_je_532bdb_532bd4_u1_ran = false;\n"
            "    pbregbody.merge_mov_rax_rdi_mem_532bd6_u1_ran = false;\n",
        )
    tt = re.sub(
        r"CHECK\(kInit531d60PostBaseRegistryBodyUnitCount == \d+\);",
        "CHECK(kInit531d60PostBaseRegistryBodyUnitCount == 50);",
        tt,
        count=1,
    )
    tt = re.sub(
        r"CHECK\(r\.post_base_registry_body\.end_rva == 0x532b[a-f0-9]+\);",
        "CHECK(r.post_base_registry_body.end_rva == 0x532bd9);",
        tt,
        count=1,
    )
    if "mov_rax_rdi_mem_532bd6_count == 1" not in tt:
        tt = tt.replace(
            "    CHECK(r.post_base_registry_body.je_532bdb_count == 1);\n",
            "    CHECK(r.post_base_registry_body.je_532bdb_count == 1);\n"
            "    CHECK(r.post_base_registry_body.mov_rax_rdi_mem_532bd6_count == 1);\n",
        )
    TEST.write_text(tt)


def region_sha() -> str:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)
    start, size = 0x532B19, 192
    peel_lea = bytes.fromhex("488d742410")
    peel_mov = bytes.fromhex("498b5d10")
    peel_mov15 = bytes.fromhex("4d8b7d18")
    peel_cmp15 = bytes.fromhex("4c39fb")
    peel_je = bytes.fromhex("741b")
    peel_nop = bytes.fromhex("0f1f00")
    data = bytearray(img.read(start, size))
    data[0x532B6A - start : 0x532B6F - start] = peel_lea
    data[0x532B6F - start] = 0
    data[0x532B70 - start : 0x532B74 - start] = peel_mov
    data[0x532B74 - start : 0x532B78 - start] = peel_mov15
    data[0x532B78 - start : 0x532B7B - start] = peel_cmp15
    data[0x532B7B - start : 0x532B7D - start] = peel_je
    data[0x532B7D - start : 0x532B80 - start] = peel_nop
    data[0x532BD6 - start : 0x532BD9 - start] = bytes.fromhex("488b07")
    return hashlib.sha256(bytes(data)).hexdigest()


def patch_body_checker(region: str) -> None:
    t = BODY_CHK.read_text()
    t = re.sub(r"START, SIZE, END = 0x532B19, \d+, 0x532B[A-Fa-f0-9]+", "START, SIZE, END = 0x532B19, 192, 0x532BD9", t, count=1)
    t = re.sub(
        r"    data\[0x532BD6-START:0x532BD9-START\]=PEEL_MOV_RAX_RDI\n"
        r"    data\[0x532BD9-START:0x532BDE-START\]=PEEL_MOV_R13_8_RAX\n",
        "    data[0x532BD6-START:0x532BD9-START]=PEEL_MOV_RAX_RDI\n",
        t,
        count=1,
    )
    t = re.sub(
        r"    if rva==0x532BD9 and sz==5:\n        return PEEL_MOV_R13_8_RAX\n",
        "",
        t,
        count=1,
    )
    t = re.sub(
        r'REGION_SHA256 = "[0-9a-f]+"',
        f'REGION_SHA256 = "{region}"',
        t,
        count=1,
    )
    t = re.sub(
        r"\n    \(0x532BD9,5,\"[0-9a-f]+\"\),\n\]",
        "\n]",
        t,
        count=1,
    )
    t = t.replace("require(len(UNITS)==51,'units')", "require(len(UNITS)==50,'units')")
    t = t.replace(
        "'kInit531d60PostBaseRegistryBodyEndRva = 0x532bde' in nh,'end')",
        "'kInit531d60PostBaseRegistryBodyEndRva = 0x532bd9' in nh,'end')",
    )
    t = t.replace("'mov_r13_8_rax_532bd9_u1' in nh,'unit enum')", "'mov_rax_rdi_mem_532bd6_u1' in nh,'unit enum')")
    t = t.replace(
        " and 'mov_r13_8_rax_532bd9_u1' in nh,'unit enum')",
        " and 'mov_rax_rdi_mem_532bd6_u1' in nh,'unit enum')",
    )
    t = re.sub(
        r"require\('merge_mov_r13_8_rax_532bd9_u1_ran' in nh,'merge mov r13 8 rax 532bd9'\)\n",
        "",
        t,
    )
    t = t.replace("kInit531d60PostBaseRegistryBodyUnitCount == 51", "kInit531d60PostBaseRegistryBodyUnitCount == 50")
    t = t.replace("kRegistryInit531d60FragmentPinnedRva = 0x532bde", "kRegistryInit531d60FragmentPinnedRva = 0x532bd9")
    t = t.replace("end=0x532bde units=51", "end=0x532bd9 units=50")
    t = t.replace("'end_rva == 0x532bde'", "'end_rva == 0x532bd9'")
    t = t.replace("kInit531d60PostBaseRegistryBodyUnitCount == 51", "kInit531d60PostBaseRegistryBodyUnitCount == 50")
    t = re.sub(r" and 'mov_r13_8_rax_532bd9_u1' in nh", "", t)
    t = t.replace("'mov_rax_rdi_mem_532bd6_u1' in nh and 'mov_rax_rdi_mem_532bd6_u1' in nh", "'mov_rax_rdi_mem_532bd6_u1' in nh")
    if "init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1" not in t:
        t = t.replace(
            "require('init_531d60_post_base_je_532bdb_532bd4_u1' in NCPP.read_text(),'cpp je 532bdb 532bd4')\n",
            "require('init_531d60_post_base_je_532bdb_532bd4_u1' in NCPP.read_text(),'cpp je 532bdb 532bd4')\n"
            "    require('init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1' in NCPP.read_text(),'cpp mov rax rdi 532bd6')\n",
        )
    if "init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1.cpp" not in t:
        t = t.replace(
            "require('init_531d60_post_base_je_532bdb_532bd4_u1.cpp' in CMAKE.read_text(),'cmake je 532bdb 532bd4')\n",
            "require('init_531d60_post_base_je_532bdb_532bd4_u1.cpp' in CMAKE.read_text(),'cmake je 532bdb 532bd4')\n"
            "    require('init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1.cpp' in CMAKE.read_text(),'cmake mov rax rdi 532bd6')\n",
        )
    BODY_CHK.write_text(t)

    mc = MOV_CHK.read_text()
    mc = mc.replace(
        "kRegistryInit531d60FragmentPinnedRva = 0x532bde",
        "kRegistryInit531d60FragmentPinnedRva = 0x532bd9",
    )
    MOV_CHK.write_text(mc)


def apply_all() -> None:
    HPP.write_text(patch_hpp(HPP.read_text()))
    CPP.write_text(patch_cpp(CPP.read_text()))
    REG.write_text(patch_registry(REG.read_text()))
    patch_slice_and_test()
    patch_body_checker(region_sha())


def run_gate() -> int:
    cmds = [
        ["cmake", "--build", "local/build/orch-r22", "-j12"],
        ["ctest", "--test-dir", "local/build/orch-r22", "-j12", "--output-on-failure"],
        ["python3", str(ROOT / "tools/lift/check_f531d60_post_base_je_532bdb_532bd4_u1.py")],
        ["python3", str(MOV_CHK)],
        ["python3", str(BODY_CHK)],
    ]
    for cmd in cmds:
        if subprocess.run(cmd, cwd=ROOT).returncode:
            return 1
    return 0


def main() -> int:
    apply_all()
    return run_gate()


if __name__ == "__main__":
    raise SystemExit(main())
