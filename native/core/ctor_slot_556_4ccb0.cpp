// CPU-01-STARTUP-SLOT-556: see ctor_slot_556_4ccb0.hpp.
#include "ctor_slot_556_4ccb0.hpp"

namespace sotc {
namespace {

SharedStr measured_view(const char* text, std::size_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text),
                     static_cast<std::int64_t>(length), 0, nullptr};
}

bool record_atexit(ConCmdRegistry& registry, std::uint64_t dtor, const void* object) noexcept {
    return registry.atexit_log.record(dtor, object);
}

}  // namespace

Slot556Result init_slot556_4ccb0(Slot556Objects& objects, lane_m5::AppInfoQueue& queue,
                                 ConCmdRegistry& registry) noexcept {
    Slot556Result out{};

    // 0x4cccb..0x4ccf2: zero the singleton cell, then register its identity destructor.
    queue.singleton = nullptr;
    out.singleton_cleared = true;
    if (!record_atexit(registry, kSlot556SingletonDtorRva, &queue.singleton)) return out;
    out.singleton_atexit_recorded = true;
    ++out.atexit_records;

    // Four named app-info entries, in body order. Each call performs the exact 0xf2f0 view, entry
    // stores, 0x4b560 queue insert, 0xf590 stack-view release and 0x4b660 atexit sequence.
    for (std::size_t i = 0; i < kSlot556AppCount; ++i) {
        const Slot556AppSpec& spec = kSlot556Apps[i];
        const SharedStr name = measured_view(spec.name, spec.name_length);
        const lane_m5::NamedDelegateSpec named{name, spec.handler};
        const lane_m5::NamedDelegateResult result =
            lane_m5::init_named_delegate_entry(objects.apps[i], named, queue, registry.atexit_log);
        if (!result.ok) return out;
        ++out.apps;
        ++out.app_registrations;
        ++out.app_atexit_records;
        ++out.atexit_records;
    }

    // 0x4d0bd..0x4d128: one console command, name "sendEmail", description 0xf99640, flags 2.
    Slot556Command& command = objects.command;
    const SharedStr name = measured_view(kSlot556CommandName, kSlot556CommandNameLength);
    const SharedStr description = measured_view(kSlot556CommandDescription,
                                                kSlot556CommandDescriptionLength);
    init_concmd_base_20af0(command.base, name, description, kSlot556CommandFlags);
    command.base.vptr = kSlot556CommandVptrRva;
    command.tail0 = reinterpret_cast<std::uint64_t>(&command.tail0);
    command.tail8 = kSlot556CommandTail8;
    command.tail10 = kSlot556CommandTail10;
    command.tail18 = kSlot556CommandHandler;

    // 0x4d15f..0x4d176: the vector is zeroed; +0x98 is not touched.
    command.vec_begin = 0;
    command.vec_end = 0;
    command.vec_cap = 0;
    out.command_vector_cleared = true;
    out.command_gap98_written = false;

    // 0x4d179: call through vtable base 0x1594dc0 + 0x20 -> 0x1594de0 -> 0x20d60.
    if (!register_concmd_20d60(command.base, registry).ok) return out;
    out.command_registered = true;
    out.command_registrations = 1;

    // 0x4d181/0x4d18b: description then name, both borrowed and inert.
    if (!release_inert_view_f590(description)) return out;
    if (!release_inert_view_f590(name)) return out;

    // 0x4d190..0x4d1a1: one __cxa_atexit(0x4b0, command, &__dso_handle).
    if (!record_atexit(registry, kSlot556CommandDtorRva, &command)) return out;
    out.command_atexit_records = 1;
    ++out.atexit_records;

    out.canary_matched = true;
    out.ok = out.singleton_cleared && out.singleton_atexit_recorded && out.apps == kSlot556AppCount &&
             out.app_registrations == kSlot556AppCount &&
             out.app_atexit_records == kSlot556AppCount && out.command_registered &&
             out.command_vector_cleared && !out.command_gap98_written &&
             out.command_registrations == 1 && out.command_atexit_records == 1 &&
             out.atexit_records == 6;
    return out;
}

}  // namespace sotc
