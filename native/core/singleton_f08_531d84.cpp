#include "singleton_f08_531d84.hpp"

#include <cstring>

namespace sotc {
namespace {

void destroy_obj(void* obj, AllocatorFront& alloc, const SingletonF08Hooks& hooks) noexcept {
    if (obj == nullptr) {
        return;
    }
    if (hooks.dtor_525b40 != nullptr) {
        hooks.dtor_525b40(obj);
    }
    alloc.delete_(obj);
}

}  // namespace

void*& singleton_f08_cell_2efc9e0() noexcept {
    static void* cell = nullptr;
    return cell;
}

void destroy_singleton_f08_at_2efc9e0(AllocatorFront& alloc,
                                     const SingletonF08Hooks& hooks) noexcept {
    void* prev = singleton_f08_cell_2efc9e0();
    singleton_f08_cell_2efc9e0() = nullptr;
    destroy_obj(prev, alloc, hooks);
}

SingletonF08Result install_singleton_f08_531d84(AllocatorFront& alloc,
                                                const SingletonF08Hooks& hooks) noexcept {
    SingletonF08Result out{};

    // OBS 0x531d84: mov edi, 0xf08; call 0x17070 (allocator does not zero-fill).
    void* neu = alloc.new_(kSingletonF08ObjectBytes);
    if (neu == nullptr) {
        out.alloc_failed = true;
        return out;
    }

    if (hooks.ctor_523bd0 != nullptr) {
        hooks.ctor_523bd0(neu);
        out.ctor_ran = true;
    } else {
        // Original always constructs; native zeros so a later destroy/delete_ is safe
        // when the huge 0x523bd0 body is not modelled.
        std::memset(neu, 0, kSingletonF08ObjectBytes);
    }

    void*& cell = singleton_f08_cell_2efc9e0();
    void* prev = cell;
    // OBS 0x531daf..0x531dcc: cmp; je skip; test; je store; dtor+delete; store.
    if (prev == neu) {
        out.reused_same = true;
        out.obj = neu;
        out.ok = true;
        return out;
    }
    if (prev != nullptr) {
        destroy_obj(prev, alloc, hooks);
        out.destroyed_prev = true;
    }
    cell = neu;
    out.obj = neu;
    out.ok = true;
    return out;
}

}  // namespace sotc
