#!/usr/bin/env python3
"""Native mutation harness for the row-3 (`0x14db00`) and row-18 (`0x3c5cb0`) owner constructors.

Not a `check_*.py` (it compiles, so it stays out of `run_all_checkers.sh`). Needs only the repo and
`clang++`; it never reads the private ELF and executes nothing from the game image.

For each target it (1) compiles the shipped fixture against the unmutated module in a temp
directory and requires exit 0 (control), then (2) applies each single-site mutation to a *copy* of
the module source, recompiles out-of-tree, runs the fixture and requires a non-zero exit (the
fixture rejected the mutant). A mutant that compiles and passes is a fixture gap and fails the run;
a mutant that does not compile is reported as a harness error (it proves nothing).

    python3 -I tools/lift/mutate_owner_ctors.py            # both targets
    python3 -I tools/lift/mutate_owner_ctors.py 3c5cb0     # one target
"""

from __future__ import annotations

import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CXX = ["clang++", "-std=c++20", "-O0", "-I", str(ROOT / "native"), "-I", str(ROOT / "native/core")]

TARGETS = {
    "3c5cb0": {
        "module": "native/core/owner_ctor_3c5cb0.cpp",
        "test": "native/tests/test_owner_ctor_3c5cb0.cpp",
        "deps": ["native/core/init_sub_3be930.cpp", "native/core/owner_members.cpp"],
        "mutants": [
            ("node B allocated with node C size",
             "hooks.allocate(kOwnerRow18NodeBSize)", "hooks.allocate(kOwnerRow18NodeCSize)"),
            ("dword +0x78 is 0 instead of -1",
             "put<std::uint32_t>(o, kOwnerRow18Dword78Offset, kOwnerRow18Dword78Value);",
             "put<std::uint32_t>(o, kOwnerRow18Dword78Offset, 0u);"),
            ("dword +0x58 store dropped",
             "    put<std::uint32_t>(o, kOwnerRow18Dword58Offset, 0u);\n", ""),
            ("sentinel B not self-linked", "    self_link(node_b);\n", ""),
            ("sentinel C only half linked",
             "    self_link(node_c);\n",
             "    put_pointer(static_cast<std::uint8_t*>(node_c), 0x00, node_c);\n"),
            ("head A not stored", "    put_pointer(o, kOwnerRow18HeadAOffset, node_a);\n", ""),
            ("callbacks swapped (first)",
             "first.function = kOwnerRow18CallbackARva;", "first.function = kOwnerRow18CallbackBRva;"),
            ("registration flag 0",
             "hooks.register_116040(pool, first, kOwnerRow18RegisterAFlag)",
             "hooks.register_116040(pool, first, 0u)"),
            ("callable self is the pool",
             "second.self = object;", "second.self = pool;"),
            ("whole object zero-filled first",
             "    auto* const o = static_cast<std::uint8_t*>(object);\n",
             "    auto* const o = static_cast<std::uint8_t*>(object);\n"
             "    std::memset(o, 0, kOwnerCtor3C5CB0ObjSize);\n"),
            ("second slot not initialised",
             "    init_slot_f180(o + kOwnerRow18Slot1Offset);\n", ""),
            ("first slot not initialised",
             "    init_slot_f180(o + kOwnerRow18Slot0Offset);\n", ""),
            ("qword +0x84 narrowed to a dword",
             "put<std::uint64_t>(o, kOwnerRow18Qword84Offset, 0u);",
             "put<std::uint32_t>(o, kOwnerRow18Qword84Offset, 0u);"),
            ("mutex initialised at +0xb8",
             "hooks.mutex_init(o + kOwnerRow18MutexOffset)",
             "hooks.mutex_init(o + kOwnerRow18MutexOffset + 8)"),
            ("second registration receives a null pool",
             "hooks.register_116540(pool, second)", "hooks.register_116540(nullptr, second)"),
            ("registration B result ignored",
             "if (hooks.register_116540 == nullptr || !hooks.register_116540(pool, second)) {",
             "if (hooks.register_116540 == nullptr) { (void)hooks.register_116540(pool, second); }\n    if (false) {"),
            ("mutex failure ignored",
             "if (hooks.mutex_init == nullptr || !hooks.mutex_init(o + kOwnerRow18MutexOffset)) {",
             "if (hooks.mutex_init == nullptr) { return {false, \"mutex_init\"}; }\n"
             "    (void)hooks.mutex_init(o + kOwnerRow18MutexOffset);\n    if (false) {"),
            ("adapter accepts any row",
             "if (row != kOwnerMemberRow18 || g_row18_binding == nullptr) {",
             "if (g_row18_binding == nullptr) {"),
            ("adapter forwards the object instead of the pool",
             "owner_ctor_3c5cb0(object, arg, g_row18_binding->hooks)",
             "owner_ctor_3c5cb0(object, object, g_row18_binding->hooks)"),
            ("byte +0x80 set to 1",
             "put<std::uint8_t>(o, kOwnerRow18Byte80Offset, 0u);",
             "put<std::uint8_t>(o, kOwnerRow18Byte80Offset, 1u);"),
        ],
    },
    "14db00": {
        "module": "native/core/owner_ctor_14db00.cpp",
        "test": "native/tests/test_owner_ctor_14db00.cpp",
        "deps": ["native/core/shared_str_release_f590.cpp", "native/core/owner_members.cpp"],
        "mutants": [
            ("description length 0x3d",
             "description.size = static_cast<std::int64_t>(kOwnerRow3CommandDescriptionLength);",
             "description.size = static_cast<std::int64_t>(kOwnerRow3CommandDescriptionLength) - 1;"),
            ("command flags 1",
             "owner_row3_command_ctor_20af0(o + kOwnerRow3CommandOffset, hooks.command_base_vptr, name,\n"
             "                                  description, kOwnerRow3CommandFlags);",
             "owner_row3_command_ctor_20af0(o + kOwnerRow3CommandOffset, hooks.command_base_vptr, name,\n"
             "                                  description, 1u);"),
            ("registered byte never set",
             "        command[kOwnerCommandRegisteredOffset] = 1;\n", ""),
            ("qword +0x78 store dropped",
             "    put<std::uint64_t>(o, kOwnerRow3Qword78Offset, 0u);\n", ""),
            ("anchor isnil byte 0",
             "    put<std::uint8_t>(a, 0x19, 1u);\n", "    put<std::uint8_t>(a, 0x19, 0u);\n"),
            ("anchor color byte 0",
             "    put<std::uint8_t>(a, 0x18, 1u);\n", "    put<std::uint8_t>(a, 0x18, 0u);\n"),
            ("anchor third link missing",
             "    put_pointer(a, 0x10, anchor);\n", ""),
            ("back pointer missing",
             "    put_pointer(o, kOwnerRow3BackPointerOffset, o);\n", ""),
            ("control weak count 0",
             "    put<std::uint32_t>(c, 0x0C, 1u);\n", "    put<std::uint32_t>(c, 0x0C, 0u);\n"),
            ("control strong count 2",
             "    put<std::uint32_t>(c, 0x08, 1u);\n", "    put<std::uint32_t>(c, 0x08, 2u);\n"),
            ("helper/control slots swapped",
             "    put_pointer(o, kOwnerRow3HelperPtrOffset, helper);\n",
             "    put_pointer(o, kOwnerRow3HelperCtrlOffset, helper);\n"),
            ("control pointer missing",
             "    put_pointer(o, kOwnerRow3HelperCtrlOffset, control);\n", ""),
            ("command ref-count increments dropped",
             "        std::atomic_ref<std::int64_t>(*str.ctl).fetch_add(1, std::memory_order_seq_cst);",
             "        (void)str;"),
            ("handler RVA off by one",
             "put<std::uint64_t>(o, kOwnerRow3HandlerOffset, kOwnerRow3HandlerRva);",
             "put<std::uint64_t>(o, kOwnerRow3HandlerOffset, kOwnerRow3HandlerRva + 1);"),
            ("command keeps the base vptr",
             "put_pointer(o, kOwnerRow3CommandOffset + kOwnerCommandVptrOffset, hooks.command_vptr);",
             "put_pointer(o, kOwnerRow3CommandOffset + kOwnerCommandVptrOffset, hooks.command_base_vptr);"),
            ("control block from 0x17070",
             "hooks.allocate_17090(kOwnerRow3ControlSize)", "hooks.allocate(kOwnerRow3ControlSize)"),
            ("byte +0x120 store dropped",
             "    put<std::uint8_t>(o, kOwnerRow3Byte120Offset, 0u);\n", ""),
            ("block +0x128 store dropped",
             "    put_bytes(o, kOwnerRow3Zero128Offset, kZero16, 16);\n", ""),
            ("block +0xf8 narrowed to 16 bytes",
             "put_bytes(o, kOwnerRow3Zero0F8Offset, kZero32, 32);",
             "put_bytes(o, kOwnerRow3Zero0F8Offset, kZero16, 16);"),
            ("whole object zero-filled first",
             "    auto* const o = static_cast<std::uint8_t*>(object);\n",
             "    auto* const o = static_cast<std::uint8_t*>(object);\n"
             "    std::memset(o, 0, kOwnerCtor14DB00ObjSize);\n"),
            ("engine not stored",
             "    put_pointer(o, kOwnerRow3EngineOffset, engine);\n", ""),
            ("map header leftmost wrong",
             "    put_pointer(o, kOwnerRow3MapLeftOffset, o + kOwnerRow3MapHeaderOffset);\n",
             "    put_pointer(o, kOwnerRow3MapLeftOffset, o);\n"),
            ("console hook before the guard byte",
             "        command[kOwnerCommandRegisteredOffset] = 1;\n"
             "        if (hooks.console_register == nullptr || !hooks.console_register(command)) {\n"
             "            return {false, \"console_register\"};\n"
             "        }\n",
             "        if (hooks.console_register == nullptr || !hooks.console_register(command)) {\n"
             "            return {false, \"console_register\"};\n"
             "        }\n"
             "        command[kOwnerCommandRegisteredOffset] = 1;\n"),
            ("helper vptr missing",
             "    put_pointer(o, 0x00, vptr);\n    put_bytes(o, 0x10, kZero16, 16);",
             "    put_bytes(o, 0x10, kZero16, 16);"),
            ("name view built with strlen - 1",
             "out.size = static_cast<std::int64_t>(std::strlen(cstr));",
             "out.size = static_cast<std::int64_t>(std::strlen(cstr)) - 1;"),
            ("mutex initialised at +0x78",
             "hooks.mutex_init(o + kOwnerRow3MutexOffset)", "hooks.mutex_init(o + kOwnerRow3Qword78Offset)"),
            ("adapter accepts any row",
             "if (row != kOwnerMemberRow3 || g_row3_binding == nullptr) {",
             "if (g_row3_binding == nullptr) {"),
            ("a vptr is not required",
             "        hooks.helper_vptr == nullptr) {", "        false) {"),
            ("wrong pool vptr",
             "put_pointer(o, kOwnerRow3VptrOffset, hooks.pool_vptr);",
             "put_pointer(o, kOwnerRow3VptrOffset, hooks.control_vptr);"),
            ("console failure ignored",
             "if (hooks.console_register == nullptr || !hooks.console_register(command)) {",
             "if (hooks.console_register == nullptr) { return {false, \"console_register\"}; }\n"
             "        (void)hooks.console_register(command);\n        if (false) {"),
        ],
    },
}


