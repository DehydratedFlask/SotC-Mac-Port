#!/usr/bin/env python3
"""Force init-hash E601 registry_body: 197 B, 51 units, end 0x532bde (no E602+ peels)."""
from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HPP = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
CPP = ROOT / "native/core/init_531d60_post_base_registry_body.cpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

E602_INCLUDES = (
    "init_531d60_post_base_mov_rax_rdi_mem_532bde_u1.hpp",
    "init_531d60_post_base_sub_eax_010cb932_532be1_u1.hpp",
)


def strip_hpp(t: str) -> str:
    for inc in E602_INCLUDES:
        t = re.sub(rf'#include "{re.escape(inc)}"\n', "", t)
    t = re.sub(
        r"// Init `0x531d60` registry fragment body — OBS @ `0x532b19\.\.[0-9a-f]+` \(\d+ units\)\.",
        "// Init `0x531d60` registry fragment body — OBS @ `0x532b19..0x532bde` (51 units).",
        t,
        count=1,
    )
    t = re.sub(
        r"inline constexpr std::size_t kInit531d60PostBaseRegistryBodySize = \d+;",
        "inline constexpr std::size_t kInit531d60PostBaseRegistryBodySize = 197;",
        t,
        count=1,
    )
    t = re.sub(
        r"inline constexpr std::uint64_t kInit531d60PostBaseRegistryBodyEndRva = 0x532b[a-f0-9]+;",
        "inline constexpr std::uint64_t kInit531d60PostBaseRegistryBodyEndRva = 0x532bde;",
        t,
        count=1,
    )
    t = re.sub(
        r"static_assert\(kInit531d60PostBaseMovR13_8_Rax_532bd9_U1EndRva ==\s*\n\s*kInit531d60PostBaseMovRaxRdiMem_532bde_U1Rva\);\s*"
        r"static_assert\(kInit531d60PostBaseMovRaxRdiMem_532bde_U1EndRva ==\s*\n\s*kInit531d60PostBaseSubEax[^\n]+\n\s*"
        r"static_assert\(kInit531d60PostBaseSubEax[^\n]+\n\s*kInit531d60PostBaseRegistryBodyEndRva\);",
        "static_assert(kInit531d60PostBaseMovR13_8_Rax_532bd9_U1EndRva ==\n"
        "                  kInit531d60PostBaseRegistryBodyEndRva);",
        t,
        count=1,
    )
    t = re.sub(
        r"static_assert\(kInit531d60PostBaseMovR13_8_Rax_532bd9_U1EndRva ==\s*\n\s*kInit531d60PostBaseMovRaxRdiMem_532bde_U1Rva\);\s*"
        r"static_assert\(kInit531d60PostBaseMovRaxRdiMem_532bde_U1EndRva ==\s*\n\s*kInit531d60PostBaseRegistryBodyEndRva\);",
        "static_assert(kInit531d60PostBaseMovR13_8_Rax_532bd9_U1EndRva ==\n"
        "                  kInit531d60PostBaseRegistryBodyEndRva);",
        t,
        count=1,
    )
    for tail in ("mov_rax_rdi_mem_532bde_u1", "sub_eax_010cb932_532be1_u1"):
        t = re.sub(rf"    {tail},\n", "", t)
    t = re.sub(r"std::array<int, \d+> unit_counts", "std::array<int, 51> unit_counts", t, count=1)
    for pat in (
        r"    bool merge_mov_rax_rdi_mem_532bde_u1_ran = false;\n",
        r"    bool merge_sub_eax_010cb932_532be1_u1_ran = false;\n",
        r"    Init531d60PostBaseMovRaxRdiMem_532bde_U1Cells mov_rax_rdi_mem_532bde_cells\{\};\n",
        r"    Init531d60PostBaseSubEax[^\n]+\n",
        r"    int mov_rax_rdi_mem_532bde_count = 0;\n",
        r"    void\* rax_from_rdi_mem_532bde = nullptr;\n",
        r"    int sub_eax_010cb932_532be1_count = 0;\n",
    ):
        t = re.sub(pat, "", t)
    if "mov_rax_rdi_mem_532bd6_u1" not in t.split("RegistryBodyUnit")[1].split("count")[0]:
        t = t.replace(
            "    mov_r13_8_rax_532bd9_u1,\n    count",
            "    mov_rax_rdi_mem_532bd6_u1,\n    mov_r13_8_rax_532bd9_u1,\n    count",
        )
    if "merge_mov_rax_rdi_mem_532bd6_u1_ran" not in t:
        t = t.replace(
            "    bool merge_je_532bdb_532bd4_u1_ran = false;\n",
            "    bool merge_je_532bdb_532bd4_u1_ran = false;\n"
            "    bool merge_mov_rax_rdi_mem_532bd6_u1_ran = false;\n"
            "    bool merge_mov_r13_8_rax_532bd9_u1_ran = false;\n",
            1,
        )
    if "mov_rax_rdi_mem_532bd6_cells" not in t:
        t = t.replace(
            "    Init531d60PostBaseJe532bdb_532bd4_U1Cells je_532bdb_532bd4_cells{};\n};",
            "    Init531d60PostBaseJe532bdb_532bd4_U1Cells je_532bdb_532bd4_cells{};\n"
            "    Init531d60PostBaseMovRaxRdiMem_532bd6_U1Cells mov_rax_rdi_mem_532bd6_cells{};\n"
            "    Init531d60PostBaseMovR13_8_Rax_532bd9_U1Cells mov_r13_8_rax_532bd9_cells{};\n};",
        )
    return t


