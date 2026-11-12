#include "frontier_31520_fios_close.hpp"

#include <cstring>

namespace sotc {

Frontier31520Result frontier_31520_fios_close(const Frontier31520Cells& cells,
                                              const Frontier31520Hooks& hooks) noexcept {
    Frontier31520Result out{};
    if (cells.object == nullptr) {
        return out;
    }
    if (hooks.wait == nullptr) {
        out.missing_hook = "wait";
        return out;
    }
    hooks.wait(cells.object, hooks.user);
    out.wait_called = true;

    std::uint32_t mode = 0;  // re-read after the virtual call, exactly as the body does
    std::memcpy(&mode, cells.object + kFrontier31520ModeOffset, sizeof(mode));
    if (mode == kFrontier31520ModeOpen) {
        if (hooks.close_sync == nullptr) {
            out.missing_hook = "close_sync";
            return out;
        }
        std::uint32_t handle = 0;
        std::memcpy(&handle, cells.object + kFrontier31520HandleOffset, sizeof(handle));
        hooks.close_sync(0, handle, hooks.user);
        out.closed = true;
        out.closed_handle = handle;
        const std::uint32_t zero32 = 0;
        const std::uint64_t zero64 = 0;
        std::memcpy(cells.object + kFrontier31520HandleOffset, &zero32, sizeof(zero32));
        std::memcpy(cells.object + kFrontier31520ModeOffset, &zero64, sizeof(zero64));
    }
    out.ok = true;
    return out;
}

}  // namespace sotc
