#!/usr/bin/env python3
"""Re-derive the target chain of the load-side indirect call at `0x5435fc`.

Task SAV-01-LOAD-DISPATCH-5435FC-01. The call sits in `FUN_00542880` (0x542880, 0x131c B) and is a
libstdc++-layout `shared_ptr` control-block release, not a game-object virtual call:

    control block          = the `shared_ptr` second word written by `0x515770` through its sret
    control block vptr     = `0x15c6f68` (`lea 0x15c6f58` + `add $0x10`), written at `0x5162f9`
    slot +0x10 (`_M_dispose`) = `*(0x15c6f78)` = `0x517220`   <-- call at 0x5435fc
    slot +0x18 (`_M_destroy`) = `*(0x15c6f80)` = `0x517250`   <-- call at 0x543611

The control block exists only because the resource map at `pool+0x102100` was populated by
`0xa71c90`'s find-or-create path, whose factory for the `'CLCT'` key namespace is the
descriptor `{self, 0x2193d0, 0, 0x516230}` built in `0x515770`. This checker re-derives every link
from the pinned image: instruction bytes, rel32 targets, R_X86_64_RELATIVE rows, FDE extents.

Read-only. Nothing is executed and no target is guessed: every step must byte-match or the check
fails. Skips cleanly (exit 0) when the private image is absent.

Usage:
  python3 tools/lift/check_sav_load_dispatch.py
  python3 tools/lift/check_sav_load_dispatch.py --json local/private/out.json
"""

from __future__ import annotations

import argparse
import hashlib
import importlib.util
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))

ELF = ROOT / "local/re/eboot.elf"
FDE = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"
DIS = ROOT / "local/validation/registry-analysis-agent/all.dis"

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"

LOADER = 0x542880          # FUN_00542880
LOADER_SIZE = 0x131C
LOADER_SHA256 = "a2985de4bc806e87c89440718213ea0c291d0a76c36a6b9726cf0048ce7c12b9"

CALL_SITE = 0x5435FC
DISPOSE_GATE = 0x5435EE
SECOND_CALL = 0x543611
CONTROL_BLOCK_SLOT = 0x198          # rsp displacement that holds the control block
PRODUCER = 0x515770                 # resource-pool accessor (sret shared_ptr)
PRODUCER_SIZE = 0x183
PRODUCER_SRET = 0x190               # rsp displacement of the sret shared_ptr
LOOKUP = 0xA71C90                   # map find-or-create
INSERT_HELPER = 0xA721C0            # private RB-tree insert helper of 0xa71c90
FIND_ONLY = 0xA72000                # read-only sibling accessor (never inserts)
FIND_ONLY_CALLER = 0x5160F0         # function that calls the read-only accessor
MAP_OWNERS = {0xA66DC0, 0xA671E0, 0xA67300, LOOKUP, FIND_ONLY}
FACTORY_TAG = 0x54434C43            # 'CLCT'
FACTORY_DESCRIPTOR_SLOT = -0x60     # rbp displacement of the descriptor
TRAMPOLINE_SLOT = 0x15FEDA0         # global holding the descriptor's invoke pointer
TRAMPOLINE = 0x2193D0               # {+0x18} thunk -> factory
FACTORY = 0x516230                  # constructs resource + control block
FACTORY_SIZE = 0xD8
FACTORY_SHA256 = "ad50460839f4033465e1ebbf6534b89896c9431f7642c89e27d3bf7edd319e1c"
CONTROL_VTABLE_BASE = 0x15C6F58     # `lea` target in the factory; address point is +0x10
CONTROL_ADDRESS_POINT = 0x15C6F68
CONTROL_SLOT = 0x10                 # bytes into the address point used by the call site
CONTROL_SLOT_ADDR = CONTROL_ADDRESS_POINT + CONTROL_SLOT
TARGET = 0x517220                   # _Sp_counted_ptr<...>::_M_dispose()
TARGET_SIZE = 0x27
TARGET_SHA256 = "073bb1e4dd219b449b5ca6a73778b5b6feca658817adaf0e80b154398d66ef9a"
DESTROY_TARGET = 0x517250           # _Sp_counted_ptr<...>::_M_destroy()
DESTROY_SIZE = 0x28
DESTROY_SHA256 = "9461adfceff484d10232a7ec76f71c482c90ab41e9117214de271307efeefa62"
CHUNK_NAME = 0x1017142              # "SecretRoomCollectibles" NUL-terminated
CHUNK_NAME_TEXT = b"SecretRoomCollectibles\0"

