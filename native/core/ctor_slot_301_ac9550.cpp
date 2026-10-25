#include "ctor_slot_301_ac9550.hpp"

#include <cstring>

namespace sotc {

namespace {
// The original's empty description points at a NUL byte inside rodata (0x12cdf29); the native view
// points at its own NUL. Borrowed view: ctl == nullptr, so no counter is ever touched.
constexpr char kEmpty[] = "";
}  // namespace

Slot301Result init_slot301_ac9550(Slot301Object& obj, ConCmdRegistry& registry) noexcept {
    Slot301Result out{};

    // 0xac9576 / 0xac957d / 0xac95a4..0xac95bf: stack views {ptr, len, 0, 0}.
    SharedStr name{reinterpret_cast<const std::uint8_t*>(kSlot301Name),
                   static_cast<std::int64_t>(sizeof kSlot301Name - 1), 0, nullptr};
    SharedStr description{reinterpret_cast<const std::uint8_t*>(kEmpty), 0, 0, nullptr};

    out.var = init_bool_var_static(obj, name, description, kSlot301FnVtbl, registry);
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
