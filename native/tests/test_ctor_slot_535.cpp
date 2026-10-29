// CPU-01-STARTUP-SLOT-535: fixture for startup constructor index 535 (0x138e20): a critical section (host
// RecursiveMutex stand-in) and a 0x20 B object whose [+8, +0x20) is zeroed. Lane M5.
#include "l4_test_util.hpp"

#include "core/ctor_slot_535_138e20.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kLaneM5Slot535Rva == 0x138e20 && kLaneM5Slot535Size == 96 && kLaneM5Slot535ObjectSize == 0x20);
    CHECK(kLaneM5Slot535MutexObjectRva == 0x2ee14f0 && kLaneM5Slot535ObjectRva == 0x2ee14f8);
    CHECK(kLaneM5Slot535MutexCtorRva == 0x3b310 && kLaneM5Slot535MutexDtorRva == 0x3b380 && kLaneM5Slot535ObjectDtorRva == 0x138370);
}

void section_alone() {
    StartupAtexitLog log;
    Slot535Objects o;
    std::memset(o.object, kPoison, sizeof o.object);
    const Slot535Result r = init_slot535_138e20(o, log);
    CHECK(r.ok && r.blocked == Slot535Stage::none && r.mutex_status == 0);
    for (std::size_t i = 0; i < 8; ++i) CHECK(o.object[i] == kPoison);              // +0..+7 never written
    for (std::size_t i = 8; i < 0x20; ++i) CHECK(o.object[i] == 0);
    CHECK(log.size() == 2);
    CHECK(log.at(0).dtor_rva == 0x3b380 && log.at(0).object == &o.mutex);
    CHECK(log.at(1).dtor_rva == 0x138370 && log.at(1).object == o.object);
    // the host mutex really is initialised and recursive: nested lock works, then unlocks
    CHECK(o.mutex.lock() == 0 && o.mutex.lock() == 0 && o.mutex.unlock() == 0 && o.mutex.unlock() == 0);
    CHECK(o.mutex.close() == 0);
}

void section_failures() {
    {
        // double initialisation of the stand-in is refused by the mutex (EALREADY) and nothing else happens
        StartupAtexitLog log;
        Slot535Objects o;
        std::memset(o.object, kPoison, sizeof o.object);
        CHECK(init_slot535_138e20(o, log).ok);
        StartupAtexitLog log2;
        const Slot535Result r = init_slot535_138e20(o, log2);
        CHECK(!r.ok && r.blocked == Slot535Stage::mutex_init && r.mutex_status != 0 && log2.size() == 0);
        CHECK(o.mutex.close() == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_535");
    return 0;
}
