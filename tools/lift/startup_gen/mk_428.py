#!/usr/bin/env python3
"""Generator for native/core/ctor_slot_428_52ec10.hpp (CPU-01-AOT-CFG-SCALE-01-BATCH19).

Reads the pinned private ELF through tools/lift/slot428_trace.py (the byte-level tracer + block parser) and writes the table-driven header of startup slot
428. The same tracer is re-run by tools/lift/check_ctor_slot_428.py, which requires the native tables to equal the ELF replay record for record.

The four 256-character hexadecimal strings of phase A look like per-title secrets (they are paired with a region name and a CUSA title id). They are WITHHELD:
the pool keeps the original rva and the original length (0x100) but the native copy is 256 x 'X'; the checker ties them to the ELF by SHA-256 only.
"""
from __future__ import annotations

import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/lift"))
from slot_pin_util import Elf  # noqa: E402
import slot428_trace as tr  # noqa: E402

BASE, END = 0x2EFD610, 0x2EFD9A0
WITHHELD = {0x1013D51, 0x1013E66, 0x1013F88, 0x101409C}


def cstr(e, a):
    b = e.raw(a, 512)
    n = b.index(0)
    return b[:n]


def lit(b: bytes) -> str:
    s = b.decode("ascii")
    assert all(0x20 <= ord(c) < 0x7F for c in s) and '"' not in s and "\\" not in s and "?" not in s, s
    return s


def objdump_agreement(e, m) -> str:
    """Cross-check the closed byte decoder against the platform objdump (an independent decoder): every instruction start between the prologue and the
    guard epilogue must be one objdump instruction start, with equal lengths. Skipped (reported) when objdump is unavailable."""
    import re
    import shutil
    import subprocess
    exe = shutil.which("objdump")
    if exe is None:
        return "objdump unavailable: agreement NOT RUN"
    out = subprocess.run([exe, "-d", "--no-show-raw-insn", f"--start-address={tr.START:#x}", f"--stop-address={tr.END:#x}", str(ROOT / "local/re/eboot.elf")],
                         capture_output=True, text=True, check=False).stdout
    starts = [int(x, 16) for x in re.findall(r"^\s+([0-9a-f]+):\s+\S", out, re.M)]
    mine = [a for a, _l, _m in m["trace"].insns]
    assert starts == mine, f"objdump and the tracer disagree: {len(starts)} vs {len(mine)} instruction starts"
    return f"objdump agrees on {len(mine)} instruction starts"


def main() -> None:
    e = Elf()
    m = tr.parse428(e.raw)
    print(objdump_agreement(e, m))
    st = m["stores"]
    lo = min(s["target"] for s in st)
    hi = max(s["target"] + s["width"] for s in st)
    assert (lo, hi) == (BASE, END), (hex(lo), hex(hi))
    pool_rva = sorted({l[1] for s in st for l in s["lanes"] if l[0] == "a"})
    pool = []
    for a in pool_rva:
        t = cstr(e, a)
        if a in WITHHELD:
            assert len(t) == 256 and all(chr(c) in "0123456789abcdef" for c in t)
            pool.append(b"X" * 256)
        else:
            pool.append(t)
    offs, pos = [], 0
    for t in pool:
        offs.append(pos)
        pos += len(t) + 1
    recs = []
    for s in st:
        kinds, vals = [], []
        for l in s["lanes"]:
            if l[0] == "c":
                kinds.append(0)
                vals.append(l[1])
            else:
                kinds.append(2)
                vals.append(pool_rva.index(l[1]))
        if s["width"] != 16:
            kinds.append(0)
            vals.append(0)
        recs.append(f"    {{{s['insn']:#x}, {s['target'] - BASE:#x}, {s['width']}, {kinds[0]}, {kinds[1]}, 0, {vals[0]:#x}, {vals[1]:#x}}},")
    # contiguity of the typed blocks
    assert [r["addr"] for r in m["recs"]] == [0x2EFD9A0 + 0x58 * i for i in range(5)]
    assert [r["addr"] for r in m["ids"]] == [0x2EFDB58 + 0x28 * i for i in range(37)]
    assert [r["addr"] for r in m["avails"]] == [0x2EFE120 + 0x48 * i for i in range(6)]
    assert [r["id"] for r in m["ids"]] == list(range(1, 38))
    for r in m["recs"]:
        assert r["name_len"] == len(cstr(e, r["name"]))
    for r in m["ids"]:
        assert r["text_len"] == len(cstr(e, r["text"]))
    for r in m["avails"]:
        assert r["a_len"] == len(cstr(e, r["a"])) and r["b_len"] == len(cstr(e, r["b"]))
    save_rows = "\n".join(
        f"    {{{r['id']}, \"{lit(cstr(e, r['name']))}\", {r['name_len']}, {r['flag']}, {{" + ", ".join(f"\"{lit(cstr(e, h))}\"" for h in r["hashes"]) + "}},"
        for r in m["recs"])
    id_rows = "\n".join(f"    {{{r['id']}, \"{lit(cstr(e, r['text']))}\", {r['text_len']}}}," for r in m["ids"])
    av_rows = "\n".join(f"    {{\"{lit(cstr(e, r['a']))}\", {r['a_len']}, \"{lit(cstr(e, r['b']))}\", {r['b_len']}}}," for r in m["avails"])
    blob = "\n".join(f'    "{lit(t)}\\0"' for t in pool)
    text = HEADER.format(
        nstores=len(st), nstrings=len(pool), recs="\n".join(recs), rvas=", ".join(f"{a:#x}" for a in pool_rva),
        offs=", ".join(f"{o:#x}" for o in offs), blob=blob, blobsize=pos + 1, save_rows=save_rows, id_rows=id_rows, av_rows=av_rows)
    (ROOT / "native/core/ctor_slot_428_52ec10.hpp").write_text(text)
    print("wrote", len(st), "stores,", len(pool), "pooled strings,", len(m["recs"]), len(m["ids"]), len(m["avails"]))


