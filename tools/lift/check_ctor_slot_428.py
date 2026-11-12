#!/usr/bin/env python3
"""ELF-tie checker for startup constructor slot 428 (RVA 0x52ec10, 5375 B; CPU-01-AOT-CFG-SCALE-01-BATCH19).

The body is a straight-line initialiser with 49 __cxa_atexit registrations and one branch (the stack-guard epilogue). It is pinned three ways:
  1. the whole-body SHA-256 and FDE length, the prologue and the guard epilogue byte for byte, the callee bodies (0xa59620, 0xa42960, 0xa2ddc0/0x162020,
     0xf590) by SHA-256 and the PLT slots of __cxa_atexit / __stack_chk_fail;
  2. a closed byte decoder + symbolic tracer (tools/lift/slot428_trace.py, overlay aware, independent of objdump) replays the 1049 instructions between the
     prologue and the epilogue and folds them into the phases: 98 static stores, 5 save descriptors with 4 hash calls each, 37 calls of 0xa59620, 6 calls of
     0xa42960 and exactly 49 __cxa_atexit calls (dtor tags 0x52e980, 0x52eaa0 x5, 0x52eb00 x37, 0x52eb10 x6, objects = the entries). The native tables must
     equal the replay record for record (store offsets / widths / lane kinds and values, the string pool re-read from .rodata, ids, lengths, flags, hash
     strings, entry addresses);
  3. the native tokens (header constants, source order, test and CMake tokens), a mutation self-test of the SPEC and of the native text (inherited), plus a
     decode mutation self-test that flips the first and the last byte of EVERY traced instruction and of every .rodata vector constant; each must be rejected.

WITHHELD: four 256-character hexadecimal strings of the static object look like per-title credentials. They are not reproduced in the repository: the native
pool holds 256 x 'X' and this checker ties the ELF text by length, hex alphabet and SHA-256 only.

NOT established: what the objects mean (INFERENCE only), what consumes them, the bodies of the four destructor tags (identity only; their FDE starts are pinned).

Usage: python3 -I tools/lift/check_ctor_slot_428.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import hashlib
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402
import check_helper_a42960 as a42  # noqa: E402
import check_helper_a59620 as a59  # noqa: E402
import helper_callee_lib as lib  # noqa: E402
import l4_slot_lib as l4  # noqa: E402
import slot428_trace as tr  # noqa: E402
from slot_pin_util import Elf, require  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
HEADER = "native/core/ctor_slot_428_52ec10.hpp"
BASE, SIZE, SHA = 0x52EC10, 5375, "0cab3c7a2c7d45e3570f51adc3bdb5c898b030ce8f998699380fce0c56f3e828"
WITHHELD = {   # rva -> sha256 of the 256 text bytes (no NUL)
    0x1013D51: "b0f3f0f894bba281358ca549c16359e59da65ed49eb831b46b21e6494993f535",
    0x1013E66: "de75899549a9d23b4977339df0afbb18ea7e5baf8acb93b3a975d7205cfc78c4",
    0x1013F88: "ad0b6bff44b4aa76370fb24c187cee51055140eb07be6ad04eb40de771d3dc5b",
    0x101409C: "477b2c50ae23224b727ec63cae0cdbeb7b87423e46c0f609ddd6e1a67d3b8cb2",
}
DTOR_FDE = {0x52E980: 284, 0x52EAA0: 96, 0x52EB00: 9, 0x52EB10: 55}
STATICS = 0x2EFD610
FNV_WRAPPER = (0xA2DDC0, 0xA, "c526d089a4b2d7d1c1acf93955168456e042e061b9f42ec2cfc7fd160fb10765")
FNV_BODY = (0x162020, 0x62, "a7d19297baf11957f065c2631731c010abc4d382f344a2125d59a09f65684d57")
PROLOGUE = "554889e54157415641554154534883ec48"
EPILOGUE = "488b0527e40c01488b00483b45d0750f4883c4485b415c415d415e415f5dc3e892f2a50090"

lib.pins.PINS.update(a59.pins_g.PINS)


# ------------------------------------------------------------------------------------------------------------------------------------
# native tables
# ------------------------------------------------------------------------------------------------------------------------------------
def parse_native(text: str) -> dict:
    records, rvas, offs, pool = bp._store_table_header(text, "Slot428")
    def block(name):
        m = re.search(rf"inline constexpr {name} kSlot428{name[7:]}s\[[^\]]*\] = \{{\n(.*?)\n\}};", text, re.S)
        require(m is not None, f"native {name} table present")
        return m.group(1).split("\n")
    saves, ids, avails = [], [], []
    for line in block("Slot428SaveSpec"):
        m = re.match(r'^\s*\{(\d+), "([^"]*)", (\d+), (\d+), \{"([^"]*)", "([^"]*)", "([^"]*)", "([^"]*)"\}\},$', line)
        require(m is not None, f"native save spec syntax: {line!r}")
        saves.append((int(m.group(1)), m.group(2), int(m.group(3)), int(m.group(4)), [m.group(k) for k in (5, 6, 7, 8)]))
    for line in block("Slot428IdSpec"):
        m = re.match(r'^\s*\{(\d+), "([^"]*)", (\d+)\},$', line)
        require(m is not None, f"native id spec syntax: {line!r}")
        ids.append((int(m.group(1)), m.group(2), int(m.group(3))))
    for line in block("Slot428AvailSpec"):
        m = re.match(r'^\s*\{"([^"]*)", (\d+), "([^"]*)", (\d+)\},$', line)
        require(m is not None, f"native avail spec syntax: {line!r}")
        avails.append((m.group(1), int(m.group(2)), m.group(3), int(m.group(4))))
    return dict(records=records, rvas=rvas, offs=offs, pool=pool, saves=saves, ids=ids, avails=avails)


def elf_text(e: Elf, addr: int, native_text: str, what: str) -> None:
    e.cstring(addr, native_text.encode("ascii"))
    require(len(native_text) > 0, f"{what}: not empty")


# ------------------------------------------------------------------------------------------------------------------------------------
# the replay tie
# ------------------------------------------------------------------------------------------------------------------------------------
def tie(e: Elf, native: dict, record: bool = True) -> None:
    try:
        m = tr.parse428(e.raw)
    except (TypeError, IndexError, KeyError, AttributeError, ValueError) as error:
        raise SystemExit(f"FAIL the replay's structure expectations were violated ({type(error).__name__}: {error})")
    t = m["trace"]
    require(len(t.insns) == 1049, "1049 instructions between the prologue and the guard epilogue")
    calls = [ev for ev in t.events if ev["kind"] == "call"]
    require(len(calls) == 161, "161 direct calls between the prologue and the epilogue (the 162nd is __stack_chk_fail)")
    counts = {}
    for c in calls:
        counts[c["target"]] = counts.get(c["target"], 0) + 1
    require(counts == {tr.ATEXIT: 49, tr.FNV: 20, tr.F590: 49, tr.ID_CTOR: 37, tr.AVAIL_CTOR: 6}, f"call census {counts}")

    # the canary saved by the prologue is the slot the guard epilogue compares (cmp disp8(%rbp), at 0x5300f4)
    cmp_off = e.raw(0x5300F7, 1)[0]
    cmp_off -= 256 if cmp_off > 127 else 0
    require(t.stack.get(cmp_off) is not None and tuple(t.stack[cmp_off][1]) == (("canary",),), "the prologue canary slot is the epilogue compare slot")
    # -- phase A: static store table
    recs, rvas, offs, pool = native["records"], native["rvas"], native["offs"], native["pool"]
    st = m["stores"]
    require(len(recs) == len(st) == 98, "98 static stores")
    require(len(rvas) == len(offs) == len(pool) == 16, "16 pooled strings")
    pos = 0
    withheld = set()
    for i, text in enumerate(pool):
        require(offs[i] == pos, f"native string offset {i}")
        pos += len(text) + 1
        require(0 < len(text) and all(0x20 <= c < 0x7F for c in text), f"native string {i} is printable")
        if rvas[i] in WITHHELD:
            require(text == b"X" * 256, f"pool {i}: the withheld string is 256 x 'X' natively")
            elf = e.read(rvas[i], 257)
            require(len(elf) == 257 and elf[256] == 0 and all(chr(c) in "0123456789abcdef" for c in elf[:256]), f"pool {i}: the ELF string is 256 hex digits")
            require(hashlib.sha256(elf[:256]).hexdigest() == WITHHELD[rvas[i]], f"pool {i}: the ELF string matches its pinned SHA-256")
            withheld.add(rvas[i])
        else:
            require(len(text) < 256, f"pool {i}: only the four pinned strings may be 256 long")
            e.cstring(rvas[i], text)
    require(withheld == set(WITHHELD), "all four withheld strings are pooled")
    require(rvas == sorted(rvas) and len(set(rvas)) == len(rvas), "native string rvas are strictly ascending")
    require(rvas == sorted({lane[1] for s in st for lane in s["lanes"] if lane[0] == "a"}), "the pool is exactly the set of address lanes of the replay")
    for n, (rec, s) in enumerate(zip(recs, st)):
        insn, off, width, k0, k1, v0, v1 = rec
        require(insn == s["insn"], f"record {n}: instruction rva")
        require(off == s["target"] - STATICS and 0 <= off and off + width <= 0x390, f"record {n}: offset")
        require(width == s["width"], f"record {n}: width")
        want = []
        for lane in s["lanes"]:
            if lane[0] == "c":
                want.append((0, lane[1]))
            else:
                require(lane[1] in rvas, f"record {n}: string lane {lane[1]:#x} is pooled")
                want.append((2, rvas.index(lane[1])))
        got = [(k0, v0)] + ([(k1, v1)] if width == 16 else [])
        require(got == want, f"record {n}: lanes {got} vs ELF {want}")
        if width != 16:
            require(k1 == 0 and v1 == 0, f"record {n}: unused lane is zero")
    # -- phase B/C/D
    require(m["atexit"][0] == (0x52E980, None), "first atexit: (0x52e980, null)")
    require(len(m["recs"]) == len(native["saves"]) == 5, "5 save descriptors")
    for i, (r, ns) in enumerate(zip(m["recs"], native["saves"])):
        nid, name, name_len, flag, hashes = ns
        require(r["addr"] == 0x2EFD9A0 + 0x58 * i, f"save {i}: address")
        require(r["id"] == nid and r["name_len"] == name_len == len(name) and r["flag"] == flag, f"save {i}: id / length / flag")
        elf_text(e, r["name"], name, f"save {i} name")
        for k in range(4):
            elf_text(e, r["hashes"][k], hashes[k], f"save {i} hash string {k}")
    require(len(m["ids"]) == len(native["ids"]) == 37, "37 id entries")
    for i, (r, ns) in enumerate(zip(m["ids"], native["ids"])):
        nid, text, length = ns
        require(r["addr"] == 0x2EFDB58 + 0x28 * i and r["id"] == nid == i + 1 and r["text_len"] == length == len(text), f"id entry {i}")
        elf_text(e, r["text"], text, f"id entry {i}")
    require(len(m["avails"]) == len(native["avails"]) == 6, "6 availability entries")
    for i, (r, ns) in enumerate(zip(m["avails"], native["avails"])):
        a, a_len, b, b_len = ns
        require(r["addr"] == 0x2EFE120 + 0x48 * i and r["a_len"] == a_len == len(a) and r["b_len"] == b_len == len(b), f"availability entry {i}")
        elf_text(e, r["a"], a, f"availability {i} a")
        elf_text(e, r["b"], b, f"availability {i} b")
    want_atexit = ([(0x52E980, None)] + [(0x52EAA0, 0x2EFD9A0 + 0x58 * i) for i in range(5)] + [(0x52EB00, 0x2EFDB58 + 0x28 * i) for i in range(37)]
                   + [(0x52EB10, 0x2EFE120 + 0x48 * i) for i in range(6)])
    require(m["atexit"] == want_atexit, "the 49 atexit registrations: dtor tags and objects in order")
    for dtor, size in DTOR_FDE.items():
        fde = lib.fde_ranges()
        if fde is not None:
            require(fde.get(hex(dtor)) == size, f"destructor tag {dtor:#x} starts a function of {size} bytes")
    # the vector constants read from .rodata (recorded pins: the framework mutates first / last byte)
    for addr, n in t.rodata_reads:
        e.read(addr, n)
    require(len(t.rodata_reads) == 6, "6 vector constants")
    # callee bodies
    for rva, size, sha in (FNV_WRAPPER, FNV_BODY, l4.COMMON_CALLEES["release_f590"]):
        e.body_hash(f"callee {rva:#x}", rva, size, sha, always=True)
    e.body_hash("0xa59620", a59.SPEC["bodies"][0]["rva"], a59.SPEC["bodies"][0]["size"], a59.SPEC["bodies"][0]["sha"], always=True)
    e.body_hash("0xa42960", a42.SPEC["bodies"][0]["rva"], a42.SPEC["bodies"][0]["size"], a42.SPEC["bodies"][0]["sha"], always=True)


def extra(e, fams) -> None:
    bp._bp_check_imports(e, ((0xF8F390, 0x15FFFB8, "__cxa_atexit"), (0xF8F3A0, 0x15FFFC0, "__stack_chk_fail")))
    require(e.rip(0x52EC21, 7, 3) == tr.GUARD_GOT and e.rip(0x52EC61, 7, 3) == tr.DSO, "stack guard GOT slot and &__dso_handle")
    tie(e, parse_native((ROOT / HEADER).read_text()))


# ------------------------------------------------------------------------------------------------------------------------------------
# decode mutation self-test
# ------------------------------------------------------------------------------------------------------------------------------------
def rex_b_rip_equivalent(e: Elf, t, byte_addr: int) -> bool:
    """The only architecturally equivalent single-bit mutants: bit 0 of a REX prefix (REX.B) on an instruction whose ModRM is RIP-relative
    (mod 00, rm 101): REX.B extends the rm / base field, which RIP-relative addressing does not have, so the instruction is unchanged."""
    for addr, ln, _mnem in t.insns:
        if addr <= byte_addr < addr + ln:
            if byte_addr != addr or ln < 7:
                return False
            raw = e.raw(addr, ln)
            # raw carries the MUTATED byte: REX.B is set on what was 0x48 / 0x4c
            return raw[0] in (0x49, 0x4D) and raw[1] in (0x89, 0x8B, 0x8D, 0xC7) and (raw[2] & 0xC7) == 0x05
    return False


def decode_mutation_test() -> int:
    e = Elf()
    native = parse_native((ROOT / HEADER).read_text())
    e.recording = False
    tie(e, native)                                         # the pristine image passes
    t = tr.Tracer(e.raw).run()
    targets = set()
    for addr, ln, _ in t.insns:
        targets.update((addr, addr + ln - 1))
    for addr, n in t.rodata_reads:
        targets.update((addr, addr + n - 1))
    for _, rva in ((0, r) for r in WITHHELD):
        targets.update((rva, rva + 255))
    runs = survivors = equivalent = 0
    for byte_addr in sorted(targets):
        e.overlay = {byte_addr: e.raw(byte_addr, 1)[0] ^ 0x01}
        runs += 1
        try:
            tie(e, native)
        except SystemExit as error:
            require(str(error).startswith("FAIL"), f"decode mutant {byte_addr:#x}: unexpected exit {error!r}")
            continue
        except Exception as error:   # noqa: BLE001 - a mutated byte must never crash the tracer silently
            raise SystemExit(f"FAIL decode mutant {byte_addr:#x} raised {type(error).__name__}: {error}")
        if rex_b_rip_equivalent(e, t, byte_addr):
            equivalent += 1
            continue
        survivors += 1
        print(f"decode mutation survived at {byte_addr:#x}", file=sys.stderr)
    e.overlay = {}
    require(survivors == 0, f"{survivors} decode mutations survived")
    return runs, equivalent


SPEC = {
    "name": "check_ctor_slot_428",
    "summary": ("slot 428 (0x52ec10, 5375 B): body SHA/FDE, prologue and guard epilogue, 1049 instructions replayed by the byte tracer into 98 static stores, "
                "5 save descriptors (20 hash calls), 37 x 0xa59620, 6 x 0xa42960 and the 49 atexit registrations, tied to the native tables record for record; "
                "4 credential-like strings withheld (SHA-256 only)"),
    "families": [],
    "bodies": [{"name": "slot 428 constructor", "rva": BASE, "size": SIZE, "fde": SIZE, "pins": None, "sha": SHA}],
    "rips": [],
    "calls": [("stack_chk_fail", 0x530109, 0xF8F3A0)],
    "jmps": [],
    "relocs": [],
    "bytes": [("prologue", BASE, PROLOGUE), ("guard epilogue", 0x5300EA, EPILOGUE)],
    "extra": extra,
    "native": {"helper_h": HEADER, "helper_c": "native/core/ctor_slot_428_52ec10.cpp", "test": "native/tests/test_ctor_slot_428.cpp", "cmake": "native/CMakeLists.txt"},
    "constants": {
        "helper_h": {"kSlot428Rva": BASE, "kSlot428Size": SIZE, "kSlot428StaticsRva": STATICS, "kSlot428StaticsSize": 0x390, "kSlot428StoreCount": 98,
                     "kSlot428StoreStringCount": 16, "kSlot428StoreDtorTag": 0x52E980, "kSlot428SaveRecordRva": 0x2EFD9A0, "kSlot428SaveRecordStride": 0x58,
                     "kSlot428SaveRecordCount": 5, "kSlot428SaveDtorTag": 0x52EAA0, "kSlot428IdEntryRva": 0x2EFDB58, "kSlot428IdEntryCount": 37,
                     "kSlot428IdDtorTag": 0x52EB00, "kSlot428AvailEntryRva": 0x2EFE120, "kSlot428AvailEntryCount": 6, "kSlot428AvailDtorTag": 0x52EB10,
                     "kSlot428AtexitCount": 49, "kSlot428WithheldLength": 256},
    },
    "tokens": {
        "helper_h": ["static_assert(sizeof(Slot428SaveRecord) == kSlot428SaveRecordStride", "WITHHELD", "SharedStrDeallocatorF590* name_dealloc;"],
        "helper_c": ["replay_store_table(objects.statics.bytes", "concmd.atexit_log.record(kSlot428StoreDtorTag, nullptr)", "fnv_cstr_a2ddc0(s.hash_text[k])",
                     "concmd.atexit_log.record(kSlot428SaveDtorTag, &r)", "register_id_entry_a59620(objects.id_vec, objects.ids[i], s.id, view, objects.alloc)",
                     "release_inert_view_f590(view)", "concmd.atexit_log.record(kSlot428IdDtorTag, &objects.ids[i])",
                     "register_avail_entry_a42960(objects.avail_vec, objects.avails[i], a, b, concmd, hooks)",
                     "!release_inert_view_f590(b) || !release_inert_view_f590(a)", "concmd.atexit_log.record(kSlot428AvailDtorTag, &objects.avails[i])",
                     "if (!contract_present(objects)) return out;", "out.ok = out.atexit_recorded == kSlot428AtexitCount;"],
        "test": ["CHECK(r.ok && r.statics_applied && r.saves_done == 5 && r.ids_done == 37 && r.avails_done == 6 && r.atexit_recorded == 49);",
                 "CHECK(g.allocs == 7 + 4 + 6 + 12);", "CHECK(withheld == 4);", "test_missing_contract();", "test_alloc_failure();",
                 "o->saves[0].hash[0] == 0x65746b20u"],
        "cmake": ["core/ctor_slot_428_52ec10.cpp", "add_test(NAME ctor_slot_428 COMMAND test_ctor_slot_428)"],
    },
}


if __name__ == "__main__":
    rc = lib.main(SPEC)
    flags = sys.argv[1:]
    if rc == 0 and "--no-mutate" not in flags and "--tamper" not in flags:
        runs, equivalent = decode_mutation_test()
        print(f"OK check_ctor_slot_428: decode mutation self-test: {runs} first/last-byte mutants of the traced instructions, vector constants and withheld strings; "
              f"{runs - equivalent} rejected, {equivalent} are REX.B flips on RIP-relative operands (architecturally identical instructions), 0 other survivors")
    raise SystemExit(rc)
