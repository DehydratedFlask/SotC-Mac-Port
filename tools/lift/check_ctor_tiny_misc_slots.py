#!/usr/bin/env python3
"""ELF-tie the 12 miscellaneous tiny constructor slots of core/ctor_tiny_misc_slots (lane L1-TINY).

Slots: 404 406 409 414 494 517 519 535 541 553 562 563.

Offline only: opens the pinned private ELF, reads bytes, never loads, relocates or executes an original
RVA. For every slot it pins (as literals, not derived from the ELF):
  * the constructor-table word (u64 at ELF FILE offset 0x2dda750 + (306 - index) * 8) == the body RVA;
  * the exact body bytes and their SHA-256;
  * the decoded effects (tools/lift/tiny_slot_decode.py straight-line decode; slot 563 has a compare/branch
    and is pinned instruction by instruction): stores, calls, atexit records (dtor, object, dso 0x1604218),
    memset/xchg effects, callee targets;
  * the callee bodies that native code inlines (0x8bd90 + its 16-byte rodata constant, 0x326a0) and the
    rodata strings of slot 541, plus an independent FNV-1a recomputation of the two seeded hashes;
  * that every constant in native/core/ctor_tiny_misc_slots.hpp equals the decoded value.
Destructors and hook callees (0x17070, 0x3b310, 0x26e340, 0x26e770, 0x162020 body) are identity tags or
hooks in native code: they are only checked to be file-backed code, never executed.
`SOTC_CHECK_TAMPER=1` corrupts one pinned expectation in memory to prove the checker is fail-visible.

Private: needs local/re/eboot.elf.
"""

from __future__ import annotations

import hashlib
import os
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
sys.path.insert(0, str(ROOT / "tools/lift"))
import ps4_dynlink as d  # noqa: E402
import tiny_slot_decode as t  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
CTOR_TABLE_FILE_OFFSET_306 = 0x2DDA750   # ELF FILE offset (not a vaddr); the table is stored backwards
DSO = 0x1604218
ATEXIT_PLT, MEMSET_PLT, TSCFREQ_PLT = 0xF8F390, 0xF8F4F0, 0xF8F860
ATEXIT_NID, MEMSET_NID, TSCFREQ_NID = "tsvEmnenz48", "8zTFvBIAIN8", "1j3S3n-tTW4"

