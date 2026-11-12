"""Shared engine for tools/lift/check_helper_{c580,3a980,6aa00,26db0}.py (CPU-01-HELPER-CALLEE-MODELS-01).

Offline and read-only: bytes of the pinned private ELF (local/re/eboot.elf, sha256 d0723afb...5861d3) are read
through slot_pin_util.Elf; nothing is loaded, relocated or executed. Each checker supplies a SPEC; this engine runs
three layers and then a mutation self-test.

  layer 1  image   : body size / FDE length / SHA-256 of every pinned body, an instruction-level byte pin for every
                     instruction (helper_callee_pins.PINS), rip operands, direct calls, type-8 relocations, the
                     functor-family computation (every 8-byte-aligned group of four consecutive RELATIVE
                     relocations whose addends are exactly one family's four slot RVAs), the closure facts
                     (initial and stored classes are family members) and, where the SPEC lists them, the call-site
                     proofs of the startup slots that motivated the helper;
  layer 2  model   : the native tag tables equal the ELF-derived families in BOTH directions, header constants equal
                     the ELF facts, and the source / test / CMake tokens that carry the semantics are present;
  layer 3  mutants : the first and last byte of every recorded byte pin and a perturbation of every recorded
                     relocation are applied (whole-body SHA disabled for that run) and must each be rejected, as must
                     every SPEC-field mutation, every native token / constant / table-entry mutation.

What this does NOT prove: that the native bodies are behaviourally equal to the ELF bodies (the fixture tests check
the models against the semantics stated in the headers) nor that a startup slot other than the ones listed in the
SPEC passes a family tag. Nothing here promotes an evidence grade beyond FIXTURE_VERIFIED /
STATIC_ANALYSIS_VERIFIED.

`--tamper elf|native` runs the check on a deliberately tampered input and must exit 1. SKIP (exit 0) when the private
inputs are absent.
"""

from __future__ import annotations

import copy
import json
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools/lift"))
from slot_pin_util import Elf, const_value, digest, require  # noqa: E402
import helper_callee_pins as pins  # noqa: E402

FDE_JSON = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"
DATA_LO, DATA_HI = 0x1590000, 0x1610000     # the .data.rel.ro window that holds the class tables

INT_SLOTS = (0xC720, 0xC740, 0xC760, 0xC770)
FLOAT_SLOTS = (0xC4A0, 0xC4C0, 0xC4E0, 0xC4F0)
FAMILY_SLOTS = {"int": INT_SLOTS, "float": FLOAT_SLOTS}
FAMILY_ARRAY = {"int": "kIntFunctorTags", "float": "kFloatFunctorTags"}
TAGS_HPP = "native/core/helper_functor_tags.hpp"

_FDE_CACHE: dict[str, int] | None = None
_CAND_CACHE: dict[tuple[int, int], list[int]] = {}


# ---------------------------------------------------------------------------------------------------------------
# ELF helpers
# ---------------------------------------------------------------------------------------------------------------
def fde_ranges() -> dict[str, int] | None:
    global _FDE_CACHE
    if _FDE_CACHE is None and FDE_JSON.is_file():
        _FDE_CACHE = json.loads(FDE_JSON.read_text(encoding="utf-8"))
    return _FDE_CACHE


def _addend(e: Elf, addr: int) -> int | None:
    r = e.relocs.get(addr)
    if r is None or r["type"] != 8:
        return None
    return r["addend"] ^ (1 if addr in e.reloc_flip else 0)


def family(e: Elf, slots: tuple[int, int, int, int]) -> list[int]:
    """Every aligned base whose four consecutive RELATIVE relocations have exactly these addends."""
    key = (id(e), slots[0])
    if key not in _CAND_CACHE:
        _CAND_CACHE[key] = sorted(a for a, r in e.relocs.items() if r["type"] == 8 and a % 8 == 0
                                  and r["addend"] in (slots[0], slots[0] ^ 1))
    found = []
    for base in _CAND_CACHE[key]:
        if all(_addend(e, base + 8 * i) == slots[i] for i in range(4)):
            found.append(base)
    if e.recording:
        for base in found:
            for i in range(4):
                e.reloc_target(base + 8 * i)         # records the relocation for the mutation run
    return found


