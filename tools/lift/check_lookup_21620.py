#!/usr/bin/env python3
"""Byte-oracle for `0x21620` = `bpe::ConCmdManager::FindConCmd` (CPU-01-INIT-21620-LOOKUP-01).

Offline only: reads `local/re/eboot.elf`, never loads, relocates or executes an original RVA. The
native model (`native/core/lookup_21620.{hpp,cpp}`) reproduces the control flow over caller-supplied
collections; nothing here is a runtime, boot, audio, save or engine-readiness claim.

What this checker pins, and why:

* **Whole body, not 449 bytes.** The spec's stop rule 5 forbids a checker that pins only the hot span.
  Pinned here: the 449-byte hot span (sha256), the full 464-byte `0x21620..0x217f0` range (sha256), the
  FDE start/length (463, `0x21620..0x217ef`), the `ret` at `0x217e0`, the reachable **cold block**
  `0x217e1..0x217e8` together with both of its edges (`0x216cb` jumps into it; its own tail jump
  `0x217e4` re-enters the body at `0x216dc`), the **stack-guard tail** `0x217e9..0x217ef`
  (`call __stack_chk_fail` + two nops), and the fact that the next FDE starts exactly at `0x217f0`
  (`ExecuteCmd`, `55 48 89 e5`).
* **Both paths.** `[0x2dfcbc8]` (manager cell) selects the locked map path or the guarded static
  pending-list path; every branch of the 19 control transfers is pinned by target.
* **Call census.** Nine relative calls to six targets. A raw `e8` scan finds ten bytes here because
  the rel8 operand of `jne 0x216b3` at `0x216c9` is itself `0xe8`; the checker therefore asserts the
  decoded call sites *and* explains the extra byte instead of pretending a raw scan is a census.
* **Lock lifetime.** `0x3b390` (= `jmp scePthreadMutexLock`) is called once on `manager+0x58` and
  `0x3b3c0` (= `jmp scePthreadMutexUnlock`) on every primary exit (hit, miss, and both tail jumps);
  the fallback path takes no lock. The returned value is read under the lock (`0x216fa` precedes
  `0x21701`).
* **Comparator.** The import at `0xf8f500` is `strcasecmp` (NID AV6ipCNa4Rw, libc), so ordering and
  equality are case-insensitive; `0x216c1` (`jns` -> descend right) and `0x216f3` (`js` -> miss) fix
  the polarity, and `0x21790` (`cmpq %r14,0x28(%rbx)`) makes the pending-list key length-sensitive
  while the map path never reads the length.
* **Key descriptor at both call sites.** `0x53327d` builds `{data = "r_fps", size = 5}` on the stack
  (`0x53320e`, `0x53323b`, `0x533257`); `0x91227` passes `this+8`, whose static initialiser stores
  `{data = "shadowDemoMode", size = 14, aux = 0, ctl = 0}` at `0x2efe320` (`0x538953`, `0x53895a`,
  `0x53899d`, `0x5389a5`). The second qword is the name length at both sites.
* **Identity.** The debug strings the body loads into its lock assertion name the function
  (`0xf95d1f`), its file (`0xf95c06`), the writer of both collections (`0xf95c3e`) and the next FDE
  (`0xf95d69`).
* **Native contract + honest limits.** Constant pins, the critsec/pending-list wiring, the CMake
  entries, the fixture's "synthetic only" marker, and the requirement that the header keeps its
  NOT CLAIMED section.

A mutation self-test flips the first and last byte of every recorded pin and requires every mutant to
be rejected.
"""

from __future__ import annotations

import importlib.util
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools/lift"))
from slot_pin_util import Elf, const_value, digest, require  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
FDE = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"

BODY_RVA = 0x21620
HOT_LEN = 449
BODY_LEN = 0x1D0
FDE_LEN = 463
HOT_SHA256 = "787bf4ada236589505be9d753be8317445d253e8f43a2db55f102b0cf77b5185"
BODY_SHA256 = "a7ba043b102574823a8381f36c654b1bf30f6ef812b64e209e9b438d670413bd"