# (slot, body RVA, body size, body SHA-256, body bytes hex)
BODIES = [
    (404, 0x5e7c10, 19, "92a5e8d436595856e586b85b9732b17463360605d7b65adb92fde2eac83ca998",
     "488d3d89e5a10231f6baf8510000e9cd789a00"),
    (406, 0x5dbe10, 92, "4df194609062ee45a64ed018f245ddb682f738fdd9158ead3b3d04e15537d0a9",
     "554889e5415741565350488d1da757a2024889dfe867ffaaff4c8d3580ffaaff4c8d3de18302014889de4c89f74c89fae84b359b00488d1da457a2024889dfe83cffaaff4c89f74889de4c89fa4883c4085b415e415f5de924359b00"),
    (409, 0x5b50f0, 89, "2e809ca61970a71a7a8402c8a53d12ad439a4a34ab94d2a816a7a2d08acc285a",
     "554889e55350c5f857c0bf40000000488d1d0251a202c5f811050251a202e85d1fa6ff488d15fef004014889004889400848894010c6401801c6401901488905dc50a202488d3dd5ebffff4889de4883c4085b5de947a29d00"),
    (414, 0x57b9b0, 72, "72a20c0d3542475cd5d0db64d9b12318bb008d5904723d2e8551bdafef53a47e",
     "554889e55350488d1ddb9e9b024889dfe84bf9abff488d3db4f9abff488d15458808014889dee8b539a100488d05be9e9b02c5f857c0c70000000000c5f81140084883c4085b5dc3"),
    (494, 0x26f7c0, 84, "eaa5bc16e6a476c97998d80c34d5bc195eefe608d4ca1a36fbe195bcb105ef6d",
     "554889e5415653488d1d6af8c7024889dfe86aebffff4c8d353b4a3901488d3dac9dffff4889de4c89f2e8a1fbd100488d1d62f8c7024889dfe872efffff488d3d0b9effff4889de4c89f25b415e5de97cfbd100"),
    (517, 0x1aa510, 56, "063b368346241adb85d70d658ccc1045725e82a451c1f4c41fd043e5d835766f",
     "554889e5488d3d55fbffff488d15f69c4501488d35f7c1d30248c705ecc1d30200000000e8574ede00488d3de8c1d30231f65de95881e8ff"),
    (519, 0x19d2d0, 83, "5ad5cb7b6e9a29aa7b7d95a2fa33d138d0f1dd00e58178341f58ce00be2b97d9",
     "554889e55350488d356b72d402488d3dccd4ffff488d152d6f4601c5f857c0c5f8110648c7461000000000e89020df00488d3d6172d40231f6ba0020000031dbe8db21df0048871d5492d4024883c4085b5dc3"),
    (535, 0x138e20, 96, "f5cca62b7739ad3c693c625f6fe01368d3bfbad45fdb262028c3129fe7e7fe08",
     "554889e5415653488d1dc286da024889dfe8da24f0ff4c8d35dbb34c01488d3d3c25f0ff4889de4c89f2e84165e500c5f857c0488d359e86da02488d3d0ff5ffff4c89f2c5f811059486da0248c7059986da02000000005b415e5de91065e500"),
    (541, 0xfd210, 52, "5d9c5cf84101cff172e28c3ae22d7f7f29113110d283e7e5721742bcdebd23d8",
     "554889e5488d3d207eea00bec59d1c81e8fb4d0600488d3d147eea00bec59d1c818905311dde02e8e44d060089052a1dde025dc3"),
    (553, 0x8bd40, 72, "5b497cfa2448192f13d2dcd90a306fabb526b1ee9b20fba8400d859c3244a09d",
     "488d3dd9e8ffff488d15ca845701488d05f343e302488d35c443e302c4e1f96ec0488905d843e302488d05d961e402c5f970c044c5fa7f05a443e302488905ad43e302e90836f000"),
    (562, 0x36100, 50, "0aea1b8f24df481f2948c1de81f5eb8b42193cba03fcd4090b3c1feba9fdf5bb",
     "554889e5488d350de6e602488d3d6ed1ffff488d15ffe05c0148c70600000000e86b92f500b8060000008705f0e5e6025dc3"),
    (563, 0x32c70, 72, "974a4e8745124bc4b58be699c5f3c1a99268aee211da700419ba405e610ce872",
     "554889e55350488d1d831ae70248833b00751be8d8cbf5004889030f31488d0d741ae70248c1e2204809c24889110f31488d0d691ae70248c1e2204809c24889114883c4085b5dc3"),
]


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(f"FAIL {message}")


def A(dtor, obj):
    return (dtor, obj, DSO, None)


# ---- pinned semantic expectations (literals; the native header is compared against these) ------------------
FNV_SEED = 0x811C9DC5
R409 = ("ret", 0x17070)
EXPECT = {
    404: [("tail", MEMSET_PLT, (0x30061A0, 0, 0x51F8, None))],
    406: [("call", 0x8BD90, (0x30015C8, None, None, None)),
          ("call", ATEXIT_PLT, A(0x8BDB0, 0x30015C8)),
          ("call", 0x8BD90, (0x30015F0, None, None, None)),
          ("tail", ATEXIT_PLT, A(0x8BDB0, 0x30015F0))],
    409: [("store", 0x2FDA210, 16, 0),
          ("call", 0x17070, (0x40, None, None, None)),
          ("store", ("ret", 0x17070, 0), 8, R409),
          ("store", ("ret", 0x17070, 8), 8, R409),
          ("store", ("ret", 0x17070, 16), 8, R409),
          ("store", ("ret", 0x17070, 24), 1, 1),
          ("store", ("ret", 0x17070, 25), 1, 1),
          ("store", 0x2FDA210, 8, R409),
          ("tail", ATEXIT_PLT, A(0x5B3D10, 0x2FDA208))],
    414: [("call", 0x3B310, (0x2F35898, None, None, None)),
          ("call", ATEXIT_PLT, A(0x3B380, 0x2F35898)),
          ("store", 0x2F358A0, 4, 0),
          ("store", 0x2F358A8, 16, 0),
          ("ret",)],
    494: [("call", 0x26E340, (0x2EEF038, None, None, None)),
          ("call", ATEXIT_PLT, A(0x269590, 0x2EEF038)),
          ("call", 0x26E770, (0x2EEF058, None, None, None)),
          ("tail", ATEXIT_PLT, A(0x269610, 0x2EEF058))],
    517: [("store", 0x2EE6720, 8, 0),
          ("call", ATEXIT_PLT, A(0x1AA070, 0x2EE6720)),
          ("tail", 0x326A0, (0x2EE6728, 0, None, None))],
    519: [("store", 0x2EE4548, 16, 0),
          ("store", 0x2EE4558, 8, 0),
          ("call", ATEXIT_PLT, A(0x19A7B0, 0x2EE4548)),
          ("call", MEMSET_PLT, (0x2EE4568, 0, 0x2000, None)),
          ("xchg", 0x2EE6570, 8, 0),
          ("ret",)],
    535: [("call", 0x3B310, (0x2EE14F0, None, None, None)),
          ("call", ATEXIT_PLT, A(0x3B380, 0x2EE14F0)),
          ("store", 0x2EE1500, 16, 0),
          ("store", 0x2EE1510, 8, 0),
          ("tail", ATEXIT_PLT, A(0x138370, 0x2EE14F8))],
    541: [("call", 0x162020, (0xFA503B, FNV_SEED, None, None)),
          ("store", 0x2EDEF68, 4, ("ret", 0x162020)),
          ("call", 0x162020, (0xFA5040, FNV_SEED, None, None)),
          ("store", 0x2EDEF6C, 4, ("ret", 0x162020)),
          ("ret",)],
    553: [("store", 0x2EC0140, 8, 0x2EC0148),
          ("store", 0x2EC0120, 16, ("dup", 0x2EC0148)),
          ("store", 0x2EC0130, 8, 0x2ED1F48),
          ("tail", ATEXIT_PLT, A(0x8A620, 0x2EC0120))],
    562: [("store", 0x2EA4718, 8, 0),
          ("call", ATEXIT_PLT, A(0x33280, 0x2EA4718)),
          ("xchg", 0x2EA4720, 4, 6),
          ("ret",)],
}

