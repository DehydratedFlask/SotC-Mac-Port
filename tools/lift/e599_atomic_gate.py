#!/usr/bin/env python3
"""Apply E599 registry_body wiring and run the init-hash gate (single critical section)."""
from __future__ import annotations

import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def patch_hpp(t: str) -> str:
    t = re.sub(
        r"// Init `0x531d60` registry fragment body — OBS @ `0x532b19\.\.0x532b[a-f0-9]+` \([^)]+\)\.",
        "// Init `0x531d60` registry fragment body — OBS @ `0x532b19..0x532bd6` (49 units after E599 peel).",
        t,
    )
    for line in (
        '#include "init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1.hpp"\n',
        '#include "init_531d60_post_base_mov_r13_8_rax_532bd9_u1.hpp"\n',
    ):
        t = t.replace(line, "")
    t = re.sub(
        r"inline constexpr std::size_t kInit531d60PostBaseRegistryBodySize = \d+;",
        "inline constexpr std::size_t kInit531d60PostBaseRegistryBodySize = 189;",
        t,
    )
    t = re.sub(
        r"inline constexpr std::uint64_t kInit531d60PostBaseRegistryBodyEndRva = 0x532b[a-f0-9]+;",
        "inline constexpr std::uint64_t kInit531d60PostBaseRegistryBodyEndRva = 0x532bd6;",
        t,
    )
    t = re.sub(
        r"static_assert\(kInit531d60PostBaseJe532bdb_532bd4_U1EndRva ==\s*\n\s*kInit531d60PostBaseMovRaxRdiMem_532bd6_U1Rva\);\s*\n"
        r"(?:static_assert\(kInit531d60PostBaseMovRaxRdiMem_532bd6_U1EndRva ==\s*\n\s*kInit531d60PostBase(?:MovR13_8_Rax_532bd9_U1Rva|RegistryBodyEndRva)\);\s*\n)?"
        r"(?:static_assert\(kInit531d60PostBaseMovR13_8_Rax_532bd9_U1EndRva ==\s*\n\s*kInit531d60PostBaseRegistryBodyEndRva\);\s*\n)?",
        "static_assert(kInit531d60PostBaseJe532bdb_532bd4_U1EndRva ==\n                  kInit531d60PostBaseRegistryBodyEndRva);\n",
        t,
    )
    t = re.sub(
        r"    je_532bdb_532bd4_u1,\n(?:    mov_rax_rdi_mem_532bd6_u1,\n)?(?:    mov_r13_8_rax_532bd9_u1,\n)?    count",
        "    je_532bdb_532bd4_u1,\n    count",
        t,
    )
    for hook in (
        "    bool merge_mov_rax_rdi_mem_532bd6_u1_ran = false;\n",
        "    bool merge_mov_r13_8_rax_532bd9_u1_ran = false;\n",
    ):
        t = t.replace(hook, "")
    t = re.sub(
        r"    Init531d60PostBaseMovRaxRdiMem_532bd6_U1Cells mov_rax_rdi_mem_532bd6_cells\{\};\n",
        "",
        t,
    )
    t = re.sub(
        r"    Init531d60PostBaseMovR13_8_Rax_532bd9_U1Cells mov_r13_8_rax_532bd9_cells\{\};\n",
        "",
        t,
    )
    t = re.sub(r"std::array<int, \d+> unit_counts\{\};", "std::array<int, 49> unit_counts{};", t)
    t = re.sub(
        r"    int mov_rax_rdi_mem_532bd6_count = 0;\n    void\* rax_from_rdi_mem_532bd6 = nullptr;\n",
        "",
        t,
    )
    t = re.sub(
        r"    int mov_r13_8_rax_532bd9_count = 0;\n    void\* rax_stored_r13_8 = nullptr;\n",
        "",
        t,
    )
    return t


