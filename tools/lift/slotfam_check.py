#!/usr/bin/env python3
"""Shared ELF byte-oracle of the L3-SLOTS-322-334 startup constructor checkers.

tools/lift/check_ctor_slot_<N>.py are thin wrappers around main(N). Offline only: the pinned private
ELF (local/re/eboot.elf, sha256 d0723afb...5861d3) is read through slot_pin_util.Elf; nothing is
loaded, relocated or executed. For slot N the checker

  * compares the table word, the complete body sha256, the nop padding after the body and the final
    stack-check failure call, plus the literal prologue / epilogue bytes;
  * walks the straight-line body with tools/lift/slotfam_interp.py (a RECORDING symbolic walk: calls
    are not followed) and requires the ordered absolute stores and calls (targets, register arguments,
    and the stack views the arguments point at) to equal the sequence derived from
    tools/lift/slotfam_specs.py; a changed operand, displacement, immediate or call target changes the
    trace or breaks decoding (fail closed);
  * pins the strings the views point at, the shared callee bodies by sha256 (always, also during the
    mutation run), the class / functor tables by their RELATIVE relocations, and for 0x26db0 /
    0x26c10 / 0x3a980 the byte patterns the native lifts rely on;
  * compares every native constant / string / call order / test token / CMake token with the spec;
  * mutation self-test (default on, `--no-mutate` to skip): the first, last and a middle byte of every
    instruction of the body is flipped in an in-memory overlay (whole-body hash disabled) and every
    pinned relocation is perturbed, plus perturbations of every native token and constant; each mutant
    must be rejected;  `--tamper ADDR` flips one body byte with the hash enabled and must exit 1.

Private: needs local/re/eboot.elf.
"""

from __future__ import annotations

import multiprocessing
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import slotfam_interp as si  # noqa: E402
import slotfam_specs as sp  # noqa: E402
from slot_pin_util import (ATEXIT, DSO, RELEASE, ROOT, TABLE_START, Elf, const_value, digest,  # noqa: E402
                           require)

STACK_CHK_FAIL = 0xF8F3A0
CANARY_CELL = 0x15FE518

