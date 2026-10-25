#include "owner_dtor_a5cb30.hpp"

#include <cstring>
#include <type_traits>

namespace sotc {
namespace {

void* load_ptr(const std::uint8_t* base, std::size_t offset) noexcept {
    void* value = nullptr;
    std::memcpy(&value, base + offset, sizeof value);
    return value;
}

void store_u64(std::uint8_t* base, std::size_t offset, std::uint64_t value) noexcept {
    std::memcpy(base + offset, &value, sizeof value);
}

void store_u32(std::uint8_t* base, std::size_t offset, std::uint32_t value) noexcept {
    std::memcpy(base + offset, &value, sizeof value);
}

void store_ptr(std::uint8_t* base, std::size_t offset, const void* value) noexcept {
    static_assert(sizeof(value) == sizeof(std::uint64_t));
    std::memcpy(base + offset, &value, sizeof value);
}

std::uint8_t* byte_add(std::uint8_t* base, std::size_t offset) noexcept {
    return reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uintptr_t>(base) + offset);
}

// The guest loops compare the cursor against `end` and add a constant stride. A span that is not
// an exact multiple would overrun in the guest; the lift reports it instead.
bool span_is_exact(const std::uint8_t* begin, const std::uint8_t* end,
                   std::size_t stride) noexcept {
    if (begin == nullptr || end == nullptr) {
        // A null cursor with a non-null limit would dereference null in the guest loop; only the
        // empty `begin == end == null` span is valid.
        return begin == end;
    }
    const auto first = reinterpret_cast<std::uintptr_t>(begin);
    const auto last = reinterpret_cast<std::uintptr_t>(end);
    return first <= last && (last - first) % stride == 0;
}

// ---- +0x8bd0 / +0x72a8: 0xc8-stride elements with a vtable slot 0, then the buffer free. -------
const char* teardown_element_vector(std::uint8_t* object, const OwnerA5C850Vector& vector,
                                    const OwnerDtorA5CB30Hooks& hooks, const char* span_step,
                                    const char* element_step, const char* buffer_step,
                                    OwnerDtorA5CB30Result& out) noexcept {
    auto* begin = static_cast<std::uint8_t*>(
        load_ptr(object, vector.header + kOwnerA5C850VectorBeginSlot));
    auto* end =
        static_cast<std::uint8_t*>(load_ptr(object, vector.header + kOwnerA5C850VectorEndSlot));
    if (!span_is_exact(begin, end, kOwnerA5C850StrideC8)) {
        return span_step;
    }
    for (auto* element = begin; element != end; element = byte_add(element, kOwnerA5C850StrideC8)) {
        if (hooks.element_release == nullptr) {
            return element_step;
        }
        void* const vptr = load_ptr(element, 0);
        if (vptr == nullptr) {
            return element_step;
        }
        if (!hooks.element_release(element, static_cast<void* const*>(vptr))) {
            return element_step;
        }
        ++out.element_releases;
    }
    // The guest reloads `begin` from the object before the free (`0xa5cba4`, `0xa5cbf4`); keep
    // that read so a hook that mutates the span cannot silently free the stale pointer.
    auto* const free_begin = static_cast<std::uint8_t*>(
        load_ptr(object, vector.header + kOwnerA5C850VectorBeginSlot));
    if (free_begin != nullptr) {
        const void* const marker = load_ptr(object, vector.header + kOwnerA5C850VectorMarkerSlot);
        if (static_cast<const void*>(free_begin) != marker) {
            if (hooks.buffer_free == nullptr || !hooks.buffer_free(free_begin)) {
                return buffer_step;
            }
            ++out.buffers_freed;
        }
    }
    return nullptr;
}

// ---- +0x6fe8 (two holders per element) and +0x6ac0 (one holder per element). -------------------
const char* teardown_shared_vector(std::uint8_t* object, const OwnerA5C850Vector& vector,
                                   std::size_t stride, bool two_holders,
                                   const OwnerDtorA5CB30Hooks& hooks, const char* span_step,
                                   const char* element_step, const char* buffer_step,
                                   OwnerDtorA5CB30Result& out) noexcept {
    auto* begin = static_cast<std::uint8_t*>(
        load_ptr(object, vector.header + kOwnerA5C850VectorBeginSlot));
    auto* end =
        static_cast<std::uint8_t*>(load_ptr(object, vector.header + kOwnerA5C850VectorEndSlot));
    if (!span_is_exact(begin, end, stride)) {
        return span_step;
    }
    for (auto* element = begin; element != end; element = byte_add(element, stride)) {
        if (hooks.shared_release == nullptr) {
            return element_step;
        }
        if (two_holders) {
            if (!hooks.shared_release(element + 0x28)) {
                return element_step;
            }
            ++out.shared_releases;
        }
        if (!hooks.shared_release(element)) {
            return element_step;
        }
        ++out.shared_releases;
    }
    auto* const free_begin = static_cast<std::uint8_t*>(
        load_ptr(object, vector.header + kOwnerA5C850VectorBeginSlot));
    if (free_begin != nullptr) {
        const void* const marker = load_ptr(object, vector.header + kOwnerA5C850VectorMarkerSlot);
        if (static_cast<const void*>(free_begin) != marker) {
            if (hooks.buffer_free == nullptr || !hooks.buffer_free(free_begin)) {
                return buffer_step;
            }
            ++out.buffers_freed;
        }
    }
    return nullptr;
}

// ---- +0x28: the 0x38-stride vector with the shared holder and the refcount pair. --------------
const char* teardown_head_vector(std::uint8_t* object, const OwnerA5C850Vector& vector,
                                 const OwnerDtorA5CB30Hooks& hooks, const char* span_step,
                                 const char* element_step, const char* conditional_step,
                                 const char* release_step, const char* buffer_step,
                                 OwnerDtorA5CB30Result& out) noexcept {
    auto* begin = static_cast<std::uint8_t*>(
        load_ptr(object, vector.header + kOwnerA5C850VectorBeginSlot));
    auto* end =
        static_cast<std::uint8_t*>(load_ptr(object, vector.header + kOwnerA5C850VectorEndSlot));
    if (!span_is_exact(begin, end, kOwnerA5C850Stride38)) {
        return span_step;
    }
    for (auto* element = begin; element != end; element = byte_add(element, kOwnerA5C850Stride38)) {
        if (hooks.shared_release == nullptr || !hooks.shared_release(element + 0x18)) {
            return element_step;
        }
        ++out.shared_releases;
        if (element[0x08] != 0) {
            if (hooks.ref_release_conditional == nullptr) {
                return conditional_step;
            }
            void* const target = load_ptr(element, 0);
            if (!hooks.ref_release_conditional(target)) {
                return conditional_step;
            }
            ++out.ref_conditionals;
            element[0x08] = 0;
        }
        if (hooks.ref_release == nullptr) {
            return release_step;
        }
        void* const target = load_ptr(element, 0);
        if (!hooks.ref_release(target)) {
            return release_step;
        }
        ++out.ref_releases;
        store_ptr(element, 0, nullptr);
    }
    auto* const free_begin = static_cast<std::uint8_t*>(
        load_ptr(object, vector.header + kOwnerA5C850VectorBeginSlot));
    if (free_begin != nullptr) {
        const void* const marker = load_ptr(object, vector.header + kOwnerA5C850VectorMarkerSlot);
        if (static_cast<const void*>(free_begin) != marker) {
            if (hooks.buffer_free == nullptr || !hooks.buffer_free(free_begin)) {
                return buffer_step;
            }
            ++out.buffers_freed;
        }
    }
    return nullptr;
}

}  // namespace

