#!/usr/bin/env python3
"""Idempotent E601 registry_body end=0x532bde (51 units) + test/checker alignment."""
from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def patch_hpp(t: str) -> str:
    t = re.sub(
        r"// Init `0x531d60` registry fragment body — OBS @ `0x532b19\.\.[0-9a-f]+` \(.*?\)\.",
        "// Init `0x531d60` registry fragment body — OBS @ `0x532b19..0x532bde` (51 units).",
        t,
        count=1,
    )
    if "init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1.hpp" not in t:
        t = t.replace(
            '#include "init_531d60_post_base_je_532bdb_532bd4_u1.hpp"\n',
            '#include "init_531d60_post_base_je_532bdb_532bd4_u1.hpp"\n'
            '#include "init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1.hpp"\n'
            '#include "init_531d60_post_base_mov_r13_8_rax_532bd9_u1.hpp"\n',
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
        r"static_assert\(kInit531d60PostBaseJe532bdb_532bd4_U1EndRva ==\s*\n\s*kInit531d60PostBaseRegistryBodyEndRva\);",
        "static_assert(kInit531d60PostBaseJe532bdb_532bd4_U1EndRva ==\n"
        "                  kInit531d60PostBaseMovRaxRdiMem_532bd6_U1Rva);\n"
        "static_assert(kInit531d60PostBaseMovRaxRdiMem_532bd6_U1EndRva ==\n"
        "                  kInit531d60PostBaseMovR13_8_Rax_532bd9_U1Rva);\n"
        "static_assert(kInit531d60PostBaseMovR13_8_Rax_532bd9_U1EndRva ==\n"
        "                  kInit531d60PostBaseRegistryBodyEndRva);",
        t,
        count=1,
    )
    if "mov_rax_rdi_mem_532bd6_u1" not in t:
        t = t.replace(
            "    je_532bdb_532bd4_u1,\n    count",
            "    je_532bdb_532bd4_u1,\n    mov_rax_rdi_mem_532bd6_u1,\n    mov_r13_8_rax_532bd9_u1,\n    count",
        )
    elif "mov_r13_8_rax_532bd9_u1" not in t:
        t = t.replace(
            "    mov_rax_rdi_mem_532bd6_u1,\n    count",
            "    mov_rax_rdi_mem_532bd6_u1,\n    mov_r13_8_rax_532bd9_u1,\n    count",
        )
    if "merge_mov_rax_rdi_mem_532bd6_u1_ran" not in t:
        t = t.replace(
            "    bool merge_je_532bdb_532bd4_u1_ran = false;\n    const void* stack_rsp",
            "    bool merge_je_532bdb_532bd4_u1_ran = false;\n"
            "    bool merge_mov_rax_rdi_mem_532bd6_u1_ran = false;\n"
            "    bool merge_mov_r13_8_rax_532bd9_u1_ran = false;\n    const void* stack_rsp",
        )
    else:
        if "merge_mov_r13_8_rax_532bd9_u1_ran" not in t:
            t = t.replace(
                "    bool merge_mov_rax_rdi_mem_532bd6_u1_ran = false;\n",
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
    elif "mov_r13_8_rax_532bd9_cells" not in t:
        t = t.replace(
            "    Init531d60PostBaseMovRaxRdiMem_532bd6_U1Cells mov_rax_rdi_mem_532bd6_cells{};\n};",
            "    Init531d60PostBaseMovRaxRdiMem_532bd6_U1Cells mov_rax_rdi_mem_532bd6_cells{};\n"
            "    Init531d60PostBaseMovR13_8_Rax_532bd9_U1Cells mov_r13_8_rax_532bd9_cells{};\n};",
        )
    t = re.sub(r"std::array<int, \d+> unit_counts\{\};", "std::array<int, 51> unit_counts{};", t, count=1)
    if "mov_rax_rdi_mem_532bd6_count" not in t:
        t = t.replace(
            "    int je_532bdb_count = 0;\n",
            "    int je_532bdb_count = 0;\n"
            "    int mov_rax_rdi_mem_532bd6_count = 0;\n"
            "    void* rax_from_rdi_mem_532bd6 = nullptr;\n"
            "    int mov_r13_8_rax_532bd9_count = 0;\n"
            "    void* rax_stored_r13_8 = nullptr;\n",
            1,
        )
    return t


def patch_cpp(t: str) -> str:
    mov_rax_block = """
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
    mov_r13_block = """
    if (!hooks.merge_mov_r13_8_rax_532bd9_u1_ran) {
        Init531d60PostBaseMovR13_8_Rax_532bd9_U1Cells cells = hooks.mov_r13_8_rax_532bd9_cells;
        if (cells.rax_in == nullptr) {
            cells.rax_in = out.rax_from_rdi_mem_532bd6;
        }
        if (cells.r13_base == nullptr) {
            if (out.r13_after_add_30 != nullptr) {
                cells.r13_base = out.r13_after_add_30;
            } else if (out.r13_from_r12_40 != nullptr) {
                cells.r13_base = out.r13_from_r12_40;
            } else if (hooks.stack_rsp != nullptr) {
                cells.r13_base = const_cast<void*>(static_cast<const void*>(
                    static_cast<const std::uint8_t*>(hooks.stack_rsp) + 0x40u));
            }
        }
        const Init531d60PostBaseMovR13_8_Rax_532bd9_U1Result mov_r13 =
            init_531d60_post_base_mov_r13_8_rax_532bd9_u1(cells);
        if (!mov_r13.ok) {
            return out;
        }
        out.mov_r13_8_rax_532bd9_count = mov_r13.mov_r13_8_rax_532bd9_count;
        out.rax_stored_r13_8 = mov_r13.rax_stored;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::mov_r13_8_rax_532bd9_u1)] = 1;
        ++out.unit_executed;
    }