CALLEES = {
    "concmd_base_ctor": (0x20AF0, 0x5A, "e9dbf86d6bb1496a9b34cbf1968ba5cc6909e06d1e5a902cb936704efedf0b45"),
    "concmd_register": (0x20D60, 0x1A, "f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403"),
    "shared_release": (0xF590, 0x49, "55eef47ed634189652e4cfc5397d8fa8ee58de4385a6cddc30f403de7ba40a47"),
    "cstr_view": (0xF2F0, 0x28, "0173e3a3f11a8aa020b87731d2eef5026621d6f1fa0967581b0dab3df646973b"),
    "bool_dtor": (0x20B0, 0x36, "73f646e91ca2f7d8dd14cf1858c93197eddbf00a4267f6ca36ea00b2500efe42"),
    "menu_toggle_ctor": (0x26EA0, 0xED, "c2c7d8c17db68783d9e94100c4f032f3bf06c1ac7d12d5f48e0e34e06a924677"),
    "menu_named_ctor": (0x26DB0, 0xEF, "903ee574f90468eb5c2ed01f6d299b010dbafd8ed3d90f18ed9a283754a62c6e"),
    "menu_group_ctor": (0x26C10, 0xC2, "cc1c2bcce3014d52d61f88a2ac0b3c6a0666198cba7527d46d3e2ebaf03114fe"),
    "menu_item_dtor": (0x272D0, 0x19E, "41ea4aa9e998a114e13cd7149228ae5c542debbeb084c1d412a988f2e801c770"),
    "bool_slot3": (0xB890, 0x6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
    "bool_slot6": (0xB930, 0x3, "251447ee91a9067dcd6ab96703133f617565974cd6c4819021760c4688c91abf"),
    "bool_fn_copy": (0xC2A0, 0x14, "b355d74a6edc25ac52cd52ef1de89719ec412b8ea6414e806f80cea7a60e5693"),
    "bool_fn_clone": (0xC2C0, 0x14, "047e110aeff5fe3000666c334afa7c4e6c799a779125a7608c000e1eed1a7474"),
    "bool_fn_move": (0xC2E0, 0x9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    "bool_fn_destroy": (0xC2F0, 0x1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
    "fconcmd_dtor": (0x4B0, 0x71, "a15bae38e545531a8c5769a079333bd4e0385846160e10a4b768827ecb0d8c8c"),
    "invoke_thunk": (0x1990, 0xC, "c5eca1d65aea48ea6feaa7c41abef0c4f4d7bc1212e753f2d12da5b89285fd12"),
    "command_slot3": (0x22AE0, 0x3, "251447ee91a9067dcd6ab96703133f617565974cd6c4819021760c4688c91abf"),
    "int_var_worker": (0x3A980, 0x142, "2cbfc2763abb6506a59551b902dd6098325889e5e520d842c0e93da7f43498ca"),
    "int_dtor": (0x18A60, 0x36, "4de4f3993b02c84c9c20824690e8ab9d4f7a1b84babfa6cbe0d89c9dd558b50b"),
    "int_slot3": (0xBF70, 0x6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
    "int_slot6": (0xBFC0, 0x6, "7140f35dee6220b79b12aecc27acf5105bf3b77d1588e89fce345de7c16c72b7"),
    "int_fn_copy": (0xC720, 0x14, "5b63116e7f29674de81acb2a9317619a1c1dcbdbc53f98785a3029c04aa7a914"),
    "int_fn_clone": (0xC740, 0x14, "3b5a8a9fd4b3e15b1f5f14f73127ac98c5becb3a88fd096f8ccfb2015e991593"),
    "int_fn_move": (0xC760, 0x9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    "int_fn_destroy": (0xC770, 0x1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
}
BOOL_CALLEES = ["concmd_base_ctor", "concmd_register", "shared_release", "bool_dtor", "menu_item_dtor",
                "bool_slot3", "bool_slot6", "bool_fn_copy", "bool_fn_clone", "bool_fn_move", "bool_fn_destroy"]
CMD_CALLEES = ["concmd_base_ctor", "concmd_register", "shared_release", "cstr_view", "fconcmd_dtor",
               "invoke_thunk", "command_slot3"]
INT_CALLEES = ["concmd_base_ctor", "concmd_register", "shared_release", "menu_toggle_ctor", "menu_item_dtor",
               "int_var_worker", "int_dtor", "int_slot3", "int_slot6", "int_fn_copy", "int_fn_clone",
               "int_fn_move", "int_fn_destroy"]

FRAME_A = ("554889e54157415641554154534883ec48", "483b45d0750f4883c4485b415c415d415e415f5dc3")
FRAME_B = ("554889e54157415641554154534883e4e04881eca0000000",
           "483b842480000000750f488d65d85b415c415d415e415f5dc3")
FRAME_C = ("554889e54157415641554154534883e4e04881ecc0000000",
           "483b8424a0000000750f488d65d85b415c415d415e415f5dc3")

# ---- the 0x26ea0 / 0x26db0 / 0x26c10 byte patterns the native lifts rely on (offset from the callee)
MENU_PATTERNS = {
    "menu_named_ctor": [
        (0x13, "48c70300000000" "48895b08" "48895b10" "48c7431800000000" "c6432000", "link header, +0x20 = 0"),
        (0x2E, "c5f81006" "c5f8114328" "488b4610" "48894338" "488b4618" "48894340", "label = *parent (+0x28..0x47)"),
        (0x50, "c7434802000000", "kind = 2"),
        (0x57, "c4c178104620" "c5f8114350" "498b4630" "48894360" "498b4638" "48894368",
         "text = target name view (+0x20..+0x3f of the target)"),
        (0x7B, "488d7b70" "e84c83feff", "empty view C through 0xf180"),
        (0x84, "4c89b390000000" "c783980000000000803f" "c7839c00000000002041" "4489bba0000000",
         "target, range {1.0f, 10.0f}, flags"),
        (0xA6, "4c89f7" "498b06" "ff5018" "83f801" "751f" "498b06" "4c89f7" "ff5030" "83f803" "7511"
               "48b8cdcccc3d0000003f" "48898398000000", "virtual slot 3 == 1 && slot 6 == 3 narrows to {0.1f, 0.5f}"),
        (0xD3, "488d052e66dd02" "4889df" "488b30" "4883c408" "5b415e415f5d" "e911faffff", "tail-jmp 0x268b0"),
    ],
    "menu_group_ctor": [
        (0x00, "48c70700000000" "48897f08" "48897f10" "48c7471800000000" "c6472000", "link header"),
        (0x1B, "c5f81006" "c5f8114728" "488b4610" "48894738" "488b4618" "48894740", "label = *parent"),
        (0x3D, "c7474801000000", "kind = 1"),
        (0x44, "c5f81002" "c5f8114750" "488b4210" "48894760" "488b4218" "48894768", "text = *caption"),
        (0x66, "c5f81002" "c5f8114770" "488b4210" "48898780000000" "488b4218" "48898788000000",
         "third view = *caption again"),
        (0x8E, "488d051368dd02" "48c7879000000000000000" "c787980000000000803f" "c7879c00000000002041"
               "898fa0000000" "488b30" "e9defbffff", "target 0, range, flags, tail-jmp 0x268b0"),
    ],
    "int_var_worker": [
        (0x31, "488d05d8975501", "worker class table 0x1594190"),
        (0x3C, "488d0d9dac5501", "seed functor table 0x1595660"),
        (0x43, "4883c010" "488903" "4c897b60" "c6436c00" "c6437000" "c6437400" "c6437800", "vptr, value ref, bounds off"),
        (0x5E, "c5fc118380000000" "48898bc0000000" "48c783b000000000000000" "48c783c800000000000000",
         "args zero, seed functor tag, payload / aux zero"),
        (0x83, "c4c178104608" "c5f8118388000000" "498b06" "48898380000000" "498b4618" "48898398000000"
               "4885c0" "740a" "4889d8" "4883e880" "488900", "args copy from the block, self pointer"),
        (0xB5, "498b4648" "498d7620" "488945a0" "498b4640", "functor block fields +0x40 / +0x48 / +0x20"),
        (0xCF, "ff5008" "488b4598" "4c8d7da8" "4c89f6" "4c89ff" "ff10", "clone through tag slot 1, copy through slot 0"),
    ],
}


# ---- expected event construction -------------------------------------------------------------------
class Any:
    def __repr__(self):
        return "ANY"


ANY = Any()
STACK = "STACK"


def V(ptr, length):
    return ("view", ptr, length)


UNK_VIEW = ("view", None, None)


class Same:
    def __init__(self, ev, arg, delta=0):
        self.ev, self.arg, self.delta = ev, arg, delta


class Builder:
    def __init__(self):
        self.ev = []

    def s(self, addr, size, value):
        self.ev.append(("S", addr, size, value))

    def c(self, target, args, snaps=None):
        self.ev.append(("C", target, list(args) + [ANY] * (6 - len(args)), snaps or {}))
        return len(self.ev) - 1

    # shapes
    def bool_var(self, o, name_rva, name_len, desc_rva, fn_tag, value, default):
        self.s(o, 8, 0x1594218)
        k = self.c(0x20AF0, [o, STACK, STACK, 1], {"rsi": V(name_rva, name_len), "rdx": V(desc_rva, 0)})
        for off, v in ((0, 0x15940B0), (0x60, o + 0xC0)):
            self.s(o + off, 8, v)
        self.s(o + 0x69, 4, 0)
        for off in (0x70, 0x78, 0x80, 0x88, 0xA0):
            self.s(o + off, 8, 0)
        self.s(o + 0xB0, 8, fn_tag)
        self.s(o + 0xB8, 8, 0)
        self.c(RELEASE, [Same(k, 2)], {"rdi": V(desc_rva, 0)})
        self.s(o, 8, 0x1594218)
        self.s(o + 0xC0, 1, value)
        self.s(o + 0x68, 1, default)
        self.c(0x20D60, [o])
        self.c(RELEASE, [Same(k, 1)], {"rdi": V(name_rva, name_len)})
        self.c(ATEXIT, [0x20B0, o, DSO])

    def item_tail(self, item):
        self.c(ATEXIT, [0x272D0, item, DSO])

    def toggle(self, item, target, label_rva, label_len, text_rva, text_len, flags):
        k = self.c(0x26EA0, [item, STACK, STACK, target, flags],
                   {"rsi": V(label_rva, label_len), "rdx": V(text_rva, text_len)})
        self.c(RELEASE, [Same(k, 2)], {"rdi": V(text_rva, text_len)})
        self.c(RELEASE, [Same(k, 1)], {"rdi": V(label_rva, label_len)})
        self.item_tail(item)

    def named(self, item, target, parent_rva, parent_len, flags):
        k = self.c(0x26DB0, [item, STACK, target, flags], {"rsi": V(parent_rva, parent_len)})
        self.c(RELEASE, [Same(k, 1)], {"rdi": V(parent_rva, parent_len)})
        self.item_tail(item)

    def group(self, item, parent_rva, parent_len, caption_rva, caption_len, flags):
        k = self.c(0x26C10, [item, STACK, STACK, flags],
                   {"rsi": V(parent_rva, parent_len), "rdx": V(caption_rva, caption_len)})
        self.c(RELEASE, [Same(k, 2)], {"rdi": V(caption_rva, caption_len)})
        self.c(RELEASE, [Same(k, 1)], {"rdi": V(parent_rva, parent_len)})
        self.item_tail(item)

    def command(self, o, name_rva, desc_rva, desc_len, handler):
        f = self.c(0xF2F0, [STACK, 0, name_rva])
        k = self.c(0x20AF0, [o, Same(f, 0), STACK, 2], {"rsi": UNK_VIEW, "rdx": V(desc_rva, desc_len)})
        self.s(o, 8, 0x1594DD0)
        self.s(o + 0x60, 8, STACK)
        self.s(o + 0x68, 8, 0x1990)
        self.s(o + 0x70, 8, 0)
        self.s(o + 0x78, 8, handler)
        self.s(o + 0x60, 8, o + 0x60)
        for off in (0x80, 0x88, 0x90):
            self.s(o + off, 8, 0)
        self.c(0x20D60, [o])
        self.c(RELEASE, [Same(k, 2)], {"rdi": V(desc_rva, desc_len)})
        self.c(RELEASE, [Same(k, 1)], {"rdi": UNK_VIEW})
        self.c(ATEXIT, [0x4B0, o, DSO])

    def int_var(self, s):
        o = s["var"]
        nl = len(s["name"])
        k = self.c(0x3A980, [o, STACK, o + 0xD0, s["flags"], STACK, STACK],
                   {"rsi": V(s["name_rva"], nl), "r9": V(s["desc_rva"], 0),
                    "r8": ("words", {0: 0, 1: 0, 2: 0, 3: 0, 6: 0, 8: s["arg_fn_tag"], 9: 0})})
        self.c(RELEASE, [Same(k, 5)], {"rdi": V(s["desc_rva"], 0)})
        self.c(0xC770, [Same(k, 4, 0x20)], {"rdi": ("words", {2: 0, 4: s["arg_fn_tag"], 5: 0})})
        self.s(o, 8, 0x1594C08)
        self.s(o + 0xD0, 4, s["value"])
        self.s(o + 0x68, 4, s["default"])
        self.c(0x20D60, [o])
        self.c(RELEASE, [Same(k, 1)], {"rdi": V(s["name_rva"], nl)})
        self.c(ATEXIT, [s["dtor"], o, DSO])


def ident(step_id: str) -> str:
    return "".join(p.capitalize() for p in step_id.split("_"))


def build_bool_slot(n: int):
    spec = sp.BOOL_SLOTS[n]
    b = Builder()
    objs = {s["id"]: s["obj"] for s in spec["steps"]}
    for s in spec["steps"]:
        if s["kind"] == "var":
            b.bool_var(s["obj"], s["name_rva"], len(s["name"]), s["desc_rva"], s["fn_tag"], s["value"],
                       s["default"])
        elif s["kind"] == "toggle":
            b.toggle(s["obj"], objs[s["target"]], s["label_rva"], len(s["label"]), s["text_rva"],
                     len(s["text"]), s["flags"])
        elif s["kind"] == "named":
            b.named(s["obj"], objs[s["target"]], s["parent_rva"], len(s["parent"]), s["flags"])
        else:
            b.group(s["obj"], s["parent_rva"], len(s["parent"]), s["caption_rva"], len(s["caption"]), s["flags"])
    return b.ev


def build_cmd_slot():
    b = Builder()
    for c in sp.CMD_SLOT["cmds"]:
        b.command(c["obj"], c["name_rva"], c["desc_rva"], len(c["desc"]), c["handler"])
    return b.ev


def build_int_slot():
    s = sp.INT_SLOT
    b = Builder()
    b.s(s["var"], 8, 0x1594C08)
    b.int_var(s)
    b.toggle(s["item"], s["var"], s["label_rva"], len(s["label"]), s["text_rva"], len(s["text"]), s["item_flags"])
    return b.ev


# ---- matching ---------------------------------------------------------------------------------------
def val_ok(exp, act, events_actual) -> bool:
    if exp is ANY:
        return True
    if exp == STACK:
        return isinstance(act, tuple) and len(act) == 2 and act[0] in ("rsp", "rbp")
    if isinstance(exp, Same):
        ref = events_actual[exp.ev][2][exp.arg]
        if exp.delta:
            return isinstance(ref, tuple) and isinstance(act, tuple) and act == (ref[0], ref[1] + exp.delta)
        return act == ref
    return isinstance(act, int) and exp == act


def snap_ok(exp, words) -> bool:
    if exp[0] == "view":
        if exp[1] is None:
            return isinstance(words[0], tuple)
        return list(words[:4]) == [exp[1], exp[2], 0, 0]
    if exp[0] == "words":
        return all(words[i] == v for i, v in exp[1].items())
    return False


def actual_events(t: si.Trace):
    out = []
    for kind, i in t.events:
        if kind == "store":
            k, addr, size, value = t.stores[i]
            if k == "abs":
                out.append(("S", addr, size, value))
        else:
            c = t.calls[i]
            out.append(("C", c.target, c.args, c.stack))
    return out


def compare_trace(t: si.Trace, expected, label: str) -> None:
    got = actual_events(t)
    require(len(got) == len(expected), f"{label}: event count {len(got)} != expected {len(expected)}")
    for idx, (exp, act) in enumerate(zip(expected, got)):
        where = f"{label}: event {idx}"
        require(exp[0] == act[0], f"{where} kind {act[0]} != {exp[0]}")
        if exp[0] == "S":
            require(exp[1] == act[1] and exp[2] == act[2], f"{where} store address/size {act[1]:#x}/{act[2]}")
            require(val_ok(exp[3], act[3], got), f"{where} store value {act[3]!r} != {exp[3]!r}")
        else:
            require(exp[1] == act[1], f"{where} call target {act[1]!r} != {exp[1]:#x}")
            for ai, ea in enumerate(exp[2]):
                require(val_ok(ea, act[2][ai], got), f"{where} call {exp[1]:#x} arg{ai} {act[2][ai]!r} != {ea!r}")
            for reg, sn in exp[3].items():
                require(reg in act[3], f"{where} call {exp[1]:#x} no stack snapshot for {reg}")
                require(snap_ok(sn, act[3][reg]), f"{where} call {exp[1]:#x} {reg} snapshot {act[3][reg]!r} != {sn!r}")


# ---- per-slot model ---------------------------------------------------------------------------------
def slot_model(n: int) -> dict:
    if n in sp.BOOL_SLOTS:
        spec = sp.BOOL_SLOTS[n]
        strings = []
        consts = {f"kSlot{n}Rva": spec["rva"], f"kSlot{n}BodySize": spec["size"]}
        nstr = {}
        order = []
        for s in spec["steps"]:
            i = ident(s["id"])
            pre = f"kSlot{n}{i}"
            consts[pre + "Rva"] = s["obj"]
            if s["kind"] == "var":
                strings += [(s["name_rva"], s["name"]), (s["desc_rva"], "")]
                consts[pre + "FnTag"] = s["fn_tag"]
                consts[pre + "Value"] = s["value"]
                consts[pre + "Default"] = s["default"]
                nstr[pre + "Name"] = s["name"]
                order.append(f"family_bool_var(o.{s['id']}, {pre}Name, sizeof {pre}Name - 1, {pre}FnTag,"
                             f" {pre}Value, {pre}Default, concmd)")
            elif s["kind"] == "toggle":
                strings += [(s["label_rva"], s["label"]), (s["text_rva"], s["text"])]
                consts[pre + "Flags"] = s["flags"]
                nstr[pre + "Label"] = s["label"]
                nstr[pre + "Text"] = s["text"]
                order.append(f"family_toggle_item_26ea0(o.{s['id']}, {pre}Label, sizeof {pre}Label - 1, {pre}Text,"
                             f" sizeof {pre}Text - 1, debug_menu_target_for(o.{s['target']}), {pre}Flags,"
                             f" concmd, menu)")
            elif s["kind"] == "named":
                strings += [(s["parent_rva"], s["parent"])]
                consts[pre + "Flags"] = s["flags"]
                nstr[pre + "Parent"] = s["parent"]
                order.append(f"family_named_item_26db0(o.{s['id']}, {pre}Parent, sizeof {pre}Parent - 1,"
                             f" o.{s['target']}.base, debug_menu_target_for(o.{s['target']}), {pre}Flags,"
                             f" concmd, menu)")
            else:
                strings += [(s["parent_rva"], s["parent"]), (s["caption_rva"], s["caption"])]
                consts[pre + "Flags"] = s["flags"]
                nstr[pre + "Parent"] = s["parent"]
                nstr[pre + "Caption"] = s["caption"]
                order.append(f"family_group_item_26c10(o.{s['id']}, {pre}Parent, sizeof {pre}Parent - 1,"
                             f" {pre}Caption, sizeof {pre}Caption - 1, {pre}Flags, concmd, menu)")
        has_menu = any(s["kind"] != "var" for s in spec["steps"])
        order.append("out.ok = " + " && ".join(f"out.{s['id']}.ok" + ("" if s["kind"] == "var" else "()")
                                              for s in spec["steps"]) + ";")
        return dict(n=n, rva=spec["rva"], size=spec["size"], sha=spec["sha"], frame=FRAME_A,
                    events=build_bool_slot(n), callees=BOOL_CALLEES, strings=strings, consts=consts,
                    nstr=nstr, order=order, has_menu=has_menu, kind="bool", name=f"shadow family bool slot {n}")
    if n == 333:
        spec = sp.CMD_SLOT
        consts = {"kSlot333Rva": spec["rva"], "kSlot333BodySize": spec["size"]}
        nstr, strings = {}, []
        for tag, c in zip("AB", spec["cmds"]):
            consts[f"kSlot333Cmd{tag}Rva"] = c["obj"]
            consts[f"kSlot333Cmd{tag}Handler"] = c["handler"]
            nstr[f"kSlot333Cmd{tag}Name"] = c["name"]
            nstr[f"kSlot333Cmd{tag}Description"] = c["desc"]
            strings += [(c["name_rva"], c["name"]), (c["desc_rva"], c["desc"])]
        order = ["a{kSlot333CmdAName, kSlot333CmdADescription, sizeof kSlot333CmdADescription - 1,"
                 " kSlot333CmdAHandler}",
                 "b{kSlot333CmdBName, kSlot333CmdBDescription, sizeof kSlot333CmdBDescription - 1,"
                 " kSlot333CmdBHandler}", "init_fconcmd_object(o.cmd_a, a, concmd)", "init_fconcmd_object(o.cmd_b, b, concmd)",
                 "out.ok = out.cmd_a.ok && out.cmd_b.ok;"]
        return dict(n=n, rva=spec["rva"], size=spec["size"], sha=spec["sha"], frame=FRAME_B,
                    events=build_cmd_slot(), callees=CMD_CALLEES, strings=strings, consts=consts,
                    nstr=nstr, order=order, has_menu=False, kind="cmd", name="time attack commands")
    if n == 334:
        spec = sp.INT_SLOT
        consts = {"kSlot334Rva": spec["rva"], "kSlot334BodySize": spec["size"], "kSlot334VarRva": spec["var"],
                  "kSlot334ItemRva": spec["item"], "kSlot334ArgFnTable": spec["arg_fn_tag"],
                  "kSlot334CtorFlags": spec["flags"], "kSlot334Value": spec["value"],
                  "kSlot334Default": spec["default"], "kSlot334DtorRva": spec["dtor"],
                  "kSlot334ItemFlags": spec["item_flags"], "kUnboundedIntWorkerRva": 0x3A980,
                  "kUnboundedIntWorkerSize": 0x142, "kUnboundedIntWorkerVptr": 0x15941A0,
                  "kUnboundedIntFinalVptr": 0x1594C08, "kUnboundedIntSeedFnTable": 0x1595660,
                  "kUnboundedIntFnTable": 0x15944A0, "kUnboundedIntFnTableB": 0x15CE8C0,
                  "kUnboundedIntSlot3Result": 1, "kUnboundedIntSlot6Result": 2}
        nstr = {"kSlot334Name": spec["name"], "kSlot334Label": spec["label"], "kSlot334Text": spec["text"]}
        strings = [(spec["name_rva"], spec["name"]), (spec["desc_rva"], ""), (spec["label_rva"], spec["label"]),
                   (spec["text_rva"], spec["text"])]
        order = ["family_view(kSlot334Name, sizeof kSlot334Name - 1)", "args.fn_vptr = kSlot334ArgFnTable;",
                 "init_unbounded_int_var_worker_3a980(o.var, name, kSlot334CtorFlags, args, description)",
                 "release_inert_view_f590(description)", "unbounded_int_fn_table_lifted(args.fn_vptr)",
                 "o.var.value = kSlot334Value;", "o.var.default_value = kSlot334Default;",
                 "register_concmd_20d60(o.var.base, concmd)", "release_inert_view_f590(name)",
                 "concmd.atexit_log.record(kSlot334DtorRva, &o.var)",
                 "family_toggle_item_26ea0(o.item, kSlot334Label, sizeof kSlot334Label - 1, kSlot334Text,"
                 " sizeof kSlot334Text - 1, unbounded_int_menu_target(o.var), kSlot334ItemFlags, concmd, menu)"]
        return dict(n=n, rva=spec["rva"], size=spec["size"], sha=spec["sha"], frame=FRAME_C,
                    events=build_int_slot(), callees=INT_CALLEES, strings=strings, consts=consts,
                    nstr=nstr, order=order, has_menu=True, kind="int", name="unbounded int variable")
    raise SystemExit(f"FAIL unknown slot {n}")


# ---- ELF checks -------------------------------------------------------------------------------------
def table_check(e: Elf, n: int, rva: int) -> None:
    require(e.u64(TABLE_START - 8 * n) == rva, f"table word {n}")


def class_checks(e: Elf, kind: str, model: dict) -> None:
    if kind == "bool":
        require(e.reloc_target(0x1594218) == 0x20B0, "bool class slot 0 = 0x20b0")
        require(e.reloc_target(0x1594218 + 0x18) == 0xB890, "bool class slot 3 = 0xb890 (returns 1)")
        require(e.reloc_target(0x1594218 + 0x30) == 0xB930, "bool class slot 6 = 0xb930 (returns 0)")
        e.bytes_at(0xB890, "b801000000c3", "bool slot 3: mov eax,1; ret")
        e.bytes_at(0xB930, "31c0c3", "bool slot 6: xor eax,eax; ret")
        tags = sorted({s["fn_tag"] for s in sp.BOOL_SLOTS[model["n"]]["steps"] if s["kind"] == "var"})
        for tag in tags:
            for i, body in enumerate((0xC2A0, 0xC2C0, 0xC2E0, 0xC2F0)):
                require(e.reloc_target(tag + 8 * i) == body, f"bool functor table {tag:#x} slot {i}")
        require(e.lea(0xC2A4) == 0x1594280 and e.lea(0xC2C0) == 0x1594280,
                "bool functor copy / clone store class tag 0x1594280")
        e.bytes_at(0xC2E0, "488b461048894710c3", "bool functor move body")
        e.bytes_at(0xC2F0, "c3", "bool functor destroy is a bare ret")
        # the class virtual slots read by 0x26ea0 / 0x26db0: bool = (1, 0) keeps the default range
        t = 0x26EA0
        e.bytes_at(t + 0x89, "c783980000000000803f" "c7839c00000000002041" "4489bba0000000",
                   "0x26ea0 default range {1.0f, 10.0f}")
        e.bytes_at(t + 0xA4, "4c89f7" "498b06" "ff5018" "83f801" "751f" "498b06" "4c89f7" "ff5030" "83f803" "7511",
                   "0x26ea0 virtual slot 3 == 1 && slot 6 == 3 gate")
        require(e.jmp(t + 0xE8) == 0x268B0, "0x26ea0 tail-calls 0x268b0")
    elif kind == "cmd":
        for i, rva in enumerate((0x4B0, 0x22A60, 0x20D60, 0x22AE0)):
            require(e.reloc_target(0x1594DD0 + 8 * i) == rva, f"command vtable slot {i} = {rva:#x}")
        e.bytes_at(0x22AE0, "31c0c3", "command slot 3: xor eax,eax; ret")
        require(e.reloc_target(0x15FE5E0) == 0x1990, "delegate invoker cell 0x15fe5e0 -> 0x1990")
        e.bytes_at(0x1990, "488b4718" "4889f7" "4889d6" "ffe0", "invoker thunk body")
    else:
        for base in (0x15CE8C0, 0x1595660, 0x15944A0):
            for i, body in enumerate((0xC720, 0xC740, 0xC760, 0xC770)):
                require(e.reloc_target(base + 8 * i) == body, f"int functor table {base:#x} slot {i}")
        e.bytes_at(0xC770, "c3", "functor destroy is a bare ret")
        require(e.rip(0xC724, 7, 3) == 0x15944A0, "functor copy stores class tag 0x15944a0")
        require(e.rip(0xC740, 7, 3) == 0x15944A0, "functor clone stores class tag 0x15944a0")
        e.bytes_at(0xC760, "488b461048894710c3", "functor move body")
        require(e.reloc_target(0x1594C08) == 0x18A60, "int class slot 0 = 0x18a60")
        require(e.reloc_target(0x1594C08 + 0x10) == 0x20D60, "int class slot 2 = 0x20d60")
        require(e.reloc_target(0x1594C08 + 0x18) == 0xBF70, "int class slot 3 = 0xbf70 (returns 1)")
        require(e.reloc_target(0x1594C08 + 0x30) == 0xBFC0, "int class slot 6 = 0xbfc0 (returns 2)")
        e.bytes_at(0xBF70, "b801000000c3", "int slot 3: mov eax,1; ret")
        e.bytes_at(0xBFC0, "b802000000c3", "int slot 6: mov eax,2; ret")
        t = 0x26EA0
        e.bytes_at(t + 0x89, "c783980000000000803f" "c7839c00000000002041" "4489bba0000000",
                   "0x26ea0 default range {1.0f, 10.0f}")
        e.bytes_at(t + 0xA4, "4c89f7" "498b06" "ff5018" "83f801" "751f" "498b06" "4c89f7" "ff5030" "83f803" "7511",
                   "0x26ea0 virtual slot 3 == 1 && slot 6 == 3 gate")
        # the worker's object placement: variable 0x32882f8 + 0xd8 = item 0x32883d0, value cell +0xd0, default +0x68
        require(sp.INT_SLOT["var"] + 0xD8 == sp.INT_SLOT["item"], "int object placement")
        require(sp.INT_SLOT["var"] + 0xD0 == 0x32883C8 and sp.INT_SLOT["var"] + 0x68 == 0x3288360,
                "value / default cells")


def menu_pins(e: Elf, kind: str, model: dict) -> None:
    names = []
    if kind in ("bool", "int"):
        kinds = {s["kind"] for s in sp.BOOL_SLOTS[model["n"]]["steps"]} if kind == "bool" else {"toggle"}
        if "named" in kinds:
            names.append("menu_named_ctor")
        if "group" in kinds:
            names.append("menu_group_ctor")
    if kind == "int":
        names.append("int_var_worker")
    for name in names:
        base = CALLEES[name][0]
        for off, hexstr, what in MENU_PATTERNS[name]:
            e.bytes_at(base + off, hexstr.replace(" ", ""), f"{name}: {what}")


def run_elf_checks(e: Elf, n: int, model: dict | None = None, trace_cache: dict | None = None) -> None:
    m = model or slot_model(n)
    rva, size = m["rva"], m["size"]
    table_check(e, n, rva)
    e.body_hash(f"slot{n}", rva, size, m["sha"])
    for name in m["callees"]:
        crva, csize, csha = CALLEES[name]
        e.body_hash(name, crva, csize, csha, always=True)
    # extent: the body ends with the stack-check failure call; nop padding up to the 16-byte boundary
    require(e.call(rva + size - 5) == STACK_CHK_FAIL, "final call is the stack-check failure path 0xf8f3a0")
    pad = (-(rva + size)) % 16 or 1
    e.bytes_at(rva + size, "90" * pad, "nop padding after the body")
    pro, epi = m["frame"]
    e.bytes_at(rva, pro, "prologue")
    e.bytes_at(rva + size - 5 - len(epi) // 2, epi, "epilogue")
    # the stack-check cell (RELATIVE reloc 0x15c3b80-style GOT cell) resolves to the canary 0x15fe518
    t = si.run_body(e, rva, size)
    require(any(s[0] == "rsp" and s[3] == ("canary", CANARY_CELL) for s in t.stores), "stack canary cell 0x15fe518")
    compare_trace(t, m["events"], f"slot {n}")
    require(si.trace_digest(t) == sp.TRACE_SHA256[n],
            f"slot {n}: recorded walk digest {si.trace_digest(t)[:16]}... != pinned {sp.TRACE_SHA256[n][:16]}...")
    for srva, text in m["strings"]:
        if text == "":
            require(e.read(srva, 1) == b"\0", f"empty description at {srva:#x} is a NUL byte")
        else:
            e.cstring(srva, text.encode())
    class_checks(e, m["kind"], m)
    menu_pins(e, m["kind"], m)


# ---- native checks ----------------------------------------------------------------------------------
HEADER = "native/core/ctor_slots_322_334.hpp"
SOURCE = "native/core/ctor_slots_322_334.cpp"
CMAKE = "native/CMakeLists.txt"


def read_native(n: int) -> dict[str, str]:
    return {"header": (ROOT / HEADER).read_text(), "source": (ROOT / SOURCE).read_text(),
            "test": (ROOT / f"native/tests/test_ctor_slot_{n}.cpp").read_text(),
            "cmake": (ROOT / CMAKE).read_text(),
            "items_c": (ROOT / "native/core/ctor_slot_family_items.cpp").read_text(),
            "item_26db0": (ROOT / "native/core/menu_item_26db0.cpp").read_text()}


def squash(text: str) -> str:
    return re.sub(r"\s+", " ", text)


def init_function(source: str, n: int, rva: int) -> str:
    m = re.search(rf"Slot{n}Result init_slot{n}_{rva:x}\(.*?\n}}\n", source, re.DOTALL)
    require(m is not None, f"cannot find init_slot{n}_{rva:x}")
    return m.group(0)


def string_const(header: str, name: str) -> str:
    m = re.search(rf"\b{re.escape(name)}\[\]\s*=\s*((?:\s*\"(?:[^\"\\]|\\.)*\")+)\s*;", header)
    require(m is not None, f"missing native string constant {name}")
    return "".join(re.findall(r"\"((?:[^\"\\]|\\.)*)\"", m.group(1)))


def test_tokens(n: int, rva: int) -> list[str]:
    return [f"kSlot{n}Rva == {rva:#x}", f"init_slot{n}_{rva:x}(", "section_constants()", "section_failures()",
            f"OK test_ctor_slot_{n}"]


def cmake_tokens(n: int) -> list[str]:
    return ["core/menu_item_26db0.cpp", "core/ctor_slot_family_items.cpp", "core/ctor_slots_322_334.cpp",
            f"add_executable(test_ctor_slot_{n} tests/test_ctor_slot_{n}.cpp)",
            f"add_test(NAME ctor_slot_{n} COMMAND test_ctor_slot_{n})"]


def check_native(t: dict[str, str], n: int, m: dict) -> None:
    for name, value in m["consts"].items():
        require(const_value(t["header"], name) == value, f"native {name} == {value:#x}")
    for name, text in m["nstr"].items():
        require(string_const(t["header"], name) == text, f"native string {name}")
    fn = squash(init_function(t["source"], n, m["rva"]))
    pos, at = [], 0
    for token in m["order"]:
        i = fn.find(token, at)
        require(i >= 0, f"slot {n} native call order: missing {token!r}")
        pos.append(i)
        at = i + len(token)
    # shared pieces the slot modules rely on (a mutated family helper is not caught by the unit tests alone)
    for token in ("init_debug_menu_item_26ea0(item, l, t, target, flags)", "release_inert_view_f590(t) &&",
                  "release_inert_view_f590(l)", "kDebugMenuItemDtorRva, &item",
                  "init_debug_menu_item_26db0(item, p, target_base, target, flags)",
                  "init_menu_group_26c10(item, p, c, flags)", "release_inert_view_f590(c) &&"):
        require(token in t["items_c"], f"family item helper token {token!r}")
    for token in ("item.kind = kDebugMenuItemKind;", "item.text = target_base.name;", "item.range_min = 0.1f;",
                  "item.range_max = 0.5f;", "target.slot3 == 1 && target.slot6 == 3", "item.flags = flags;"):
        require(token in t["item_26db0"], f"0x26db0 native token {token!r}")
    for token in test_tokens(n, m["rva"]):
        require(token in t["test"], f"test token {token!r}")
    for token in cmake_tokens(n):
        require(token in t["cmake"], f"cmake token {token!r}")


def mutate_string(header: str, name: str) -> str:
    m = re.search(rf"\b{re.escape(name)}\[\]\s*=\s*\"", header)
    require(m is not None, f"string constant {name} not found")
    i = m.end()
    return header[:i] + ("#" if header[i] != "#" else "%") + header[i + 1:]


def native_mutants(t: dict[str, str], n: int, m: dict) -> int:
    ran = 0
    raw = init_function(t["source"], n, m["rva"])
    for token in m["order"]:
        anchor = token.split("(")[0] if "(" in token else token
        if anchor not in raw:
            anchor = token.split()[0]
        require(anchor in raw, f"mutation anchor {anchor!r} not found in the raw function")
        mutated = dict(t)
        mutated["source"] = t["source"].replace(raw, raw.replace(anchor, "\0MUTATED\0", 1))
        expect_failure(f"source anchor {anchor!r}", lambda mt=mutated: check_native(mt, n, m))
        ran += 1
    for name, value in m["consts"].items():
        pattern = re.compile(rf"({re.escape(name)}\s*=\s*)(0x[0-9A-Fa-f]+|\d+)(u?\s*;)")
        mutated = dict(t)
        mutated["header"] = pattern.sub(lambda mo, v=value: f"{mo.group(1)}{v ^ 0x4:#x}{mo.group(3)}",
                                        t["header"], count=1)
        require(mutated["header"] != t["header"], f"constant {name} pattern did not match")
        expect_failure(f"constant {name}", lambda mt=mutated: check_native(mt, n, m))
        ran += 1
    for name in m["nstr"]:
        mutated = dict(t)
        mutated["header"] = mutate_string(t["header"], name)
        expect_failure(f"string {name}", lambda mt=mutated: check_native(mt, n, m))
        ran += 1
    for key, tokens in (("test", test_tokens(n, m["rva"])), ("cmake", cmake_tokens(n)),
                        ("items_c", ["init_debug_menu_item_26ea0(item, l, t, target, flags)",
                                     "kDebugMenuItemDtorRva, &item", "init_menu_group_26c10(item, p, c, flags)"]),
                        ("item_26db0", ["item.kind = kDebugMenuItemKind;", "item.text = target_base.name;",
                                        "item.range_min = 0.1f;", "target.slot3 == 1 && target.slot6 == 3"])):
        for token in tokens:
            mutated = dict(t)
            mutated[key] = t[key].replace(token, "\0MUTATED\0")
            expect_failure(f"{key} token {token!r}", lambda mt=mutated: check_native(mt, n, m))
            ran += 1
    return ran


def expect_failure(label: str, fn) -> None:
    try:
        fn()
    except SystemExit as error:
        require(str(error).startswith("FAIL"), f"mutation {label}: unexpected exit {error!r}")
        return
    raise SystemExit(f"FAIL mutation survived: {label}")


# ---- ELF mutation self-test (process pool) ----------------------------------------------------------
_W: dict = {}


def _disassembly(e: Elf, m: dict):
    return [(i.addr, i.length, i.mnem, i.ops) for i in si.decode(e, m["rva"], m["size"])]


def _alias_normalised(dis):
    """The disassembly with a `[r12` memory base read as `[rsp`: slot 334 copies rsp into r12 for its
    name-view pointer, so flipping REX.B on an rsp-based store yields a different instruction that is
    value-identical while r12 == rsp. Such a mutant is only accepted when the full recorded walk digest
    still matches (see _mutant), never on the disassembly alone."""
    return [(a, ln, mn, re.sub(r"\[r12\b", "[rsp", ops)) for a, ln, mn, ops in dis]


def _worker_init(n: int) -> None:
    _W["e"] = Elf()
    _W["n"] = n
    _W["model"] = slot_model(n)
    _W["base_dis"] = _disassembly(_W["e"], _W["model"])


def _mutant(arg):
    kind, value = arg
    e, n, model = _W["e"], _W["n"], _W["model"]
    e.skip_hash = True
    e.recording = False
    e.overlay, e.reloc_flip = {}, set()
    if kind == "byte":
        e.overlay = {value: e.raw(value, 1)[0] ^ 0x01}
    else:
        e.reloc_flip = {value}
    try:
        if _disassembly(e, model) == _W["base_dis"]:
            return ("equivalent", value)    # alternative encoding of the same instruction (e.g. REX.B on a
                                            # rip-relative operand): the disassembly is identical, not a gap
        dis = _disassembly(e, model)
        run_elf_checks(e, n, model)
        if _alias_normalised(dis) == _alias_normalised(_W["base_dis"]):
            return ("alias", value)         # rsp/r12 base alias, walk digest and every pin still identical
    except SystemExit:
        return None
    except Exception:   # undecodable mutant: also a rejection (fail closed)
        return None
    return arg


def mutation_targets(e: Elf, m: dict) -> list[int]:
    targets: set[int] = set()
    for insn in si.decode(e, m["rva"], m["size"]):
        addrs = {insn.addr, insn.addr + insn.length - 1}
        if insn.length > 2:
            addrs.add(insn.addr + insn.length // 2)
        targets |= addrs
    return sorted(targets)


def mutation_self_test(e: Elf, n: int, m: dict, texts: dict[str, str]) -> tuple[int, int, int]:
    targets = mutation_targets(e, m)
    # pinned relocation slots: record them with one recording pass
    e.pins, e.reloc_pins = set(), set()
    e.recording = True
    run_elf_checks(e, n, m)
    e.recording = False
    jobs = [("byte", a) for a in targets] + [("reloc", s) for s in sorted(e.reloc_pins)]
    survivors, equivalent = [], 0
    with multiprocessing.Pool(min(8, multiprocessing.cpu_count()), _worker_init, (n,)) as pool:
        for res in pool.imap_unordered(_mutant, jobs, chunksize=4):
            if res is None:
                continue
            if res[0] in ("equivalent", "alias"):
                equivalent += 1
            else:
                survivors.append(res)
    for kind, v in sorted(survivors):
        print(f"mutation survived: {kind} {v:#x}", file=sys.stderr)
    require(not survivors, f"{len(survivors)} ELF mutations survived")
    # whole-body hash: a byte outside the semantic pins (the last padding-free byte before the epilogue)
    e.overlay = {m["rva"] + 1: e.raw(m["rva"] + 1, 1)[0] ^ 0x80}
    expect_failure("body hash / prologue", lambda: run_elf_checks(e, n, m))
    e.overlay = {}
    native_ran = native_mutants(texts, n, m)
    return len(jobs) - equivalent, native_ran, equivalent


def main(n: int, description: str = "") -> int:
    args = sys.argv[1:]
    m = slot_model(n)
    e = Elf()
    texts = read_native(n)
    if "--tamper" in args:
        addr = int(args[args.index("--tamper") + 1], 0)
        e.overlay = {addr: e.raw(addr, 1)[0] ^ 0x01}
        run_elf_checks(e, n, m)
        print("FAIL tamper did NOT break the check", file=sys.stderr)
        return 1
    run_elf_checks(e, n, m)
    check_native(texts, n, m)
    note = ""
    if "--no-mutate" not in args:
        elf_runs, native_runs, equivalent = mutation_self_test(e, n, m, texts)
        note = (f"; mutation self-test {elf_runs} ELF + {native_runs} native mutants all rejected"
                f" ({equivalent} byte flips were identical-disassembly encodings, not counted)")
    print(f"OK check_ctor_slot_{n}: {description or m['name']} - body {m['sha'][:12]}... {m['size']} B "
          f"hash/extent/frame, {len(m['events'])} ordered store/call events, callees by hash, "
          f"strings, class tables, native constants/order/tests/cmake; guest bytes NOT executed{note}")
    return 0