def build_and_run(work: Path, name: str, module_text: str, target: dict) -> tuple[str, str]:
    module_path = work / f"{name}.cpp"
    module_path.write_text(module_text)
    binary = work / name
    command = CXX + [str(ROOT / target["test"]), str(module_path)]
    command += [str(ROOT / dep) for dep in target["deps"]] + ["-o", str(binary)]
    build = subprocess.run(command, capture_output=True, text=True)
    if build.returncode != 0:
        return "compile-error", build.stderr.strip().splitlines()[0] if build.stderr else ""
    run = subprocess.run([str(binary)], capture_output=True, text=True)
    return ("pass" if run.returncode == 0 else "rejected"), run.stderr.strip()


def run_target(key: str) -> tuple[int, int, int]:
    target = TARGETS[key]
    source = (ROOT / target["module"]).read_text()
    rejected = survived = errors = 0
    with tempfile.TemporaryDirectory() as tmp:
        work = Path(tmp)
        # The module `#include "owner_ctor_xxx.hpp"` resolves through -I native/core.
        status, detail = build_and_run(work, "control", source, target)
        if status != "pass":
            raise SystemExit(f"FAIL control for {key}: {status} {detail}")
        for index, (label, old, new) in enumerate(target["mutants"]):
            if source.count(old) != 1:
                print(f"  HARNESS-ERROR {label!r}: snippet occurs {source.count(old)} times")
                errors += 1
                continue
            status, detail = build_and_run(work, f"mutant{index}", source.replace(old, new), target)
            if status == "rejected":
                rejected += 1
                print(f"  rejected  {label}  [{detail.splitlines()[0] if detail else ''}]")
            elif status == "pass":
                survived += 1
                print(f"  SURVIVED  {label}")
            else:
                errors += 1
                print(f"  COMPILE-ERROR {label}: {detail}")
    return rejected, survived, errors


def main() -> int:
    keys = sys.argv[1:] or list(TARGETS)
    total_bad = 0
    for key in keys:
        if key not in TARGETS:
            raise SystemExit(f"unknown target {key}")
        print(f"== {key}")
        rejected, survived, errors = run_target(key)
        print(f"== {key}: {rejected} rejected, {survived} survived, {errors} harness/compile errors")
        total_bad += survived + errors
    return 1 if total_bad else 0


if __name__ == "__main__":
    sys.exit(main())
