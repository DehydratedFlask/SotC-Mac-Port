// Init `0x531d60` — the four remaining small spans that no other module claims.
//
// These spans are NOT one contiguous run; they are pinned together because they are the last
// bytes of `0x531d60..0x532b0c` without a model. Each span is documented and checked separately.
//
//   Span A (entry frame) 0x531d60..0x531d84 (36 B, 11 units)
//     push rbp; mov rbp,rsp; push r15/r14/r13/r12/rbx; and rsp,-0x20; sub rsp,0xc40;
//     mov r13,[0x15fe518] (__stack_chk_guard); mov [rsp+0x20],rdi
//     Observable model: the incoming `self` is saved to the frame slot and exposed as
//     `saved_self`; the guard cell is pinned. The real 0xc40 frame and the aligned rsp are
//     NOT modelled (no stack emulation).
//
//   Span B (base-initialize call site) 0x531dcf..0x531ddf (16 B, 4 units)
//     mov r12,[rsp+0x20]; mov rsi,r15; mov rdi,r12; call 0x255130; test al,al
//     Observable model: the saved self round-trips into rdi and the base Initialize contract
//     returns a bool that is exposed as `base_ok`. The callee body is the already-lifted
//     `base_initialize_255130` module; the slice also invokes it separately, so this span is a
//     call-site pin, not a second execution.
//
//   Span C (ret-nop call site) 0x531df9..0x531dfe (5 B, 1 unit)
//     call 0x56dca0 — a one-byte `ret` in the original (already lifted as ret_nop_56dca0).
//
//   Span D (je-not-taken continuation) 0x5327b1..0x5327b6 (5 B, 1 unit)
//     jmp 0x532904 — the unconditional merge jump after batch B's `je 0x5328e0`.
//
// Claimed: the four spans, their sha256 values, the two call targets, the guard cell and the jump
// target. NOT modelled: the 0xc40 stack frame, r15/r12/r13 provenance, the body of 0x255130 here,
// the ret-nop body, and anything after the jump. G1 PARTIAL — pins, not sequencing.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInitEntrySpanARva = 0x531D60;
inline constexpr std::size_t kInitEntrySpanASize = 36;
inline constexpr std::uint64_t kInitEntrySpanBRva = 0x531DCF;
inline constexpr std::size_t kInitEntrySpanBSize = 16;
inline constexpr std::uint64_t kInitEntrySpanCRva = 0x531DF9;
inline constexpr std::size_t kInitEntrySpanCSize = 5;
inline constexpr std::uint64_t kInitEntrySpanDRva = 0x5327B1;
inline constexpr std::size_t kInitEntrySpanDSize = 5;
inline constexpr std::uint64_t kInitEntryGuardCellRva = 0x15FE518;
inline constexpr std::uint64_t kInitEntryBaseInitRva = 0x255130;
inline constexpr std::uint64_t kInitEntryRetNopRva = 0x56DCA0;
inline constexpr std::uint64_t kInitEntryJumpTargetRva = 0x532904;

struct InitEntrySpan {
    std::uint64_t rva = 0;
    std::size_t size = 0;
    const char* what = "";
};

inline constexpr std::array<InitEntrySpan, 4> kInitEntrySpans{{
    {kInitEntrySpanARva, kInitEntrySpanASize, "entry frame + guard + self save"},
    {kInitEntrySpanBRva, kInitEntrySpanBSize, "reload self + base Initialize + test al"},
    {kInitEntrySpanCRva, kInitEntrySpanCSize, "call 0x56dca0 (ret-nop)"},
    {kInitEntrySpanDRva, kInitEntrySpanDSize, "jmp 0x532904"},
}};

struct InitEntryGapsHooks {
    // Base Initialize contract (0x255130). Return value becomes base_ok. Null => base_ok stays
    // false and the call is still counted, so a missing callee cannot look like success.
    bool (*call_base_initialize)(void* self, void* arg) = nullptr;
    // 0x56dca0 ret-nop. Null => no-op, still counted.
    void (*call_ret_nop_56dca0)() = nullptr;
};

struct InitEntryGapsResult {
    bool ok = false;
    int units_executed = 0;          // expect 17
    int guard_loads = 0;             // expect 1
    int self_saves = 0;              // expect 1
    int self_reloads = 0;            // expect 1
    int base_call_count = 0;         // expect 1
    int ret_nop_count = 0;           // expect 1
    int jump_pins = 0;               // expect 1
    void* saved_self = nullptr;      // value stored at [rsp+0x20]
    bool base_ok = false;            // test al,al on the base Initialize result
    std::uint64_t jump_target_rva = kInitEntryJumpTargetRva;
};

// Runs the four pinned spans in address order (A, B, C, D). `self` is what span A stores.
[[nodiscard]] InitEntryGapsResult init_531d60_entry_gaps(
    void* self, const InitEntryGapsHooks& hooks = {}) noexcept;

}  // namespace sotc