# native header constants that must equal the decoded facts (name -> value)
HDR = {
    "kSlot404BufferRva": 0x30061A0, "kSlot404BufferSize": 0x51F8,
    "kSlot406Obj0Rva": 0x30015C8, "kSlot406Obj1Rva": 0x30015F0, "kSlot406InitRva": 0x8BD90,
    "kSlot406DtorRva": 0x8BDB0, "kSlot406ConstRva": 0xF9EBD0,
    "kSlot406ConstLo": 0x0000001D00000000, "kSlot406ConstHi": 0xFFFFFFFF00000000,
    "kSlot409ObjRva": 0x2FDA208, "kSlot409DtorRva": 0x5B3D10, "kSlot409AllocRva": 0x17070,
    "kSlot409NodeSize": 0x40, "kSlot409NodeColorOffset": 0x18, "kSlot409NodeIsNilOffset": 0x19,
    "kSlot414ObjRva": 0x2F35898, "kSlot414MutexInitRva": 0x3B310, "kSlot414DtorRva": 0x3B380,
    "kSlot494Obj0Rva": 0x2EEF038, "kSlot494Obj1Rva": 0x2EEF058, "kSlot494Ctor0Rva": 0x26E340,
    "kSlot494Ctor1Rva": 0x26E770, "kSlot494Dtor0Rva": 0x269590, "kSlot494Dtor1Rva": 0x269610,
    "kSlot517CellRva": 0x2EE6720, "kSlot517RecordRva": 0x2EE6728, "kSlot517DtorRva": 0x1AA070,
    "kSlot517StampCalleeRva": 0x326A0,
    "kSlot519ObjRva": 0x2EE4548, "kSlot519BufferRva": 0x2EE4568, "kSlot519CellRva": 0x2EE6570,
    "kSlot519DtorRva": 0x19A7B0, "kSlot519BufferSize": 0x2000, "kSlot519ObjectSize": 0x18,
    "kSlot535MutexRva": 0x2EE14F0, "kSlot535ObjRva": 0x2EE14F8, "kSlot535MutexDtorRva": 0x3B380,
    "kSlot535ObjDtorRva": 0x138370, "kSlot535ObjectSize": 0x20,
    "kSlot541CellRva": 0x2EDEF68, "kSlot541Name0Rva": 0xFA503B, "kSlot541Name1Rva": 0xFA5040,
    "kSlot553ObjRva": 0x2EC0120, "kSlot553SentinelOffset": 0x28, "kSlot553ExternalRva": 0x2ED1F48,
    "kSlot553DtorRva": 0x8A620, "kSlot553ObjectSize": 0x30,
    "kSlot562CellRva": 0x2EA4718, "kSlot562FlagRva": 0x2EA4720, "kSlot562DtorRva": 0x33280,
    "kSlot562FlagValue": 6,
    "kSlot563StampRva": 0x2EA4710,
}
for _slot, _rva, _size, _sha, _hx in BODIES:
    HDR[f"kSlot{_slot}Rva"] = _rva
    HDR[f"kSlot{_slot}Size"] = _size