def patch_cpp(c: str) -> str:
    c = re.sub(
        r"\n    if \(!hooks\.merge_mov_rax_rdi_mem_532bd6_u1_ran\) \{.*?\n        \+\+out\.unit_executed;\n    \}",
        "",
        c,
        flags=re.S,
    )
    c = re.sub(
        r"\n    if \(!hooks\.merge_mov_r13_8_rax_532bd9_u1_ran\) \{.*?\n        \+\+out\.unit_executed;\n    \}",
        "",
        c,
        flags=re.S,
    )
    c = re.sub(
        r"    const int expected_mov_rax_rdi_mem_532bd6 =\n        hooks\.merge_mov_rax_rdi_mem_532bd6_u1_ran \? 0 : 1;\n",
        "",
        c,
    )
    c = re.sub(
        r"    const int expected_mov_r13_8_rax_532bd9 =\n        hooks\.merge_mov_r13_8_rax_532bd9_u1_ran \? 0 : 1;\n",
        "",
        c,
    )
    c = re.sub(
        r" \+ expected_je_532bdb;\s*\n\s+expected_mov_rax_rdi_mem_532bd6(?: \+ expected_mov_r13_8_rax_532bd9)?;",
        " + expected_je_532bdb;",
        c,
    )
    c = re.sub(r"\n\s+expected_mov_rax_rdi_mem_532bd6(?: \+ expected_mov_r13_8_rax_532bd9)?;", "", c)
    c = re.sub(
        r"             \(out\.je_532bdb_count == expected_je_532bdb\) &&\n             \(out\.mov_rax_rdi_mem_532bd6_count == expected_mov_rax_rdi_mem_532bd6\)(?: &&\n             \(out\.mov_r13_8_rax_532bd9_count == expected_mov_r13_8_rax_532bd9\))?;",
        "             (out.je_532bdb_count == expected_je_532bdb);",
        c,
    )
    if "out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;" not in c.split("const int expected_call")[0][-600:]:
        c = c.replace(
            "        ++out.unit_executed;\n    }\n    const int expected_call",
            "        ++out.unit_executed;\n    }\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;\n    const int expected_call",
            1,
        )
    c = re.sub(
        r"(expected_test_rdi_rdi \+ expected_je_532bdb)\s*\n\s+out\.ok",
        r"\1;\n    out.ok",
        c,
    )
    return c


def patch_others() -> None:
    rp = ROOT / "native/core/registry_init_531d60.hpp"
    r = rp.read_text()
    r = re.sub(
        r"inline constexpr std::uint64_t kRegistryInit531d60FragmentPinnedRva = 0x532b[a-f0-9]+;",
        "inline constexpr std::uint64_t kRegistryInit531d60FragmentPinnedRva = 0x532bd6;",
        r,
    )
    for hook in (
        "    bool merge_mov_rax_rdi_mem_532bd6_u1_ran = false;\n",
        "    bool merge_mov_r13_8_rax_532bd9_u1_ran = false;\n",
    ):
        r = r.replace(hook, "")
    rp.write_text(r)

    sp = ROOT / "native/core/appmain_init_ctor_98a90_slice.cpp"
    s = sp.read_text()
    s = re.sub(
        r"\n    if \(out\.post_base_registry_body\.mov_rax_rdi_mem_532bd6_count == 1\) \{.*?\n    \}\n(?:    if \(out\.post_base_registry_body\.mov_r13_8_rax_532bd9_count == 1\) \{.*?\n    \}\n)?",
        "\n",
        s,
        flags=re.S,
    )
    sp.write_text(s)

    tp = ROOT / "native/tests/test_appmain_init_ctor_98a90_slice.cpp"
    tt = tp.read_text()
    for hook in (
        "    pbregbody.merge_mov_rax_rdi_mem_532bd6_u1_ran = false;\n",
        "    pbregbody.merge_mov_r13_8_rax_532bd9_u1_ran = false;\n",
    ):
        tt = tt.replace(hook, "")
    tt = tt.replace("    CHECK(r.post_base_registry_body.mov_rax_rdi_mem_532bd6_count == 1);\n", "")
    tt = tt.replace("    CHECK(r.post_base_registry_body.mov_r13_8_rax_532bd9_count == 1);\n", "")
    tt = re.sub(
        r"CHECK\(kInit531d60PostBaseRegistryBodyUnitCount == \d+\);",
        "CHECK(kInit531d60PostBaseRegistryBodyUnitCount == 49);",
        tt,
    )
    tt = re.sub(
        r"CHECK\(r\.post_base_registry_body\.end_rva == 0x532b[a-f0-9]+\);",
        "CHECK(r.post_base_registry_body.end_rva == 0x532bd6);",
        tt,
    )
    tp.write_text(tt)


def apply_all() -> None:
    hpp = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
    cpp = ROOT / "native/core/init_531d60_post_base_registry_body.cpp"
    hpp.write_text(patch_hpp(hpp.read_text()))
    cpp.write_text(patch_cpp(cpp.read_text()))
    patch_others()


def build_core() -> int:
    return subprocess.run(
        ["cmake", "--build", "local/build/orch-r22", "-j12", "--target", "sotc_core"],
        cwd=ROOT,
    ).returncode


def run_gate() -> int:
    je = next(ROOT.glob("tools/lift/check_f531d60_post_base_je_532bdb_532bd4*.py"))
    for cmd in (
        ["cmake", "--build", "local/build/orch-r22", "-j12"],
        ["ctest", "--test-dir", "local/build/orch-r22", "-j12", "--output-on-failure"],
        ["python3", str(je)],
        ["python3", "tools/lift/check_f531d60_post_base_registry_body.py"],
    ):
        if subprocess.run(cmd, cwd=ROOT).returncode:
            return 1
    return 0


def main() -> int:
    for _ in range(40):
        apply_all()
        if build_core() == 0:
            break
    else:
        return 1
    apply_all()
    return run_gate()


if __name__ == "__main__":
    sys.exit(main())
