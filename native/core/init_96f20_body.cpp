#include "init_96f20_body.hpp"

#include <cstdio>
#include <cstring>

namespace sotc {
namespace {

void add_call(InitPostCallLog& log, std::uint64_t callee, const std::uint64_t* args,
              std::size_t count, const InitPostCallOutcome& outcome) noexcept {
    if (log.count >= kInitPostCallMax) return;
    InitPostCallRecord& r = log.records[log.count++];
    r.callee_rva = callee;
    r.caller = "0x96f20";
    r.executed = outcome.executed;
    r.has_return = outcome.has_value;
    r.return_value = outcome.value;
    r.blocker = outcome.executed ? nullptr : outcome.blocker;
    r.arg_count = static_cast<std::uint8_t>(count > kInitPostCallArgMax ? kInitPostCallArgMax
                                                                        : count);
    for (std::size_t i = 0; i < r.arg_count; ++i) r.args[i] = args[i];
    std::memcpy(r.detail, outcome.detail, sizeof r.detail);
    r.detail[sizeof r.detail - 1] = '\0';
}

}  // namespace

Init96f20Result run_init_96f20_body(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2,
                                    std::uint32_t a3,
                                    const Init96f20Hooks& hooks) noexcept {
    Init96f20Result out{};
    if (hooks.call_97a50 == nullptr) {
        out.blocked = "call_97a50_missing";
        return out;
    }

    const std::uint64_t args[6] = {a0, a1, kInit96f20FixedArg2, kInit96f20FixedArg3, a2, a3};
    const InitPostCallOutcome callee =
        hooks.call_97a50(a0, a1, kInit96f20FixedArg2, kInit96f20FixedArg3, a2, a3, 0, 0, 0, 0);
    add_call(out.call_log, kInit96f20CalleeRva, args, 6, callee);
    if (!callee.executed) {
        out.blocked = "call_97a50_refused";
        return out;
    }
    if (!callee.has_value) {
        out.blocked = "call_97a50_no_return";
        return out;
    }

    out.return_value = callee.value;
    out.ok = true;
    return out;
}

}  // namespace sotc
