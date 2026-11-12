"""Shared extras for the batch-3 bool debug-page slot checkers (410 362 477 423 449).

Generalises the two slot-507 extras (PLT import NIDs of __cxa_atexit / __stack_chk_fail and the bool-variable
mid / final class vtable identities) so every batch-3 `check_ctor_slot_<N>.py` shares one definition. Re-exports
the lane-M6 driver (`main`) unchanged. Offline only: reads the pinned private ELF; nothing is executed.
"""

from __future__ import annotations

from functools import lru_cache
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lane_m6_lib as lib  # noqa: E402
from slot_pin_util import ELF_SHA256, ROOT, digest, require  # noqa: E402

sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as dyn  # noqa: E402
import tiny_slot_decode as tiny  # noqa: E402


@lru_cache(maxsize=1)
def _bp_import_context():
    """Parse the hash-pinned ELF's dynamic imports without executing game code."""
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(digest(elf) == ELF_SHA256, "batch-3 bool page import parser ELF SHA256")
    parsed = dyn.parse(elf)
    return parsed, dyn.Image(elf, parsed["ph"])


def x_bp_atexit_imports(e, spec) -> None:
    """Tie the two imported PLT calls to their unique JMPREL NID records."""
    _bp_check_imports(e, (
        (0xF8F390, 0x15FFFB8, "__cxa_atexit"),
        (0xF8F3A0, 0x15FFFC0, "__stack_chk_fail"),
    ))


def x_bp_strlen_import(e, spec) -> None:
    """Batch 5 (slot 437): 0xf2f0 measures its string through the strlen PLT thunk 0xf8f3b0 (GOT 0x15fffc8)."""
    _bp_check_imports(e, ((0xF8F3B0, 0x15FFFC8, "strlen"),))


def _bp_check_imports(e, imports) -> None:
    parsed, image = _bp_import_context()
    for plt, expected_got, expected_name in imports:
        stub = e.read(plt, 6)
        require(stub[:2] == b"\xff\x25", f"PLT thunk {plt:#x} opcode")
        got = e.rip(plt, 6, 2)
        require(got == expected_got, f"PLT thunk {plt:#x} GOT target")
        rows = [r for r in parsed["relocs"] if r["table"] == "JMPREL" and r["target"] == got]
        require(len(rows) == 1, f"one JMPREL row for {plt:#x} GOT {got:#x}")
        reloc = rows[0]
        require(reloc["type"] == 7, f"{plt:#x} relocation is R_X86_64_JUMP_SLOT")
        require(reloc["addend"] == 0, f"{plt:#x} JMPREL addend is zero")

        # Let the shared mutation driver perturb this import relocation too.
        # JMPREL symbol metadata is parsed from the pinned ELF; the addend
        # mutation hook must still make this assertion fail closed.
        e.reloc_pins.add(got)
        effective_addend = reloc["addend"] ^ (1 if got in e.reloc_flip else 0)
        require(effective_addend == 0, f"{plt:#x} JMPREL addend mutation")

        symbol = parsed["symbols"][reloc["symbol"]]
        expected_nid = dyn.nid(expected_name)
        require(symbol["type"] == "FUNC" and not symbol["defined"], f"{plt:#x} external function symbol")
        require(symbol["nid"] == expected_nid, f"{plt:#x} NID hash for {expected_name}")
        require(tiny.import_name(parsed, image, plt) == expected_nid,
                f"{plt:#x} PLT import decoder agrees for {expected_name}")


def x_bp_object_vtables(e, spec) -> None:
    """Pin the base and final bool-variable vtable identities written by the body."""
    tables = (
        (0x15940B0, (0xB810, 0xB850, 0x20D60, 0xB890, 0xB8A0)),
        (0x1594218, (0x20B0, 0xC260, 0x20D60, 0xB890, 0xB8A0, 0x223C0, 0xB930)),
    )
    for table, targets in tables:
        for index, target in enumerate(targets):
            require(e.reloc_target(table + 8 * index) == target,
                    f"batch-3 bool page object vtable {table:#x} entry {index}")


