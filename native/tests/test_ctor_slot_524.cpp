// CPU-01-STARTUP-SLOT-524: fixture for startup constructor index 524 (0x16a810), the `screenshot`
// command, plus its 544 B argument-token helper 0x16a380. Checks every stored field, the bytes the
// bodies do NOT write (pre-filled with 0x5a), the token/enum semantics, atexit/pending order, and
// the fail-closed stages.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_524_16a810.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kSentinel = 0x5a;
constexpr std::uint32_t kSentinelKey = 0x5a5a5a5a;

template <class T>
void fill(T& obj) { std::memset(static_cast<void*>(&obj), kSentinel, sizeof obj); }

template <class T>
bool all_sentinel(const T& obj) {
    const auto* b = reinterpret_cast<const std::uint8_t*>(&obj);
    for (std::size_t i = 0; i < sizeof obj; ++i) if (b[i] != kSentinel) return false;
    return true;
}

bool same_view(const SharedStr& v, const char* text, std::size_t len) {
    return v.data != nullptr && v.size == static_cast<std::int64_t>(len) && v.aux == 0 &&
           v.ctl == nullptr && std::memcmp(v.data, text, len) == 0;
}

SharedStr view_of(const char* text, std::size_t len) {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text),
                     static_cast<std::int64_t>(len), 0, nullptr};
}

void section_constants() {
    CHECK(kSlot524Rva == 0x16a810 && kSlot524Size == 727);
    CHECK(kSlot524Rva + kSlot524Size == 0x16aae7);
    CHECK(kSlot524TableWordRva == 0x2dd6080);
    CHECK(kSlot524CanaryCellRva == 0x15fe518 && kSlot524CanaryOffset == 0xe0);
    CHECK(kSlot524CanaryBranchRva == 0x16aad0 && kSlot524CanaryTargetRva == 0x16aae1);
    CHECK(kSlot524StackChkFailRva == 0xf8f3a0 && kSlot524FrameBytes == 0x100);
    CHECK(kSlot524CellRva == 0x2ee3ce0 && kSlot524CommandRva == 0x2ee3ce8);
    CHECK(kSlot524CellRva + sizeof(Slot524Cell) == kSlot524CommandRva);
    CHECK(kSlot524CellDtorRva == 0x166f20 && kSlot524CellDtorSize == 0x2e);
    CHECK(kSlot524CommandVtableRva == 0x1594dc0 && kSlot524CommandVptrRva == 0x1594dd0);
    CHECK(kSlot524CommandRegisterRva == 0x20d60 && kSlot524CommandHandlerRva == 0x169be0);
    CHECK(kSlot524CommandDtorRva == 0x4b0 && kSlot524CommandFlags == 2);
    CHECK(kSlot524CommandThunkCellRva == 0x15fe5e0 && kSlot524CommandTail8 == 0x1990);
    CHECK(kSlot524NameRva == 0xfaba58 && kSlot524DescriptionRva == 0xfaba63);
    CHECK(kSlot524ResolutionRva == 0xfabc04 && kSlot524FilenameRva == 0xfabc0f);
    CHECK(kSlot524ScratchVtableRva == 0x1598780 && kSlot524ScratchFnRva == 0x16a1f0);
    CHECK(kSlot524ScratchVcallRva == 0x16a5d0 && kSlot524ScratchSize == 0x30);
    CHECK(kSlot524ReserveRva == 0x20540 && kSlot524GrowRva == 0x20640);
    CHECK(kSlot524AtexitCount == 2 && kSlot524TokenCount == 2);
    CHECK(kSlot524PendingCount == 1 && kSlot524MenuCount == 0);
    CHECK(kSlot524FirstKind == 1 && kSlot524SecondKind == 4 && kSlot524FirstKey == 0);
    CHECK(sizeof(Slot524Objects) == 0x178);
    CHECK(offsetof(Slot524Objects, cell) == 0 && offsetof(Slot524Objects, command) == 8);
    CHECK(offsetof(Slot524Objects, tokens) == 0xa8 && offsetof(Slot524Objects, scratch) == 0x148);
    CHECK(std::strlen(kSlot524Name) == kSlot524NameLength);
    CHECK(std::strlen(kSlot524Description) == kSlot524DescriptionLength);
    CHECK(std::strlen(kSlot524ResolutionName) == kSlot524ResolutionLength);
    CHECK(std::strlen(kSlot524FilenameName) == kSlot524FilenameLength);

    // Helper identity and the four-row enum table.
    CHECK(kAppend16a380Rva == 0x16a380 && kAppend16a380Size == 0x220);
    CHECK(kAppend16a380TableRva == 0x2ee3da0 && kAppend16a380GuardByteRva == 0x2ee3e00);
    CHECK(kAppend16a380TableStride == 0x18 && kAppend16a380TableCount == 4);
    CHECK(kVirtualResolutionLabels.size() == 4);
    CHECK(std::strcmp(kVirtualResolutionLabels[0].text, "Off") == 0 &&
          kVirtualResolutionLabels[0].key == 0);
    CHECK(std::strcmp(kVirtualResolutionLabels[1].text, "4x") == 0 &&
          kVirtualResolutionLabels[1].key == 1);
    CHECK(std::strcmp(kVirtualResolutionLabels[2].text, "9x") == 0 &&
          kVirtualResolutionLabels[2].key == 2);
    CHECK(kVirtualResolutionLabels[3].text == nullptr &&
          std::strcmp(kVirtualResolutionLabels[3].prefix, "EVirtualResolutionMode") == 0);
    CHECK(std::strlen(kVirtualResolutionPrefix) == 24);
    CHECK(find_virtual_resolution_label(0)->text == kVirtualResolutionLabels[0].text);
    CHECK(find_virtual_resolution_label(1)->text == kVirtualResolutionLabels[1].text);
    CHECK(find_virtual_resolution_label(2)->text == kVirtualResolutionLabels[2].text);
    CHECK(find_virtual_resolution_label(7) == nullptr);
    CHECK(std::strcmp(vector_append_stage_name(VectorAppendStage::label_replace),
                      "label_replace") == 0);
    CHECK(std::strcmp(slot524_stage_name(Slot524Stage::token0_append), "token0_append") == 0);
    CHECK(sizeof(ConCmdArgToken) == 0x50 && offsetof(ConCmdArgToken, key) == 0x48);
    CHECK(kSlot524ScratchVtableWords[0] == 0x16a5a0 && kSlot524ScratchVtableWords[3] == 0x16a5d0);
    CHECK(kSlot524ScratchVtableWords[4] == 0 && kSlot524ScratchVtableWords[5] == 0);
    CHECK(kSlot524ScratchVtableWords[6] == 0x16f550 && kSlot524ScratchVtableWords[7] == 0x16f720);
}