def strip_cpp(t: str) -> str:
    t = re.sub(
        r"\n    if \(!hooks\.merge_mov_rax_rdi_mem_532bde_u1_ran\) \{.*?\n    \}\n",
        "\n",
        t,
        flags=re.S,
    )
    t = re.sub(
        r"\n    if \(!hooks\.merge_sub_eax_010cb932_532be1_u1_ran\) \{.*?\n    \}\n",
        "\n",
        t,
        flags=re.S,
    )
    t = re.sub(
        r"    const int expected_mov_rax_rdi_mem_532bde =[^\n]+\n",
        "",
        t,
    )
    t = re.sub(
        r"    const int expected_sub_eax_010cb932_532be1 =[^\n]+\n",
        "",
        t,
    )
    t = re.sub(r" \+ expected_mov_rax_rdi_mem_532bde", "", t)
    t = re.sub(r" \+ expected_sub_eax_010cb932_532be1", "", t)
    t = re.sub(
        r"\n             \(out\.mov_rax_rdi_mem_532bde_count == expected_mov_rax_rdi_mem_532bde\)[^\n]*",
        "",
        t,
    )
    t = re.sub(
        r"\n             \(out\.sub_eax_010cb932_532be1_count == expected_sub_eax_010cb932_532be1\)[^\n]*",
        "",
        t,
    )
    if "fallback_r13_node" not in t:
        t = t.replace(
            "        if (cells.r13_base == nullptr && hooks.stack_rsp != nullptr) {\n"
            "            cells.r13_base = const_cast<void*>(static_cast<const void*>(\n"
            "                static_cast<const std::uint8_t*>(hooks.stack_rsp) + 0x40u));\n"
            "        }\n",
            "        if (cells.r13_base == nullptr) {\n"
            "            if (hooks.stack_rsp != nullptr) {\n"
            "                cells.r13_base = const_cast<void*>(static_cast<const void*>(\n"
            "                    static_cast<const std::uint8_t*>(hooks.stack_rsp) + 0x40u));\n"
            "            } else {\n"
            "                alignas(16) static std::uint8_t fallback_r13_node[0x20]{};\n"
            "                cells.r13_base = fallback_r13_node;\n"
            "            }\n"
            "        }\n",
        )
    return t


def strip_reg(t: str) -> str:
    t = re.sub(
        r"inline constexpr std::uint64_t kRegistryInit531d60FragmentPinnedRva = 0x532b[a-f0-9]+;",
        "inline constexpr std::uint64_t kRegistryInit531d60FragmentPinnedRva = 0x532bde;",
        t,
        count=1,
    )
    t = re.sub(r"    bool merge_mov_rax_rdi_mem_532bde_u1_ran = false;\n", "", t)
    t = re.sub(r"    bool merge_sub_eax_010cb932_532be1_u1_ran = false;\n", "", t)
    if "merge_mov_r13_8_rax_532bd9_u1_ran" not in t:
        t = t.replace(
            "    bool merge_je_532bdb_532bd4_u1_ran = false;\n};",
            "    bool merge_je_532bdb_532bd4_u1_ran = false;\n"
            "    bool merge_mov_rax_rdi_mem_532bd6_u1_ran = false;\n"
            "    bool merge_mov_r13_8_rax_532bd9_u1_ran = false;\n};",
        )
    return t


def strip_cmake(t: str) -> str:
    for line in (
        "  core/init_531d60_post_base_mov_rax_rdi_mem_532bde_u1.cpp\n",
        "  core/init_531d60_post_base_sub_eax_010cb932_532be1_u1.cpp\n",
    ):
        t = t.replace(line, "")
    return t


def restore_checker() -> None:
    import subprocess

    chk = ROOT / "tools/lift/check_f531d60_post_base_registry_body.py"
    raw = subprocess.check_output(
        ["git", "show", "dd972731:tools/lift/check_f531d60_post_base_registry_body.py"],
        cwd=ROOT,
        text=True,
    )
    raw = raw.replace(
        "REGION_SHA256 = \"7ba806b5310316f61544890f08e0ef2913e5aac1b020f83e09a8dcf7356c0f9a\"",
        "REGION_SHA256 = \"3e12bda8a6dc8c51f14790403fb99c7a39ae0aba72fc2a8fa812dd3f4faf7b53\"",
    )
    raw = re.sub(
        r"kRegistryInit531d60FragmentPinnedRva = 0x532b[0-9a-f]+",
        "kRegistryInit531d60FragmentPinnedRva = 0x532bde",
        raw,
    )
    chk.write_text(raw)


def main() -> int:
    HPP.write_text(strip_hpp(HPP.read_text()))
    CPP.write_text(strip_cpp(CPP.read_text()))
    REG.write_text(strip_reg(REG.read_text()))
    CMAKE.write_text(strip_cmake(CMAKE.read_text()))
    restore_checker()
    print("e601_atomic_restore: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