def x_bp_holder_vtables(e, spec) -> None:
    """Pin the address-point relocations of the 0x30-byte name holder (batch 4, slot 550; slot 539's shape).

    0x15976B0 is the holder's constructed vptr (0x15976A0 + 0x10); 0x1597658 is the base vptr that the 0x8ef00
    destructor rewrites (0x1597648 + 0x10). Both are identity tags that the native model records and never runs.
    """
    address_points = (
        (0x15976B0, (0x91210, 0, 0, 0x90420, 0x91270, 0x13D10, 0x13D20)),
        (0x1597658, (0x911C0, 0x358A0, 0x358C0, 0x358E0, 0x358F0, 0xC2A0, 0xC2C0)),
    )
    for address_point, targets in address_points:
        for index, target in enumerate(targets):
            slot = address_point + 8 * index
            if target:
                require(e.reloc_target(slot) == target,
                        f"name-holder vtable {address_point:#x} slot {index} -> {target:#x}")
            else:
                require(e.u64(slot) == 0, f"name-holder vtable {address_point:#x} slot {index} is null")


def x_bp_string_var(e, spec) -> None:
    """String console variable (0x257a0 -> 0x258f0), as used by startup slots 310 / 486 / 555 and batch 13 (slot 370): the final class 0x1594ef0
    (slot 0 = dtor 0x23c90, slot 2 = register 0x20d60, slot 3 = 0x246a0 `mov eax,1; ret`, slot 6 = 0x24780 `mov eax,4; ret`), the transient class
    stores of the two constructors, the argument-functor class 0x1595068 slots (copy / clone / move / destroy) and the empty description byte."""
    require(e.read(0xF96388, 1) == b"\0", "string-var description view points at a NUL byte")
    require(e.reloc_target(0x1594EF0) == 0x23C90, "string class slot 0 = 0x23c90")
    require(e.reloc_target(0x1594EF0 + 0x10) == 0x20D60, "string class slot 2 = 0x20d60")
    require(e.reloc_target(0x1594EF0 + 0x18) == 0x246A0, "string class slot 3 = 0x246a0 (returns 1)")
    require(e.reloc_target(0x1594EF0 + 0x30) == 0x24780, "string class slot 6 = 0x24780 (returns 4)")
    e.bytes_at(0x246A0, "b801000000c3", "string slot 3: mov eax,1; ret")
    e.bytes_at(0x24780, "b804000000c3", "string slot 6: mov eax,4; ret")
    for i, body in enumerate((0x25A50, 0x25A70, 0x25A90, 0x25AA0)):
        require(e.reloc_target(0x1595068 + 8 * i) == body, f"string functor class 0x1595068 slot {i}")
    require(e.rip(0x25A54, 7, 3) == 0x1595068 and e.rip(0x25A70, 7, 3) == 0x1595068, "string functor stores its class tag")
    require(e.rip(0x257BF, 7, 3) == 0x1594EE0 and e.rip(0x25921, 7, 3) == 0x1594F58, "string constructor vtables")


def x_bp_holder_classes(e, spec) -> None:
    """Name-holder classes beyond the two of `bp_holder_vtables` (batch 13, slot 370 holder class 0x15a0a48 / destructor 0x2b5050): the
    constructed address point (relocation targets or null per slot), the 0x17-byte destructor's lea of the base address point it rewrites, and
    that base address point's slots. The destructor body itself is hash-pinned through `m6_callees`; its `lea` operand is tied here."""
    for c in spec["holder_classes"]:
        for index, target in enumerate(c["slots"]):
            slot = c["vptr"] + 8 * index
            if target:
                require(e.reloc_target(slot) == target, f"holder class {c['vptr']:#x} slot {index} -> {target:#x}")
            else:
                require(e.u64(slot) == 0, f"holder class {c['vptr']:#x} slot {index} is null")
        require(e.rip(c["dtor_lea"], 7, 3) == c["base_vptr"], f"holder destructor {c['dtor']:#x} rewrites vptr {c['base_vptr']:#x}")
        for index, target in enumerate(c["base_slots"]):
            slot = c["base_vptr"] + 8 * index
            if target:
                require(e.reloc_target(slot) == target, f"holder base class {c['base_vptr']:#x} slot {index} -> {target:#x}")
            else:
                require(e.u64(slot) == 0, f"holder base class {c['base_vptr']:#x} slot {index} is null")