MANAGER_CELL = 0x2DFCBC8
PENDING_HEAD = 0x2DFCE50
PENDING_SENTINEL = 0x2DFCE48
PENDING_GUARD = 0x2DFCE60

# (site, target) in address order. Nine calls; the tenth `e8` byte is the jne operand at 0x216c9.
CALLS = (
    (0x21689, 0x3B390),   # scePthreadMutexLock(manager+0x58, &{file,func,line 0x9b})
    (0x216BA, 0xF8F500),  # strcasecmp(node+0x20, key->data)   (descent)
    (0x216EC, 0xF8F500),  # strcasecmp(key->data, candidate+0x20)  (equality)
    (0x21701, 0x3B3C0),   # scePthreadMutexUnlock  (hit)
    (0x2170E, 0x3B3C0),   # scePthreadMutexUnlock  (miss)
    (0x2172C, 0xF8F3D0),  # __cxa_guard_acquire(0x2dfce60)
    (0x21755, 0xF8F3E0),  # __cxa_guard_release(0x2dfce60)
    (0x2179D, 0xF8F500),  # strcasecmp(entry+0x20, key->data)
    (0x217E9, 0xF8F3A0),  # __stack_chk_fail (canary tail)
)

# Every branch and jump in the body: (site, opcode bytes, target). The opcode is pinned exactly, so a
# mutant that turns `je` into `jne` cannot hide behind a permissive decoder.
BRANCHES = (
    (0x21650, "0f84", 0x2171B),  # je   -> fallback: no manager
    (0x2169F, "74", 0x216DC),    # je   -> empty map
    (0x216C1, "79", 0x216D0),    # jns  -> candidate, descend right
    (0x216C9, "75", 0x216B3),    # jne  -> left child
    (0x216DA, "75", 0x216B0),    # jne  -> right child
    (0x216DF, "74", 0x2170B),    # je   -> miss (candidate == end)
    (0x216F3, "78", 0x2170B),    # js   -> miss (key < candidate)
    (0x216F8, "74", 0x2170B),    # je   -> miss (redundant end check)
    (0x21723, "75", 0x2175A),    # jne  -> guard already released
    (0x21733, "74", 0x2175A),    # je   -> guard_acquire returned 0
    (0x21779, "74", 0x217BF),    # je   -> empty pending list
    (0x21794, "75", 0x217A6),    # jne  -> size mismatch, next element
    (0x217A4, "74", 0x217BC),    # je   -> hit
    (0x217B8, "75", 0x21790),    # jne  -> scan loop
    (0x217CD, "75", 0x217E9),    # jne  -> canary mismatch tail
)

# Relative jumps (e9) of the body.
JUMPS = (
    (0x216CB, "e9", 0x217E1),  # left-subtree exhausted -> the COLD BLOCK
    (0x21706, "e9", 0x217BF),  # hit tail
    (0x21716, "e9", 0x217BF),  # miss tail
    (0x217E4, "e9", 0x216DC),  # cold block re-enters the verify step
)

HDR = ROOT / "native/core/lookup_21620.hpp"
SRC = ROOT / "native/core/lookup_21620.cpp"
TEST = ROOT / "native/tests/test_lookup_21620.cpp"
PEER = ROOT / "native/core/vslot_40_533100_miss.cpp"
CMAKE = ROOT / "native/CMakeLists.txt"


def load_fdes():
    spec = importlib.util.spec_from_file_location("cfg_census", ROOT / "tools/lift/cfg_census.py")
    census = importlib.util.module_from_spec(spec)
    sys.modules["cfg_census"] = census
    spec.loader.exec_module(census)
    ranges, _duplicates = census.load_fde_ranges(FDE)
    return ranges


def branch_target(e: Elf, site: int, opcode_hex: str) -> int:
    """Target of one pinned branch: the opcode bytes are compared first, then the displacement."""
    opcode = bytes.fromhex(opcode_hex)
    got = e.read(site, len(opcode))
    require(got == opcode, f"0x{site:x} opcode {got.hex()} != {opcode_hex}")
    if opcode_hex == "e9":
        return site + 5 + struct.unpack("<i", e.read(site + 1, 4))[0]
    if opcode_hex == "0f84":
        return site + 6 + struct.unpack("<i", e.read(site + 2, 4))[0]
    rel = e.read(site + 1, 1)[0]
    return site + 2 + struct.unpack("<b", bytes([rel]))[0]


