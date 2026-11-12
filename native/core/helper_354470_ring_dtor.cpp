#include "helper_354470_ring_dtor.hpp"

#include <cstring>

namespace sotc {
namespace {

void* load_ptr(const void* base, std::size_t offset) noexcept {
    void* value;
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof value);
    return value;
}

}  // namespace

RingDtorResult ring_dtor_354470(void* object, const RingDtorHooks& hooks) noexcept {
    RingDtorResult out;
    auto* const begin = static_cast<std::uint8_t*>(load_ptr(object, 0x00));   // mov rbx,[r14]
    auto* const end = static_cast<std::uint8_t*>(load_ptr(object, 0x08));     // mov r15,[r14+8]
    if (begin != end && hooks.free_type2 == nullptr) return out;              // a reachable free with no hook is not faked
    out.ok = true;
    for (std::uint8_t* element = begin; element != end; element += kRingDtorElementSize) {
        ++out.elements;
        void* const buffer = load_ptr(element, kRingDtorElementBufferOffset);
        if (buffer != nullptr && buffer != load_ptr(element, kRingDtorElementInlineOffset)) {
            if (hooks.free_type2 == nullptr) { out.ok = false; return out; }
            hooks.free_type2(buffer);                                          // call 0x170c0
            ++out.buffers_freed;
        }
        SharedHolderF590 holder;
        std::memcpy(&holder, element + kRingDtorElementHolderOffset, sizeof holder);   // lea rdi,[rbx+8]
        const SharedReleaseF590Result released = shared_release_f590(holder, hooks.release);   // call 0xf590
        if (!released.ok) { out.ok = false; return out; }
        if (released.was_last) ++out.releases_last;
    }
    void* const ring = load_ptr(object, 0x00);                                 // mov rbx,[r14] (reloaded after the loop)
    if (ring != nullptr && ring != load_ptr(object, kRingDtorInlineStorageOffset)) {
        if (hooks.free_type2 == nullptr) { out.ok = false; return out; }
        hooks.free_type2(ring);                                                // jmp 0x170c0
        out.ring_buffer_freed = true;
    }
    return out;
}

}  // namespace sotc
