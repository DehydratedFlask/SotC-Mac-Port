#include "ctor_tiny_misc_slots.hpp"

#include <cstdlib>
#include <cstring>

#include "fnv_path_cstr_162020.hpp"

namespace sotc {
namespace {

static_assert(offsetof(Slot517Record, tsc) == 8 && offsetof(Slot517Record, zero) == 0x10 &&
              sizeof(Slot517Record) == 0x18);
static_assert(kSlot553SentinelOffset + sizeof(std::uint64_t) <= kSlot553ObjectSize);

constexpr std::uint32_t kFnvSeed = kFnvOffsetBasisA2ddc0;   // 0x811c9dc5, the esi of both 0x162020 calls

TinyMiscResult blocked(const char* stage) noexcept {
    TinyMiscResult r;
    r.blocked = stage;
    return r;
}

TinyMiscResult done() noexcept {
    TinyMiscResult r;
    r.ok = true;
    return r;
}

void store_u64(std::uint8_t* at, std::uint64_t v) noexcept { std::memcpy(at, &v, sizeof(v)); }

// 0x8bd90(obj): mov qword [obj+0x10], 0; 16-byte constant -> [obj+0x18].
void ctor_8bd90(std::uint8_t* obj) noexcept {
    store_u64(obj + 0x10, 0);
    store_u64(obj + 0x18, kSlot406ConstLo);
    store_u64(obj + 0x20, kSlot406ConstHi);
}

}  // namespace

// ---------------------------------------------------------------------------------------------- 404
void init_slot404_5e7c10(Slot404Buffer& buffer) noexcept {
    std::memset(buffer.bytes.data(), 0, buffer.bytes.size());   // tail call to memset(buf, 0, 0x51f8)
}

// ---------------------------------------------------------------------------------------------- 406
TinyMiscResult init_slot406_5dbe10(Slot406State& state, StartupAtexitLog& log) noexcept {
    for (std::size_t k = 0; k < 2; ++k) {
        ctor_8bd90(state.obj[k].data());
        if (!log.record(kSlot406DtorRva, state.obj[k].data())) return blocked("atexit_record");
    }
    return done();
}

// ---------------------------------------------------------------------------------------------- 409
Slot409Hooks default_slot409_hooks() noexcept {
    Slot409Hooks hooks;
    hooks.alloc_17070 = [](std::size_t size) noexcept -> void* { return std::malloc(size); };
    return hooks;
}

TinyMiscResult init_slot409_5b50f0(Slot409State& state, const Slot409Hooks& hooks,
                                   StartupAtexitLog& log) noexcept {
    if (hooks.alloc_17070 == nullptr) return blocked("alloc_hook");   // nothing written yet
    // vmovups [obj+8], xmm0(0): head and size are zero before the allocation.
    state.obj.head = nullptr;
    state.obj.size = 0;
    void* node = hooks.alloc_17070(kSlot409NodeSize);
    if (node == nullptr) return blocked("alloc");
    auto* bytes = static_cast<std::uint8_t*>(node);
    store_u64(bytes + 0x00, reinterpret_cast<std::uintptr_t>(node));   // left   = node
    store_u64(bytes + 0x08, reinterpret_cast<std::uintptr_t>(node));   // parent = node
    store_u64(bytes + 0x10, reinterpret_cast<std::uintptr_t>(node));   // right  = node
    bytes[kSlot409NodeColorOffset] = 1;
    bytes[kSlot409NodeIsNilOffset] = 1;
    state.node = node;
    state.obj.head = node;                                             // mov [0x2fda210], rax
    if (!log.record(kSlot409DtorRva, &state.obj)) return blocked("atexit_record");
    return done();
}

// ---------------------------------------------------------------------------------------------- 414
TinyMiscResult init_slot414_57b9b0(Slot414State& state, const Slot414Hooks& hooks,
                                   StartupAtexitLog& log) noexcept {
    if (hooks.mutex_init_3b310 == nullptr) return blocked("mutex_hook");
    std::uint8_t* obj = state.obj.data();
    if (!hooks.mutex_init_3b310(obj)) return blocked("mutex_init");
    if (!log.record(kSlot414DtorRva, obj)) return blocked("atexit_record");
    std::memset(obj + 8, 0, 4);        // mov dword [obj+8], 0
    std::memset(obj + 0x10, 0, 16);    // vmovups [obj+0x10], xmm0(0)
    return done();
}

// ---------------------------------------------------------------------------------------------- 494
TinyMiscResult init_slot494_26f7c0(Slot494State& state, const Slot494Hooks& hooks,
                                   StartupAtexitLog& log) noexcept {
    if (hooks.ctor_26e340 == nullptr) return blocked("ctor_26e340_hook");
    if (hooks.ctor_26e770 == nullptr) return blocked("ctor_26e770_hook");
    if (!hooks.ctor_26e340(state.obj[0].data())) return blocked("ctor_26e340");
    if (!log.record(kSlot494Dtor0Rva, state.obj[0].data())) return blocked("atexit_record0");
    if (!hooks.ctor_26e770(state.obj[1].data())) return blocked("ctor_26e770");
    if (!log.record(kSlot494Dtor1Rva, state.obj[1].data())) return blocked("atexit_record1");
    return done();
}

// ---------------------------------------------------------------------------------------------- 517
TinyMiscResult init_slot517_1aa510(Slot517State& state, StartupAtexitLog& log,
                                   const StoreTsc323d0Hooks& tsc_hooks) noexcept {
    state.cell = 0;
    if (!log.record(kSlot517DtorRva, &state.cell)) return blocked("atexit_record");
    // tail call 0x326a0(rec, 0): rec.flag = sil (0); shared guard/base cells; rec.tsc; rec.zero = 0.
    state.record.flag = 0;
    store_tsc_323d0(&state.record.tsc, tsc_hooks);
    state.record.zero = 0;
    return done();
}

// ---------------------------------------------------------------------------------------------- 519
TinyMiscResult init_slot519_19d2d0(Slot519State& state, StartupAtexitLog& log) noexcept {
    std::memset(state.obj.data(), 0, state.obj.size());                 // vmovups + qword zero
    if (!log.record(kSlot519DtorRva, state.obj.data())) return blocked("atexit_record");
    std::memset(state.buffer.data(), 0, state.buffer.size());           // memset(0x2ee4568, 0, 0x2000)
    state.cell.exchange(0);                                              // xchg [0x2ee6570], rbx(0)
    return done();
}

// ---------------------------------------------------------------------------------------------- 535
TinyMiscResult init_slot535_138e20(Slot535State& state, const Slot414Hooks& hooks,
                                   StartupAtexitLog& log) noexcept {
    if (hooks.mutex_init_3b310 == nullptr) return blocked("mutex_hook");
    if (!hooks.mutex_init_3b310(state.mutex.data())) return blocked("mutex_init");
    if (!log.record(kSlot535MutexDtorRva, state.mutex.data())) return blocked("atexit_record_mutex");
    std::memset(state.obj.data() + 8, 0, 0x18);                          // [+8,+0x18) vmovups, [+0x18,+0x20) qword
    if (!log.record(kSlot535ObjDtorRva, state.obj.data())) return blocked("atexit_record");
    return done();
}

// ---------------------------------------------------------------------------------------------- 541
void init_slot541_fd210(Slot541State& state) noexcept {
    state.hash0 = fnv_path_cstr_162020(kSlot541Name0, kFnvSeed);
    state.hash1 = fnv_path_cstr_162020(kSlot541Name1, kFnvSeed);
}

// ---------------------------------------------------------------------------------------------- 553
TinyMiscResult init_slot553_8bd40(Slot553State& state, StartupAtexitLog& log) noexcept {
    std::uint8_t* base = state.obj.data();
    const auto sentinel = reinterpret_cast<std::uintptr_t>(base + kSlot553SentinelOffset);   // 0x2ec0148
    store_u64(base + 0x20, sentinel);                                    // mov [0x2ec0140], rax
    store_u64(base + 0x00, sentinel);                                    // vmovdqu [0x2ec0120], {rax,rax}
    store_u64(base + 0x08, sentinel);
    store_u64(base + 0x10, reinterpret_cast<std::uintptr_t>(state.external.data()));   // 0x2ed1f48
    if (!log.record(kSlot553DtorRva, base)) return blocked("atexit_record");
    return done();
}

// ---------------------------------------------------------------------------------------------- 562
TinyMiscResult init_slot562_36100(Slot562State& state, StartupAtexitLog& log) noexcept {
    state.cell = 0;
    if (!log.record(kSlot562DtorRva, &state.cell)) return blocked("atexit_record");
    state.flag.exchange(kSlot562FlagValue);                              // mov eax,6; xchg [0x2ea4720], eax
    return done();
}

// ---------------------------------------------------------------------------------------------- 563
void init_slot563_32c70(Slot563Stamp& stamp, const StoreTsc323d0Hooks& tsc_hooks) noexcept {
    // 0x32c70 is 0x323d0 inlined: cmp qword [0x2ea4700],0 / hook / base stamp / stamp at 0x2ea4710.
    store_tsc_323d0(&stamp.tsc, tsc_hooks);
}

}  // namespace sotc
