#include "vslot_40_533100_header.hpp"

#include <cstring>

namespace sotc {
namespace {

void destroy_obj(void* obj, AllocatorFront& alloc,
                 const Vslot40_533100HeaderHooks& hooks) noexcept {
    if (obj == nullptr) {
        return;
    }
    if (hooks.destroy_prev != nullptr) {
        hooks.destroy_prev(obj);
    }
    // OBS ends with call 0x170b0 on the object after 0x3b440 cleanup.
    // When destroy_prev is provided it may already free; still delete_ for the
    // hooked/test path that only counts. Prefer double-free-safe tests that
    // null the cell first. Here: if destroy_prev ran, still delete_ (matches
    // OBS order after 0x3b440). If destroy_prev null, delete_ alone.
    alloc.delete_(obj);
}

}  // namespace

void*& vslot40_cell_2ef8c08() noexcept {
    static void* cell = nullptr;
    return cell;
}

void destroy_vslot40_at_2ef8c08(AllocatorFront& alloc,
                                const Vslot40_533100HeaderHooks& hooks) noexcept {
    void* prev = vslot40_cell_2ef8c08();
    vslot40_cell_2ef8c08() = nullptr;
    destroy_obj(prev, alloc, hooks);
}

Vslot40_533100HeaderResult vslot_40_533100_header(AllocatorFront& alloc, void* /*self*/,
                                                  void* /*arg*/,
                                                  const Vslot40_533100HeaderHooks& hooks) noexcept {
    Vslot40_533100HeaderResult out{};

    // OBS 0x533122: mov edi, 0x1c0; call 0x17070
    void* neu = alloc.new_(kVslot40533100ObjectBytes);
    if (neu == nullptr) {
        out.alloc_failed = true;
        return out;
    }

    if (hooks.ctor_493230 != nullptr) {
        hooks.ctor_493230(neu);
        out.ctor_ran = true;
    } else {
        std::memset(neu, 0, kVslot40533100ObjectBytes);
    }

    void*& cell = vslot40_cell_2ef8c08();
    void* prev = cell;
    // OBS 0x533148..0x5331ca: cmp; je skip; test; je store; destroy; store.
    if (prev == neu) {
        out.reused_same = true;
        out.obj = neu;
        out.ok = true;
        return out;
    }
    if (prev != nullptr) {
        // Clear cell before destroy so a re-entrant reader sees null.
        cell = nullptr;
        destroy_obj(prev, alloc, hooks);
        out.destroyed_prev = true;
    }
    cell = neu;
    out.obj = neu;
    out.ok = true;
    return out;
}

}  // namespace sotc