# 0x8bd90 (ctor_8bd90): vmovups xmm0,[rip+0xf12e38] (-> 0xf9ebd0); mov qword [rdi+0x10],0; vmovups [rdi+0x18],xmm0; ret
CALLEE_8BD90 = "c5f81005382ef10048c7471000000000c5f8114718c3"
CONST_F9EBD0 = "000000001d00000000000000ffffffff"
# 0x326a0 (stamp record helper called as 0x326a0(rdi = record, esi = 0)):
#   mov [rbx],sil ; cmp qword [r14=0x2ea4700],0 ; jne ; call 0xf8f860 ; mov [r14],rax ; rdtsc ; -> [0x2ea4708]
#   ; rdtsc ; -> [rbx+8] ; mov qword [rbx+0x10],0 ; ret
CALLEE_326A0 = ("554889e54156534c8d355220e7024889fb40883349833e00751be8a1d1f5004989060f31488d0d3d20e70248c1e2204809c2"
                "4889110f3148c1e2204809c24889530848c74310000000005b415e5dc3")
# 0x32c70 / 563, instruction by instruction (cmp qword [rbx],0 -- identical shape to 0x323d0)
BODY_563_FIELDS = [
    (0, "554889e55350"),                 # push rbp; mov rbp,rsp; push rbx; push rax
    (6, "488d1d831ae702"),               # lea rbx,[rip+..] -> 0x2ea4700 (guard)
    (13, "48833b00"),                    # cmp qword [rbx],0
    (17, "751b"),                        # jne +0x1b (skip the init)
    (19, "e8d8cbf500"),                  # call 0xf8f860 (sceKernelGetTscFrequency PLT)
    (24, "488903"),                      # mov [rbx],rax
    (27, "0f31"),                        # rdtsc
    (29, "488d0d741ae702"),              # lea rcx,[rip+..] -> 0x2ea4708 (base)
    (36, "48c1e2204809c2488911"),  # shl rdx,32; or rdx,rax; mov [rcx],rdx
    (46, "0f31"),                        # rdtsc
    (48, "488d0d691ae702"),              # lea rcx,[rip+..] -> 0x2ea4710 (stamp)
    (55, "48c1e2204809c2488911"),
    (65, "4883c4085b5dc3"),              # add rsp,8; pop rbx; pop rbp; ret
]


def header_consts(text: str):
    out = {}
    for m in re.finditer(r"constexpr\s+[\w:]+\s+(k\w+)\s*=\s*(0x[0-9a-fA-F]+|\d+)(?:u|ull|ul|l)?\s*;", text):
        out[m.group(1)] = int(m.group(2), 0)
    return out


def fnv_upper(text: bytes, seed: int) -> int:
    h = seed
    for c in text.upper():
        h = ((h ^ c) * 0x01000193) & 0xFFFFFFFF
    return h