def body_checks(e: Elf, spec: dict) -> None:
    fde = fde_ranges()
    for body in spec["bodies"]:
        name, rva, size = body["name"], body["rva"], body["size"]
        if fde is not None:
            require(fde.get(hex(rva)) == body["fde"], f"{name}: FDE length at {rva:#x}")
        e.body_hash(name, rva, size, body["sha"])
        table = pins.PINS.get(body["pins"]) if body.get("pins") else None
        if table is not None:
            require(sum(len(h) // 2 for _, h in table) == size, f"{name}: pin table covers the body")
            for off, hexstr in table:
                e.bytes_at(rva + off, hexstr, f"{name}+{off:#x}")


def fact_checks(e: Elf, spec: dict, fams: dict[str, list[int]]) -> None:
    for what, site, length, disp_off, target, *fam in spec.get("rips", []):
        got = e.rip(site, length, disp_off)
        require(got == target, f"{what}: rip operand at {site:#x} is {got:#x}, want {target:#x}")
        if fam and fam[0]:
            require(target in fams[fam[0]], f"{what}: {target:#x} is not a {fam[0]} functor family table")
    for what, site, target in spec.get("calls", []):
        require(e.call(site) == target, f"{what}: call at {site:#x} -> {target:#x}")
    for what, site, target in spec.get("jmps", []):
        require(e.jmp(site) == target, f"{what}: jmp at {site:#x} -> {target:#x}")
    for what, addr, target in spec.get("relocs", []):
        require(e.reloc_target(addr) == target, f"{what}: relocation at {addr:#x} -> {target:#x}")
    for what, site, hexstr in spec.get("bytes", []):
        e.bytes_at(site, hexstr, what)


def site_checks(e: Elf, spec: dict, fams: dict[str, list[int]]) -> None:
    """Call-site proofs: the slot passes exactly one family tag to the helper and runs the family's bare-ret destroy."""
    for site in spec.get("sites", []):
        name, base, size = site["name"], site["base"], site["size"]
        fde = fde_ranges()
        if fde is not None:
            require(fde.get(hex(base)) == site["fde"], f"{name}: FDE length")
        e.body_hash(name, base, size, site["sha"], always=True)   # always: the hash owns the uncovered bytes
        fam = fams[site["family"]]
        tag = site["tag"]
        require(tag in fam, f"{name}: tag {tag:#x} is in the {site['family']} functor family")
        require(e.lea(site["lea"]) == tag, f"{name}: the tag lea at {site['lea']:#x} is {tag:#x}")
        for addr, hexstr in pins.SITE_PINS[site["pins"]]:
            e.bytes_at(addr, hexstr, f"{name} site pin {addr:#x}")
        require(e.call(site["call"]) == site["callee"], f"{name}: call at {site['call']:#x} reaches the helper")
        slots = FAMILY_SLOTS[site["family"]]
        require(e.reloc_target(tag + 0x08) == slots[1] and e.reloc_target(tag + 0x18) == slots[3],
                f"{name}: the tag's clone (+8) and destroy (+0x18) slots are the family bodies")
        # whole-body scans (unrecorded reads: the instruction pins above own the mutation coverage)
        body = e.raw(base, size)
        calls = [i for i in range(size - 4) if body[i] == 0xE8
                 and base + i + 5 + struct.unpack("<i", body[i + 1:i + 5])[0] == site["callee"]]
        require(calls == [site["call"] - base], f"{name}: exactly one call to {site['callee']:#x}")
        family_leas = set()
        for i in range(size - 6):
            if body[i] in (0x48, 0x4C) and body[i + 1] == 0x8D and (body[i + 2] & 0xC7) == 0x05:
                target = base + i + 7 + struct.unpack("<i", body[i + 3:i + 7])[0]
                if DATA_LO <= target < DATA_HI and target in set(fams["int"]) | set(fams["float"]):
                    family_leas.add(target)
        require(family_leas == {tag}, f"{name}: the only functor-family table the slot loads is {tag:#x}, got "
                                      f"{sorted(hex(x) for x in family_leas)}")


def check_elf(e: Elf, spec: dict, texts: dict[str, str] | None, fams_cache: dict | None = None) -> dict:
    fams = {}
    for fam in spec.get("families", []):
        fams[fam] = family(e, FAMILY_SLOTS[fam])
    for fam in ("int", "float"):
        fams.setdefault(fam, [])
    for fam in spec.get("families", []):
        require(len(fams[fam]) == spec["family_count"][fam], f"{fam} family has {spec['family_count'][fam]} tables, "
                f"got {len(fams[fam])}")
    body_checks(e, spec)
    fact_checks(e, spec, fams)
    site_checks(e, spec, fams)
    if "extra" in spec:
        spec["extra"](e, fams)
    if texts is not None:
        check_tie(spec, texts, fams)
    return fams


# ---------------------------------------------------------------------------------------------------------------
# native layer
# ---------------------------------------------------------------------------------------------------------------
def native_tags(text: str, array: str) -> list[int]:
    match = re.search(rf"{array}\s*=\s*\{{(.*?)\}};", text, re.S)
    require(match is not None, f"missing native array {array}")
    return [int(x, 16) for x in re.findall(r"0x[0-9a-fA-F]+", match.group(1))]


def check_tie(spec: dict, texts: dict[str, str], fams: dict[str, list[int]]) -> None:
    """The native tag tables are the ELF families, both directions (no missing and no extra table)."""
    for fam in spec.get("families", []):
        native = native_tags(texts["tags_h"], FAMILY_ARRAY[fam])
        require(native == fams[fam], f"native {FAMILY_ARRAY[fam]} equals the ELF {fam} family "
                f"({len(native)} native vs {len(fams[fam])} ELF)")
        require(native == sorted(native) and len(set(native)) == len(native), f"{FAMILY_ARRAY[fam]} sorted, unique")


def check_native(spec: dict, texts: dict[str, str]) -> None:
    for key, constants in spec["constants"].items():
        for name, value in constants.items():
            require(const_value(texts[key], name) == value, f"native constant {name} == {value:#x}")
    for key, tokens in spec["tokens"].items():
        for token in tokens:
            require(token in texts[key], f"{key} token {token!r}")


def read_native(spec: dict) -> dict[str, str]:
    return {key: (ROOT / path).read_text() for key, path in spec["native"].items()}


def run_all(e: Elf, spec: dict, texts: dict[str, str]) -> dict:
    fams = check_elf(e, spec, texts)
    check_native(spec, texts)
    return fams


# ---------------------------------------------------------------------------------------------------------------
# mutation self-test
# ---------------------------------------------------------------------------------------------------------------
def expect_failure(label: str, fn) -> None:
    try:
        fn()
    except SystemExit as error:
        require(str(error).startswith("FAIL"), f"mutant {label}: unexpected exit {error!r}")
        return
    raise SystemExit(f"FAIL mutant survived: {label}")


def _spec_mutants(spec: dict):
    """Yield (label, mutated deep copy) for every ELF-fact field in the SPEC."""
    def clone() -> dict:
        c = copy.deepcopy(spec)
        c["extra"] = spec.get("extra")
        return c

    for i, body in enumerate(spec["bodies"]):
        for field, alter in (("size", lambda v: v + 1), ("fde", lambda v: v + 1),
                             ("sha", lambda v: ("0" if v[0] != "0" else "1") + v[1:]), ("rva", lambda v: v + 0x10)):
            c = clone()
            c["bodies"][i][field] = alter(body[field])
            yield f"body {body['name']} {field}", c
    for key in ("rips", "calls", "jmps", "relocs"):
        for i, row in enumerate(spec.get(key, [])):
            c = clone()
            row = list(row)
            idx = 4 if key == "rips" else 2
            row[idx] = row[idx] ^ 0x10
            c[key][i] = tuple(row)
            yield f"{key} {row[0]}", c
    for i, row in enumerate(spec.get("bytes", [])):
        c = clone()
        h = row[2]
        c["bytes"][i] = (row[0], row[1], ("0" if h[0] != "0" else "1") + h[1:])
        yield f"bytes {row[0]}", c
    for i, site in enumerate(spec.get("sites", [])):
        for field, alter in (("callee", lambda v: v + 0x10), ("tag", lambda v: v + 0x10), ("lea", lambda v: v + 1),
                             ("call", lambda v: v + 1), ("sha", lambda v: ("0" if v[0] != "0" else "1") + v[1:]),
                             ("size", lambda v: v + 1), ("fde", lambda v: v + 1)):
            c = clone()
            c["sites"][i][field] = alter(site[field])
            yield f"site {site['name']} {field}", c


def mutation_self_test(e: Elf, spec: dict, texts: dict[str, str], fams: dict[str, list[int]]) -> tuple[int, int, int]:
    # -- ELF byte / relocation mutants (whole-body hashes disabled so the instruction pins must catch each) -------
    e.recording = True
    check_elf(e, spec, texts)
    e.recording = False
    pin_list = sorted(e.pins)
    reloc_list = sorted(e.reloc_pins)
    e.skip_hash = True
    elf_runs = survivors = 0
    for addr, size in pin_list:
        for byte_addr in sorted({addr, addr + size - 1}):
            e.overlay = {}
            pristine = e.raw(byte_addr, 1)[0]
            e.overlay = {byte_addr: pristine ^ 0x01}
            elf_runs += 1
            try:
                check_elf(e, spec, texts)
            except SystemExit:
                continue
            survivors += 1
            print(f"mutation survived at {byte_addr:#x} (pin {addr:#x}+{size})", file=sys.stderr)
    e.overlay = {}
    for slot in reloc_list:
        e.reloc_flip = {slot}
        elf_runs += 1
        try:
            check_elf(e, spec, texts)
        except SystemExit:
            continue
        survivors += 1
        print(f"relocation mutation survived at {slot:#x}", file=sys.stderr)
    e.reloc_flip = set()
    e.skip_hash = False
    require(survivors == 0, f"{survivors} ELF mutations survived")
    check_elf(e, spec, texts)    # the pristine image still passes after the run

    # -- SPEC-field mutants -----------------------------------------------------------------------------------------
    spec_runs = 0
    for label, mutated in _spec_mutants(spec):
        expect_failure(f"spec {label}", lambda m=mutated: check_elf(e, m, texts))
        spec_runs += 1

    # -- native mutants -----------------------------------------------------------------------------------------------
    native_runs = 0
    for key, tokens in spec["tokens"].items():
        for token in tokens:
            mutated = dict(texts)
            mutated[key] = texts[key].replace(token, "\0MUTATED\0")
            expect_failure(f"{key} token {token!r}", lambda m=mutated: check_native(spec, m))
            native_runs += 1
    for key, constants in spec["constants"].items():
        for name, value in constants.items():
            pattern = re.compile(rf"({re.escape(name)}\s*=\s*)(0x[0-9A-Fa-f]+|\d+)(u?\s*;)")
            mutated = dict(texts)
            mutated[key] = pattern.sub(lambda m, v=value: f"{m.group(1)}{v ^ 0x4:#x}{m.group(3)}", texts[key], count=1)
            require(mutated[key] != texts[key], f"constant {name} pattern did not match")
            expect_failure(f"constant {name}", lambda m=mutated: check_native(spec, m))
            native_runs += 1
    for fam in spec.get("families", []):
        array = FAMILY_ARRAY[fam]
        text = texts["tags_h"]
        start = text.index(array)
        end = text.index("};", start)
        region = text[start:end]
        for tag in native_tags(text, array):
            mutated_region = re.sub(rf"\b{tag:#x}\b,?", "", region, count=1)
            require(mutated_region != region, f"tag {tag:#x} not found in {array}")
            mutated = dict(texts)
            mutated["tags_h"] = text[:start] + mutated_region + text[end:]
            expect_failure(f"{array} without {tag:#x}", lambda m=mutated: check_tie(spec, m, fams))
            native_runs += 1
        mutated = dict(texts)
        mutated["tags_h"] = text[:end] + "    0x15ffff0,\n" + text[end:]
        expect_failure(f"{array} with a bogus tag", lambda m=mutated: check_tie(spec, m, fams))
        native_runs += 1
    return elf_runs, spec_runs, native_runs


def main(spec: dict, argv: list[str] | None = None) -> int:
    arguments = sys.argv[1:] if argv is None else argv
    name = spec["name"]
    if not (ROOT / "local/re/eboot.elf").is_file():
        print(f"SKIP {name}: local/re/eboot.elf not present")
        return 0
    e = Elf()
    texts = read_native(spec)
    if "--tamper" in arguments:
        i = arguments.index("--tamper")
        mode = arguments[i + 1] if len(arguments) > i + 1 else "elf"
        if mode == "elf":
            rva = spec["bodies"][0]["rva"] + 0x10
            e.overlay = {rva: e.raw(rva, 1)[0] ^ 0x40}
        else:
            texts = dict(texts)
            key = next(iter(spec["constants"]))
            constant = next(iter(spec["constants"][key]))
            texts[key] = texts[key].replace(constant, "kTampered", 1)
    e.recording = True
    fams = run_all(e, spec, texts)
    e.recording = False
    note = ""
    if "--no-mutate" not in arguments and "--tamper" not in arguments:
        elf_runs, spec_runs, native_runs = mutation_self_test(e, spec, texts, fams)
        note = (f"; mutation self-test {elf_runs} ELF + {spec_runs} spec + {native_runs} native mutants all rejected")
    fam_note = ", ".join(f"{len(fams[f])} {f}" for f in spec.get("families", []))
    print(f"OK {name}: {spec['summary']} ({fam_note + ' functor tables; ' if fam_note else ''}guest bytes NOT "
          f"executed){note}")
    return 0
