#include "helper_5b5530_vector_dtors.hpp"

#include <cstring>

namespace sotc {
namespace {

std::uint8_t* load_ptr(const void* base, std::size_t offset) noexcept {
    std::uint8_t* value;
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof value);
    return value;
}

std::uint64_t load_u64(const void* base, std::size_t offset) noexcept {
    std::uint64_t value;
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof value);
    return value;
}

void store_u64(void* base, std::size_t offset, std::uint64_t value) noexcept {
    std::memcpy(static_cast<std::uint8_t*>(base) + offset, &value, sizeof value);
}

void clear_header(std::uint8_t* object) noexcept {   // vmovdqu xmm0(zero) -> [obj+8] ; mov qword [obj+0x18], 0
    store_u64(object, kVectorHeaderOffset, 0);
    store_u64(object, kVectorHeaderOffset + 8, 0);
    store_u64(object, kVectorHeaderOffset + 0x10, 0);
}

}  // namespace

bool sso_vector_dtor_5b5530(void* object, const VectorDtorHooks& hooks) noexcept {
    auto* const obj = static_cast<std::uint8_t*>(object);
    std::uint8_t* const begin = load_ptr(obj, kVectorHeaderOffset);          // mov rbx,[r14+8]
    if (begin == nullptr) return true;                                       // test rbx,rbx ; je ret
    std::uint8_t* const end = load_ptr(obj, kVectorHeaderOffset + 8);        // mov r15,[r14+0x10]
    if (hooks.free_type1 == nullptr) return false;                           // call 0x170b0 is reachable
    for (std::uint8_t* element = begin; element != end; element += kSsoElementSize) {
        if (load_u64(element, kSsoElementCapacityOffset) >= 0x10) hooks.free_type1(load_ptr(element, kSsoElementBufferOffset));
        store_u64(element, kSsoElementSizeFieldOffset, 0);                   // vmovdqu {0, 0xf} -> [e+0x18]
        store_u64(element, kSsoElementCapacityOffset, kSsoInlineCapacity);
        element[kSsoElementBufferOffset] = 0;                                // mov byte [e+8], 0
    }
    hooks.free_type1(begin);                                                 // rdi = [r14] ; call 0x170b0
    clear_header(obj);
    return true;
}

bool inline_guard_dtor_5b55c0(void* object, const VectorDtorHooks& hooks) noexcept {
    std::uint8_t* const pointer = load_ptr(object, 0x00);                    // mov rdi,[rax]
    if (pointer == nullptr || pointer == load_ptr(object, 0x20)) return true;   // test ; cmp rdi,[rax+0x20]
    if (hooks.free_type2 == nullptr) return false;
    hooks.free_type2(pointer);                                               // jmp 0x170c0
    return true;
}

bool vec30_dtor_5c7c00(void* object, const VectorDtorHooks& hooks) noexcept {
    auto* const obj = static_cast<std::uint8_t*>(object);
    std::uint8_t* const begin = load_ptr(obj, kVectorHeaderOffset);
    if (begin == nullptr) return true;
    std::uint8_t* const end = load_ptr(obj, kVectorHeaderOffset + 8);
    if (hooks.free_type1 == nullptr) return false;
    for (std::uint8_t* element = begin; element != end; element += kVec30ElementSize) {
        std::uint8_t* const buffer = load_ptr(element, kVec30ElementBufferOffset);
        if (buffer == nullptr) continue;
        hooks.free_type1(buffer);                                            // call 0x170b0
        store_u64(element, kVec30ElementBufferOffset, 0);                    // vmovups zero -> [e+0x18]
        store_u64(element, kVec30ElementBufferOffset + 8, 0);
        store_u64(element, kVec30ElementBufferOffset + 0x10, 0);             // mov qword [e+0x28], 0
    }
    hooks.free_type1(begin);
    clear_header(obj);
    return true;
}

}  // namespace sotc