FConCmdArgRecord* rec(ConCmdArgToken* token) noexcept {
    return reinterpret_cast<FConCmdArgRecord*>(token);
}

struct GrowContext {
    ConCmdArgToken* storage;
    ConCmdCommand* observed;
    const std::uint64_t* guard;
    bool append;
};

bool grow_hook(void* raw, ConCmdCommand& owner, const SharedStr& name, std::uint32_t kind) noexcept {
    auto& ctx = *static_cast<GrowContext*>(raw);
    ctx.observed = &owner;
    if (ctx.guard != nullptr) {
        *const_cast<std::uint64_t*>(ctx.guard) ^= 0x8000000000000000ull;
    }
    if (!ctx.append) return false;
    ConCmdArgToken& token = *reinterpret_cast<ConCmdArgToken*>(owner.args_end);
    token.kind = kind;
    token.name = name;
    if (token.name.ctl != nullptr) __atomic_add_fetch(token.name.ctl, 1, __ATOMIC_SEQ_CST);
    init_slot_f180(&token.text);
    owner.args_end = reinterpret_cast<FConCmdArgRecord*>(&token + 1);
    return true;
}

void section_helper() {
    ConCmdCommand cmd{};
    std::array<ConCmdArgToken, 3> tokens;
    fill(tokens);
    cmd.args_begin = rec(tokens.data());
    cmd.args_end = rec(tokens.data());
    cmd.args_cap = rec(tokens.data() + 3);
    const SharedStr resolution = view_of("resolution", 10);

    const VectorAppendResult16a380 off = append_arg_token_16a380(cmd, resolution, 0);
    CHECK(off.ok && off.inline_push && !off.grow_path && off.label_found && !off.fallback_taken);
    CHECK(off.token_index == 0 && off.key == 0 && off.label_length == 3);
    CHECK(std::strcmp(off.label, "Off") == 0 && off.text_normalized);
    CHECK(tokens[0].kind == 1 && tokens[0].key == 0);
    CHECK(same_view(tokens[0].name, "resolution", 10));
    CHECK(same_view(tokens[0].text, "Off", 3));
    for (std::uint8_t b : tokens[0].gap04) CHECK(b == kSentinel);
    for (std::uint8_t b : tokens[0].gap4c) CHECK(b == kSentinel);
    CHECK(cmd.args_end == rec(tokens.data() + 1));

    const VectorAppendResult16a380 four = append_arg_token_16a380(cmd, view_of("x", 1), 1);
    CHECK(four.ok && four.inline_push && four.label_found);
    CHECK(same_view(tokens[1].text, "4x", 2) && tokens[1].key == 1);
    const VectorAppendResult16a380 nine = append_arg_token_16a380(cmd, view_of("y", 1), 2);
    CHECK(nine.ok && nine.label_found && same_view(tokens[2].text, "9x", 2));
    CHECK(cmd.args_end == rec(tokens.data() + 3));

    // Unknown key: sentinel row stops the scan; the literal formatter argument 2 is the text.
    ConCmdCommand fallback_cmd{};
    ConCmdArgToken fallback_token{};
    fill(fallback_token);
    fallback_cmd.args_begin = rec(&fallback_token);
    fallback_cmd.args_end = rec(&fallback_token);
    fallback_cmd.args_cap = rec(&fallback_token + 1);
    const VectorAppendResult16a380 fallback =
        append_arg_token_16a380(fallback_cmd, view_of("v", 1), 7);
    CHECK(fallback.ok && fallback.inline_push && !fallback.label_found);
    CHECK(fallback.fallback_taken && fallback.label_length == 1);
    CHECK(same_view(fallback_token.text, "2", 1) && fallback_token.key == 7);
    for (std::uint8_t b : fallback_token.gap4c) CHECK(b == kSentinel);

    // Grow branch without a contract fails closed before the key is stored.
    ConCmdCommand full{};
    ConCmdArgToken full_token{};
    fill(full_token);
    full.args_begin = rec(&full_token);
    full.args_end = rec(&full_token);
    full.args_cap = rec(&full_token);
    const VectorAppendResult16a380 blocked =
        append_arg_token_16a380(full, view_of("g", 1), 0);
    CHECK(!blocked.ok && blocked.grow_path && blocked.blocked == VectorAppendStage::grow);
    CHECK(all_sentinel(full_token));

    // With a host grow contract the append completes and the helper stores the key.
    GrowContext ctx{&full_token, nullptr, nullptr, true};
    VectorAppendHooks16a380 hooks{};
    hooks.grow = &grow_hook;
    hooks.grow_context = &ctx;
    const VectorAppendResult16a380 grown =
        append_arg_token_16a380(full, view_of("g", 1), 0, hooks);
    CHECK(grown.ok && grown.grow_path && !grown.inline_push);
    CHECK(ctx.observed == &full && full.args_end == rec(&full_token + 1));
    CHECK(full_token.kind == 1 && full_token.key == 0 && same_view(full_token.text, "Off", 3));

    // Malformed inputs are refused without touching the vector. The model can only reject
    // inconsistencies it can see (end before begin, a length that is not a whole token count); a
    // host pointer that is merely unmapped is the host's contract, exactly as in the original.
    ConCmdCommand bad{};
    std::array<ConCmdArgToken, 3> bad_tokens;
    fill(bad_tokens);
    bad.args_begin = rec(&bad_tokens[1]);
    bad.args_end = rec(&bad_tokens[0]);
    bad.args_cap = rec(&bad_tokens[3]);
    const VectorAppendResult16a380 refused =
        append_arg_token_16a380(bad, view_of("z", 1), 0);
    CHECK(!refused.ok && refused.blocked == VectorAppendStage::validate);
    CHECK(bad.args_begin == rec(&bad_tokens[1]) && bad.args_end == rec(&bad_tokens[0]));
    bad.args_begin = rec(&bad_tokens[0]);
    bad.args_end = rec(reinterpret_cast<ConCmdArgToken*>(
        reinterpret_cast<std::uint8_t*>(&bad_tokens[0]) + 0x10));
    bad.args_cap = rec(&bad_tokens[1]);
    CHECK(!append_arg_token_16a380(bad, view_of("z", 1), 0).ok);
    CHECK(!append_arg_token_16a380(bad, SharedStr{nullptr, 3, 0, nullptr}, 0).ok);

    // The canary is read at entry and compared at exit; a grow hook mutates it in between.
    std::uint64_t guard = 0x1122334455667788ull;
    ConCmdCommand canary_cmd{};
    ConCmdArgToken canary_token{};
    fill(canary_token);
    canary_cmd.args_begin = rec(&canary_token);
    canary_cmd.args_end = rec(&canary_token);
    canary_cmd.args_cap = rec(&canary_token);
    GrowContext canary_ctx{&canary_token, nullptr, &guard, true};
    VectorAppendHooks16a380 canary_hooks{};
    canary_hooks.stack_guard = &guard;
    canary_hooks.grow = &grow_hook;
    canary_hooks.grow_context = &canary_ctx;
    const VectorAppendResult16a380 mismatched =
        append_arg_token_16a380(canary_cmd, view_of("c", 1), 0, canary_hooks);
    CHECK(!mismatched.ok && mismatched.blocked == VectorAppendStage::canary);
    CHECK(!mismatched.canary_matched && mismatched.grow_path);
}

