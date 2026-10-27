#include "ctor_tiny_atexit_slots.hpp"

#include <cstring>

namespace sotc {

const TinyAtexitRow* find_tiny_atexit_row(std::uint32_t slot) noexcept {
    for (const TinyAtexitRow& row : kTinyAtexitRows) {
        if (row.slot == slot) return &row;
    }
    return nullptr;
}

std::size_t tiny_atexit_row_index(std::uint32_t slot) noexcept {
    for (std::size_t i = 0; i < kTinyAtexitRows.size(); ++i) {
        if (kTinyAtexitRows[i].slot == slot) return i;
    }
    return kTinyAtexitRowCount;
}

bool init_tiny_atexit_record(const TinyAtexitRecord& record, std::uint8_t* object,
                             StartupAtexitLog& log) noexcept {
    // The original stores (xmm/qword zero) come first, then the tail/real call to __cxa_atexit.
    std::memset(object + record.zero_offset, 0, record.zero_length);
    return log.record(record.dtor_rva, object);
}

TinyAtexitResult init_tiny_atexit_slot(std::uint32_t slot, TinyAtexitState& state,
                                       StartupAtexitLog* log) noexcept {
    TinyAtexitResult result;
    result.slot = slot;
    const std::size_t index = tiny_atexit_row_index(slot);
    if (index == kTinyAtexitRowCount) {
        result.blocked = "unknown_slot";
        return result;
    }
    if (log == nullptr) {
        result.blocked = "atexit_log";
        return result;
    }
    const TinyAtexitRow& row = kTinyAtexitRows[index];
    for (std::uint32_t r = 0; r < row.record_count; ++r) {
        if (!init_tiny_atexit_record(row.rec[r], state.objects[index][r].bytes.data(), *log)) {
            result.blocked = "atexit_record";
            return result;
        }
        ++result.records_done;
    }
    result.ok = true;
    return result;
}

}  // namespace sotc
