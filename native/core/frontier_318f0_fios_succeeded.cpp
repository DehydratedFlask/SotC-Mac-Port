#include "frontier_318f0_fios_succeeded.hpp"

#include <cstring>

namespace sotc {

Frontier318f0Result frontier_318f0_fios_succeeded(const Frontier318f0Cells& cells) noexcept {
    Frontier318f0Result out{};
    if (cells.object == nullptr) {
        return out;
    }
    std::memcpy(&out.error, cells.object + kFrontier318f0ErrorOffset, sizeof(out.error));
    out.value = out.error == 0;
    out.ok = true;
    return out;
}

}  // namespace sotc
