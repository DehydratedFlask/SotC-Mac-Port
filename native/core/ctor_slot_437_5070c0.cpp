#include "ctor_slot_437_5070c0.hpp"

#include <cstring>

namespace sotc {

namespace {

// 0xf2f0: [out] = text; [out+8] = strlen(text); [out+0x10..0x20) = 0 (a borrowed view: no counter, no allocation).
void borrowed_view_f2f0(SharedStr& out, const char* text) noexcept {
    out.data = reinterpret_cast<const std::uint8_t*>(text);
    out.size = text == nullptr ? 0 : static_cast<std::int64_t>(std::strlen(text));
    out.aux = 0;
    out.ctl = nullptr;
}

}  // namespace

Slot437Result init_slot437_5070c0(Slot437Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot437Result out{};
    out.var0 = lane_m6::init_bool_var_literal(objects.var0, kSlot437Var0Name, kSlot437Var0NameLength,
                                                "", 0, kSlot437BoolFnTag, concmd, 0, 0);
    out.var1 = lane_m6::init_bool_var_literal(objects.var1, kSlot437Var1Name, kSlot437Var1NameLength,
                                                "", 0, kSlot437BoolFnTag, concmd, 0, 0);
    out.var2 = lane_m6::init_bool_var_literal(objects.var2, kSlot437Var2Name, kSlot437Var2NameLength,
                                                "", 0, kSlot437BoolFnTag, concmd, 0, 0);
    bool views_ok = true;
    for (std::size_t chunk = 0; chunk < kSlot437ChunkCount; ++chunk) {
        for (std::size_t i = 0; i < kSlot437ChunkSize; ++i) {
            const std::size_t element = chunk * kSlot437ChunkSize + i;
            borrowed_view_f2f0(objects.views[element], kSlot437ViewText[element]);
        }
        // __cxa_atexit(chunk_dtor, NULL, &__dso_handle): the destructor's argument is the original's null.
        if (concmd.atexit_log.record(kSlot437ChunkDtorRva[chunk], nullptr)) {
            ++out.chunk_records;
        } else {
            views_ok = false;
        }
    }
    out.views_ok = views_ok && out.chunk_records == kSlot437ChunkCount;
    out.ok = out.var0.ok && out.var1.ok && out.var2.ok && out.views_ok;
    return out;
}

bool slot437_chunk_destructor(Slot437Objects& objects, std::size_t chunk) noexcept {
    if (chunk >= kSlot437ChunkCount) return false;
    bool inert = true;
    for (std::size_t i = kSlot437ChunkSize; i-- > 0;) {
        inert = release_inert_view_f590(objects.views[chunk * kSlot437ChunkSize + i]) && inert;
    }
    return inert;
}

}  // namespace sotc