def x_cmd_target_gate(e, spec) -> None:
    """Console command class 0x1594dd0 (the target of a 0x26ea0 / 0x26db0 toggle): slot 0 = 0x4b0, slot 1 = 0x22a60, slot 2 = 0x20d60, slot 3 = 0x22ae0 (`xor eax,eax; ret`),
    so the `slot3 == 1 && slot6 == 3` narrowing never holds and the toggle keeps its default range {1.0, 10.0} (same pin as check_ctor_slot_340.py)."""
    for index, target in enumerate((0x4B0, 0x22A60, 0x20D60, 0x22AE0)):
        require(e.reloc_target(0x1594DD0 + 8 * index) == target, f"console command vtable 0x1594dd0 slot {index} -> {target:#x}")
    e.bytes_at(0x22AE0, "31c0c3", "command slot 3: xor eax,eax; ret")


def _store_table_header(text: str, sym: str):
    """Parse the native store table out of the generated header text: (records, string_rva[], string_offset[], blob_strings[])."""
    import re
    m = re.search(rf"inline constexpr StoreTableRecord k{sym}Stores\[[^\]]*\] = \{{\n(.*?)\n\}};", text, re.S)
    require(m is not None, "native store table array present")
    rec = re.compile(r"^\s*\{(0x[0-9a-f]+), (0x[0-9a-f]+), (\d+), (\d), (\d), 0, (0x[0-9a-f]+), (0x[0-9a-f]+)\},$")
    records = []
    for line in m.group(1).split("\n"):
        r = rec.match(line)
        require(r is not None, f"native store record syntax: {line!r}")
        records.append((int(r.group(1), 16), int(r.group(2), 16), int(r.group(3)), int(r.group(4)), int(r.group(5)), int(r.group(6), 16), int(r.group(7), 16)))
    def arr(name):
        a = re.search(rf"inline constexpr std::uint32_t k{sym}{name}\[[^\]]*\] = \{{([^}}]*)\}};", text)
        require(a is not None, f"native {name} array present")
        return [int(x, 16) for x in a.group(1).split(",") if x.strip()]
    rvas, offs = arr("StoreStringRva"), arr("StoreStringOffset")
    b = re.search(rf"inline constexpr char k{sym}StoreStringBlob\[\] =\n(.*?);\n", text, re.S)
    require(b is not None, "native string blob present")
    pool = []
    for line in b.group(1).split("\n"):
        lm = re.match(r'^\s*"(.*)\\0"$', line)
        require(lm is not None, f"native string literal syntax: {line!r}")
        pool.append(lm.group(1).replace('\\"', '"').replace("\\\\", "\\").encode("latin1"))
    return records, rvas, offs, pool


