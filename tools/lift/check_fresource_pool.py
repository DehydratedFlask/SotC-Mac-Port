#!/usr/bin/env python3
"""Tie native/core/resource_pool.{hpp,cpp} to the resource-pool static audit.

Private: needs local/re/eboot.elf. Every check is a byte pin, a vtable pin or a native-header text pin;
no native build directory is required, so this checker runs in the normal tools/lift sweep.

Pins the ten report §7 functions (comparator, wrapper ctor, AddReference, RemoveReference, RequestState,
ReleaseDependency, null wrapper, GetResource, TryGetResource, MarkResourceWrapperDead) plus four
supporting bodies (map insert/destroy and the two locked-ref Lock helpers). Structural pins cover the
comparator's hash-then-strcmp shape, the 0x50 map node with key +0x20 / value +0x48 and no AddReference,
erase only at +0x30 == 0 with the node-key release, the ctor's no-vtable store and state 11, and the
owned=0 writes on both GetResource paths and TryGetResource.
"""
import hashlib
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import leaf_check_common as base  # noqa: E402
import tiny_leaf_decode as T  # noqa: E402

require, ROOT = base.require, base.ROOT

# The ten functions named by report §7 (hashes and extents from scratch/pins.log).
CORE = {
    0x14C850: (32, "2dd8095d67d4c5b6034930a9195fe2a422a36c135bc189480d60a57af3c35bbb"),
    0x14C8A0: (243, "aac6ec82cbed49f70e1bdf5b5a70c1488c3b1006da72d8ef36295f38b6c749e3"),
    0x14CCB0: (5, "7a16c895ed92b3611f04079a9ae0e329424c2dead2e80a90a368d231c33ac90f"),
    0x14CCC0: (563, "08ebd6d6c14cb47bc2ac2acc7007ef639b697cfdbec7fa6f17cc39059e5f779b"),
    0x14CF20: (748, "b250f62d911472200d1cc356689263aeb11c3d96374cf0316ac2aa823cf68734"),
    0x14D2D0: (289, "573d7e09e0a6fe0fa9879b7f1844e8fffc59eb572400c05ae0fa42e8d0c90831"),
    0x14D880: (121, "b7586580add23db86b9d050ada729b080b534dba3bd2af1f62c8fa55869ffff4"),
    0x14E260: (466, "8a4a021822f1b7bebb454c483b99826046d1efe1f5bbecbbcae8fbeffcdef2c8"),
    0x14E440: (284, "de1abcb2a3fca383d52094b337f48ac526032405392f462b26046a06f2b8f0af"),
    0x14E9B0: (335, "359a76f3ef79b5e2d70e634a6a649156097d563833a24555197f6fb795cef0dc"),
}

# Supporting bodies cited by the same report (§4.4, §4.5, §5.2).
SUPPORT = {
    0x150360: (367, "e72f531b946262bf3e51f0940e23e759d357f08c510b562a5047475064f14596"),
    0x150300: (83, "df3ae10b4916ea6e0fc1e1b34d871cc0c6da83d794e26eb161dffe2bf9c6f6bf"),
    0x95580: (133, "abb48f515fc9278a0a168354cb6b5b4c2576a79ff2b8c9043dd79265f1886d38"),
    0x140D90: (143, "9823e24f9e1ec88b4d14c4b792b728438dd0fdbfbc7f3a9f8e6b71b385c6cc78"),
}

# native/core/resource_pool.hpp static_asserts: the 0x100 B layout the model must keep.
HEADER_PINS = (
    "static_assert(sizeof(ResourceWrapper) == 0x100);",
    "static_assert(offsetof(ResourceWrapper, refs) == 0x30);",
    "static_assert(offsetof(ResourceWrapper, deps) == 0x40);",
    "static_assert(offsetof(ResourceWrapper, guard) == 0x50);",
    "static_assert(offsetof(ResourceWrapper, flags60) == 0x60);",
    "static_assert(offsetof(ResourceWrapper, pool) == 0x68);",
    "static_assert(offsetof(ResourceWrapper, key) == 0x70);",
    "static_assert(offsetof(ResourceWrapper, state) == 0x98);",
    "static_assert(offsetof(ResourceWrapper, children) == 0xa8);",
    "static_assert(offsetof(ResourceWrapper, owner_id) == 0xc8);",
    "static_assert(offsetof(ResourceWrapper, f0) == 0xf0);",
    "static_assert(offsetof(ResourceWrapper, f4) == 0xf4);",
    "static_assert(offsetof(ResourceWrapper, f8) == 0xf8);",
    "static_assert(sizeof(ResourceRef) == 0x10);",
)


