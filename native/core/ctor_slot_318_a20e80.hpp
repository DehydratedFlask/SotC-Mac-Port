// CPU-01-STARTUP-SLOT-318-01: native recovery of original constructor-table index 318 / RVA 0xa20e80
// (458 B, [0xa20e80, 0xa2104a)), ELF sha256 d0723afb...5861d3. The slot constructs ten small global
// "controller input" objects (eight different classes, no strings, no ConCmd/menu work) and registers one
// destructor for each with __cxa_atexit(dtor, obj, &__dso_handle = 0x1604218) (0xf8f390). Every byte below
// was read from the pinned image with xcrun llvm-objdump and is pinned by tools/lift/check_ctor_slot_318.py.
// Nothing here executes an original RVA: vtable pointers and destructors are identity tags.
//
// The objects are modelled as raw byte images (the field meaning of these classes is NOT recovered, only
// the exact stores the constructors perform), so the fixture can check every written byte and every byte
// left untouched. Base constructors (all plain stores):
//   0xb0c260 base_a(obj): [+0]=0x15d5780+0x10; dword [+8]=0; 16 zero bytes at +0x10
//   0xb0c2a0 base_b(obj): base_a(obj); [+0]=0x15d57b0+0x10; qword [+0x20]=0; qword [+0x40]=0
//   0xb06980 vec4(obj):   16 zero bytes at +0; dword [+0x10]=0x40000000 (2.0f); dword [0x3305a24] += 1;
//                         dword [0x3305a28] += 0x18        (two process-wide counters, modelled in state)
// Sequence (object, constructor, class vtable (+0x10 is stored), registered destructor):
//   A 0x32f3418 0x70 B  0xaeb990 0x15d5380 0xaeb9d0   dword [+0x48]=0; qword [+0x50]=0
//   B 0x32f3488 0x58 B  0xaf3af0 0x15d5410 0xaf3b20   16 zero bytes at +0x48
//   C 0x32f34e0 0x148 B 0xae09c0 0x15d5320 0xae09f0   byte [+0x144]=1
//   D 0x32f3628 0xa8 A  0xaee310 0x15d53e0 0xaee360   16 zero at +0x70; dword [+0x80]=0; dword [+0x9c]=0x7f7fffff;
//                                                     word [+0xa0]=0x100
//   E 0x32f36d0 0x138 A 0xaf6d70 0x15d5470 0xaf6dc0   vec4(obj+0x20); 16 zero at +0x44, +0x38; word [+0x130]=0x101
//   F 0x32f3808 0x68 B  0xace890 0x15d52c0 0xace8c0   (no extra stores)
//   G1 0x32f3870 0x38   0xb06980 -            0xa20460  16 zero at +0x18; qword [+0x28]=0; dword [+0x30]=1.1f
//   G2 0x32f38a8 0x60   0xb06980 -            0xa20470  qword [+0x18]=0; dword [+0x5c]=1.1f; 16 zero at +0x48,
//                                                       +0x3c, +0x20; qword [+0x30]=0; 1.0f at +0x58, +0x48, +0x38
//   G3 0x32f3908 0x40   0xb06980 -            0xa20480  32 zero at +0x18; dword [+0x38]=0; dword [+0x3c]=1.1f
//   G4 0x32f3948 ?      0xb06980 -            0xa20490  (no extra stores; the object's extent is not observed)
// The thunks 0xa20460..0xa20490 each `jmp 0xb06ae0` (the vec4 destructor); the class destructors
// 0xaeb9d0 / 0xaf6dc0 additionally release a heap block / call 0xb06ae0, the other four tail-call 0xb0c2e0
// or 0xb0c280. All are recorded, never run. Object sizes come from the .bss placement (next object's
// address); the real extent of G4 is not observed, only the 0x14 bytes its constructor writes are modelled.
// What these objects ARE (input-device state) and their consumers are NOT modelled.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot318Rva = 0xa20e80;
inline constexpr std::size_t kSlot318BodySize = 458;
inline constexpr std::uint64_t kSlot318DsoHandle = 0x1604218;
inline constexpr std::uint64_t kSlot318CounterCountRva = 0x3305a24;
inline constexpr std::uint64_t kSlot318CounterBytesRva = 0x3305a28;
inline constexpr std::uint32_t kSlot318Vec4Tail = 0x40000000;   // 2.0f at +0x10 of vec4
inline constexpr std::uint32_t kSlot318Vec4CountStep = 1;
inline constexpr std::uint32_t kSlot318Vec4ByteStep = 0x18;
inline constexpr std::uint32_t kSlot318OneAndTenth = 0x3f8ccccd;   // 1.1f
inline constexpr std::uint32_t kSlot318One = 0x3f800000;           // 1.0f
inline constexpr std::uint64_t kSlot318BaseAVptr = 0x15d5780 + 0x10;
inline constexpr std::uint64_t kSlot318BaseBVptr = 0x15d57b0 + 0x10;
inline constexpr std::uint64_t kSlot318ClassAVptr = 0x15d5380 + 0x10;
inline constexpr std::uint64_t kSlot318ClassBVptr = 0x15d5410 + 0x10;
inline constexpr std::uint64_t kSlot318ClassCVptr = 0x15d5320 + 0x10;
inline constexpr std::uint64_t kSlot318ClassDVptr = 0x15d53e0 + 0x10;
inline constexpr std::uint64_t kSlot318ClassEVptr = 0x15d5470 + 0x10;
inline constexpr std::uint64_t kSlot318ClassFVptr = 0x15d52c0 + 0x10;