def rip_target(body: bytes, rva: int, off: int, length: int, disp_at: int) -> int:
    return rva + off + length + struct.unpack_from("<i", body, disp_at)[0]


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    parsed = d.parse(elf)
    img = d.Image(elf, parsed["ph"])

    bodies = list(BODIES)
    if os.environ.get("SOTC_CHECK_TAMPER") == "1":   # mutation sanity: one expected byte flipped
        i = [b[0] for b in bodies].index(409)
        slot, rva, size, sha, hx = bodies[i]
        bodies[i] = (slot, rva, size, sha, hx[:-2] + ("00" if hx[-2:] != "00" else "01"))

    for plt, nid_name, nid in ((ATEXIT_PLT, "__cxa_atexit", ATEXIT_NID), (MEMSET_PLT, "memset", MEMSET_NID),
                               (TSCFREQ_PLT, "sceKernelGetTscFrequency", TSCFREQ_NID)):
        require(t.import_name(parsed, img, plt) == nid == d.nid(nid_name), f"{plt:#x} is not the {nid_name} import")

    header_text = (ROOT / "native/core/ctor_tiny_misc_slots.hpp").read_text()
    source = (ROOT / "native/core/ctor_tiny_misc_slots.cpp").read_text()
    cmake = (ROOT / "native/CMakeLists.txt").read_text()
    consts = header_consts(header_text)
    for name, value in HDR.items():
        require(consts.get(name) == value, f"native header {name} = {consts.get(name)} != {value:#x}")
    require('kSlot541Name0[] = "wind"' in header_text and 'kSlot541Name1[] = "mesh_instance"' in header_text,
            "native slot 541 names")

    slots = [b[0] for b in BODIES]
    require(slots == [404, 406, 409, 414, 494, 517, 519, 535, 541, 553, 562, 563], f"slot list {slots}")

    dtors = set()
    for slot, rva, size, sha, hx in bodies:
        tag = f"slot {slot} / {rva:#x}"
        word = struct.unpack_from("<Q", elf, CTOR_TABLE_FILE_OFFSET_306 + (306 - slot) * 8)[0]
        require(word == rva, f"{tag}: constructor-table word {word:#x}")
        body = img.read(rva, size)
        require(hashlib.sha256(body).hexdigest() == sha, f"{tag}: body sha256")
        require(body.hex() == hx, f"{tag}: body bytes")
        if slot == 563:
            for off, field in BODY_563_FIELDS:
                require(body[off:off + len(field) // 2].hex() == field, f"{tag}: instruction at +{off:#x}")
            require(body[17] == 0x75 and 17 + 2 + body[18] == 46, f"{tag}: jne must land on the second rdtsc")
            continue
        events = t.run(img, rva, size)
        require(events == EXPECT[slot], f"{tag}: decoded effects differ\n  got  {events}\n  want {EXPECT[slot]}")
        for e in events:
            if e[0] in ("call", "tail") and e[1] == ATEXIT_PLT:
                dtor, obj = e[2][0], e[2][1]
                require(e[2][2] == DSO, f"{tag}: dso")
                try:
                    img.read(obj, 1)
                    require(False, f"{tag}: object {obj:#x} must be .bss (not file-backed)")
                except ValueError:
                    pass
                require(len(img.read(dtor, 1)) == 1, f"{tag}: dtor {dtor:#x} not file-backed code")
                dtors.add(dtor)

    # callee bodies the native code inlines
    require(img.read(0x8BD90, len(CALLEE_8BD90) // 2).hex() == CALLEE_8BD90, "0x8bd90 body")
    body_8bd90 = img.read(0x8BD90, 8)
    require(0x8BD90 + 8 + struct.unpack_from("<i", body_8bd90, 4)[0] == 0xF9EBD0 == consts["kSlot406ConstRva"],
            "0x8bd90 constant address")
    require(img.read(0xF9EBD0, 16).hex() == CONST_F9EBD0, "0xf9ebd0 constant bytes")
    lo, hi = struct.unpack("<QQ", img.read(0xF9EBD0, 16))
    require((lo, hi) == (consts["kSlot406ConstLo"], consts["kSlot406ConstHi"]), "native 16-byte constant")
    require(img.read(0x326A0, len(CALLEE_326A0) // 2).hex() == CALLEE_326A0, "0x326a0 body")
    c = img.read(0x326A0, len(CALLEE_326A0) // 2)
    require(rip_target(c, 0x326A0, 7, 7, 10) == 0x2EA4700, "0x326a0 guard cell")
    require(rip_target(c, 0x326A0, 36, 7, 39) == 0x2EA4708, "0x326a0 base cell")

    # slot 541: strings and an independent FNV recomputation
    require(img.read(0xFA503B, 5) == b"wind\x00" and img.read(0xFA5040, 14) == b"mesh_instance\x00", "541 strings")
    require(fnv_upper(b"wind", FNV_SEED) == 0x167DE847 and fnv_upper(b"mesh_instance", FNV_SEED) == 0xD9E6AAE6,
            "541 FNV values")
    require(img.read(0x162020, 4).hex() == "554889e5", "0x162020 is code")
    # hook callees must be file-backed code (identity only in native)
    for hook in (0x17070, 0x3B310, 0x26E340, 0x26E770):
        require(len(img.read(hook, 4)) == 4, f"hook callee {hook:#x}")

    # Native source: destructors are tags only, no call through an original address.
    require("log.record(" in source and "kSlot406DtorRva" in source, "native must RECORD destructors")
    require("test_ctor_tiny_misc_slots" in cmake and "ctor_tiny_misc_slots" in cmake, "CMake wiring")

    print(f"OK check_ctor_tiny_misc_slots: {len(BODIES)} slots, dso {DSO:#x}, imports {ATEXIT_NID}/{MEMSET_NID}/"
          f"{TSCFREQ_NID}; {len(dtors)} distinct destructors (tags only)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