void section_build() {
    ConCmdRegistry creg;
    Slot524Objects o;
    fill(o);
    std::uint64_t guard = 0x1234;
    Slot524Hooks hooks{};
    hooks.stack_guard = &guard;
    const Slot524Result r = init_slot524_16a810(o, creg, hooks);
    CHECK(r.ok && r.blocked == Slot524Stage::none);
    CHECK(r.atexit_records == 2 && r.reserved && r.reserve_count == 2 && r.tokens == 2);
    CHECK(r.first_key_written && r.first_label_off && r.second_inline_push);
    CHECK(!r.second_flag_written && r.empty_normalized && r.registered && r.noop_vcall);
    CHECK(r.canary_checked && r.canary_matched && r.views_inert);
    CHECK(r.first.ok && r.first.label_found && std::strcmp(r.first.label, "Off") == 0);
    CHECK(r.registration.ok && r.registration.path == ConCmdRegisterPath::pending_list);

    CHECK(o.cell.word == 0);
    CHECK(creg.manager.manager == nullptr);

    const ConCmdCommand& c = o.command;
    CHECK(&o.command == reinterpret_cast<const ConCmdCommand*>(
                              reinterpret_cast<const std::uint8_t*>(&o) + 8));
    CHECK(c.base.vptr == kSlot524CommandVptrRva && c.base.flags == 2);
    CHECK(c.base.registered == 1);
    CHECK(same_view(c.base.name, "screenshot", 10));
    CHECK(same_view(c.base.description, "Take a screenshot. Params: [+4x] [<filename or folder>]", 0x37));
    CHECK(c.delegate.object == reinterpret_cast<std::uint64_t>(&c.delegate));
    CHECK(c.delegate.fn == 0x1990 && c.delegate.adj == 0);
    CHECK(c.delegate.handler == kSlot524CommandHandlerRva);
    CHECK(c.args_begin == reinterpret_cast<const FConCmdArgRecord*>(o.tokens.data()));
    CHECK(c.args_end == reinterpret_cast<const FConCmdArgRecord*>(o.tokens.data() + 2));
    CHECK(c.args_cap == reinterpret_cast<const FConCmdArgRecord*>(o.tokens.data() + 2));
    for (std::uint8_t b : c.gap98) CHECK(b == kSentinel);

    const ConCmdArgToken& first = o.tokens[0];
    CHECK(first.kind == 1 && first.key == 0);
    CHECK(same_view(first.name, "resolution", 10));
    CHECK(same_view(first.text, "Off", 3));
    for (std::uint8_t b : first.gap04) CHECK(b == kSentinel);
    for (std::uint8_t b : first.gap4c) CHECK(b == kSentinel);

    const ConCmdArgToken& second = o.tokens[1];
    CHECK(second.kind == 4);
    CHECK(same_view(second.name, "filename", 8));
    CHECK(second.text.data == empty_string_f730().data && second.text.size == 0 &&
          second.text.aux == 0 && second.text.ctl == nullptr);
    CHECK(second.key == kSentinelKey);   // 0x16aa43..0x16aa74 never stores +0x48
    for (std::uint8_t b : second.gap04) CHECK(b == kSentinel);
    for (std::uint8_t b : second.gap4c) CHECK(b == kSentinel);

    CHECK(o.scratch.vtable == kSlot524ScratchVtableRva);
    CHECK(o.scratch.tag == kSlot524ScratchFnRva);

    // The 0x30 B scratch only writes +0x20/+0x28; the first 0x20 bytes are untouched.
    {
        const auto* raw = reinterpret_cast<const std::uint8_t*>(&o.scratch);
        for (std::size_t i = 0; i < 0x20; ++i) CHECK(raw[i] == kSentinel);
    }

    // Pending list: exactly the command, queued once.
    CHECK(creg.pending.size() == 1);
    CHECK(creg.pending.first() == &c.base.pending);
    CHECK(creg.pending.first()->next == &creg.pending.head);

    // Atexit order: cell first, command second.
    CHECK(creg.atexit_log.size() == 2);
    CHECK(creg.atexit_log.at(0).dtor_rva == kSlot524CellDtorRva);
    CHECK(creg.atexit_log.at(0).object == &o.cell);
    CHECK(creg.atexit_log.at(1).dtor_rva == kSlot524CommandDtorRva);
    CHECK(creg.atexit_log.at(1).object == &o.command);
}