def elf_pins(e: Elf) -> None:
    """Every byte-level statement about the image. Called again under mutation overlays."""
    body = e.read(BODY_RVA, BODY_LEN)
    hot = e.read(BODY_RVA, HOT_LEN)
    if not e.skip_hash:
        require(digest(hot) == HOT_SHA256, "0x21620 hot span (449 B) sha256")
        require(digest(body) == BODY_SHA256, "0x21620 whole body (464 B) sha256")

    # ---- boundary: FDE, cold block, stack-guard tail, next function -------------------------------
    found = load_fdes().find(BODY_RVA)
    require(found is not None and found[0] == BODY_RVA, "0x21620 is not an FDE start")
    require(found[1] == FDE_LEN,
            f"0x21620 FDE length {found[1]} != {FDE_LEN} (the 464 B pin adds the pad nop at 0x217ef)")
    require(body[0x217E0 - BODY_RVA] == 0xC3, "retq at 0x217e0")
    e.bytes_at(0x217E1, "4d89eee9f3feffff", "cold block: mov r13->r14 then jmp back into the body")
    e.bytes_at(0x217E9, "e8b2dbf6009090", "tail: call __stack_chk_fail + two nops")
    require(e.read(0x217F0, 4) == bytes.fromhex("554889e5"), "0x217f0 starts the next function")
    next_fde = load_fdes().find(0x217F0)
    require(next_fde is not None and next_fde[0] == 0x217F0, "0x217f0 is the next FDE start")

    # ---- call census + control flow ---------------------------------------------------------------
    for site, target in CALLS:
        require(e.call(site) == target, f"0x{site:x} call target (want 0x{target:x})")
    decoded = {site for site, _target in CALLS}
    raw_e8 = {BODY_RVA + i for i, byte in enumerate(body) if byte == 0xE8}
    require(raw_e8 - decoded == {0x216CA},
            f"unexpected call-like e8 bytes: {sorted(hex(x) for x in raw_e8 - decoded)}")
    e.bytes_at(0x216C9, "75e8", "the extra e8 byte is the rel8 operand of jne 0x216b3, not a call")
    for site, opcode, target in BRANCHES + JUMPS:
        require(branch_target(e, site, opcode) == target, f"0x{site:x} branch target (want 0x{target:x})")
    require(len(BRANCHES) + len(JUMPS) + len(CALLS) == 9 + 15 + 4, "transfer census")

    # ---- prologue / epilogue: no byte of the pinned body is covered only by the digest -----------
    e.bytes_at(0x21620, "554889e5" "4157415641554154" "53" "4883ec38", "prologue through sub rsp,0x38")
    e.bytes_at(0x21638, "48897da8", "0x21638: spill the key descriptor")
    e.bytes_at(0x21643, "488b00", "0x21643: load the stack-guard canary")
    e.bytes_at(0x21646, "488945d0" "4c8b23" "4d85e4", "0x21646: save the canary, load the manager cell")
    e.bytes_at(0x217BF, "488b05", "0x217bf: re-read the stack guard")
    e.bytes_at(0x217C6, "488b00" "483b45d0", "0x217c6: compare the canary")
    e.bytes_at(0x217CF, "4c89f8" "4883c438" "5b" "415c" "415d" "415e" "415f" "5d",
               "0x217cf: return-value move + full epilogue")

    # ---- the two paths, and the shared tail ------------------------------------------------------
    require(e.lea(0x2163C) == MANAGER_CELL, "0x2163c leas the manager cell 0x2dfcbc8")
    e.bytes_at(0x21664, "4983c458", "0x21664: the lock address is manager+0x58")
    e.bytes_at(0x21691, "498b4718", "0x21691: root = [manager+0x18]")
    e.bytes_at(0x21695, "4983c708", "0x21695: end = manager+8")
    e.bytes_at(0x216B3, "498b7e20", "0x216b3: node name at +0x20")
    e.bytes_at(0x216FA, "4d8b7e40", "0x216fa: the value is read at node+0x40, under the lock")
    e.bytes_at(0x21790, "4c397328", "0x21790: pending key = (entry+0x28 == key->size)")
    e.bytes_at(0x21796, "488b7b20", "0x21796: pending entry name at +0x20")
    e.bytes_at(0x217AA, "488d58f8", "0x217aa: element = link - 8")
    e.bytes_at(0x217AE, "4885c0", "0x217ae: test the link")
    e.bytes_at(0x217B1, "480f44d8", "0x217b1: a null link collapses the -8 (cmoveq)")
    require(e.read(0x2175A, 3) == bytes.fromhex("488b05"), "0x2175a: `mov [0x2dfce50],%rax`")
    require(e.rip(0x2175A, 7, 3) == PENDING_HEAD, "0x2175a reads the pending head 0x2dfce50")
    require(e.lea(0x21761) == PENDING_SENTINEL, "0x21761 leas the sentinel 0x2dfce48")
    require(e.lea(0x21735) == PENDING_HEAD, "0x21735 initialises [0x2dfce50]")
    require(e.rip(0x2174D, 8, 4) == PENDING_HEAD, "0x2174d stores the self-links into 0x2dfce50")
    e.bytes_at(0x2174D, "c5fa7f05", "0x2174d is the `vmovdqu [rip+..]` self-link store")
    require(e.lea(0x21725) == PENDING_GUARD and e.lea(0x2173C) == PENDING_GUARD,
            "0x21725/0x2173c use the guard byte 0x2dfce60")
    require(e.read(0x2171B, 2) == bytes.fromhex("8a05"), "0x2171b is `movb [rip+..],%al`")
    require(e.rip(0x2171B, 6, 2) == PENDING_GUARD, "0x2171b tests the guard byte 0x2dfce60")

    # ---- lock debug metadata, stack guard, assert line -------------------------------------------
    e.bytes_at(0x21682, "c745c09b000000", "0x21682: assert line 0x9b = 155")
    require(e.lea(0x21656) == 0xF95D1F and e.lea(0x2165D) == 0xF95C06,
            "0x21656/0x2165d build the lock's {function, file} descriptor")
    e.bytes_at(0x21631, "488b05", "0x21631 loads the stack-guard GOT slot")
    require(e.rip(0x21631, 7, 3) == 0x15FE518, "0x21631 reads the stack guard at 0x15fe518")
    require(e.rip(0x217BF, 7, 3) == 0x15FE518, "0x217bf re-reads the stack guard")

    # ---- identity strings the body itself loads --------------------------------------------------
    e.cstring(0xF95D1F, b"static bpe::IConBase *bpe::ConCmdManager::FindConCmd(const bpe::String &)")
    e.cstring(0xF95C06, b"d:\\dev\\cp10\\Main\\Code\\BPE\\Source\\Core\\System\\ConVar.cpp")
    e.cstring(0xF95C3E, b"static void bpe::ConCmdManager::Register(bpe::IConBase *, bpe::ConCmdManager *)")
    e.cstring(0xF95D69, b"bpe::ECmdExecuteResult bpe::ConCmdManager::ExecuteCmd(const bpe::Command &)")

    # ---- call site A: 0x53327d, stack key {"r_fps", 5} -------------------------------------------
    e.cstring(0x1014E56, b"r_fps")
    require(e.lea(0x53320E) == 0x1014E56, "0x53320e leas \"r_fps\" (0x1014e56)")
    require(e.call(0x53327D) == BODY_RVA, "0x53327d calls 0x21620")
    e.bytes_at(0x533238, "4c89ff", "0x533238: rdi is the stack key")
    e.bytes_at(0x53323B, "48c785b8feffff05000000", "0x53323b stores the pinned length 5 at key+0x08")
    e.bytes_at(0x533257, "c5fa7f8dd8feffff", "0x533257 stores {name, 5} into the key block")

    # ---- call site B: 0x91227, key = this+8 = 0x2efe320 {"shadowDemoMode", 14, 0, 0} -------------
    e.cstring(0x1014CA3, b"shadowDemoMode")
    require(e.lea(0x538953) == 0x1014CA3, "0x538953 leas \"shadowDemoMode\"")
    e.bytes_at(0x53895A, "b80e000000", "0x53895a loads the pinned length 0xe = 14")
    e.bytes_at(0x53899D, "c5fa7f05", "0x53899d is the key data/size store")
    require(e.rip(0x53899D, 8, 4) == 0x2EFE320, "0x53899d writes the key at 0x2efe320 (= this+8)")
    e.bytes_at(0x5389A5, "c5f81115", "0x5389a5 zeroes the key's aux/ctl (16 B)")
    require(e.rip(0x5389A5, 8, 4) == 0x2EFE330, "0x5389a5 zeroes key+0x10")
    e.bytes_at(0x91223, "488d7b08", "0x91223: rdi = this+8")
    require(e.call(0x91227) == BODY_RVA, "0x91227 calls 0x21620")

    # ---- the lock/unlock thunks the body calls are the reviewed ones -----------------------------
    require(e.jmp(0x3B390) == 0xF8F8E0, "0x3b390 tail-jumps to scePthreadMutexLock")
    require(e.jmp(0x3B3C0) == 0xF8F900, "0x3b3c0 tail-jumps to scePthreadMutexUnlock")
    lock_fde = load_fdes().find(0x3B390)
    unlock_fde = load_fdes().find(0x3B3C0)
    require(lock_fde == (0x3B390, 5), f"0x3b390 FDE {lock_fde}")
    require(unlock_fde == (0x3B3C0, 5), f"0x3b3c0 FDE {unlock_fde}")


