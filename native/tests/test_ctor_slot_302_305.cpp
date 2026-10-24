#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <thread>

#include "core/ctor_slot_302_aaeee0.hpp"
#include "core/ctor_slot_305_a77b60.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    using namespace sotc;
    CHECK(kSlot302Rva == 0xaaeee0 && kSlot302BodySize == 343);
    CHECK(kSlot302HandleCount == 10 && kSlot302FirstHandleRva == 0x3305440);
    CHECK(kSlot302IdentityCellRva == kSlot302FirstHandleRva + 8 * kSlot302HandleCount);
    Slot302Objects objects;
    StartupAtexitLog log;
    int previous = 1;
    for (void*& handle : objects.handles) handle = &previous;
    objects.identity_vptr = 0xffff;
    CHECK(init_slot302_aaeee0(objects, log));
    CHECK(log.size() == 10 && previous == 1);  // zero helper never frees an earlier pointee
    CHECK(objects.identity_vptr == 0x15d4638);
    for (std::size_t i = 0; i < objects.handles.size(); ++i) {
        CHECK(objects.handles[i] == nullptr);
        CHECK(log.at(i).dtor_rva == 0xaaf050);
        CHECK(log.at(i).object == &objects.handles[i]);
    }
    objects.handles[9] = &previous;
    CHECK(init_slot302_aaeee0(objects, log));
    CHECK(log.size() == 20 && objects.handles[9] == nullptr);
    for (std::size_t i = 0; i < objects.handles.size(); ++i)
        CHECK(log.at(10 + i).object == &objects.handles[i]);

    CHECK(kSlot305Rva == 0xa77b60 && kSlot305BodySize == 462);
    CHECK(kSlot305PairCount == 6 && kSlot305FirstCellRva == 0x32fb130);
    CHECK(kSlot305SecondStaticRva == 0x32fb1a8 && kSlot305SecondGuardRva == 0x32fb1b8);
    // Contending first use verifies the two compiler-provided guards. Each thread writes its
    // own destinations, matching independent startup-state ownership without a destination race.
    std::array<Slot305Pairs, 8> destinations;
    std::array<std::thread, 8> threads;
    for (std::size_t i = 0; i < destinations.size(); ++i) {
        for (Pair128& pair : destinations[i]) pair = {~0ull, ~0ull};
        threads[i] = std::thread([&, i] { init_slot305_a77b60(destinations[i]); });
    }
    for (auto& thread : threads) thread.join();
    for (const auto& pairs : destinations)
        for (const Pair128& pair : pairs) CHECK(pair.lo == 0 && pair.hi == 0);
    destinations[0][0] = {1, 2};
    destinations[0][5] = {3, 4};
    init_slot305_a77b60(destinations[0]);
    for (const Pair128& pair : destinations[0]) CHECK(pair.lo == 0 && pair.hi == 0);
    const Pair128& shared = static_pair_a77960();
    CHECK(shared.lo == 0 && shared.hi == 0);
    std::puts("ctor_slot_302_305: 10 cleanup records per call; 8 contending runs; all checks passed");
}
