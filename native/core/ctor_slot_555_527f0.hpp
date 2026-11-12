// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): native model of startup constructor slot 555 (RVA 0x527f0, 585 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 555. Every body fact below is
// byte-pinned by tools/lift/check_ctor_slot_555.py. NOTHING here executes, loads or calls an original RVA: vtables,
// functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order: (1) the string console variable `smtp_server` (object 0x2ea8b80) built by 0x257a0 with the borrowed
// initial view `smtp-relay.bluepointgames.local`, f590(init), f590(name), __cxa_atexit(0x23c90); (2) the UNBOUNDED
// int variable `smtp_server_port` (object 0x2ea8cc8, empty description view at 0xf99eb5, flags 1, value = default =
// 25, caller functor tag 0x1595e60) built by 0x3a980 through the family wrapper helper_3a980_unbounded_int (the tag
// is a member of the 37-table int family; HelperRefusal::none only for such a tag), f590(description), call
// [argfn + 0x18] (bare ret), [obj] = 0x1594c08, 0x20d60, f590(name), __cxa_atexit(0x18a60); (3) the string variable
// `smtp_from` (object 0x2ea8da0) with initial view `bpe_ps4-mailer@bluepointgames.com`, f590 x2, __cxa_atexit(0x23c90).
// All views are borrowed rodata, so every 0xf590 release is an exact no-op. No debug-menu item follows. The strings
// are plain configuration defaults (a mail relay host, port 25 and a sender address); whether anything ever sends
// mail is NOT established (INFERENCE from names only). The __stack_chk_guard compare is a hook (not modelled).
#pragma once

#include <cstddef>
#include <cstdint>

#include "helper_int_var_sites.hpp"
#include "lane_m4_shapes.hpp"
#include "string_var_257a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot555Rva = 0x527f0;
inline constexpr std::size_t kSlot555Size = 585;
inline constexpr std::uint64_t kSlot555IntFnTag = 0x1595e60;
inline constexpr char kSlot555ServerName[] = "smtp_server";
inline constexpr char kSlot555ServerInit[] = "smtp-relay.bluepointgames.local";
inline constexpr char kSlot555PortName[] = "smtp_server_port";
inline constexpr char kSlot555FromName[] = "smtp_from";
inline constexpr char kSlot555FromInit[] = "bpe_ps4-mailer@bluepointgames.com";
inline constexpr std::int32_t kSlot555PortValue = 25;
inline constexpr std::uint64_t kSlot555ServerRva = 0x2ea8b80;
inline constexpr std::uint64_t kSlot555PortRva = 0x2ea8cc8;
inline constexpr std::uint64_t kSlot555FromRva = 0x2ea8da0;

struct Slot555Objects {
    ConCmdStringVar server;      // 0x2ea8b80
    ConCmdRangedVar port;        // 0x2ea8cc8
    ConCmdStringVar from;        // 0x2ea8da0
};

struct Slot555Result {
    bool ok = false;
    StringVarResult server{};
    bool server_views_inert = false;
    bool server_atexit_recorded = false;   // 0x23c90 recorded, not run
    HelperIntSiteResult port{};
    StringVarResult from{};
    bool from_views_inert = false;
    bool from_atexit_recorded = false;
};

[[nodiscard]] Slot555Result init_slot555_527f0(Slot555Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