def x_store_table(e, spec) -> None:
    """Batch 18 (slot 424): re-derive the whole static store table from the ELF BYTES (overlay aware, so an ELF perturbation changes the decode) with the
    closed byte decoder tools/lift/store_bytes_replay.py and require it to equal the NATIVE table of the generated header record for record
    (instruction rva, offset, width, lane kinds and values), the string pool (every pooled string is re-read from .rodata) and the identity tags."""
    from slot_pin_util import ROOT as _root
    import store_bytes_replay as sb
    st = spec["store_table"]
    base, size, dso = st["obj"], st["size"], st["dso"]
    b = sb.ByteReplay(e.raw)
    require(b.step(st["seed_at"]) == st["seed_end"] - st["seed_at"], "store table seed instruction length")
    require(b.g.get(14) == ("a", dso), "store table seed: r14 = &__dso_handle")
    try:
        b.run(st["start"], st["end"])
    except sb.ReplayRefused as err:
        raise SystemExit(f"FAIL store table replay refused: {err}")
    require(b.nins == st["nins"] + 1, "store table instruction count (the seed lea plus the range)")
    call = e.raw(st["end"], 5)
    require(call[0] == 0xE8 and st["end"] + 5 + int.from_bytes(call[1:], "little", signed=True) == 0xF8F390, "store table range ends at the __cxa_atexit PLT call")
    require(b.g.get(7) == ("a", st["dtor"]) and b.g.get(6) == ("c", 0) and b.g.get(2) == ("a", dso), "store table atexit arguments (dtor, 0, &__dso_handle)")
    require(len(b.stores) == st["nstores"], "store table store count")
    records, rvas, offs, pool = _store_table_header((_root / spec["native"]["header"]).read_text(), st["sym"])
    require(len(records) == len(b.stores), "native record count equals the ELF replay")
    require(len(rvas) == len(offs) == len(pool) == st["nstrings"], "native string pool size")
    pos = 0
    for i, t in enumerate(pool):
        require(offs[i] == pos, f"native string offset {i}")
        pos += len(t) + 1
        require(0 < len(t) and all(0x20 <= c < 0x7F for c in t), f"native string {i} is printable")
        e.cstring(rvas[i], t)                                    # the pooled text is the .rodata text at the recorded address
    require(rvas == sorted(rvas) and len(set(rvas)) == len(rvas), "native string rvas are strictly ascending")
    tags = set()
    import re
    lines = re.search(rf"inline constexpr StoreTableRecord k{st['sym']}Stores\[[^\]]*\] = \{{\n(.*?)\n\}};", (_root / spec["native"]["header"]).read_text(), re.S).group(1).split("\n")
    for n, (s_, line) in enumerate(zip(b.stores, lines)):
        f = re.match(r"^\s*\{(0x[0-9a-f]+), (0x[0-9a-f]+), (\d+), (\d), (\d), 0, (0x[0-9a-f]+), (0x[0-9a-f]+)\},$", line).groups()
        insn, off, width, k0, k1, v0, v1 = int(f[0], 16), int(f[1], 16), int(f[2]), int(f[3]), int(f[4]), int(f[5], 16), int(f[6], 16)
        require(insn == s_["addr"], f"record {n}: instruction rva")
        require(off == s_["target"] - base and 0 <= off and off + width <= size, f"record {n}: offset")
        require(width == s_["width"], f"record {n}: width")
        want = []
        for kind, v in s_["lanes"]:
            if kind == "c":
                want.append((0, v))
            elif base <= v < base + size:
                want.append((1, v - base))
            elif st["str_lo"] <= v < st["str_hi"]:
                require(v in rvas, f"record {n}: string lane address {v:#x} is in the pool")
                want.append((2, rvas.index(v)))
            elif v < st["text_hi"] or st["relro_lo"] <= v < st["relro_hi"]:
                tags.add(v)
                want.append((3, v))
            else:
                raise SystemExit(f"FAIL record {n}: address lane {v:#x} outside every known window")
        got = [(k0, v0)] + ([(k1, v1)] if width == 16 else [])
        require(got == want, f"record {n}: lanes {got} vs ELF {want}")
        if width != 16:
            require(k1 == 0 and v1 == 0, f"record {n}: unused lane is zero")
    require(sorted(tags) == st["tags"], "identity tags equal the ELF replay")
    require(b.g.get(14) == ("a", dso), "store table keeps r14 = &__dso_handle")


def _fnv_a2ddc0_oracle(text: bytes) -> int:
    """Independent Python port of 0xa2ddc0 -> 0x162020 (basis 0x811c9dc5, prime 0x1000193, ASCII upper-case, backslash folded to slash); the body bytes are pinned by the checker."""
    h = 0x811C9DC5
    for c in text:
        if 0x61 <= c <= 0x7A:
            c -= 0x20
        if c == 0x5C:
            c = 0x2F
        h = ((h ^ c) * 0x1000193) & 0xFFFFFFFF
    return h


