// CPU-01-STARTUP-BATCH-14: shared bodies of the generated startup-row kinds that need a host input or a helper that is not a console object.
//
//  * empty tree header (slot 390 at 0x30157c8; the same shape as slot 409's tree, with a 0x30 byte node): the original zeroes [obj+0x08, obj+0x18),
//    calls the allocator wrapper 0x17070(node_size), stores left = parent = right = node, byte [node+0x18] = byte [node+0x19] = 1, stores
//    [obj+0x08] = node and records __cxa_atexit(dtor, obj). The allocator is a HOST INPUT (the engine allocator 0x12820 is not lifted); a missing
//    allocator or a null result refuses the row visibly with the header zeroed and no atexit record. The tree destructor (0x6918e0) is an
//    IDENTITY TAG, recorded and never run; the node block is never freed here.
//  * node-reference record (slot 390 at 0x30157e0): 0x56b4b0(obj) (native, helper_56b4b0_node_ref.hpp) then __cxa_atexit(0x560540, obj).
//
//  * inline-built ranged enum variable (slot 402 at 0x300c538, "CP10_AnimMode"): the compiler inlined a 0xd8 byte ranged-int constructor, so the row is
//    0x20af0(obj, name, empty description, flags) with the stores [obj] = mid vptr, [+0x60] = &value (+0xd0), dword [+0x6c] = min, byte [+0x70] = 1,
//    dword [+0x74] = max, byte [+0x78] = 1, ymm [+0x80] = 0, [+0xb0] = 0, [+0xc0] = functor class tag, [+0xc8] = 0, then f590(description), [obj] = final
//    vptr, dword [+0xd0] = value, dword [+0x68] = default, 0x20d60 register, f590(name) and __cxa_atexit(dtor, obj). No functor is installed (the
//    tag is stored directly). Class vtables, the tag and the destructor are IDENTITY TAGS.
//
// NOTHING here executes, loads or calls an original RVA. STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED only.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "helper_56b4b0_node_ref.hpp"

namespace sotc {

inline constexpr std::size_t kEmptyTreeNodeColorOffset = 0x18;
inline constexpr std::size_t kEmptyTreeNodeIsNilOffset = 0x19;

struct EmptyTreeHeader {
    std::uint64_t q00;      // never written
    void* head;             // +0x08 -> the sentinel node
    std::uint64_t size;     // +0x10 zero
};
static_assert(sizeof(EmptyTreeHeader) == 0x18);

// Host stand-in for the allocator wrapper 0x17070(size). nullptr == no allocator supplied (the row is refused).
struct EmptyTreeAlloc {
    void* (*allocate_17070)(std::size_t size) noexcept = nullptr;
};
[[nodiscard]] EmptyTreeAlloc default_empty_tree_alloc() noexcept;   // std::malloc, NOT the engine allocator; release with std::free

[[nodiscard]] bool build_empty_tree(EmptyTreeHeader& header, std::size_t node_size, const EmptyTreeAlloc& alloc, std::uint64_t dtor_rva,
                                    ConCmdRegistry& concmd) noexcept;

[[nodiscard]] bool build_node_ref_row(NodeRef56b4b0& ref, std::uint64_t dtor_rva, ConCmdRegistry& concmd) noexcept;

struct InlineEnumSpec {
    const char* name;
    std::size_t name_length;
    std::uint32_t flags;
    std::uint64_t mid_vptr;     // [obj] after the base constructor
    std::uint64_t final_vptr;   // [obj] before the base constructor and again before the registration
    std::uint64_t fn_tag;       // [obj+0xc0], stored directly
    std::int32_t min_value;     // dword [obj+0x6c]
    std::int32_t max_value;     // dword [obj+0x74]
    std::int32_t value;         // dword [obj+0xd0]
    std::int32_t default_value; // dword [obj+0x68]
    std::uint64_t dtor_rva;     // __cxa_atexit target (final class slot 0)
    bool bounded = true;        // false: the constructor stores ONE zero byte at +0x6c / +0x74 and clears the engaged bytes (+0x70 / +0x78); the other bound bytes are never written (slot 534)
};

struct InlineEnumResult {
    bool ok = false;
    ConCmdRegisterResult registration{};
    bool views_inert = false;
    bool atexit_recorded = false;
};

[[nodiscard]] InlineEnumResult init_inline_enum_var(ConCmdRangedVar& var, const InlineEnumSpec& spec, ConCmdRegistry& concmd) noexcept;

// The two virtual-slot results 0x26db0 / 0x26ea0 read from a variable of the given final class (`known` only when [obj] is exactly that class).
[[nodiscard]] DebugMenuTarget inline_enum_menu_target(const ConCmdRangedVar& var, std::uint64_t final_vptr, std::uint32_t slot3, std::uint32_t slot6) noexcept;

}  // namespace sotc
