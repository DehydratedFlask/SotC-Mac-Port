#include "registry_fill_52a220.hpp"

#include "fnv_cstr_161fb0.hpp"
#include "fnv_path_cstr_162020.hpp"
#include "registry_sorted_insert.hpp"

#include <cstring>

namespace sotc {
namespace {

void bind_vecs(RegistryObject2efd160& obj, RegistryVecA& a, RegistryVecB& b) noexcept {
    a.begin = static_cast<RegistryEntryA*>(obj.table_a_begin);
    a.end = static_cast<RegistryEntryA*>(obj.table_a_end);
    a.cap = static_cast<RegistryEntryA*>(obj.table_a_cap);
    b.begin = static_cast<RegistryEntryB*>(obj.table_b_begin);
    b.end = static_cast<RegistryEntryB*>(obj.table_b_end);
    b.cap = static_cast<RegistryEntryB*>(obj.table_b_cap);
}

void store_vecs(RegistryObject2efd160& obj, const RegistryVecA& a, const RegistryVecB& b) noexcept {
    obj.table_a_begin = a.begin;
    obj.table_a_end = a.end;
    obj.table_a_cap = a.cap;
    obj.table_b_begin = b.begin;
    obj.table_b_end = b.end;
    obj.table_b_cap = b.cap;
}

bool reserve_exact_a(RegistryVecA& vec, std::size_t count, AllocatorFront& alloc) {
    const std::size_t have =
        (vec.cap != nullptr && vec.begin != nullptr)
            ? static_cast<std::size_t>(vec.cap - vec.begin)
            : 0;
    if (have >= count) {
        return true;
    }
    auto* fresh = static_cast<RegistryEntryA*>(alloc.new_array(count * sizeof(RegistryEntryA)));
    if (fresh == nullptr && count != 0) {
        return false;
    }
    const std::size_t old_count =
        (vec.end != nullptr && vec.begin != nullptr) ? static_cast<std::size_t>(vec.end - vec.begin)
                                                     : 0;
    if (old_count != 0) {
        std::memcpy(fresh, vec.begin, old_count * sizeof(RegistryEntryA));
    }
    if (vec.begin != nullptr) {
        alloc.delete_array(vec.begin);
    }
    vec.begin = fresh;
    vec.end = fresh + old_count;
    vec.cap = fresh + count;
    return true;
}

bool reserve_exact_b(RegistryVecB& vec, std::size_t count, AllocatorFront& alloc) {
    const std::size_t have =
        (vec.cap != nullptr && vec.begin != nullptr)
            ? static_cast<std::size_t>(vec.cap - vec.begin)
            : 0;
    if (have >= count) {
        return true;
    }
    auto* fresh = static_cast<RegistryEntryB*>(alloc.new_array(count * sizeof(RegistryEntryB)));
    if (fresh == nullptr && count != 0) {
        return false;
    }
    const std::size_t old_count =
        (vec.end != nullptr && vec.begin != nullptr) ? static_cast<std::size_t>(vec.end - vec.begin)
                                                     : 0;
    if (old_count != 0) {
        std::memcpy(fresh, vec.begin, old_count * sizeof(RegistryEntryB));
    }
    if (vec.begin != nullptr) {
        alloc.delete_array(vec.begin);
    }
    vec.begin = fresh;
    vec.end = fresh + old_count;
    vec.cap = fresh + count;
    return true;
}

}  // namespace

RegistryFill52a220Status fill_registry_object_52a220(RegistryObject2efd160& obj,
                                                     InputTable551140& table,
                                                     AllocatorFront& alloc,
                                                     std::uint32_t count) {
    if (count > kInputTable551140Count) {
        return RegistryFill52a220Status::missing_names;
    }
    for (std::uint32_t i = 0; i != count; ++i) {
        if (table[i].name_a == nullptr || table[i].name_b == nullptr) {
            return RegistryFill52a220Status::missing_names;
        }
    }

    init_registry_object_header_52a220(obj, table.data());

    RegistryVecA a{};
    RegistryVecB b{};
    bind_vecs(obj, a, b);
    if (!reserve_exact_a(a, count, alloc) || !reserve_exact_b(b, count, alloc)) {
        store_vecs(obj, a, b);
        return RegistryFill52a220Status::alloc_failed;
    }
    store_vecs(obj, a, b);

    for (std::uint32_t i = 0; i != count; ++i) {
        InputTableEntry551140& entry = table[i];
        const std::uint32_t hash_a = fnv_cstr_52a990(entry.name_a);
        const std::uint32_t hash_b = fnv_cstr_a2ddc0(entry.name_b);
        entry.hash_a = hash_a;
        entry.hash_b = hash_b;

        bind_vecs(obj, a, b);
        RegistryEntryA* ea = registry_insert_unique_a(a, {hash_a, 0}, alloc);
        if (ea == nullptr) {
            store_vecs(obj, a, b);
            return RegistryFill52a220Status::alloc_failed;
        }
        ea->val = 0;  // OBS: fill loop always zeros +4 even on duplicate

        RegistryEntryB* eb = registry_insert_unique_b(b, {hash_a, 0, &entry}, alloc);
        if (eb == nullptr) {
            store_vecs(obj, a, b);
            return RegistryFill52a220Status::alloc_failed;
        }
        eb->ptr = &entry;  // OBS: last duplicate wins
        store_vecs(obj, a, b);
        (void)hash_b;  // stored on entry; table B keys use hash_a
    }
    return RegistryFill52a220Status::ok;
}

void clear_registry_object_tables_52a220(RegistryObject2efd160& obj,
                                         AllocatorFront& alloc) noexcept {
    RegistryVecA a{};
    RegistryVecB b{};
    bind_vecs(obj, a, b);
    registry_vec_a_clear(a, alloc);
    registry_vec_b_clear(b, alloc);
    store_vecs(obj, a, b);
}

}  // namespace sotc