def call_target(body, offset, rva):
    return rva + offset + 5 + struct.unpack_from("<i", body, offset + 1)[0]


def call_offsets(body, rva, target):
    return [
        offset
        for offset in range(len(body) - 4)
        if body[offset] == 0xE8 and call_target(body, offset, rva) == target
    ]


def main():
    elf, image = T.load_image()
    require(hashlib.sha256(elf).hexdigest() == T.ELF_SHA256, "unexpected ELF SHA256")

    for rva, (size, digest) in {**CORE, **SUPPORT}.items():
        body = image.read(rva, size)
        got = hashlib.sha256(body).hexdigest()
        require(got == digest, f"{rva:#x} body changed: {got}")

    # ---- ResourceWrapper ctor 0x14c8a0: no vtable, state 11, pool back-pointer, key copy. -------------
    ctor = image.read(0x14C8A0, CORE[0x14C8A0][0])
    require(b"\x48\xc7\x03\x00\x00\x00\x00" in ctor, "ctor no longer stores 0 at +0x00")
    forbidden = [
        bytes([rex, 0x89, modrm])
        for rex in (0x48, 0x4C)
        for modrm in (0x03, 0x0B, 0x13, 0x1B, 0x23, 0x2B, 0x33, 0x3B)
    ]
    require(not any(p in ctor for p in forbidden), "ctor stores a register at [this+0x00] (vtable?)")
    require(b"\xC7\x83\x98\x00\x00\x00\x0B\x00\x00\x00" in ctor, "ctor state 11 store changed")
    require(b"\xC7\x83\xF4\x00\x00\x00\x0B\x00\x00\x00" in ctor, "ctor +0xf4 = 11 store changed")
    require(b"\x48\x89\x53\x68" in ctor, "ctor pool back-pointer store changed")
    require(b"\x48\x8D\x7B\x70" in ctor, "ctor key lea changed")
    require(call_target(ctor, 0x44, 0x14C8A0) == 0x14C3A0, "ctor key copy target changed")

    # ---- Comparator 0x14c850: unsigned hash compare, then strcmp (not memcmp). -----------------------
    cmp_body = image.read(0x14C850, CORE[0x14C850][0])
    require(
        cmp_body[:19] == bytes.fromhex("8b0eb8ffffffff390f7207b8010000007601c3"),
        "comparator hash compare changed",
    )
    require(cmp_body[19:27] == bytes.fromhex("488b7f08488b7608"), "comparator strcmp argument load changed")
    require(call_target(cmp_body, 27, 0x14C850) == 0xF8F500, "comparator no longer tail-calls strcmp")

    # ---- Map insert 0x150360: 0x50 node, key +0x20, raw value +0x48, no AddReference. -----------------
    ins = image.read(0x150360, SUPPORT[0x150360][0])
    node_alloc = ins.find(b"\xBF\x50\x00\x00\x00")
    key_copy = ins.find(b"\x49\x8D\x7F\x20")
    value_store = ins.find(b"\x49\x89\x47\x48")
    rebalance = call_offsets(ins, 0x150360, 0x42D30)
    require(node_alloc >= 0, "map node size 0x50 pin missing")
    require(key_copy >= 0, "map node key +0x20 pin missing")
    require(value_store >= 0, "map node value +0x48 pin missing")
    require(node_alloc < key_copy < value_store, "map node construction order changed")
    require(rebalance and rebalance[0] > value_store, "map rebalance call moved before the value store")
    require(call_offsets(ins, 0x150360, 0x17090), "map node allocator 0x17090 call missing")
    require(not call_offsets(ins, 0x150360, 0x14CCB0), "map insert now takes a wrapper reference")
    require(call_offsets(ins, 0x150360, 0x14C3A0), "map insert key copy call missing")

    # ---- MarkResourceWrapperDead 0x14e9b0: only at refs30 == 0, releases the node key. ----------------
    era = image.read(0x14E9B0, CORE[0x14E9B0][0])
    require(b"\x41\x83\x7D\x30\x00" in era, "erase no longer tests +0x30 == 0 first")
    require(b"\x49\x8D\x7C\x24\x28" in era, "erase node key +0x28 lea missing")
    require(call_offsets(era, 0x14E9B0, 0xF590), "erase no longer releases the node key via 0xf590")

    # ---- map_destroy_subtree 0x150300 also releases each node key. -----------------------------------
    md = image.read(0x150300, SUPPORT[0x150300][0])
    require(b"\x48\x8D\x7B\x28" in md, "map destroy node key +0x28 lea missing")
    require(call_offsets(md, 0x150300, 0xF590), "map destroy no longer releases the node key")

    # ---- GetResource 0x14e260: owned=0 on the hit and miss paths; no payload factory on a miss. ------
    get = image.read(0x14E260, CORE[0x14E260][0])
    require(get.find(b"\xC6\x43\x08\x00") >= 0, "GetResource owned=0 store missing")
    require(call_target(get, 0xEC, 0x14E260) == 0x14E3F0, "GetResource hit path no longer joins the epilogue")
    miss_call = 0x18B
    require(get[miss_call] == 0xE8 and call_target(get, miss_call, 0x14E260) == 0x14CCC0,
            "GetResource miss path changed")
    require(0x14E3F0 - 0x14E260 == miss_call + 5, "GetResource miss path no longer falls into owned=0")

    # ---- TryGetResource 0x14e440: owned=0 on both the hit and the null-wrapper miss path. -------------
    try_get = image.read(0x14E440, CORE[0x14E440][0])
    require(b"\x41\xC6\x47\x08\x00" in try_get, "TryGetResource owned=0 store missing")

    # ---- Lock helpers: owned=1 after RequestState; 0x95580 pins priority 13, 0x140d90 is dynamic. ----
    lock13 = image.read(0x95580, SUPPORT[0x95580][0])
    require(b"\x80\x7B\x08\x00" in lock13, "Lock(13) owned test missing")
    require(b"\xBE\x0D\x00\x00\x00" in lock13, "Lock(13) priority 13 pin missing")
    require(b"\xC6\x43\x08\x01" in lock13, "Lock(13) owned=1 store missing")
    require(call_offsets(lock13, 0x95580, 0x14CF20), "Lock(13) no longer calls RequestState")
    lock9 = image.read(0x140D90, SUPPORT[0x140D90][0])
    require(b"\x80\x7B\x08\x00" in lock9, "Lock(slot,priority) owned test missing")
    require(b"\x44\x89\xF6" in lock9, "Lock(slot,priority) no longer forwards the priority argument")
    require(b"\xC6\x43\x08\x01" in lock9, "Lock(slot,priority) owned=1 store missing")
    require(call_offsets(lock9, 0x140D90, 0x14CF20), "Lock(slot,priority) no longer calls RequestState")

    # ---- Pool vtable: slot 3 is GetResource; slots 0..4 are the reported five. ------------------------
    slots = [struct.unpack_from("<Q", image.read(0x1598628 + 8 * i, 8))[0] for i in range(5)]
    require(slots == [0x14DE00, 0x14E170, 0x14E190, 0x14E260, 0x14E440], f"pool vtable slots changed: {slots}")
    require(struct.unpack_from("<Q", image.read(0x1598640, 8))[0] == 0x14E260, "pool slot 3 is not GetResource")

    # ---- Native header layout pins. ------------------------------------------------------------------
    header = (ROOT / "native/core/resource_pool.hpp").read_text()
    for pin in HEADER_PINS:
        require(pin in header, f"native header pin missing: {pin}")
    source = (ROOT / "native/core/resource_pool.cpp").read_text()
    require("copy_hashed_14c3a0(this->key, key);" in source, "native ctor key copy missing")
    require("std::strcmp(a_text, b_text) < 0" in source, "native comparator is not strcmp")
    require("map_.emplace(node_key, wrapper)" in source, "native map insert missing")
    require("release_shared_str_f590" in source, "native key release does not use the 0xf590 lift")

    print(
        "OK fresource_pool: 10 core + 4 supporting pins; comparator 0x14c850 hash-then-strcmp; "
        "map node 0x50 key +0x20 value +0x48 non-owning; ctor no vtable state 11; "
        "owned=0 on both GetResource paths and TryGetResource; native header 0x100 layout pinned"
    )


if __name__ == "__main__":
    main()