inline constexpr std::uint64_t kSlot318ObjA = 0x32f3418;
inline constexpr std::uint64_t kSlot318ObjB = 0x32f3488;
inline constexpr std::uint64_t kSlot318ObjC = 0x32f34e0;
inline constexpr std::uint64_t kSlot318ObjD = 0x32f3628;
inline constexpr std::uint64_t kSlot318ObjE = 0x32f36d0;
inline constexpr std::uint64_t kSlot318ObjF = 0x32f3808;
inline constexpr std::uint64_t kSlot318ObjG1 = 0x32f3870;
inline constexpr std::uint64_t kSlot318ObjG2 = 0x32f38a8;
inline constexpr std::uint64_t kSlot318ObjG3 = 0x32f3908;
inline constexpr std::uint64_t kSlot318ObjG4 = 0x32f3948;

inline constexpr std::uint64_t kSlot318DtorA = 0xaeb9d0;
inline constexpr std::uint64_t kSlot318DtorB = 0xaf3b20;
inline constexpr std::uint64_t kSlot318DtorC = 0xae09f0;
inline constexpr std::uint64_t kSlot318DtorD = 0xaee360;
inline constexpr std::uint64_t kSlot318DtorE = 0xaf6dc0;
inline constexpr std::uint64_t kSlot318DtorF = 0xace8c0;
inline constexpr std::uint64_t kSlot318DtorG1 = 0xa20460;
inline constexpr std::uint64_t kSlot318DtorG2 = 0xa20470;
inline constexpr std::uint64_t kSlot318DtorG3 = 0xa20480;
inline constexpr std::uint64_t kSlot318DtorG4 = 0xa20490;

// A raw, zero-initialised byte image of one global object.
template <std::size_t N>
struct alignas(8) Slot318Raw {
    std::array<std::uint8_t, N> bytes{};
};

// 0x3305a24 / 0x3305a28: the process-wide counters 0xb06980 increments (live count, accounted bytes).
struct Slot318Counters {
    std::uint32_t count = 0;   // 0x3305a24
    std::uint32_t bytes = 0;   // 0x3305a28
};

struct Slot318Objects {
    Slot318Raw<0x70> a{};     // 0x32f3418
    Slot318Raw<0x58> b{};     // 0x32f3488
    Slot318Raw<0x148> c{};    // 0x32f34e0
    Slot318Raw<0xa8> d{};     // 0x32f3628
    Slot318Raw<0x138> e{};    // 0x32f36d0
    Slot318Raw<0x68> f{};     // 0x32f3808
    Slot318Raw<0x38> g1{};    // 0x32f3870
    Slot318Raw<0x60> g2{};    // 0x32f38a8
    Slot318Raw<0x40> g3{};    // 0x32f3908
    Slot318Raw<0x14> g4{};    // 0x32f3948 (only the bytes the constructor writes)
    Slot318Counters counters{};
};

enum class Slot318Stage : std::uint8_t {
    none,
    atexit_a,
    atexit_b,
    atexit_c,
    atexit_d,
    atexit_e,
    atexit_f,
    atexit_g1,
    atexit_g2,
    atexit_g3,
    atexit_g4,
};
[[nodiscard]] const char* slot318_stage_name(Slot318Stage stage) noexcept;

struct Slot318Result {
    bool ok = false;
    Slot318Stage blocked = Slot318Stage::none;
};

// `registry.atexit_log` receives the ten (dtor, object) pairs; a host that cannot allocate makes the
// fragment report ok == false at the failing stage (the objects built before it stay built, like the
// original's constructors that ran before the failing atexit).
[[nodiscard]] Slot318Result init_slot318_a20e80(Slot318Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
