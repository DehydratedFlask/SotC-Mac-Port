#include "ctor_slot_535_138e20.hpp"

#include <cstring>

namespace sotc {

Slot535Result init_slot535_138e20(Slot535Objects& objects, StartupAtexitLog& log) noexcept {
    Slot535Result out{};
    out.mutex_status = objects.mutex.initialize();                                  // call 0x3b310
    if (out.mutex_status != 0) { out.blocked = Slot535Stage::mutex_init; return out; }
    if (!log.record(kLaneM5Slot535MutexDtorRva, &objects.mutex)) {                         // __cxa_atexit(0x3b380, mutex)
        out.blocked = Slot535Stage::mutex_atexit;
        return out;
    }
    std::memset(objects.object + 0x08, 0, 0x10);                                     // 0x138e64: 16 B zero at +8
    std::memset(objects.object + 0x18, 0, 0x08);                                     // 0x138e6c: qword 0 at +0x18
    if (!log.record(kLaneM5Slot535ObjectDtorRva, objects.object)) {                        // tail jmp __cxa_atexit(0x138370)
        out.blocked = Slot535Stage::object_atexit;
        return out;
    }
    out.ok = true;
    return out;
}

}  // namespace sotc
