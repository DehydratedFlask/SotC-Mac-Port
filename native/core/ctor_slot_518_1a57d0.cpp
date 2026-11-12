// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 3): startup constructor index 518 / RVA 0x1a57d0.
// Thin adapter plus the bounded static-float tail. The variable's statement sequence is
// core/debug_page_var_item (a data row, ctor_slot_518_1a57d0.hpp); only the tail is written here.
#include "ctor_slot_518_1a57d0.hpp"

#include <bit>

namespace sotc {

namespace {

float f32(std::uint32_t bits) noexcept { return std::bit_cast<float>(bits); }
std::uint32_t bits_of(float value) noexcept { return std::bit_cast<std::uint32_t>(value); }

}  // namespace

void apply_ambient_spacing_tail(AmbientSpacingBasis& basis, std::uint32_t cache_bits) noexcept {
    // The literal dwords, in the original's store order (0x1a593e..0x1a597d).
    basis.pair1[0] = kSlot518TailPair1Bits[0];   // movl $0x0, (%rdx)
    basis.pair1[1] = kSlot518TailPair1Bits[1];   // movl $0x3f5db22d, 4(%rdx)
    basis.pair0[0] = kSlot518TailPair0Bits[0];   // movl $0x3f800000, (%rax)
    basis.pair0[1] = kSlot518TailPair0Bits[1];   // movl $0x3f000000, 4(%rax)
    basis.pair2[0] = kSlot518TailPair2Bits[0];   // movl $0x3f800000, (%rcx)
    basis.pair2[1] = kSlot518TailPair2Bits[1];   // movl $0xbf13ce56, 4(%rcx)
    basis.pair3[0] = kSlot518TailPair3Bits[0];   // movabs $0x3f93ce5600000000 -> (%rsi), low dword
    basis.pair3[1] = kSlot518TailPair3Bits[1];   //                                       high dword

    // vmovss (cache); vmulss 0.5f; store; vaddss self; vmulss 0.866f; store; vdivss 1.0f / that; store.
    const float spacing = f32(cache_bits) * f32(kSlot518TailMulHalfBits);
    basis.spacing = bits_of(spacing);
    const float pitch = (spacing + spacing) * f32(kSlot518TailMulScaleBits);
    basis.pitch = bits_of(pitch);
    basis.inverse = bits_of(f32(kSlot518TailOneBits) / pitch);
}

Slot518Result init_slot518_1a57d0(Slot518Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                  const debug_page::Hooks& hooks) noexcept {
    const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1), std::span<DebugMenuItem>(),
                                   std::span<NameHashCell>()};
    Slot518Result out{};
    out.page = debug_page::init_debug_page(kSlot518Script, view, concmd, menu, hooks);
    // The tail follows the variable's __cxa_atexit record in the original's straight line (it precedes only the
    // canary compare). A block that stopped earlier never reaches it.
    if (out.page.vars_built == 1 && out.page.atexit_records == 1) {
        apply_ambient_spacing_tail(objects.basis, objects.cache_bits);
        out.tail_written = true;
    }
    out.ok = out.page.ok && out.tail_written;
    return out;
}

}  // namespace sotc