HEADER = r'''// CPU-01-AOT-CFG-SCALE-01-BATCH19: native model of startup constructor slot 428 (RVA 0x52ec10, 5375 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 428. Generated from the ELF by tools/lift/startup_gen/mk_428.py (which runs the byte tracer tools/lift/slot428_trace.py)
// and tied back to it record for record by tools/lift/check_ctor_slot_428.py. NOTHING here executes, loads or calls an original RVA: the four destructor RVAs
// are IDENTITY TAGS, recorded in the atexit log and never run.
//
// Shape (a straight-line body, 49 __cxa_atexit registrations in this order; one branch only, the stack-guard epilogue):
//   A. a 0x390 B static object (0x2efd610..0x2efd9a0) written by 98 stores, then atexit(0x52e980, null). The stores are replayed from an ELF-tied record
//      table (core/store_table). INFERENCE ONLY: seven 0x80 B records of three string views and numeric fields (a region name, a CUSA title id, a
//      256-character hexadecimal string) for the four region / title pairs. The four 256-character strings look like per-title credentials: they are
//      WITHHELD from this repository (the native copy is 256 'X'; the checker ties the ELF text by SHA-256 only). Nothing reads them natively.
//   B. five 0x58 B save descriptors at 0x2efd9a0 (stride 0x58): {{u32 id, never-written +4, SharedStr name view at +8, four FNV hashes at +0x28 of the
//      SAVEINFO_* names (0xa2ddc0 -> native fnv_cstr_a2ddc0), u32 flag at +0x38, 0x18 zero bytes}}; atexit(0x52eaa0, record) after each.
//   C. 37 id + name entries (0xa59620, native register_id_entry_a59620) at 0x2efdb58 (stride 0x28), ids 1..37, each source a borrowed stack view released
//      through the inert 0xf590, atexit(0x52eb00, entry) after each. INFERENCE ONLY: an achievement-like table (id -> display name).
//   D. 6 availability entries (0xa42960, native register_avail_entry_a42960) at 0x2efe120 (stride 0x48), each with a formatted-name bool variable
//      "entitlement_available_<id>", two borrowed views released (b then a), atexit(0x52eb10, entry) after each. INFERENCE ONLY: add-on item ids -> names.
//   The constants, strings and ids below are the OBSERVED data; what consumes the objects is NOT established.
//
// Allocation: phases C and D allocate (pointer-vector doubling, the 0xc8 B variables and their owned names). The original goes through the engine allocator
// 0x12820 / 0x12af0 (not modelled); the host contract is Slot428Objects::alloc (default_slot428_alloc(): malloc / free, like default_slot513_record_alloc) and
// the name deallocator. A missing contract or a failed allocation is reported (ok == false) and stops the slot at that step; nothing is faked.
#pragma once

#include <cstddef>
#include <cstdint>

#include "alloc_wrappers_17070.hpp"
#include "concmd_base.hpp"
#include "fnv_path_cstr_162020.hpp"
#include "hashed_string_14c3a0.hpp"
#include "helper_a42960_avail_entry.hpp"
#include "helper_a59620_id_entry.hpp"
#include "shared_str_ctor_f1a0.hpp"
#include "shared_str_release_f590.hpp"
#include "store_table.hpp"

namespace sotc {{

inline constexpr std::uint64_t kSlot428Rva = 0x52ec10;
inline constexpr std::size_t kSlot428Size = 5375;
inline constexpr std::uint64_t kSlot428StaticsRva = 0x2efd610;
inline constexpr std::size_t kSlot428StaticsSize = 0x390;
inline constexpr std::size_t kSlot428StoreCount = {nstores};
inline constexpr std::size_t kSlot428StoreStringCount = {nstrings};
inline constexpr std::uint64_t kSlot428StoreDtorTag = 0x52e980;
inline constexpr std::uint64_t kSlot428SaveRecordRva = 0x2efd9a0;
inline constexpr std::size_t kSlot428SaveRecordStride = 0x58;
inline constexpr std::size_t kSlot428SaveRecordCount = 5;
inline constexpr std::uint64_t kSlot428SaveDtorTag = 0x52eaa0;
inline constexpr std::uint64_t kSlot428IdEntryRva = 0x2efdb58;
inline constexpr std::size_t kSlot428IdEntryCount = 37;
inline constexpr std::uint64_t kSlot428IdDtorTag = 0x52eb00;
inline constexpr std::uint64_t kSlot428AvailEntryRva = 0x2efe120;
inline constexpr std::size_t kSlot428AvailEntryCount = 6;
inline constexpr std::uint64_t kSlot428AvailDtorTag = 0x52eb10;
inline constexpr std::size_t kSlot428AtexitCount = 49;
inline constexpr std::size_t kSlot428WithheldLength = 256;

struct Slot428Statics {{
    alignas(8) std::uint8_t bytes[kSlot428StaticsSize];
}};
static_assert(sizeof(Slot428Statics) == kSlot428StaticsSize);

// The 0x58 B save descriptor of phase B. Bytes +4..+7 and +0x3c..+0x3f are never written by the original (zero in .bss).
struct Slot428SaveRecord {{
    std::uint32_t id;            // +0x00
    std::uint8_t gap04[4];       // never written
    SharedStr name;              // +0x08: borrowed view {{data, size, 0, null}}
    std::uint32_t hash[4];       // +0x28: fnv_cstr_a2ddc0 of the four SAVEINFO_ names
    std::uint32_t flag;          // +0x38
    std::uint8_t gap3c[4];       // never written
    std::uint8_t zero40[16];     // +0x40 written as zero
    std::uint64_t zero50;        // +0x50 written as zero
}};
static_assert(sizeof(Slot428SaveRecord) == kSlot428SaveRecordStride && offsetof(Slot428SaveRecord, name) == 8 && offsetof(Slot428SaveRecord, hash) == 0x28 &&
              offsetof(Slot428SaveRecord, flag) == 0x38 && offsetof(Slot428SaveRecord, zero40) == 0x40 && offsetof(Slot428SaveRecord, zero50) == 0x50);

struct Slot428SaveSpec {{
    std::uint32_t id;
    const char* name;
    std::uint64_t name_length;
    std::uint32_t flag;
    const char* hash_text[4];
}};
struct Slot428IdSpec {{
    std::uint32_t id;
    const char* text;
    std::uint64_t text_length;
}};
struct Slot428AvailSpec {{
    const char* a;
    std::uint64_t a_length;
    const char* b;
    std::uint64_t b_length;
}};

// Phase A store table (lane kinds: 0 constant, 2 string pool index). The four withheld pool strings keep the original rva and length; their text is 'X'.
inline constexpr StoreTableRecord kSlot428Stores[kSlot428StoreCount] = {{
{recs}
}};
inline constexpr std::uint32_t kSlot428StoreStringRva[kSlot428StoreStringCount] = {{{rvas}}};
inline constexpr std::uint32_t kSlot428StoreStringOffset[kSlot428StoreStringCount] = {{{offs}}};
inline constexpr char kSlot428StoreStringBlob[] =
{blob};
static_assert(sizeof(kSlot428StoreStringBlob) == {blobsize});   // the pool plus the literal's own terminator

inline constexpr Slot428SaveSpec kSlot428SaveSpecs[kSlot428SaveRecordCount] = {{
{save_rows}
}};
inline constexpr Slot428IdSpec kSlot428IdSpecs[kSlot428IdEntryCount] = {{
{id_rows}
}};
inline constexpr Slot428AvailSpec kSlot428AvailSpecs[kSlot428AvailEntryCount] = {{
{av_rows}
}};

struct Slot428Objects {{
    Slot428Statics statics;                         // 0x2efd610
    Slot428SaveRecord saves[kSlot428SaveRecordCount];   // 0x2efd9a0
    IdEntry ids[kSlot428IdEntryCount];              // 0x2efdb58
    AvailEntry avails[kSlot428AvailEntryCount];     // 0x2efe120
    PtrVec24 id_vec;                                // V1 {{0x32f8dc8, 0x32f8dd0, 0x32f8dd8}}
    PtrVec24 avail_vec;                             // V2 {{0x32f6f38, 0x32f6f40, 0x32f6f48}}
    AllocDispatchContract alloc;                    // host stand-in for 0x12820 / 0x12af0 (vector growth, 0xc8 B variables, owned names)
    SharedStrDeallocatorF590* name_dealloc;         // host stand-in for the name release of the variables
}};

struct Slot428Result {{
    bool ok = false;
    bool statics_applied = false;
    std::size_t saves_done = 0;
    std::size_t ids_done = 0;
    std::size_t avails_done = 0;
    std::size_t atexit_recorded = 0;
}};

// malloc / free stand-ins (ignore the tags and flags), like default_slot513_record_alloc(); the default deallocator frees with std::free.
[[nodiscard]] AllocDispatchContract default_slot428_alloc() noexcept;
[[nodiscard]] SharedStrDeallocatorF590* default_slot428_name_dealloc() noexcept;

[[nodiscard]] Slot428Result init_slot428_52ec10(Slot428Objects& objects, ConCmdRegistry& concmd) noexcept;

}}  // namespace sotc
'''

if __name__ == "__main__":
    main()
