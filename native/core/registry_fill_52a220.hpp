// Fill-loop deepen of registry ctor 0x52a220 (after header + capacity reserve).
// ELF sha256 d0723afb...5861d3. OBS from offline objdump of 0x52a520..0x52a67a:
//   for each input-table entry (stride 0x60):
//     hash_a = fnv_cstr_161fb0(name_a, 0x811c9dc5)  → entry+0x58
//     hash_b = fnv_cstr_a2ddc0(name_b)               → entry+0x5c
//     insert/find table A with key=hash_a, then force val=0
//     insert/find table B with key=hash_a, then force ptr=&entry (last duplicate wins)
// Pre-loop: reserve exact `count` slots for A (8-B) and B (16-B) via new[] 0x17090.
//
// NOT modelled: pool globals 0x2efd170/190/1b0, listener groups, second debug-name loop
// after 0x52a680, stack-canary / __stack_chk_fail. Name strings come from a private fixture
// (fail-visible if missing). Not registered into recovered_constructors().
#pragma once

#include "allocator_front.hpp"
#include "input_table_551140.hpp"
#include "registry_object_2efd160.hpp"

#include <cstdint>

namespace sotc {

enum class RegistryFill52a220Status : std::uint8_t {
    ok = 0,
    missing_names = 1,  // name_a/name_b null on an entry
    alloc_failed = 2,
};

// Header + exact reserve(count) + hash/insert loop. Mutates `table` hashes at +0x58/+0x5c.
// Requires every entry's name_a/name_b non-null (apply private fixture first).
// On failure, `obj` / vectors may be partially filled — caller should clear.
[[nodiscard]] RegistryFill52a220Status fill_registry_object_52a220(RegistryObject2efd160& obj,
                                                                   InputTable551140& table,
                                                                   AllocatorFront& alloc,
                                                                   std::uint32_t count =
                                                                       kInputTable551140CountU32);

// Release A/B storage owned by obj (listeners left alone — still null in this deepen).
void clear_registry_object_tables_52a220(RegistryObject2efd160& obj,
                                         AllocatorFront& alloc) noexcept;

}  // namespace sotc
