#include "init_531d60_post_base_1fc80_u1.hpp"

#include "lookup_1fc80.hpp"

#include <strings.h>

namespace sotc {
namespace {

int strcasecmp_plt(const char* a, const char* b) noexcept {
    if (a == nullptr || b == nullptr) {
        return (a == b) ? 0 : 1;
    }
    return strcasecmp(a, b);
}

}  // namespace

Init531d60PostBase1fc80U1Result init_531d60_post_base_1fc80_u1(
    const Init531d60PostBase1fc80U1Hooks& hooks) noexcept {
    Init531d60PostBase1fc80U1Result out{};

    // OBS: prep 0x532765..0x532775 (16 B) NOT modelled as units.

    void* rdi = hooks.incoming_rdi;
    if (rdi == nullptr && hooks.rsp20_slot != nullptr) {
        const void* rax = *hooks.rsp20_slot;
        auto* p = const_cast<std::uint8_t*>(static_cast<const std::uint8_t*>(rax));
        if (p != nullptr) {
            p += 8;
            rdi = p;
        }
    }

    const std::uintptr_t rsi_rva = hooks.incoming_rsi_rva;

    if (hooks.call_1fc80 != nullptr) {
        hooks.call_1fc80(rdi, rsi_rva);
    }
    void* lookup_rax = nullptr;
    bool lookup_known = false;
    if (hooks.call_1fc80_result != nullptr) {
        lookup_rax = hooks.call_1fc80_result(rdi, rsi_rva);
        lookup_known = true;
    } else if (hooks.use_native_lookup_1fc80 && hooks.nointro_needle != nullptr) {
        Lookup1fc80Hooks lookup_hooks{};
        lookup_hooks.compare_f8f500 = &strcasecmp_plt;
        lookup_rax = lookup_1fc80(rdi, hooks.nointro_needle, lookup_hooks);
        lookup_known = true;
    }
    ++out.call_1fc80_count;

    if (lookup_known) {
        out.rax_from_1fc80 = lookup_rax;
        out.rax_from_1fc80_known = true;
    }

    // Next: test rax,rax @ 0x53277a — NOT modelled here.
    out.ok = (out.call_1fc80_count == 1);
    return out;
}

}  // namespace sotc