OwnerDtorA5CB30Result owner_dtor_a5cb30(void* object,
                                        const OwnerDtorA5CB30Hooks& hooks) noexcept {
    OwnerDtorA5CB30Result out{};
    const auto fail = [&out](const char* at) {
        out.ok = false;
        out.failed_at = at;
        return out;
    };

    if (object == nullptr) {
        return fail("null-object");
    }
    if (reinterpret_cast<std::uintptr_t>(object) % alignof(std::uint64_t) != 0) {
        return fail("object-align");
    }
    auto* const o = static_cast<std::uint8_t*>(object);

    // ---- 1. event flags: +0xa510 first, then +0xa500 (`0xa5cb3d`, `0xa5cb49`). ----------------
    if (hooks.event_flag_destroy == nullptr ||
        !hooks.event_flag_destroy(o + kOwnerA5C850Event1Offset)) {
        return fail("event_flag_1");
    }
    out.event_flags = 1;
    if (!hooks.event_flag_destroy(o + kOwnerA5C850Event0Offset)) {
        return fail("event_flag_0");
    }
    out.event_flags = 2;

    // ---- 2. `+0xa4f8`: vtable slot +8, then clear the field (`0xa5cb55..0xa5cb67`). -----------
    {
        void* const polymorphic = load_ptr(o, kOwnerA5C850PolyOffset);
        if (polymorphic != nullptr) {
            if (hooks.poly_release == nullptr) {
                return fail("poly_release");
            }
            void* const vptr = load_ptr(static_cast<std::uint8_t*>(polymorphic), 0);
            if (vptr == nullptr ||
                !hooks.poly_release(polymorphic, static_cast<void* const*>(vptr))) {
                return fail("poly_release");
            }
            ++out.polymorphic_releases;
            store_ptr(o, kOwnerA5C850PolyOffset, nullptr);
        }
    }

    // ---- 3. `+0x8bd0` vector (`0xa5cb72..0xa5cbc1`). ------------------------------------------
    if (const char* const at = teardown_element_vector(
            o, kOwnerA5C850Vectors[5], hooks, "vector_8bd0_span", "vector_8bd0_element",
            "vector_8bd0_buffer", out)) {
        return fail(at);
    }

    // ---- 4. `+0x72a8` vector (`0xa5cbc1..0xa5cc11`). ------------------------------------------
    if (const char* const at = teardown_element_vector(
            o, kOwnerA5C850Vectors[4], hooks, "vector_72a8_span", "vector_72a8_element",
            "vector_72a8_buffer", out)) {
        return fail(at);
    }

    // ---- 5. tree `+0x7270` -> 0xb09690 (`0xa5cc11`). ------------------------------------------
    if (hooks.tree_destroy == nullptr || !hooks.tree_destroy(o + kOwnerA5C850TreeOffset)) {
        return fail("tree_destroy");
    }
    ++out.tree_destroys;

    // ---- 6. shared `+0x7250` -> 0xf590 (`0xa5cc1d`). -------------------------------------------
    if (hooks.shared_release == nullptr || !hooks.shared_release(o + kOwnerA5C850SharedOffset)) {
        return fail("shared_7250");
    }
    ++out.shared_releases;

    // ---- 7. `+0x6fe8` vector: +0x28 before +0x00 (`0xa5cc29..0xa5cc77`). ---------------------
    if (const char* const at = teardown_shared_vector(
            o, kOwnerA5C850Vectors[3], kOwnerA5C850Stride48, true, hooks, "vector_6fe8_span",
            "vector_6fe8_element", "vector_6fe8_buffer", out)) {
        return fail(at);
    }

    // ---- 8. `+0x6ac0` vector: one holder per element (`0xa5cc77..0xa5ccbe`). -----------------
    if (const char* const at = teardown_shared_vector(
            o, kOwnerA5C850Vectors[2], kOwnerA5C850Stride28, false, hooks, "vector_6ac0_span",
            "vector_6ac0_element", "vector_6ac0_buffer", out)) {
        return fail(at);
    }

    // ---- 9. `+0x68a0` POD buffer (`0xa5ccbe..0xa5ccd8`). --------------------------------------
    {
        auto* const begin =
            static_cast<std::uint8_t*>(load_ptr(o, kOwnerA5C850Vectors[1].header + 0x00));
        if (begin != nullptr) {
            const void* const marker = load_ptr(o, kOwnerA5C850Vectors[1].header + 0x20);
            if (static_cast<const void*>(begin) != marker) {
                if (hooks.buffer_free == nullptr || !hooks.buffer_free(begin)) {
                    return fail("vector_68a0_buffer");
                }
                ++out.buffers_freed;
            }
        }
    }

    // ---- 10. drain `+0x5c60` -> 0xa62340 (`0xa5ccd8`). ---------------------------------------
    {
        auto* const base = o + kOwnerA5C850LaneAtomicOffset;
        if (load_ptr(base, kOwnerA5C850DrainWordOffset) != nullptr) {
            // Armed: 0xa62340 waits on the flag word and walks the lane vtables. A host contract.
            if (hooks.drain_armed == nullptr || !hooks.drain_armed(base)) {
                return fail("drain");
            }
        } else {
            // The constructor's `+0x5c70 == 0` path through 0xa62340: skip the spin and the lane
            // walk, then execute the three unconditional tail stores (`0xa62412..0xa6242a`).
            store_u64(base, kOwnerA5C850DrainTailMaskOffset, 0u);
            store_u32(base, kOwnerA5C850DrainTailCountOffset, 0u);
            store_u64(base, kOwnerA5C850DrainTailSeqOffset, ~std::uint64_t{0});
        }
        out.drain_ran = true;
    }

    // ---- 11. `+0x28` vector (`0xa5cce4..0xa5cd4a`). -------------------------------------------
    if (const char* const at = teardown_head_vector(
            o, kOwnerA5C850Vectors[0], hooks, "vector_28_span", "vector_28_element",
            "vector_28_conditional", "vector_28_release", "vector_28_buffer", out)) {
        return fail(at);
    }

    // ---- 12. head gate (`0xa5cd4a..0xa5cd77`). ------------------------------------------------
    // The gate byte is 0 for every object the constructor produced; only then is the `+0x18` byte
    // read, so the constructor's one-byte `+0x10` store and the untouched `+0x18` byte are inert.
    if (o[kOwnerA5C850HeadGateOffset] != 0) {
        if (o[kOwnerA5C850HeadFlagOffset] != 0) {
            if (hooks.ref_release_conditional == nullptr) {
                return fail("head_conditional");
            }
            void* const target = load_ptr(o, kOwnerA5C850HeadPtrOffset);
            if (!hooks.ref_release_conditional(target)) {
                return fail("head_conditional");
            }
            ++out.ref_conditionals;
            o[kOwnerA5C850HeadFlagOffset] = 0;
        }
        if (hooks.ref_release == nullptr) {
            return fail("head_release");
        }
        void* const target = load_ptr(o, kOwnerA5C850HeadPtrOffset);
        if (!hooks.ref_release(target)) {
            return fail("head_release");
        }
        ++out.ref_releases;
        store_ptr(o, kOwnerA5C850HeadPtrOffset, nullptr);
    }

    out.ok = true;
    out.failed_at = nullptr;
    return out;
}

}  // namespace sotc