struct NoopContext {
    int calls = 0;
    const std::uint64_t* guard = nullptr;
    bool flip = false;
    bool result = true;
};

bool noop_hook(void* raw, Slot524Scratch& scratch) noexcept {
    auto& ctx = *static_cast<NoopContext*>(raw);
    ++ctx.calls;
    if (scratch.vtable != kSlot524ScratchVtableRva) return false;
    if (ctx.flip && ctx.guard != nullptr) {
        *const_cast<std::uint64_t*>(ctx.guard) ^= 0x4000000000000000ull;
    }
    return ctx.result;
}

void section_failures() {
    // Missing manager insert: registration fails after both tokens, before scratch/releases/atexit.
    {
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot524Objects o;
        fill(o);
        const Slot524Result r = init_slot524_16a810(o, creg);
        CHECK(!r.ok && r.blocked == Slot524Stage::registration);
        CHECK(r.atexit_records == 1 && r.tokens == 2 && !r.registered);
        CHECK(creg.atexit_log.size() == 1);
        CHECK(creg.atexit_log.at(0).dtor_rva == kSlot524CellDtorRva);
        CHECK(o.command.args_end == reinterpret_cast<const FConCmdArgRecord*>(o.tokens.data() + 2));
        CHECK(same_view(o.tokens[0].text, "Off", 3));
    }

    // The scratch callback can abort the slot before the releases and the command atexit row.
    {
        ConCmdRegistry creg;
        Slot524Objects o;
        fill(o);
        NoopContext ctx{};
        ctx.result = false;
        Slot524Hooks hooks{};
        hooks.noop_vcall = &noop_hook;
        hooks.noop_context = &ctx;
        const Slot524Result r = init_slot524_16a810(o, creg, hooks);
        CHECK(!r.ok && r.blocked == Slot524Stage::scratch_call);
        CHECK(ctx.calls == 1 && r.atexit_records == 1 && !r.noop_vcall);
        CHECK(creg.pending.size() == 1 && creg.atexit_log.size() == 1);
    }

    // A cookie mismatch is reported at the very end, after the body has completed its stores.
    {
        ConCmdRegistry creg;
        Slot524Objects o;
        fill(o);
        std::uint64_t guard = 0x99;
        NoopContext ctx{};
        ctx.guard = &guard;
        ctx.flip = true;
        Slot524Hooks hooks{};
        hooks.stack_guard = &guard;
        hooks.noop_vcall = &noop_hook;
        hooks.noop_context = &ctx;
        const Slot524Result r = init_slot524_16a810(o, creg, hooks);
        CHECK(!r.ok && r.blocked == Slot524Stage::canary);
        CHECK(r.canary_checked && !r.canary_matched);
        CHECK(r.atexit_records == 2 && r.tokens == 2 && r.registered);
        CHECK(creg.atexit_log.size() == 2);
    }
}

}  // namespace

int main() {
    section_constants();
    section_helper();
    section_build();
    section_failures();
    std::puts("OK test_ctor_slot_524");
    return 0;
}
