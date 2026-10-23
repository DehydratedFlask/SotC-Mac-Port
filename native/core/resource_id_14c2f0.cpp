#include "resource_id_14c2f0.hpp"

#include <cstring>

#include "fnv_span_161f80.hpp"

namespace sotc {

bool resource_id_ctor_14c2f0(HashedStr& out, const char* src,
                             const ResourceIdCtorHooks& hooks) noexcept {
    const char* const placeholder = src == nullptr ? nullptr : std::strstr(src, "****");
    if (placeholder != nullptr && hooks.platform_tag == nullptr) {
        return false;  // the original would call the runtime tag table
    }
    if (!init_shared_str_f1a0(out.str, src, hooks.str)) {
        return false;
    }

    if (placeholder != nullptr) {
        auto* const data = const_cast<std::uint8_t*>(out.str.data);
        std::uint32_t raw = 0;
        std::memcpy(&raw, data + out.str.size - 4, sizeof(raw));
        const std::uint32_t fourcc = __builtin_bswap32(raw);  // movbe
        const std::uint32_t tag = hooks.platform_tag(fourcc);
        std::memcpy(data + (placeholder - src), &tag, sizeof(tag));
    }

    out.gap04[0] = 0;
    out.gap04[1] = 0;
    out.gap04[2] = 0;
    out.gap04[3] = 0;
    const ByteSpan span{out.str.data, out.str.size};
    out.hash = fnv_span_161f80(span, kFnvOffsetBasis161f80);
    return true;
}

}  // namespace sotc