def native_pins() -> None:
    hdr = HDR.read_text()
    src = SRC.read_text()
    test = TEST.read_text()
    peer = PEER.read_text()
    cmake = CMAKE.read_text()

    require("FindConCmd" in hdr and "0x21620" in hdr, "header must name the function and the RVA")
    require(const_value(hdr, "kLookup21620Rva") == BODY_RVA, "RVA pin")
    require(const_value(hdr, "kLookup21620HotSize") == HOT_LEN, "449-byte hot-span pin")
    require(const_value(hdr, "kLookup21620BodySize") == BODY_LEN, "464-byte body pin")
    require(const_value(hdr, "kLookup21620FdeLength") == FDE_LEN, "FDE-length pin")
    require(HOT_SHA256 in hdr and BODY_SHA256 in hdr, "both body digests must stay in the header")
    for name, want in (("kLookup21620ManagerRootOffset", 0x18), ("kLookup21620ManagerEndOffset", 0x08),
                       ("kLookup21620ManagerMutexOffset", 0x58), ("kLookup21620NodeLeftOffset", 0x00),
                       ("kLookup21620NodeRightOffset", 0x08), ("kLookup21620NodeKeyOffset", 0x20),
                       ("kLookup21620NodeValueOffset", 0x40), ("kLookup21620NodeBytes", 0x48),
                       ("kLookup21620PendingNameOffset", 0x20), ("kLookup21620PendingSizeOffset", 0x28)):
        require(const_value(hdr, name) == want, f"offset pin {name}")
    require(const_value(hdr, "kLookup21620ManagerBytes") == 0x60, "manager size pin")
    require(const_value(hdr, "kLookup21620PendingHeadRva") == PENDING_HEAD, "pending head pin")
    require(const_value(hdr, "kLookup21620PendingSentinelRva") == PENDING_SENTINEL, "sentinel pin")
    require(const_value(hdr, "kLookup21620PendingGuardRva") == PENDING_GUARD, "guard pin")
    require(const_value(hdr, "kLookup21620CompareRva") == 0xF8F500, "comparator import pin")
    require("strcasecmp" in hdr, "the comparator's identity must be stated")
    require(const_value(hdr, "kLookup21620LockBeginRva") == 0x3B390, "lock thunk pin")
    require(const_value(hdr, "kLookup21620LockEndRva") == 0x3B3C0, "unlock thunk pin")
    require("NOT CLAIMED" in hdr and "engine readiness" in hdr,
            "the header must keep its explicit non-claims")
    require("HOST-ONLY ADDITIONS" in hdr, "the header must label its host-only additions")
    require("shadowDemoMode" in hdr and "r_fps" in hdr, "the pinned call-site strings must be cited")

    require("critsec_lock_at" in src and "critsec_unlock_at" in src,
            "the manager path must use the reviewed CritSec lock/unlock")
    require("ensure_concmd_pending_list" in src, "the fallback path must share the guarded init")
    require("load_shared_str" in src, "the adapters must read the descriptor out of caller memory")
    require("concmd_strcasecmp" in src, "the native comparator must stay reachable")
    require("0x21620" in src and "reinterpret_cast<std::uintptr_t>(0x21620" not in src,
            "the RVA may only appear as an identity tag")
    require("kLookup21620PendingGuardRva" in hdr and "kLookup21620PendingSentinelRva" in hdr,
            "the fallback globals must stay documented as identity tags")

    # Fixture honesty: synthetic names only, and no fixture claim about the retail strings.
    require("SYNTHETIC ONLY" in test, "the fixture must state that its registry is synthetic")
    require('"shadowDemoMode"' not in test and 'key_of("r_fps"' not in test,
            "the fixture must not look up either pinned retail name")
    require('"demo_alpha"' in test, "the fixture must use an invented name")
    require("find_concmd_descriptor_hosted" in test and "find_concmd_descriptor" in test,
            "the fixture must drive both adapters")
    require("run_lazy_bind_91210" in test and "vslot_40_533100_miss" in test,
            "the fixture must wire both retail call sites")
    require("reject_lock_with" in test, "the fixture must inject a lock refusal")

    require("g_name_obj + 0x08" in peer,
            "the vslot stand-in must carry the pinned key length at +0x08 (0x53323b)")

    for needle in ("core/lookup_21620.cpp", "tests/test_lookup_21620.cpp", "test_lookup_21620"):
        require(needle in cmake, f"cmake missing {needle}")


