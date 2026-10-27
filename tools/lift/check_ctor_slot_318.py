#!/usr/bin/env python3
"""ELF-tie startup constructor slot 318 (0xa20e80): ten small global input-state objects (eight classes)
built by plain stores plus ten __cxa_atexit registrations and the two 0xb06980 counters.

Offline only: opens the pinned private ELF (local/re/eboot.elf), reads bytes, never loads, relocates or
executes an original RVA. Pins: ELF sha256, constructor-table word 318, the 458-byte body (sha256, extent,
padding), every call / rip operand / immediate of the body, the six class constructors, both bases, the
0xb06980 vector constructor, the registered destructors and thunks (by hash), the class vtable destructor
slots (relocations at vtable+0x10 must equal the registered destructor) and the native header / source /
test / CMake tokens. Mutation self-test and --tamper elf|native (exits 1) come from l2_slot_check.py.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from l2_slot_check import Spec, main, require  # noqa: E402

BASE = 0xA20E80
SIZE = 458
SHA = "a9eebea6e06ac03b15319e245d3ce4a9cf578a7da99ee56fb6a537a4406fe9b5"

PINS = [
    ('rip', 0xa20e87, 7, 3, 0x32f3418),
    ('call', 0xa20e91, 0xaeb990),
    ('rip', 0xa20e96, 7, 3, 0x1604218),
    ('rip', 0xa20e9d, 7, 3, 0xaeb9d0),
    ('call', 0xa20eaa, 0xf8f390),
    ('rip', 0xa20eaf, 7, 3, 0x32f3488),
    ('call', 0xa20eb9, 0xaf3af0),
    ('rip', 0xa20ebe, 7, 3, 0xaf3b20),
    ('call', 0xa20ecb, 0xf8f390),
    ('rip', 0xa20ed0, 7, 3, 0x32f34e0),
    ('call', 0xa20eda, 0xae09c0),
    ('rip', 0xa20edf, 7, 3, 0xae09f0),
    ('call', 0xa20eec, 0xf8f390),
    ('rip', 0xa20ef1, 7, 3, 0x32f3628),
    ('call', 0xa20efb, 0xaee310),
    ('rip', 0xa20f00, 7, 3, 0xaee360),
    ('call', 0xa20f0d, 0xf8f390),
    ('rip', 0xa20f12, 7, 3, 0x32f36d0),
    ('call', 0xa20f1c, 0xaf6d70),
    ('rip', 0xa20f21, 7, 3, 0xaf6dc0),
    ('call', 0xa20f2e, 0xf8f390),
    ('rip', 0xa20f33, 7, 3, 0x32f3808),
    ('call', 0xa20f3d, 0xace890),
    ('rip', 0xa20f42, 7, 3, 0xace8c0),
    ('call', 0xa20f4f, 0xf8f390),
    ('rip', 0xa20f54, 7, 3, 0x32f3870),
    ('call', 0xa20f5e, 0xb06980),
    ('rip', 0xa20f63, 7, 3, 0xa20460),
    ('bytes', 0xa20f79, '48c7432800000000'),
    ('bytes', 0xa20f81, 'c74330cdcc8c3f'),
    ('call', 0xa20f88, 0xf8f390),
    ('rip', 0xa20f8d, 7, 3, 0x32f38a8),
    ('call', 0xa20f97, 0xb06980),
    ('rip', 0xa20f9c, 7, 3, 0xa20470),
    ('bytes', 0xa20fad, '48c7431800000000'),
    ('bytes', 0xa20fb5, 'c7435ccdcc8c3f'),
    ('bytes', 0xa20fcb, '48c7433000000000'),
    ('bytes', 0xa20fd3, 'c743580000803f'),
    ('bytes', 0xa20fda, 'c743480000803f'),
    ('bytes', 0xa20fe1, 'c743380000803f'),
    ('call', 0xa20fe8, 0xf8f390),
    ('rip', 0xa20fed, 7, 3, 0x32f3908),
    ('call', 0xa20ff7, 0xb06980),
    ('rip', 0xa20ffc, 7, 3, 0xa20480),
    ('bytes', 0xa21012, 'c7433800000000'),
    ('bytes', 0xa21019, 'c7433ccdcc8c3f'),
    ('call', 0xa21020, 0xf8f390),
    ('rip', 0xa21025, 7, 3, 0x32f3948),
    ('call', 0xa2102f, 0xb06980),
    ('rip', 0xa21034, 7, 3, 0xa20490),
    ('jmp', 0xa21045, 0xf8f390),
]

CALLEES = {
    "base_a": (0xB0C260, 0x1f, "7d183f561bbe710e57958614f24da393659a8d89c0b641dbf810db05a3ea8a0f"),
    "base_b": (0xB0C2A0, 0x33, "1dea1362e3b81c8f5195d22bbe4f6059f21f506c44ee1bd3dd3d450139b62327"),
    "vec4": (0xB06980, 0x23, "ee6253cedf97e7ba3c1b32ee1063528845db5492fec7c1d9229b99ef170c0d2f"),
    "ctor_a": (0xAEB990, 0x32, "698c9e03252a345fdd1c35b985bf7366acd4fbe6bda0e16c88cda27f56138e46"),
    "ctor_b": (0xAF3AF0, 0x2c, "4aff4413cf370395d6f9a212aa457526e60462eb1b70ef64d004cd4e152f8dcf"),
    "ctor_c": (0xAE09C0, 0x2a, "fad2944bd459e643f79287706b9a560f0b80258e4316992fa31a6c0410ce41a1"),
    "ctor_d": (0xAEE310, 0x49, "6d6565b89e10497a35389573253b706b62744d5a6ecd57fe5831d78e3296b4c1"),
    "ctor_e": (0xAF6D70, 0x43, "80bd6f6b04c1749d0624e4b8ced691a33458c89b309fd4869df917b5b010d188"),
    "ctor_f": (0xACE890, 0x23, "369853f3ee0e19b430db171b101e5faaa05a0a558db6eba73983a9d9f7332148"),
    "dtor_a": (0xAEB9D0, 0x3f, "d314eb06f9d55edf40b46501888e3f880e01478e84856b971bf822f8a146397b"),
    "dtor_b": (0xAF3B20, 0x5, "337cf4b99620b9e9b079467ae52e8a65ccc2d414df3c7e36a9ee6c82510aaafb"),
    "dtor_c": (0xAE09F0, 0x5, "d8fc4e21b311505c39c95d25bf9c35e9971f792b5272f938532f0dd41a0f888a"),
    "dtor_d": (0xAEE360, 0x5, "75b980910edf3e9f32aa9fd0baf951c38ea8cc4107b66f14ede0fcf423d45374"),
    "dtor_e": (0xAF6DC0, 0x2e, "808632c174218a930cfccd0cd7b6d5e1e06ba437f28005bf7a69d49a80f12f30"),
    "dtor_f": (0xACE8C0, 0x5, "dc416f8777af2e06ce3adfca7682c28160faf9fc6bd00cd5c78e1618abf59f5c"),
    "thunk_g1": (0xA20460, 0x5, "b7c7216f053bd09f8e991174c3e3e2a463387f05a8a9aafc168508a658aea3bb"),
    "thunk_g2": (0xA20470, 0x5, "3a95d689a50c8b8f8001a824ac9c91f38fa8afb0caeb1c7a666972d5ead9dba2"),
    "thunk_g3": (0xA20480, 0x5, "dde819af8e5e33b39d5c2fe4b40ff1a9ec7848aaacc69de38d0e3a3f8ac29ab7"),
    "thunk_g4": (0xA20490, 0x5, "28a9f4c4064a12aa137dba232773fd1a44f61a053b5ee11f0a2ca57fe85f0454"),
    "base_a_dtor": (0xB0C2E0, 0x1b, "a7e916aa0a95698197d9dd5378f60188fe744b4668f35d2b58949c710df0e6a4"),
}

# (object, class vtable symbol, registered destructor) in construction order
CLASSES = [(0x32F3418, 0x15D5380, 0xAEB9D0), (0x32F3488, 0x15D5410, 0xAF3B20), (0x32F34E0, 0x15D5320, 0xAE09F0),
           (0x32F3628, 0x15D53E0, 0xAEE360), (0x32F36D0, 0x15D5470, 0xAF6DC0), (0x32F3808, 0x15D52C0, 0xACE8C0)]


def elf_extra(e) -> None:
    # class vtable address point +0 (symbol + 0x10) is the destructor registered with __cxa_atexit
    for _obj, vt, dtor in CLASSES:
        require(e.reloc_target(vt + 0x10) == dtor, f"class vtable {vt:#x}+0x10 destructor {dtor:#x}")
    # thunk destructors tail-call the 0xb06980 object destructor 0xb06ae0
    for thunk in (0xA20460, 0xA20470, 0xA20480, 0xA20490):
        require(e.jmp(thunk) == 0xB06AE0, f"thunk {thunk:#x} -> 0xb06ae0")
    # the plain destructors tail-call 0xb0c2e0 (base B) / 0xb0c280 (bare ret)
    require(e.jmp(0xAF3B20) == 0xB0C2E0 and e.jmp(0xAE09F0) == 0xB0C2E0 and e.jmp(0xACE8C0) == 0xB0C2E0,
            "class B destructors -> 0xb0c2e0")
    require(e.jmp(0xAEE360) == 0xB0C280, "class D destructor -> 0xb0c280")
    e.bytes_at(0xB0C280, "c3", "0xb0c280 is a bare ret")
    # the DSO handle pushed to 0xf8f390 and the two counters of 0xb06980
    require(e.rip(0xA20E96, 7, 3) == 0x1604218, "dso handle")
    require(e.rip(0xB06980, 7, 3) == 0x3305A24 and e.rip(0xB06987, 7, 3) == 0x3305A28, "0xb06980 counters")
    e.bytes_at(0xB06996, "c7471000000040", "0xb06980 stores 2.0f at +0x10")
    e.bytes_at(0xB0699D, "ff00" "8301" "18", "0xb06980 counter steps 1 and 0x18")
    # object placement is contiguous with the native layout
    sizes = [0x70, 0x58, 0x148, 0xA8, 0x138, 0x68, 0x38, 0x60, 0x40]
    objs = [0x32F3418, 0x32F3488, 0x32F34E0, 0x32F3628, 0x32F36D0, 0x32F3808, 0x32F3870, 0x32F38A8, 0x32F3908,
            0x32F3948]
    for a, s, b in zip(objs, sizes, objs[1:]):
        require(a + s == b, f"object placement {a:#x}+{s:#x}")


SPEC = Spec(
    name="slot318", index=318, base=BASE, size=SIZE, sha=SHA, pins=PINS, callees=CALLEES, elf_extra=elf_extra,
    native_files={"header": "native/core/ctor_slot_318_a20e80.hpp", "source": "native/core/ctor_slot_318_a20e80.cpp",
                  "test": "native/tests/test_ctor_slot_318.cpp", "cmake": "native/CMakeLists.txt"},
    header_constants={"kSlot318Rva": BASE, "kSlot318BodySize": SIZE, "kSlot318DsoHandle": 0x1604218,
                      "kSlot318CounterCountRva": 0x3305A24, "kSlot318CounterBytesRva": 0x3305A28,
                      "kSlot318Vec4Tail": 0x40000000, "kSlot318Vec4CountStep": 1, "kSlot318Vec4ByteStep": 0x18,
                      "kSlot318OneAndTenth": 0x3F8CCCCD, "kSlot318One": 0x3F800000,
                      "kSlot318ObjA": 0x32F3418, "kSlot318ObjB": 0x32F3488, "kSlot318ObjC": 0x32F34E0,
                      "kSlot318ObjD": 0x32F3628, "kSlot318ObjE": 0x32F36D0, "kSlot318ObjF": 0x32F3808,
                      "kSlot318ObjG1": 0x32F3870, "kSlot318ObjG2": 0x32F38A8, "kSlot318ObjG3": 0x32F3908,
                      "kSlot318ObjG4": 0x32F3948, "kSlot318DtorA": 0xAEB9D0, "kSlot318DtorB": 0xAF3B20,
                      "kSlot318DtorC": 0xAE09F0, "kSlot318DtorD": 0xAEE360, "kSlot318DtorE": 0xAF6DC0,
                      "kSlot318DtorF": 0xACE8C0, "kSlot318DtorG1": 0xA20460, "kSlot318DtorG2": 0xA20470,
                      "kSlot318DtorG3": 0xA20480, "kSlot318DtorG4": 0xA20490},
    header_tokens=["0x15d5380 + 0x10", "0x15d5410 + 0x10", "0x15d5320 + 0x10", "0x15d53e0 + 0x10",
                   "0x15d5470 + 0x10", "0x15d52c0 + 0x10", "0x15d5780 + 0x10", "0x15d57b0 + 0x10",
                   "struct Slot318Objects", "Slot318Raw<0x148> c{}"],
    source_order=["base_b(o.a)", "log.record(kSlot318DtorA, &o.a)", "base_b(o.b)", "log.record(kSlot318DtorB, &o.b)",
                  "put_u8(o.c, 0x144, 1)", "log.record(kSlot318DtorC, &o.c)",
                  "put_u32(o.d, 0x9c, 0x7f7fffff)", "put_u16(o.d, 0xa0, 0x100)", "log.record(kSlot318DtorD, &o.d)",
                  "vec4(o.e, 0x20, o.counters)", "put_u16(o.e, 0x130, 0x101)", "log.record(kSlot318DtorE, &o.e)",
                  "log.record(kSlot318DtorF, &o.f)", "log.record(kSlot318DtorG1, &o.g1)",
                  "put_u32(o.g2, 0x58, kSlot318One)", "log.record(kSlot318DtorG2, &o.g2)",
                  "zero(o.g3, 0x18, 32)", "log.record(kSlot318DtorG3, &o.g3)", "log.record(kSlot318DtorG4, &o.g4)"],
    test_tokens=["kSlot318Rva == 0xa20e80", "section_build()", "o.counters.count == 7 + 5",
                 "{0xa20490, &o.g4}", "{0x48, 16, 0}"],
    cmake_tokens=["core/ctor_slot_318_a20e80.cpp", "add_executable(test_ctor_slot_318 tests/test_ctor_slot_318.cpp)",
                  "add_test(NAME ctor_slot_318 COMMAND test_ctor_slot_318)"],
    summary="ten input-state objects (6 class ctors + 4 vec4) + 10 atexit registrations + 2 counters",
)

if __name__ == "__main__":
    raise SystemExit(main(SPEC))
