#include "startup_row_kinds.hpp"

#include <cstdlib>
#include <cstring>

#include "l2_slot_common.hpp"

namespace sotc {

EmptyTreeAlloc default_empty_tree_alloc() noexcept {
    EmptyTreeAlloc a;
    a.allocate_17070 = [](std::size_t size) noexcept -> void* { return std::malloc(size); };
    return a;
}

bool build_empty_tree(EmptyTreeHeader& header, std::size_t node_size, const EmptyTreeAlloc& alloc, std::uint64_t dtor_rva,
                      ConCmdRegistry& concmd) noexcept {
    header.head = nullptr;                                  // vmovups [obj+8], xmm0 (0): head and size, before the allocation
    header.size = 0;
    if (alloc.allocate_17070 == nullptr) return false;      // no host allocator: the original would fault, we refuse
    auto* node = static_cast<std::uint8_t*>(alloc.allocate_17070(node_size));   // call 0x17070(node_size)
    if (node == nullptr) return false;
    const std::uint64_t self = reinterpret_cast<std::uintptr_t>(node);
    std::memcpy(node + 0x00, &self, 8);                     // left   = node
    std::memcpy(node + 0x08, &self, 8);                     // parent = node
    std::memcpy(node + 0x10, &self, 8);                     // right  = node
    node[kEmptyTreeNodeColorOffset] = 1;
    node[kEmptyTreeNodeIsNilOffset] = 1;
    header.head = node;                                     // mov [obj+8], rax
    return concmd.atexit_log.record(dtor_rva, &header);     // __cxa_atexit(dtor, obj, &dso)
}

bool build_node_ref_row(NodeRef56b4b0& ref, std::uint64_t dtor_rva, ConCmdRegistry& concmd) noexcept {
    build_node_ref_56b4b0(ref);                             // call 0x56b4b0(obj)
    return concmd.atexit_log.record(dtor_rva, &ref);        // __cxa_atexit(0x560540, obj, &dso)
}

InlineEnumResult init_inline_enum_var(ConCmdRangedVar& var, const InlineEnumSpec& spec, ConCmdRegistry& concmd) noexcept {
    InlineEnumResult out{};
    const SharedStr name = l2::measured_view(spec.name, spec.name_length);
    const SharedStr description = l2::measured_view("", 0);   // the stack view {static empty literal, 0, 0, 0}
    var.base.vptr = spec.final_vptr;                          // pre-store, overwritten by the base constructor
    init_concmd_base_20af0(var.base, name, description, spec.flags);
    var.base.vptr = spec.mid_vptr;
    var.value_ref = &var.value;                               // [obj+0x60] = obj + 0xd0
    var.min_value[0] = 0;
    if (spec.bounded) {
        std::memcpy(var.min_value, &spec.min_value, 4);       // dword [obj+0x6c]
        var.min_engaged = 1;                                  // byte  [obj+0x70]
        std::memcpy(var.max_value, &spec.max_value, 4);       // dword [obj+0x74]
        var.max_engaged = 1;                                  // byte  [obj+0x78]
    } else {
        var.max_value[0] = 0;                                 // byte  [obj+0x74] (the other three bytes are never written)
        var.min_engaged = 0;                                  // byte  [obj+0x70]
        var.max_engaged = 0;                                  // byte  [obj+0x78]
    }
    std::memset(var.args, 0, sizeof var.args);                // ymm   [obj+0x80] = 0
    var.fn_payload = 0;                                       // [obj+0xb0]
    var.fn_vptr = spec.fn_tag;                                // [obj+0xc0], stored directly
    var.fn_aux = 0;                                           // [obj+0xc8]
    const bool desc_inert = release_inert_view_f590(description);
    var.base.vptr = spec.final_vptr;
    var.value = static_cast<std::uint32_t>(spec.value);
    var.default_value = static_cast<std::uint32_t>(spec.default_value);
    out.registration = register_concmd_20d60(var.base, concmd);
    const bool name_inert = release_inert_view_f590(name);
    out.views_inert = desc_inert && name_inert;
    out.atexit_recorded = concmd.atexit_log.record(spec.dtor_rva, &var);
    out.ok = out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

DebugMenuTarget inline_enum_menu_target(const ConCmdRangedVar& var, std::uint64_t final_vptr, std::uint32_t slot3, std::uint32_t slot6) noexcept {
    DebugMenuTarget target{};
    target.object = &var;
    target.known = var.base.vptr == final_vptr;
    target.slot3 = target.known ? slot3 : 0;
    target.slot6 = target.known ? slot6 : 0;
    return target;
}

}  // namespace sotc
