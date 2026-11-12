// CPU-01-AOT-CFG-SCALE-01-BATCH22 (slot 532): see ctor_slot_532_143f50.hpp.
#include "ctor_slot_532_143f50.hpp"

#include <cstring>

namespace sotc {

Slot532Result init_slot532_143f50(Slot532Objects& objects, StartupAtexitLog& log,
                                  const char* const* texts) noexcept {
    Slot532Result out{};
    if (texts == nullptr) {
        out.refused = true;
        return out;
    }
    // Fail closed before the first store: a short table would otherwise leave half the statics
    // built and half registered.
    for (std::size_t i = 0; i < kSlot532Count; ++i) {
        if (texts[i] == nullptr) {
            out.refused = true;
            return out;
        }
    }

    for (std::size_t i = 0; i < kSlot532Count; ++i) {
        // 0xf2f0(destination, 0, cstring): borrowed view {ptr, strlen, 0, 0}.
        SharedStr& view = objects.views[i];
        view.data = reinterpret_cast<const std::uint8_t*>(texts[i]);
        view.size = static_cast<std::int64_t>(std::strlen(texts[i]));
        view.aux = 0;
        view.ctl = nullptr;
        ++out.views_built;

        // __cxa_atexit(0xf7b0, object, 0x1604218): identity only.
        if (!log.record(kSlot532Dtor, &view)) return out;
        ++out.atexit_records;
    }

    out.ok = out.views_built == kSlot532Count && out.atexit_records == kSlot532Count;
    return out;
}

}  // namespace sotc
