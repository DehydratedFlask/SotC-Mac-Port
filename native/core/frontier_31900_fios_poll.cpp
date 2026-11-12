#include "frontier_31900_fios_poll.hpp"

#include <cstring>

namespace sotc {
namespace {

template <typename T>
T load(const std::uint8_t* base, std::uint32_t offset) noexcept {
    T value{};
    std::memcpy(&value, base + offset, sizeof(T));
    return value;
}

template <typename T>
void store(std::uint8_t* base, std::uint32_t offset, T value) noexcept {
    std::memcpy(base + offset, &value, sizeof(T));
}

Frontier31900Result missing(Frontier31900Result out, const char* name) noexcept {
    out.ok = false;
    out.missing_hook = name;
    return out;
}

}  // namespace

Frontier31900Result frontier_31900_fios_poll(const Frontier31900Cells& cells,
                                             const Frontier31900Hooks& hooks) noexcept {
    Frontier31900Result out{};
    std::uint8_t* const obj = cells.object;
    if (obj == nullptr) {
        return out;
    }
    out.state_before = load<std::uint32_t>(obj, kFrontier31900StateOffset);
    if (out.state_before == 0) {  // 0x3190d: nothing outstanding
        out.settled = true;
        out.ok = true;
        return out;
    }

    if (hooks.op_is_done == nullptr) {
        return missing(out, "op_is_done");
    }
    out.is_done_called = true;
    out.done = hooks.op_is_done(load<std::uint32_t>(obj, kFrontier31900OpOffset), hooks.user);
    if (!out.done) {  // 0x31919: still running, return state == 0 (false)
        out.settled = load<std::uint32_t>(obj, kFrontier31900StateOffset) == 0;
        out.ok = true;
        return out;
    }

    // Every path from here reaches the tail (0x31971), so its hooks are needed unconditionally.
    if (hooks.op_get_error == nullptr) return missing(out, "op_get_error");
    if (hooks.op_get_actual_count == nullptr) return missing(out, "op_get_actual_count");
    if (hooks.op_delete == nullptr) return missing(out, "op_delete");

    out.error = hooks.op_get_error(load<std::uint32_t>(obj, kFrontier31900OpOffset), hooks.user);
    out.count = hooks.op_get_actual_count(load<std::uint32_t>(obj, kFrontier31900OpOffset), hooks.user);

    const std::uint32_t state = load<std::uint32_t>(obj, kFrontier31900StateOffset);
    const std::uint32_t arm = state - 1u;  // `dec ecx; cmp ecx,3; ja`: unsigned
    const bool wants_size = arm == 0 && out.error == 0 &&
                            (load<std::uint8_t>(obj, kFrontier31900FlagsOffset) &
                             kFrontier31900FlagsSizeMask) != 0;
    if (wants_size && hooks.file_size == nullptr) {
        return missing(out, "file_size");  // before any write: the object is untouched
    }

    store<std::int32_t>(obj, kFrontier31900ErrorOffset, out.error);
    store<std::int32_t>(obj, kFrontier31900CountOffset, out.count);

    if (arm == 0) {  // jump-table entry 0 -> 0x31953
        if (out.error != 0) {
            store<std::uint32_t>(obj, kFrontier31900HandleOffset, 0);
        } else {  // 0x31995
            store<std::uint32_t>(obj, kFrontier31900ModeOffset, 1);  // dword write only
            if (wants_size) {
                out.size_called = true;
                store<std::uint64_t>(obj, kFrontier31900CursorOffset, hooks.file_size(obj, hooks.user));
            }
        }
    } else if (arm == 1) {  // entry 1 -> 0x31962
        store<std::uint32_t>(obj, kFrontier31900HandleOffset, 0);
        store<std::uint64_t>(obj, kFrontier31900ModeOffset, 0);  // qword write
    } else if (arm == 2 || arm == 3) {  // entries 2, 3 -> 0x3194b (cdqe; add)
        const std::uint64_t cursor = load<std::uint64_t>(obj, kFrontier31900CursorOffset);
        const std::uint64_t delta = static_cast<std::uint64_t>(static_cast<std::int64_t>(out.count));
        store<std::uint64_t>(obj, kFrontier31900CursorOffset, cursor + delta);
    }
    // anything else falls straight to 0x31971

    store<std::uint32_t>(obj, kFrontier31900StateOffset, 0);  // 0x31971
    hooks.op_delete(load<std::uint32_t>(obj, kFrontier31900OpOffset), hooks.user);
    out.deleted = true;
    store<std::uint32_t>(obj, kFrontier31900OpOffset, 0);

    out.settled = load<std::uint32_t>(obj, kFrontier31900StateOffset) == 0;  // 0x31987
    out.ok = true;
    return out;
}

}  // namespace sotc
