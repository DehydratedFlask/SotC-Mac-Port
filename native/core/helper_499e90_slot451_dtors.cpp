#include "helper_499e90_slot451_dtors.hpp"

#include <cstring>

namespace sotc {
namespace {

void* load_ptr(const void* base, std::size_t offset) noexcept {
    void* value;
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof value);
    return value;
}

}  // namespace

bool critsec_dtor_3b380(void* object, const Slot451DtorHooks& hooks) noexcept {
    if (hooks.critsec_destroy == nullptr) return false;
    hooks.critsec_destroy(object);                         // jmp 0xf8f8c0
    return true;
}

bool six_buffer_dtor_49c9e0(void* object, const Slot451DtorHooks& hooks) noexcept {
    if (hooks.free_type2 == nullptr || hooks.critsec_destroy == nullptr) return false;
    for (const std::size_t offset : kSixBufferOffsets49c9e0) {
        if (void* const buffer = load_ptr(object, offset)) hooks.free_type2(buffer);   // mov rdi,[rbx+off] ; test ; call 0x170c0
    }
    return critsec_dtor_3b380(object, hooks);              // mov rdi,rbx ; jmp 0x3b380
}

bool tail_dtor_499e50(void* object, const Slot451DtorHooks& hooks) noexcept {
    return six_buffer_dtor_49c9e0(object, hooks);          // jmp 0x49c9e0
}

bool buffer_dtor_499e60(void* object, const Slot451DtorHooks& hooks) noexcept {
    if (hooks.free_type2 == nullptr || hooks.critsec_destroy == nullptr) return false;
    if (void* const buffer = load_ptr(object, kBufferDtor499e60BufferOffset)) hooks.free_type2(buffer);   // [rbx+8]
    return critsec_dtor_3b380(object, hooks);              // jmp 0x3b380
}

bool node_dtor_499e90(void** slot, const Slot451DtorHooks& hooks) noexcept {
    void* const node = *slot;                              // mov rbx,[r14] ; test rbx,rbx ; je ret
    if (node == nullptr) return true;
    if (hooks.free_type1 == nullptr || hooks.free_type2 == nullptr || hooks.critsec_destroy == nullptr) return false;
    if (void* const outer = load_ptr(node, kNodeDtor499e90OuterOffset)) hooks.free_type2(outer);   // [rbx+0x28]
    if (void* const inner = load_ptr(node, kNodeDtor499e90InnerOffset)) hooks.free_type2(inner);   // [rbx+8]
    if (!critsec_dtor_3b380(node, hooks)) return false;    // call 0x3b380
    hooks.free_type1(node);                                // call 0x170b0
    *slot = nullptr;                                       // mov qword [r14],0
    return true;
}

}  // namespace sotc
