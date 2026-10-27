// CPU-01-STARTUP-SLOT-318-01: see ctor_slot_318_a20e80.hpp. Plain byte stores in the exact order of the
// original constructors; nothing executes an original RVA.
#include "ctor_slot_318_a20e80.hpp"

#include <cstring>

#include "l2_slot_common.hpp"

namespace sotc {

const char* slot318_stage_name(Slot318Stage stage) noexcept {
    switch (stage) {
        case Slot318Stage::none: return "none";
        case Slot318Stage::atexit_a: return "atexit_a";
        case Slot318Stage::atexit_b: return "atexit_b";
        case Slot318Stage::atexit_c: return "atexit_c";
        case Slot318Stage::atexit_d: return "atexit_d";
        case Slot318Stage::atexit_e: return "atexit_e";
        case Slot318Stage::atexit_f: return "atexit_f";
        case Slot318Stage::atexit_g1: return "atexit_g1";
        case Slot318Stage::atexit_g2: return "atexit_g2";
        case Slot318Stage::atexit_g3: return "atexit_g3";
        case Slot318Stage::atexit_g4: return "atexit_g4";
    }
    return "unknown";
}

namespace {

template <std::size_t N>
void put(Slot318Raw<N>& obj, std::size_t off, const void* src, std::size_t len) noexcept {
    if (off + len <= N) std::memcpy(obj.bytes.data() + off, src, len);   // the offsets are constants; guard is defensive
}
template <std::size_t N>
void put_u64(Slot318Raw<N>& o, std::size_t off, std::uint64_t v) noexcept { put(o, off, &v, 8); }
template <std::size_t N>
void put_u32(Slot318Raw<N>& o, std::size_t off, std::uint32_t v) noexcept { put(o, off, &v, 4); }
template <std::size_t N>
void put_u16(Slot318Raw<N>& o, std::size_t off, std::uint16_t v) noexcept { put(o, off, &v, 2); }
template <std::size_t N>
void put_u8(Slot318Raw<N>& o, std::size_t off, std::uint8_t v) noexcept { put(o, off, &v, 1); }
template <std::size_t N>
void zero(Slot318Raw<N>& o, std::size_t off, std::size_t len) noexcept {
    if (off + len <= N) std::memset(o.bytes.data() + off, 0, len);
}

// 0xb0c260
template <std::size_t N>
void base_a(Slot318Raw<N>& o) noexcept {
    put_u64(o, 0, kSlot318BaseAVptr);
    put_u32(o, 8, 0);
    zero(o, 0x10, 16);
}
// 0xb0c2a0
template <std::size_t N>
void base_b(Slot318Raw<N>& o) noexcept {
    base_a(o);
    put_u64(o, 0, kSlot318BaseBVptr);
    put_u64(o, 0x20, 0);
    put_u64(o, 0x40, 0);
}
// 0xb06980 (at byte offset `at` of the object)
template <std::size_t N>
void vec4(Slot318Raw<N>& o, std::size_t at, Slot318Counters& c) noexcept {
    zero(o, at, 16);
    put_u32(o, at + 0x10, kSlot318Vec4Tail);
    c.count += kSlot318Vec4CountStep;
    c.bytes += kSlot318Vec4ByteStep;
}

}  // namespace

Slot318Result init_slot318_a20e80(Slot318Objects& o, ConCmdRegistry& concmd) noexcept {
    Slot318Result out{};
    l2::StageFail<Slot318Result, Slot318Stage> fail{out};
    StartupAtexitLog& log = concmd.atexit_log;

    // 0xa20e87: 0xaeb990 -> 0x32f3418; __cxa_atexit(0xaeb9d0)
    base_b(o.a);
    put_u64(o.a, 0, kSlot318ClassAVptr);
    put_u32(o.a, 0x48, 0);
    put_u64(o.a, 0x50, 0);
    if (!log.record(kSlot318DtorA, &o.a)) return (void)fail.at(Slot318Stage::atexit_a), out;

    // 0xa20eaf: 0xaf3af0 -> 0x32f3488; atexit 0xaf3b20
    base_b(o.b);
    put_u64(o.b, 0, kSlot318ClassBVptr);
    zero(o.b, 0x48, 16);
    if (!log.record(kSlot318DtorB, &o.b)) return (void)fail.at(Slot318Stage::atexit_b), out;

    // 0xa20ed0: 0xae09c0 -> 0x32f34e0; atexit 0xae09f0
    base_b(o.c);
    put_u64(o.c, 0, kSlot318ClassCVptr);
    put_u8(o.c, 0x144, 1);
    if (!log.record(kSlot318DtorC, &o.c)) return (void)fail.at(Slot318Stage::atexit_c), out;

    // 0xa20ef1: 0xaee310 -> 0x32f3628 (base 0xb0c260); atexit 0xaee360
    base_a(o.d);
    put_u64(o.d, 0, kSlot318ClassDVptr);
    zero(o.d, 0x70, 16);
    put_u32(o.d, 0x80, 0);
    put_u32(o.d, 0x9c, 0x7f7fffff);
    put_u16(o.d, 0xa0, 0x100);
    if (!log.record(kSlot318DtorD, &o.d)) return (void)fail.at(Slot318Stage::atexit_d), out;

    // 0xa20f12: 0xaf6d70 -> 0x32f36d0 (base 0xb0c260; sub-object 0xb06980 at +0x20); atexit 0xaf6dc0
    base_a(o.e);
    put_u64(o.e, 0, kSlot318ClassEVptr);
    vec4(o.e, 0x20, o.counters);
    zero(o.e, 0x44, 16);
    zero(o.e, 0x38, 16);
    put_u16(o.e, 0x130, 0x101);
    if (!log.record(kSlot318DtorE, &o.e)) return (void)fail.at(Slot318Stage::atexit_e), out;

    // 0xa20f33: 0xace890 -> 0x32f3808; atexit 0xace8c0
    base_b(o.f);
    put_u64(o.f, 0, kSlot318ClassFVptr);
    if (!log.record(kSlot318DtorF, &o.f)) return (void)fail.at(Slot318Stage::atexit_f), out;

    // 0xa20f54: 0xb06980 -> 0x32f3870, then the post-stores; atexit 0xa20460
    vec4(o.g1, 0, o.counters);
    zero(o.g1, 0x18, 16);
    put_u64(o.g1, 0x28, 0);
    put_u32(o.g1, 0x30, kSlot318OneAndTenth);
    if (!log.record(kSlot318DtorG1, &o.g1)) return (void)fail.at(Slot318Stage::atexit_g1), out;

    // 0xa20f8d: 0xb06980 -> 0x32f38a8; the stores in program order (they overlap, order matters)
    vec4(o.g2, 0, o.counters);
    put_u64(o.g2, 0x18, 0);
    put_u32(o.g2, 0x5c, kSlot318OneAndTenth);
    zero(o.g2, 0x48, 16);
    zero(o.g2, 0x3c, 16);
    zero(o.g2, 0x20, 16);
    put_u64(o.g2, 0x30, 0);
    put_u32(o.g2, 0x58, kSlot318One);
    put_u32(o.g2, 0x48, kSlot318One);
    put_u32(o.g2, 0x38, kSlot318One);
    if (!log.record(kSlot318DtorG2, &o.g2)) return (void)fail.at(Slot318Stage::atexit_g2), out;

    // 0xa20fed: 0xb06980 -> 0x32f3908; atexit 0xa20480
    vec4(o.g3, 0, o.counters);
    zero(o.g3, 0x18, 32);
    put_u32(o.g3, 0x38, 0);
    put_u32(o.g3, 0x3c, kSlot318OneAndTenth);
    if (!log.record(kSlot318DtorG3, &o.g3)) return (void)fail.at(Slot318Stage::atexit_g3), out;

    // 0xa21025: 0xb06980 -> 0x32f3948; tail-called __cxa_atexit(0xa20490)
    vec4(o.g4, 0, o.counters);
    if (!log.record(kSlot318DtorG4, &o.g4)) return (void)fail.at(Slot318Stage::atexit_g4), out;

    out.ok = true;
    return out;
}

}  // namespace sotc
