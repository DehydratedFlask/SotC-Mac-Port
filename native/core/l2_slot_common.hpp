// Small helpers shared by the lane-L2 startup constructor modules (slots 307, 308, 310, 312, 313, 317,
// 318). Nothing here models an original function; it only removes boilerplate that every module needs:
//   * borrowed SharedStr views over string literals (control == nullptr, so no counter is ever touched,
//     exactly like the stack views the original builds with {literal, immediate length, 0, 0});
//   * the DebugMenuTarget of a console COMMAND object (the target of a 0x26ea0 toggle that points at a
//     command, not at a variable);
//   * a fail-closed stage recorder.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "debug_menu_item.hpp"
#include "fconcmd_object_slot306.hpp"

namespace sotc::l2 {

// Borrowed view over a literal with an explicit length (the original stores the length as an
// immediate; the modules static_assert that it equals the literal's length).
template <std::size_t N>
[[nodiscard]] constexpr SharedStr literal_view(const char (&text)[N]) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(N - 1), 0, nullptr};
}

// Borrowed view over any NUL-terminated string whose length is measured (0xf2f0 does this at runtime).
[[nodiscard]] inline SharedStr measured_view(const char* text, std::size_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(length), 0, nullptr};
}

// 0x26ea0 reads two virtual slots of its target object. For a console COMMAND the class is the
// FConCmd vtable 0x1594dc0 (address point 0x1594dd0): slot 3 = 0x22ae0 `xor eax,eax; ret` => 0, so
// the `slot3 == 1 && slot6 == 3` narrowing cannot hold and slot 6 is never read by the original. The
// native stand-in therefore reports {slot3 = 0, slot6 = 0}; the second value is unobserved, not
// lifted. Stored by 0x26ea0 only as an opaque pointer.
inline constexpr std::uint64_t kCommandClassSlot3Rva = 0x22ae0;
[[nodiscard]] inline DebugMenuTarget command_menu_target(const ConCmdCommand& cmd) noexcept {
    return DebugMenuTarget{&cmd, 0, 0, true};
}

// Records the first failing stage of a fragment (stage type is the module's own enum).
template <class Result, class Stage>
struct StageFail {
    Result& out;
    bool at(Stage stage) noexcept {
        out.blocked = stage;
        return false;
    }
};

}  // namespace sotc::l2
