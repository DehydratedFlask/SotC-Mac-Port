#include "ctor_slot_559_3f2d0.hpp"

#include "lane_m6_shapes.hpp"

namespace sotc {

Slot559Result init_slot559_3f2d0(Slot559Objects& objects, ConCmdRegistry& registry) noexcept {
    Slot559Result out{};
    // 0x3f2f8..0x3f37f: three inline vectors
    for (Slot559Vector& v : objects.vectors) {
        v.begin = v.storage;
        v.end = v.storage;
        v.storage_end = reinterpret_cast<std::uint8_t*>(&v) + kSlot559VectorBytes;
        v.inline_begin = v.storage;
    }
    // 0x3f386: __cxa_atexit(0x3d920, 0, &dso)
    out.object_atexit = registry.atexit_log.record(kSlot559ObjectDtorRva, nullptr);
    // 0x3f38b..0x3f46f: bool var "loggingIncludeTimestamp" (value 0, default 0, empty description)
    out.var = lane_m6::init_bool_var_literal(objects.var, kSlot559VarName, kSlot559VarNameLength, "", 0,
                                             kSlot559BoolFnTag, registry, 0, 0);
    out.ok = out.object_atexit && out.var.ok;
    return out;
}

}  // namespace sotc