# instruction bytes, verified against the pinned image
BYTES = {
    0x543094: "488dbc2490010000",   # leaq 0x190(%rsp), %rdi      (sret pointer for 0x515770)
    0x5430BF: "e8ac26fdff",         # callq 0x515770
    0x5435C8: "488b9c2498010000",   # movq 0x198(%rsp), %rbx      (control block)
    0x5435D5: "4885db",             # testq %rbx, %rbx            (empty -> skip release)
    0x5435EE: "7e06",               # jle 0x5435f6                (strong count reached zero)
    0x5435F6: "488b03",             # movq (%rbx), %rax           (control-block vptr)
    0x5435F9: "4889df",             # movq %rbx, %rdi             (this = control block)
    0x5435FC: "ff5010",             # callq *0x10(%rax)
    0x543611: "ff5018",             # callq *0x18(%rax)
    0x5157B2: "488d0d770a0000",     # leaq 0x516230(%rip), %rcx   (factory into descriptor[+0x18])
    0x5157BD: "be434c4354",         # movl $0x54434c43, %esi      ('CLCT' namespace tag)
    0x5157C4: "4c8945a0",           # movq %r8, -0x60(%rbp)       (descriptor self pointer)
    0x5157CD: "48894db8",           # movq %rcx, -0x48(%rbp)      (descriptor[+0x18] = factory)
    0x5157D4: "e8b7c45500",         # callq 0xa71c90              (find-or-create)
    0x515824: "49c7470800000000",   # movq $0x0, 0x8(%r15)        (miss -> empty shared_ptr)
    0x515866: "4d8927",             # movq %r12, (%r15)           (hit -> object)
    0x515869: "49895f08",           # movq %rbx, 0x8(%r15)        (hit -> control block)
    0x2193D6: "4889f04889d64889fb",  # movq %rsi,%rax / movq %rdx,%rsi / movq %rdi,%rbx
    0x2193DF: "ff5018",             # callq *0x18(%rax)           (descriptor[+0x18] = factory)
    0x5162D9: "488d0d780c0b01",     # leaq 0x15c6f58(%rip), %rcx  (control-block vtable base)
    0x5162E0: "c7400801",           # movl $0x1, 0x8(%rax)        (strong = 1)
    0x5162EB: "c7400c01",           # movl $0x1, 0xc(%rax)        (weak = 1)
    0x5162F5: "4883c110",           # addq $0x10, %rcx            (address point)
    0x5162F9: "488908",             # movq %rcx, (%rax)           (control-block vptr)
    0x5162FC: "48895810",           # movq %rbx, 0x10(%rax)       (_M_ptr)
    0x517232: "488b07ff5008",       # movq (%rdi),%rax / callq *0x8(%rax)
}

CALL_REL32 = {
    0x5430BF: PRODUCER,
    0x5157D4: LOOKUP,
}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(f"FAIL check_sav_load_dispatch: {message}")


def load_census():
    spec = importlib.util.spec_from_file_location("cfg_census", ROOT / "tools/lift/cfg_census.py")
    module = importlib.util.module_from_spec(spec)
    sys.modules["cfg_census"] = module
    spec.loader.exec_module(module)
    return module


def rel32_target(image, address: int) -> int:
    """`e8 rel32` / `e9 rel32` target."""
    raw = image.read(address, 5)
    return address + 5 + struct.unpack("<i", raw[1:5])[0]


