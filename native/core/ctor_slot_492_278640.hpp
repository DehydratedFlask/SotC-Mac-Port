// CPU-01-STARTUP-SLOTS-M5 (lane M5): native model of startup constructor slot 492 (RVA 0x278640, 289 bytes).
// Original ELF sha256 d0723afb...5861d3; the constructor-table word for slot 492 is at 0x2dd70e0 - 492 * 8. Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_492.py.
// NOTHING here executes, loads or calls an original RVA: the handler address, the invoker 0x4cca0 and the entry
// destructor 0x4b660 are IDENTITY TAGS, recorded and never run.
//
// Shape: ONE named delegate entry `RequestTelemetryChannels` (length 24, entry object 0x2eef4f0, handler tag 0x278300) queued on the
// application-info queue by 0x4b560, built by the shared sequence of core/lane_m5_app_entry (see that header for the
// 0x50 B layout and the 0x4b560 contract): 0xf2f0 name view, entry stores, 0x4b560(entry), 0xf590(view),
// __cxa_atexit(0x4b660, entry, &dso). OBSERVATION only: the handler body (and what drains the queue) is NOT modelled;
// the handler tag is identity.
#pragma once

#include "lane_m5_app_entry.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot492Rva = 0x278640;
inline constexpr std::size_t kSlot492Size = 289;
inline constexpr char kSlot492Name[] = "RequestTelemetryChannels";
inline constexpr std::uint64_t kSlot492ObjectRva = 0x2eef4f0;
inline constexpr std::uint64_t kSlot492HandlerTag = 0x278300;

struct Slot492Objects {
    lane_m5::AppInfoQueueEntry entry;   // 0x2eef4f0
};

struct Slot492Result {
    bool ok = false;
    lane_m5::NamedDelegateResult entry{};
};

// `queue` is the shared application-info queue ([0x2ea8978] singleton + [0x2ea8980] head, owned by StartupState).
[[nodiscard]] Slot492Result init_slot492_278640(Slot492Objects& objects, lane_m5::AppInfoQueue& queue,
                                                ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
