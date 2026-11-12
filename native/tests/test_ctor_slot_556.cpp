// CPU-01-STARTUP-SLOT-556: fixture for slot 556 / 0x4ccb0.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_556_4ccb0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

constexpr std::uint64_t kPoison64 = 0xa5a5a5a5a5a5a5a5ull;

bool view_is(const SharedStr& v, const char* text, std::size_t length) {
    return v.data != nullptr && v.size == static_cast<std::int64_t>(length) && v.aux == 0 &&
           v.ctl == nullptr && std::memcmp(v.data, text, length) == 0;
}

void check_entry(const lane_m5::AppInfoQueueEntry& entry, const Slot556AppSpec& spec,
                 const lane_m5::AppInfoQueueEntry* next) {
    CHECK(entry.registered == 1);
    CHECK(view_is(entry.name, spec.name, spec.name_length));
    CHECK(entry.next == next);
    CHECK(entry.delegate.object == reinterpret_cast<std::uint64_t>(&entry.delegate));
    CHECK(entry.delegate.fn == kSlot556DelegateInvokerRva);
    CHECK(entry.delegate.adj == 0 && entry.delegate.handler == spec.handler);
    for (std::uint8_t byte : entry.gap01) CHECK(byte == 0xa5);   // not written by the entry sequence
}

void section_alone() {
    Slot556Objects objects;
    std::memset(&objects, 0xa5, sizeof objects);

    lane_m5::AppInfoQueueEntry sentinel{};
    sentinel.next = nullptr;
    lane_m5::AppInfoQueue queue{};
    queue.singleton = &sentinel;   // overwritten with null by the body
    queue.head = &sentinel;

    ConCmdRegistry registry{};
    const Slot556Result r = init_slot556_4ccb0(objects, queue, registry);

    CHECK(r.ok && r.canary_matched);
    CHECK(r.singleton_cleared && r.singleton_atexit_recorded);
    CHECK(r.apps == 4 && r.app_registrations == 4 && r.app_atexit_records == 4);
    CHECK(r.command_registered && r.command_registrations == 1 && r.command_atexit_records == 1);
    CHECK(r.command_vector_cleared && !r.command_gap98_written);
    CHECK(r.atexit_records == 6);
    CHECK(queue.singleton == nullptr);

    // The four entries are pushed in body order, so the last one is the queue head.
    CHECK(queue.head == &objects.apps[3]);
    CHECK(objects.apps[3].next == &objects.apps[2]);
    CHECK(objects.apps[2].next == &objects.apps[1]);
    CHECK(objects.apps[1].next == &objects.apps[0]);
    CHECK(objects.apps[0].next == &sentinel);
    for (std::size_t i = 0; i < kSlot556AppCount; ++i) {
        check_entry(objects.apps[i], kSlot556Apps[i],
                    i == 0 ? &sentinel : &objects.apps[i - 1]);
    }

    const Slot556Command& command = objects.command;
    CHECK(command.base.vptr == kSlot556CommandVptrRva);
    CHECK(command.base.registered == 1 && command.base.flags == kSlot556CommandFlags);
    CHECK(view_is(command.base.name, kSlot556CommandName, kSlot556CommandNameLength));
    CHECK(view_is(command.base.description, kSlot556CommandDescription,
                  kSlot556CommandDescriptionLength));
    CHECK(command.tail0 == reinterpret_cast<std::uint64_t>(&command.tail0));
    CHECK(command.tail8 == kSlot556CommandTail8);
    CHECK(command.tail10 == kSlot556CommandTail10);
    CHECK(command.tail18 == kSlot556CommandHandler);
    CHECK(command.vec_begin == 0 && command.vec_end == 0 && command.vec_cap == 0);
    CHECK(command.gap98 == kPoison64);   // +0x98 is not written by the body

    CHECK(registry.pending.size() == 1);
    CHECK(registry.pending.first() == &command.base.pending);
    CHECK(registry.atexit_log.size() == 6);
    CHECK(registry.atexit_log.at(0).dtor_rva == kSlot556SingletonDtorRva);
    CHECK(registry.atexit_log.at(0).object == &queue.singleton);
    for (std::size_t i = 0; i < kSlot556AppCount; ++i) {
        CHECK(registry.atexit_log.at(1 + i).dtor_rva == kSlot556EntryDtorRva);
        CHECK(registry.atexit_log.at(1 + i).object == &objects.apps[i]);
    }
    CHECK(registry.atexit_log.at(5).dtor_rva == kSlot556CommandDtorRva);
    CHECK(registry.atexit_log.at(5).object == &command);
}

}  // namespace

int main() {
    CHECK(kSlot556Rva == 0x4ccb0 && kSlot556Size == 0x514);
    CHECK(kSlot556TableWord == 0x2dd5f80);
    CHECK(kSlot556AppCount == 4);
    CHECK(kSlot556Apps[0].name_length == std::strlen(kSlot556Apps[0].name));
    CHECK(kSlot556Apps[1].name_length == std::strlen(kSlot556Apps[1].name));
    CHECK(kSlot556Apps[2].name_length == std::strlen(kSlot556Apps[2].name));
    CHECK(kSlot556Apps[3].name_length == std::strlen(kSlot556Apps[3].name));
    CHECK(std::strcmp(kSlot556CommandName, "sendEmail") == 0 &&
          kSlot556CommandNameLength == 9);
    CHECK(std::strlen(kSlot556CommandDescription) == kSlot556CommandDescriptionLength);
    CHECK(sizeof(Slot556Command) == 0xa0 && sizeof(Slot556Objects) == 0x1e0);
    section_alone();
    std::puts("OK test_ctor_slot_556");
    return 0;
}