def lea_target(image, address: int) -> int:
    """`48 8d 0d rel32` (lea reg,[rip+disp32]) target; 7-byte form only."""
    raw = image.read(address, 7)
    require(raw[:3] == b"\x48\x8d\x0d", f"0x{address:x} is not `lea reg,[rip+disp32]`")
    return address + 7 + struct.unpack("<i", raw[3:7])[0]


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--json", type=Path, default=None, help="write the resolved chain as JSON")
    args = parser.parse_args(argv)

    if not ELF.is_file() or not FDE.is_file():
        print("SKIP check_sav_load_dispatch: private image not present "
              "(chain documented in docs/checkpoints/2026-11-01-sav01-load-dispatch-5435fc.md)")
        return 0

    import ps4_dynlink as dyn  # noqa: E402

    raw = ELF.read_bytes()
    require(hashlib.sha256(raw).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    parsed = dyn.parse(raw)
    image = dyn.Image(raw, parsed["ph"])
    fdes, _duplicates = load_census().load_fde_ranges(FDE)

    # 1. function extent and body
    extent = fdes.find(LOADER)
    require(extent is not None and extent == (LOADER, LOADER_SIZE),
            f"0x{LOADER:x} FDE extent {extent!r}, expected {(LOADER, LOADER_SIZE)!r}")
    require(hashlib.sha256(image.read(LOADER, LOADER_SIZE)).hexdigest() == LOADER_SHA256,
            f"0x{LOADER:x} body sha256")
    require(fdes.find(CALL_SITE)[0] == LOADER, "0x5435fc outside FUN_00542880")

    # 2. the call site and its two register definitions
    for address, expected in sorted(BYTES.items()):
        got = image.read(address, len(expected) // 2).hex()
        require(got == expected, f"bytes at 0x{address:x}: {got} != {expected}")
    for address, expected in CALL_REL32.items():
        require(rel32_target(image, address) == expected,
                f"call at 0x{address:x} does not target 0x{expected:x}")
    require(fdes.find(PRODUCER)[0] == PRODUCER, "0x515770 is not an FDE start")

    # 3. the control block arrives through the sret of 0x515770
    require(CONTROL_BLOCK_SLOT == PRODUCER_SRET + 8,
            "control-block slot is not sret+8 of the producer")
    for store in (0x515866, 0x515869, 0x515824):
        extent = fdes.find(store)
        require(extent is not None and extent[0] == PRODUCER,
                f"shared_ptr store at 0x{store:x} is outside 0x{PRODUCER:x}")

    # 4. 0x515770 asks the map for a 'CLCT' entry and passes the factory descriptor
    require(fdes.find(LOOKUP)[0] == LOOKUP, "0xa71c90 is not an FDE start")
    require(struct.unpack("<I", image.read(0x5157BE, 4))[0] == FACTORY_TAG,
            "tag immediate at 0x5157be is not 'CLCT'")
    require(lea_target(image, 0x5157B2) == FACTORY,
            f"0x5157b2 lea does not resolve to 0x{FACTORY:x}")

    # 5. descriptor[+0x08]: a RELATIVE-relocated global, read-only in the image
    relocated = {reloc["target"]: reloc["addend"] for reloc in parsed["relocs"] if reloc["type"] == 8}
    require(TRAMPOLINE_SLOT in relocated, f"0x{TRAMPOLINE_SLOT:x} has no R_X86_64_RELATIVE row")
    require(relocated[TRAMPOLINE_SLOT] == TRAMPOLINE,
            f"0x{TRAMPOLINE_SLOT:x} addend 0x{relocated[TRAMPOLINE_SLOT]:x} != 0x{TRAMPOLINE:x}")
    require(struct.unpack("<Q", image.read(TRAMPOLINE_SLOT, 8))[0] == TRAMPOLINE,
            "0x15feda0 raw qword disagrees with its relocation addend")
    require(fdes.find(TRAMPOLINE)[0] == TRAMPOLINE, "0x2193d0 is not an FDE start")

    # 6. the factory constructs the control block with vtable base 0x15c6f58
    require(fdes.find(FACTORY)[0] == FACTORY and fdes.find(FACTORY)[1] == FACTORY_SIZE,
            "0x516230 FDE extent")
    require(hashlib.sha256(image.read(FACTORY, FACTORY_SIZE)).hexdigest() == FACTORY_SHA256,
            "0x516230 body sha256")
    require(lea_target(image, 0x5162D9) == CONTROL_VTABLE_BASE,
            f"0x5162d9 lea does not resolve to 0x{CONTROL_VTABLE_BASE:x}")
    require(CONTROL_VTABLE_BASE + 0x10 == CONTROL_ADDRESS_POINT, "address point pin")

    # 7. slot +0x10 of that vtable is the RELATIVE-relocated 0x517220
    require(CONTROL_SLOT_ADDR in relocated, f"0x{CONTROL_SLOT_ADDR:x} has no RELATIVE row")
    require(relocated[CONTROL_SLOT_ADDR] == TARGET,
            f"0x{CONTROL_SLOT_ADDR:x} addend 0x{relocated[CONTROL_SLOT_ADDR]:x} != 0x{TARGET:x}")
    require(struct.unpack("<Q", image.read(CONTROL_SLOT_ADDR, 8))[0] == TARGET,
            "control-block vtable slot disagrees with its relocation addend")
    destroy_slot = CONTROL_ADDRESS_POINT + 0x18
    require(relocated.get(destroy_slot) == DESTROY_TARGET,
            f"0x{destroy_slot:x} addend != 0x{DESTROY_TARGET:x} (second call site)")

    # 8. the target is a function, and its body is the dispose thunk
    extent = fdes.find(TARGET)
    require(extent == (TARGET, TARGET_SIZE), f"0x{TARGET:x} FDE extent {extent!r}")
    require(hashlib.sha256(image.read(TARGET, TARGET_SIZE)).hexdigest() == TARGET_SHA256,
            f"0x{TARGET:x} body sha256")
    extent = fdes.find(DESTROY_TARGET)
    require(extent == (DESTROY_TARGET, DESTROY_SIZE), f"0x{DESTROY_TARGET:x} FDE extent {extent!r}")
    require(hashlib.sha256(image.read(DESTROY_TARGET, DESTROY_SIZE)).hexdigest() == DESTROY_SHA256,
            f"0x{DESTROY_TARGET:x} body sha256")

    # 9. the looked-up name is the save chunk name (links the site to SAV-01)
    require(image.read(CHUNK_NAME, len(CHUNK_NAME_TEXT)) == CHUNK_NAME_TEXT,
            "0x1017142 is not the SecretRoomCollectibles chunk name")

    # 10. exactly one *inserting* 'CLCT' producer, and it is the site above
    segments = [(seg["vaddr"], raw[seg["offset"]:seg["offset"] + seg["filesz"]])
                for seg in parsed["ph"]
                if seg["type"] == dyn.PT_LOAD and seg["flags"] & 1]

    def tag_sites(value: int):
        needle = b"\xbe" + struct.pack("<I", value)
        found = []
        for vaddr, blob in segments:
            index = 0
            while True:
                index = blob.find(needle, index)
                if index < 0:
                    break
                found.append(vaddr + index)
                index += 1
        return sorted(found)

    def call_sites(target: int):
        found = []
        for vaddr, blob in segments:
            index = 0
            while True:
                index = blob.find(b"\xe8", index)
                if index < 0:
                    break
                if index + 5 <= len(blob):
                    rel = struct.unpack("<i", blob[index + 1:index + 5])[0]
                    if vaddr + index + 5 + rel == target:
                        found.append(vaddr + index)
                index += 1
        return sorted(found)

    require(tag_sites(FACTORY_TAG) == [0x5157BD, 0x516126],
            f"'CLCT' tag immediates at {[hex(a) for a in tag_sites(FACTORY_TAG)]}, "
            "expected [0x5157bd, 0x516126]")
    # 0xa71c90 is the only inserting accessor: its private insert helper 0xa721c0 is called once.
    require(call_sites(INSERT_HELPER) == [0xA71F8A],
            f"insert helper call sites {[hex(a) for a in call_sites(INSERT_HELPER)]}")
    require(0x5157D4 in call_sites(LOOKUP), "0x5157d4 is not a call to 0xa71c90")
    require(0x516130 in call_sites(FIND_ONLY),
            "0x516130 is not a call to the read-only accessor")
    require(fdes.find(0x516126)[0] == FIND_ONLY_CALLER,
            "0x516126 is not inside the read-only accessor")

    # 11. optional cross-check against the pinned disassembly
    dis_notes = []
    if DIS.is_file():
        lines = []
        for line in open(DIS, "r", encoding="utf-8", errors="replace"):
            if "0x1017142 <" in line or "0x15feda0 <" in line:
                lines.append(line.strip())
        name_sites = [line for line in lines if "0x1017142 <" in line]
        trampoline_sites = [line for line in lines if "0x15feda0 <" in line]
        name_refs = sorted(int(site.split(":", 1)[0].strip(), 16)
                           for site in name_sites if "leaq" in site)
        require(name_refs == [0x54319C, 0x543385, 0x5433F8, 0x544942],
                f"chunk-name leaq sites {[hex(a) for a in name_refs]}")
        require(all("vmovq" in site for site in trampoline_sites),
                "0x15feda0 is stored to, not only loaded from")
        map_refs = set()
        for line in open(DIS, "r", encoding="utf-8", errors="replace"):
            if "0x10210" not in line:
                continue
            head = line.split(":", 1)[0].strip()
            try:
                address = int(head, 16)
            except ValueError:
                continue
            owner = fdes.find(address)
            if owner is not None:
                map_refs.add(owner[0])
        require(map_refs <= MAP_OWNERS,
                f"unexpected functions touch the resource map: "
                f"{sorted(hex(a) for a in map_refs - MAP_OWNERS)}")
        dis_notes.append(f"all.dis: {len(name_refs)} name leaq sites, "
                         f"{len(trampoline_sites)} trampoline loads, "
                         f"{len(map_refs)} functions touch the map")
    else:
        dis_notes.append("all.dis absent: uniqueness cross-check skipped")

    chain = {
        "schema": "sotc-sav-load-dispatch/1",
        "task": "SAV-01-LOAD-DISPATCH-5435FC-01",
        "inputs": {"elf_sha256": ELF_SHA256},
        "function": {"start": f"0x{LOADER:x}", "size": LOADER_SIZE},
        "call_site": f"0x{CALL_SITE:x}",
        "control_block": {"source": f"0x{PRODUCER:x} sret +8",
                          "address_point": f"0x{CONTROL_ADDRESS_POINT:x}",
                          "slot": "0x10"},
        "target": {"address": f"0x{TARGET:x}", "semantics": "_M_dispose (shared_ptr control block)",
                   "fde": {"start": f"0x{TARGET:x}", "size": TARGET_SIZE}},
        "second_call": {"address": f"0x{SECOND_CALL:x}", "target": f"0x{DESTROY_TARGET:x}",
                        "semantics": "_M_destroy"},
        "chunk_name": CHUNK_NAME_TEXT[:-1].decode(),
        "notes": dis_notes,
    }
    if args.json is not None:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(json.dumps(chain, indent=2, sort_keys=True) + "\n")
        print(f"wrote {args.json}")

    print(f"OK check_sav_load_dispatch: FUN_00542880 0x{LOADER:x}+0x{LOADER_SIZE:x}; "
          f"0x{CALL_SITE:x} call *0x10 -> 0x{TARGET:x} (_M_dispose, {TARGET_SIZE} B); "
          f"0x{SECOND_CALL:x} call *0x18 -> 0x{DESTROY_TARGET:x} (_M_destroy)")
    print("   chain: sret(0x515770) -> map 'CLCT' find-or-create(0xa71c90) -> factory 0x516230 -> "
          "vtable 0x15c6f68 slot +0x10")
    print("STATIC_ANALYSIS_VERIFIED only: nothing was executed; bytes, relocations and FDE extents.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