"""
    if "init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1(cells)" not in t:
        t = t.replace(
            "        ++out.unit_executed;\n    }\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;",
            "        ++out.unit_executed;\n    }\n" + mov_rax_block + mov_r13_block + "\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;",
            1,
        )
    elif "init_531d60_post_base_mov_r13_8_rax_532bd9_u1(cells)" not in t:
        t = t.replace(
            "        ++out.unit_executed;\n    }\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;",
            "        ++out.unit_executed;\n    }\n" + mov_r13_block + "\n    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;",
            1,
        )
    if "expected_mov_rax_rdi_mem_532bd6" not in t:
        t = t.replace(
            "    const int expected_je_532bdb =\n        hooks.merge_je_532bdb_532bd4_u1_ran ? 0 : 1;\n    const int expected_units",
            "    const int expected_je_532bdb =\n        hooks.merge_je_532bdb_532bd4_u1_ran ? 0 : 1;\n"
            "    const int expected_mov_rax_rdi_mem_532bd6 =\n        hooks.merge_mov_rax_rdi_mem_532bd6_u1_ran ? 0 : 1;\n"
            "    const int expected_mov_r13_8_rax_532bd9 =\n        hooks.merge_mov_r13_8_rax_532bd9_u1_ran ? 0 : 1;\n    const int expected_units",
        )
    t = re.sub(
        r"expected_test_rdi_rdi \+ expected_je_532bdb;\s*\n\s*out\.ok",
        "expected_test_rdi_rdi + expected_je_532bdb +\n"
        "                               expected_mov_rax_rdi_mem_532bd6 + expected_mov_r13_8_rax_532bd9;\n    out.ok",
        t,
        count=1,
    )
    t = re.sub(
        r"expected_test_rdi_rdi \+ expected_je_532bdb;\s*\n\s*out\.ok",
        "expected_test_rdi_rdi + expected_je_532bdb +\n"
        "                               expected_mov_rax_rdi_mem_532bd6 + expected_mov_r13_8_rax_532bd9;\n    out.ok",
        t,
        count=1,
    )
    t = re.sub(
        r"expected_test_rdi_rdi \+ expected_je_532bdb;\s*\n\s*out\.ok",
        "expected_test_rdi_rdi + expected_je_532bdb +\n"
        "                               expected_mov_rax_rdi_mem_532bd6 + expected_mov_r13_8_rax_532bd9;\n    out.ok",
        t,
        count=1,
    )
    if "mov_rax_rdi_mem_532bd6_count == expected_mov_rax_rdi_mem_532bd6" not in t:
        t = t.replace(
            "(out.je_532bdb_count == expected_je_532bdb);",
            "(out.je_532bdb_count == expected_je_532bdb) &&\n"
            "             (out.mov_rax_rdi_mem_532bd6_count == expected_mov_rax_rdi_mem_532bd6) &&\n"
            "             (out.mov_r13_8_rax_532bd9_count == expected_mov_r13_8_rax_532bd9);",
        )
    return t


def patch_test(t: str) -> str:
    t = t.replace(
        "CHECK(kInit531d60PostBaseRegistryBodyUnitCount == 50);",
        "CHECK(kInit531d60PostBaseRegistryBodyUnitCount == 51);",
    )
    if "merge_mov_r13_8_rax_532bd9_u1_ran" not in t:
        t = t.replace(
            "    pbregbody.merge_mov_rax_rdi_mem_532bd6_u1_ran = false;\n",
            "    pbregbody.merge_mov_rax_rdi_mem_532bd6_u1_ran = false;\n"
            "    pbregbody.merge_mov_r13_8_rax_532bd9_u1_ran = false;\n",
            1,
        )
    if "registry_r13_node" not in t:
        t = t.replace(
            "    alignas(16) static std::uint8_t registry_body_stack[0x200]{};\n    pbregbody.stack_rsp = registry_body_stack;",
            "    alignas(16) static std::uint8_t registry_body_stack[0x200]{};\n"
            "    alignas(16) static std::uint8_t registry_r13_node[0x20]{};\n"
            "    pbregbody.stack_rsp = registry_body_stack;\n"
            "    pbregbody.mov_r13_8_rax_532bd9_cells.r13_base = registry_r13_node;",
        )
    t = re.sub(
        r"CHECK\(r\.post_base_registry_body\.mov_rax_rdi_mem_532bd6_count == 1\);\n"
        r"(?:    CHECK\(r\.post_base_registry_body\.mov_r13_8_rax_532bd9_count == 1\);\n)?"
        r"    CHECK\(r\.post_base_registry_body\.end_rva == 0x532b[a-f0-9]+\);",
        "CHECK(r.post_base_registry_body.mov_rax_rdi_mem_532bd6_count == 1);\n"
        "    CHECK(r.post_base_registry_body.mov_r13_8_rax_532bd9_count == 1);\n"
        "    CHECK(r.post_base_registry_body.end_rva == 0x532bde);",
        t,
        count=1,
    )
    return t


def patch_reg(t: str) -> str:
    t = re.sub(
        r"kRegistryInit531d60FragmentPinnedRva = 0x532b[a-f0-9]+;",
        "kRegistryInit531d60FragmentPinnedRva = 0x532bde;",
        t,
        count=1,
    )
    if "merge_mov_r13_8_rax_532bd9_u1_ran" not in t:
        t = t.replace(
            "    bool merge_je_532bdb_532bd4_u1_ran = false;\n};",
            "    bool merge_je_532bdb_532bd4_u1_ran = false;\n"
            "    bool merge_mov_rax_rdi_mem_532bd6_u1_ran = false;\n"
            "    bool merge_mov_r13_8_rax_532bd9_u1_ran = false;\n};",
        )
    elif "merge_mov_rax_rdi_mem_532bd6_u1_ran" not in t:
        t = t.replace(
            "    bool merge_je_532bdb_532bd4_u1_ran = false;\n};",
            "    bool merge_je_532bdb_532bd4_u1_ran = false;\n"
            "    bool merge_mov_rax_rdi_mem_532bd6_u1_ran = false;\n"
            "    bool merge_mov_r13_8_rax_532bd9_u1_ran = false;\n};",
        )
    return t


def patch_slice(t: str) -> str:
    if "merge_mov_r13_8_rax_532bd9_u1_ran = true" in t:
        return t
    return t.replace(
        "    if (out.post_base_registry_body.mov_rax_rdi_mem_532bd6_count == 1) {\n"
        "        registry_hooks.merge_mov_rax_rdi_mem_532bd6_u1_ran = true;\n"
        "    }\n    out.remainder = run_appmain_init_remainder_slice",
        "    if (out.post_base_registry_body.mov_rax_rdi_mem_532bd6_count == 1) {\n"
        "        registry_hooks.merge_mov_rax_rdi_mem_532bd6_u1_ran = true;\n"
        "    }\n"
        "    if (out.post_base_registry_body.mov_r13_8_rax_532bd9_count == 1) {\n"
        "        registry_hooks.merge_mov_r13_8_rax_532bd9_u1_ran = true;\n"
        "    }\n    out.remainder = run_appmain_init_remainder_slice",
    )


def main() -> int:
    paths = {
        ROOT / "native/core/init_531d60_post_base_registry_body.hpp": patch_hpp,
        ROOT / "native/core/init_531d60_post_base_registry_body.cpp": patch_cpp,
        ROOT / "native/tests/test_appmain_init_ctor_98a90_slice.cpp": patch_test,
        ROOT / "native/core/registry_init_531d60.hpp": patch_reg,
        ROOT / "native/core/appmain_init_ctor_98a90_slice.cpp": patch_slice,
    }
    for path, fn in paths.items():
        text = path.read_text()
        path.write_text(fn(text))
    print("e601_finalize: patched", len(paths), "files")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