def x_hash_store(e, spec) -> None:
    """Batch 20 (slot 445): re-derive the 79 `lea rdi,<string> ; call 0xa2ddc0 ; mov [rbx+d],eax` sites from the ELF BYTES (overlay aware) with the closed byte decoder
    tools/lift/hash_store_bytes.py and require them to equal the NATIVE records (call rva, store rva, offset, string index), the string pool (re-read from .rodata)
    and the expected dwords (computed here by an independent FNV port, never taken from the native text)."""
    import re
    import hash_store_bytes as hb
    hs = spec["hash_store"]
    sym, base, size = hs["sym"], hs["obj"], hs["size"]
    try:
        r = hb.replay(e.raw, hs["start"], hs["end"], hs["str_lo"], hs["str_hi"])
    except hb.HashStoreRefused as err:
        raise SystemExit(f"FAIL hash store replay refused: {err}")
    require(r["nins"] == hs["nins"], "hash store instruction count")
    require(len(r["stores"]) == hs["nstores"], "hash store count")
    require(r["guard_loads"] <= 1, "at most one stack-guard load in the hash-store run")
    text = (ROOT / spec["native"]["header"]).read_text()
    m = re.search(rf"inline constexpr HashStoreRecord k{sym}HashStores\[[^\]]*\] = \{{\n(.*?)\n\}};", text, re.S)
    require(m is not None, "native hash store records present")
    lines = m.group(1).split("\n")
    require(len(lines) == len(r["stores"]), "native record count equals the ELF replay")

    def arr(name):
        a = re.search(rf"inline constexpr std::uint32_t k{sym}{name}\[[^\]]*\] = \{{([^}}]*)\}};", text)
        require(a is not None, f"native {name} array present")
        return [int(x, 16) for x in a.group(1).split(",") if x.strip()]
    rvas, offs, want_hash = arr("HashStringRva"), arr("HashStringOffset"), arr("HashExpected")
    b = re.search(rf"inline constexpr char k{sym}HashStringBlob\[\] =\n(.*?);\n", text, re.S)
    require(b is not None, "native hash string blob present")
    pool = []
    for line in b.group(1).split("\n"):
        lm = re.match(r'^\s*"(.*)\\0"$', line)
        require(lm is not None, f"native hash string literal syntax: {line!r}")
        pool.append(lm.group(1).replace('\\"', '"').replace("\\\\", "\\").encode("latin1"))
    require(len(rvas) == len(offs) == len(pool) == hs["nstrings"], "native hash string pool size")
    require(len(want_hash) == len(r["stores"]), "native expected-dword count")
    pos = 0
    for i, t in enumerate(pool):
        require(offs[i] == pos, f"native hash string offset {i}")
        pos += len(t) + 1
        require(0 < len(t) and all(0x20 <= c < 0x7F for c in t), f"native hash string {i} is printable")
        e.cstring(rvas[i], t)                                    # the pooled text is the .rodata text at the recorded address
    require(rvas == sorted(rvas) and len(set(rvas)) == len(rvas), "native hash string rvas are strictly ascending")
    seen = set()
    for n, (s_, line) in enumerate(zip(r["stores"], lines)):
        f = re.match(r"^\s*\{(0x[0-9a-f]+), (0x[0-9a-f]+), (0x[0-9a-f]+), (\d+)\},$", line)
        require(f is not None, f"native hash record {n} syntax: {line!r}")
        call, store, off, sidx = int(f.group(1), 16), int(f.group(2), 16), int(f.group(3), 16), int(f.group(4))
        require((call, store) == (s_[0], s_[1]), f"hash record {n}: call / store rva")
        require(off == s_[2] - base and 0 <= off and off + 4 <= size and off % 4 == 0, f"hash record {n}: offset")
        require(off not in seen, f"hash record {n}: destination written once")
        seen.add(off)
        require(rvas[sidx] == s_[3], f"hash record {n}: string address")
        require(want_hash[n] == _fnv_a2ddc0_oracle(pool[sidx]) == hs["expected"][n], f"hash record {n}: expected dword")


lib.EXTRAS["cmd_target_gate"] = x_cmd_target_gate
lib.EXTRAS["hash_store"] = x_hash_store
lib.EXTRAS["store_table"] = x_store_table
lib.EXTRAS["bp_atexit_imports"] = x_bp_atexit_imports
lib.EXTRAS["bp_strlen_import"] = x_bp_strlen_import
lib.EXTRAS["bp_object_vtables"] = x_bp_object_vtables
lib.EXTRAS["bp_holder_vtables"] = x_bp_holder_vtables
lib.EXTRAS["bp_string_var"] = x_bp_string_var
lib.EXTRAS["bp_holder_classes"] = x_bp_holder_classes


def _tamper_native(spec) -> int:
    """`--tamper native`: perturb one pinned native constant in memory; the native-token checks must reject it."""
    import re
    texts = lib.base.read_native(spec)
    consts = spec["native"].get("consts", {})
    require(consts, "native tamper probe needs at least one pinned constant")
    name, value = sorted(consts.items())[0]
    pattern = re.compile(rf"({name} = )(0x[0-9A-Fa-f]+|\d+)(u?;)")
    mutated = dict(texts)
    mutated["header"] = pattern.sub(lambda m: f"{m.group(1)}{value ^ 0x4:#x}{m.group(3)}", texts["header"], count=1)
    require(mutated["header"] != texts["header"], f"constant {name} pattern did not match")
    lib.base.check_native(spec, mutated)   # raises SystemExit("FAIL ...") (exit 1) when the tamper is detected
    print("native tamper NOT detected", file=sys.stderr)
    return 0


def main(spec, summary: str) -> int:
    """lane-M6 driver plus a real `--tamper native` probe (the shared driver only tampers the ELF)."""
    args = sys.argv[1:]
    if "--tamper" in args and "native" in args:
        return _tamper_native(spec)
    return lib.main(spec, summary)