def mutation_self_test(e: Elf) -> int:
    e.skip_hash = True
    e.recording = True
    e.pins = set()
    elf_pins(e)
    e.recording = False
    survivors = 0
    mutants = 0
    for addr, size in sorted(e.pins):
        for byte_addr in sorted({addr, addr + size - 1}):
            mutants += 1
            e.overlay = {}  # pristine value first: never compute a mutant from a mutated byte
            e.overlay = {byte_addr: e.raw(byte_addr, 1)[0] ^ 0x01}
            try:
                elf_pins(e)
            except SystemExit:
                pass
            else:
                survivors += 1
                print(f"mutation survived at 0x{byte_addr:x} (pin 0x{addr:x}+{size})", file=sys.stderr)
    e.overlay = {}
    e.skip_hash = False
    require(survivors == 0, f"{survivors} ELF mutations survived")
    return mutants


def main() -> int:
    # ---- native contract (always checked, even without the private image) ------------------------
    native_pins()

    if not (ROOT / "local/re/eboot.elf").is_file() or not FDE.is_file():
        print("SKIP check_lookup_21620: private image not present (module wiring checked)")
        return 0

    raw = (ROOT / "local/re/eboot.elf").read_bytes()
    require(digest(raw) == ELF_SHA256, "unexpected ELF SHA256")

    e = Elf()
    e.recording = True
    elf_pins(e)
    mutants = mutation_self_test(e)

    print("OK check_lookup_21620: 0x21620 464 B body (FDE 463) incl. cold block 0x217e1 and canary tail "
          "0x217e9, 9 calls / 6 targets, manager+0x58 lock bracket, strcasecmp polarity, "
          f"length-sensitive pending list, both call-site keys; {mutants} mutants rejected. "
          "No lookup result, engine bind or boot behaviour is proven.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
