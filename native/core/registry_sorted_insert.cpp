#include "registry_sorted_insert.hpp"

#include <cstring>

namespace sotc {
namespace {

template <typename Entry>
Entry* lower_bound(Entry* begin, Entry* end, std::uint32_t key) noexcept {
    std::size_t count = static_cast<std::size_t>(end - begin);
    Entry* first = begin;
    while (count > 0) {
        const std::size_t step = count / 2;
        Entry* mid = first + step;
        if (mid->key < key) {
            first = mid + 1;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return first;
}

template <typename Entry>
bool grow_double(Entry*& begin, Entry*& end, Entry*& cap, std::size_t insert_index, const Entry& entry,
                 AllocatorFront& alloc) {
    const std::size_t old_count = static_cast<std::size_t>(end - begin);
    const std::size_t old_bytes = old_count * sizeof(Entry);
    // OBS: empty -> 1 entry; else double byte capacity (= double entry count).
    const std::size_t new_count = (old_count == 0) ? 1 : (old_count * 2);
    const std::size_t new_bytes = new_count * sizeof(Entry);
    auto* fresh = static_cast<Entry*>(alloc.new_array(new_bytes));
    if (fresh == nullptr) {
        return false;
    }
    // Copy [begin, insert) then entry then [insert, end).
    if (insert_index > 0) {
        std::memcpy(fresh, begin, insert_index * sizeof(Entry));
    }
    fresh[insert_index] = entry;
    if (insert_index < old_count) {
        std::memcpy(fresh + insert_index + 1, begin + insert_index,
                    (old_count - insert_index) * sizeof(Entry));
    }
    if (begin != nullptr) {
        alloc.delete_array(begin);
    }
    begin = fresh;
    end = fresh + old_count + 1;
    cap = fresh + new_count;
    (void)old_bytes;
    return true;
}

template <typename Entry>
Entry* insert_unique(Entry*& begin, Entry*& end, Entry*& cap, const Entry& entry, AllocatorFront& alloc) {
    Entry* pos = lower_bound(begin, end, entry.key);
    if (pos != end && pos->key == entry.key) {
        return pos;  // duplicate: leave existing entry unchanged
    }
    const std::size_t index = static_cast<std::size_t>(pos - begin);
    if (end != cap) {
        // Shift [pos, end) one slot right, then write.
        if (pos != end) {
            std::memmove(pos + 1, pos, static_cast<std::size_t>(end - pos) * sizeof(Entry));
        }
        *pos = entry;
        ++end;
        return pos;
    }
    if (!grow_double(begin, end, cap, index, entry, alloc)) {
        return nullptr;
    }
    return begin + index;
}

}  // namespace

RegistryEntryA* registry_insert_unique_a(RegistryVecA& vec, const RegistryEntryA& entry,
                                         AllocatorFront& alloc) {
    return insert_unique(vec.begin, vec.end, vec.cap, entry, alloc);
}

RegistryEntryB* registry_insert_unique_b(RegistryVecB& vec, const RegistryEntryB& entry,
                                         AllocatorFront& alloc) {
    RegistryEntryB cleaned = entry;
    cleaned.pad = 0;
    return insert_unique(vec.begin, vec.end, vec.cap, cleaned, alloc);
}

void registry_vec_a_clear(RegistryVecA& vec, AllocatorFront& alloc) noexcept {
    if (vec.begin != nullptr) {
        alloc.delete_array(vec.begin);
    }
    vec.begin = vec.end = vec.cap = nullptr;
}

void registry_vec_b_clear(RegistryVecB& vec, AllocatorFront& alloc) noexcept {
    if (vec.begin != nullptr) {
        alloc.delete_array(vec.begin);
    }
    vec.begin = vec.end = vec.cap = nullptr;
}

}  // namespace sotc
