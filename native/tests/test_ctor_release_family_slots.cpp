// L5-RELEASE-FAMILY: fixture for the seventeen startup constructor slots 371 372 405 431 433 434 435 438
// 447 470 472 481 483 484 485 487 529 (native/core/ctor_release_family_slots).
//
// The expectation table below is an INDEPENDENT copy (kind letter, global, vptr tag, string, dtor), kept in
// a different shape from kReleaseFamilySteps, and the expected hashes are computed here with a private
// FNV-1a loop (not the library helpers), so a missing, extra, swapped or re-parameterised step fails.
//
// Sections:
//   1. the independent table versus kReleaseFamilySteps / kReleaseFamilySlots
//   2. a full run of every slot on one fresh state: object bytes, atexit log order, hash dwords
//   3. per-slot isolation (running one slot touches only its own steps)
//   4. fail-closed: a refused f590 release stops the slot at the named step, builds nothing after it,
//      and never reports ok; unknown slots; the default inert release contract
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_release_family_slots.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

struct Exp {
    unsigned slot;
    char kind;              // N name object, S span hash, C cstr hash, Z literal zero
    std::uint64_t global;
    std::uint64_t vptr;
    const char* text;
    std::uint64_t dtor;
};

constexpr Exp kExp[] = {
    {371, 'N', 0x3245460, 0x159bb30, "r_WantsHDR_Game", 0x4cba00},
    {371, 'N', 0x3245490, 0x159bab8, "r_Wants4K_Game", 0x4cb9e0},
    {372, 'N', 0x3245420, 0x15976b0, "shadowDemoMode", 0x8ef00},
    {405, 'N', 0x30059b0, 0x159e1a0, "r_hardwareGamma", 0x2fced0},
    {405, 'N', 0x30059e0, 0x159e1a0, "r_hardwareGamma_HDR", 0x2fced0},
    {431, 'Z', 0x2efc970, 0x0, "", 0x0},
    {431, 'C', 0x2efc974, 0x0, "UI_LANGUAGE_ARABIC", 0x0},
    {431, 'C', 0x2efc978, 0x0, "UI_LANGUAGE_CHINESE", 0x0},
    {431, 'C', 0x2efc97c, 0x0, "UI_LANGUAGE_SIMPLIFIED_CHINESE", 0x0},
    {431, 'C', 0x2efc980, 0x0, "UI_LANGUAGE_CZECH", 0x0},
    {431, 'C', 0x2efc984, 0x0, "UI_LANGUAGE_DANISH", 0x0},
    {431, 'C', 0x2efc988, 0x0, "UI_LANGUAGE_DUTCH", 0x0},
    {431, 'C', 0x2efc98c, 0x0, "UI_LANGUAGE_ENGLISH", 0x0},
    {431, 'C', 0x2efc990, 0x0, "UI_LANGUAGE_ENGLISH_UK", 0x0},
    {431, 'C', 0x2efc994, 0x0, "UI_LANGUAGE_FINNISH", 0x0},
    {431, 'C', 0x2efc998, 0x0, "UI_LANGUAGE_FRENCH", 0x0},
    {431, 'C', 0x2efc99c, 0x0, "UI_LANGUAGE_FRENCH_CANADIAN", 0x0},
    {431, 'C', 0x2efc9a0, 0x0, "UI_LANGUAGE_GERMAN", 0x0},
    {431, 'C', 0x2efc9a4, 0x0, "UI_LANGUAGE_GREEK", 0x0},
    {431, 'C', 0x2efc9a8, 0x0, "UI_LANGUAGE_ITALIAN", 0x0},
    {431, 'C', 0x2efc9ac, 0x0, "UI_LANGUAGE_JAPANESE", 0x0},
    {431, 'C', 0x2efc9b0, 0x0, "UI_LANGUAGE_KOREAN", 0x0},
    {431, 'C', 0x2efc9b4, 0x0, "UI_LANGUAGE_NORWEGIAN", 0x0},
    {431, 'C', 0x2efc9b8, 0x0, "UI_LANGUAGE_POLISH", 0x0},
    {431, 'C', 0x2efc9bc, 0x0, "UI_LANGUAGE_PORTUGUESE", 0x0},
    {431, 'C', 0x2efc9c0, 0x0, "UI_LANGUAGE_BRAZILIAN_PORTUGUESE", 0x0},
    {431, 'C', 0x2efc9c4, 0x0, "UI_LANGUAGE_RUSSIAN", 0x0},
    {431, 'C', 0x2efc9c8, 0x0, "UI_LANGUAGE_SPANISH", 0x0},
    {431, 'C', 0x2efc9cc, 0x0, "UI_LANGUAGE_LATIN_AMERICAN_SPANISH", 0x0},
    {431, 'C', 0x2efc9d0, 0x0, "UI_LANGUAGE_SWEDISH", 0x0},
    {431, 'C', 0x2efc9d4, 0x0, "UI_LANGUAGE_TURKISH", 0x0},
    {431, 'C', 0x2efc9d8, 0x0, "UI_LANGUAGE_THAI", 0x0},
    {433, 'N', 0x2efc900, 0x15976b0, "shadowDemoMode", 0x8ef00},
    {434, 'S', 0x2efc8f8, 0x0, "hdr_curWhiteLevel", 0x0},
    {434, 'S', 0x2efc8fc, 0x0, "hdr_maxWhiteLevel", 0x0},
    {435, 'S', 0x2efc8c8, 0x0, "", 0x0},
    {435, 'S', 0x2efc8cc, 0x0, "FilmGrain", 0x0},
    {438, 'S', 0x2efc2f0, 0x0, "logo_texture", 0x0},
    {447, 'C', 0x2efa870, 0x0, "UI_PHOTO_MODE_CAMERA_FOCUS_PLAYER", 0x0},
    {447, 'C', 0x2efa874, 0x0, "UI_PHOTO_MODE_CAMERA_FOCUS_HORSE", 0x0},
    {447, 'C', 0x2efa878, 0x0, "UI_PHOTO_MODE_DOF_MODE_USE_GAME", 0x0},
    {447, 'C', 0x2efa87c, 0x0, "UI_PHOTO_MODE_DOF_MODE_CUSTOM", 0x0},
    {447, 'C', 0x2efa880, 0x0, "UI_PHOTO_MODE_TINT_LABEL", 0x0},
    {447, 'C', 0x2efa884, 0x0, "UI_PHOTO_MODE_HIGHLIGHTS_LABEL", 0x0},
    {447, 'C', 0x2efa888, 0x0, "UI_PHOTO_MODE_MIDTONES_LABEL", 0x0},
    {447, 'C', 0x2efa88c, 0x0, "UI_PHOTO_MODE_SHADOW_LABEL", 0x0},
    {470, 'N', 0x2ef5530, 0x15ad1d0, "r_fullscreenBlurEnabled", 0x389800},
    {470, 'N', 0x2ef5560, 0x159e1a0, "r_fullscreenBlurBlendWeight", 0x2fced0},
    {472, 'N', 0x2ef5470, 0x15976b0, "arealoader_displayZoneInfo", 0x8ef00},
    {472, 'N', 0x2ef54a0, 0x15976b0, "arealoader_enableMapLoading", 0x8ef00},
    {481, 'N', 0x2eefdf8, 0x159e1a0, "wwise_masterVolume", 0x2fced0},
    {483, 'N', 0x2eefd00, 0x159e1b8, "disableInput", 0x2c84f0},
    {484, 'N', 0x2eefcd0, 0x159e1b8, "disableInput", 0x2c84f0},
    {485, 'N', 0x2eefca0, 0x159e1b8, "disableInput", 0x2c84f0},
    {487, 'N', 0x2eefa40, 0x159e1b8, "showPathFinding", 0x2c84f0},
    {529, 'N', 0x2ee39f0, 0x15976b0, "res_printAllFileLoads", 0x8ef00},
};
constexpr std::size_t kExpCount = sizeof(kExp) / sizeof(kExp[0]);

constexpr unsigned kSlots[] = {371, 372, 405, 431, 433, 434, 435, 438, 447, 470, 472, 481, 483, 484, 485, 487, 529};

std::uint32_t fnv(const char* s, std::size_t n) {
    std::uint32_t h = 0x811c9dc5u;
    for (std::size_t i = 0; i != n; ++i) h = (h ^ static_cast<std::uint8_t>(s[i])) * 0x01000193u;
    return h;
}

// Expected dword for a hash step (all family strings are upper-case/lower-case without backslashes; the
// path variant 0xa2ddc0 folds a-z to A-Z, so apply that to the C-string kind).
std::uint32_t expected_hash(const Exp& e) {
    char buf[64] = {};
    const std::size_t n = std::strlen(e.text);
    CHECK(n < sizeof(buf));
    for (std::size_t i = 0; i != n; ++i) {
        char c = e.text[i];
        if (e.kind == 'C' && c >= 'a' && c <= 'z') c = static_cast<char>(c - 'a' + 'A');
        buf[i] = c;
    }
    return fnv(buf, n);
}

// Index of expectation row `i` among the object rows / dword rows.
std::size_t obj_ordinal(std::size_t i) { std::size_t k = 0; for (std::size_t j = 0; j != i; ++j) k += kExp[j].kind == 'N'; return k; }
std::size_t dw_ordinal(std::size_t i) { std::size_t k = 0; for (std::size_t j = 0; j != i; ++j) k += kExp[j].kind != 'N'; return k; }

ReleaseFamilyKind kind_of(char c) {
    switch (c) {
        case 'N': return ReleaseFamilyKind::name_object;
        case 'S': return ReleaseFamilyKind::span_hash;
        case 'C': return ReleaseFamilyKind::cstr_hash;
        default: return ReleaseFamilyKind::const_zero;
    }
}

int g_release_calls = 0;
int g_fail_at_call = -1;   // 1-based call index that is refused; -1 never
bool counting_release(const SharedStr& view) noexcept {
    ++g_release_calls;
    CHECK(view.ctl == nullptr);   // every stack view is borrowed
    CHECK(view.aux == 0);
    return g_fail_at_call != g_release_calls;
}

void section1_table() {
    CHECK(kReleaseFamilyStepCount == kExpCount);
    CHECK(kReleaseFamilySlotCount == sizeof(kSlots) / sizeof(kSlots[0]));
    std::size_t objects = 0, dwords = 0;
    for (std::size_t i = 0; i != kExpCount; ++i) {
        const ReleaseFamilyStep& s = kReleaseFamilySteps[i];
        const Exp& e = kExp[i];
        CHECK(s.slot == e.slot);
        CHECK(s.kind == kind_of(e.kind));
        CHECK(s.global_rva == e.global);
        CHECK(s.aux_rva == (e.kind == 'N' ? e.vptr : 0));
        CHECK(std::strcmp(s.text, e.text) == 0);
        CHECK(s.length == static_cast<std::int64_t>(std::strlen(e.text)));
        CHECK(s.dtor_rva == (e.kind == 'N' ? e.dtor : 0));
        CHECK(s.seed == (e.kind == 'S' || e.kind == 'C' ? 0x811c9dc5u : 0u));
        (e.kind == 'N' ? objects : dwords)++;
        // every global is unique (no two steps write the same BSS cell)
        for (std::size_t j = 0; j != i; ++j) CHECK(kExp[j].global != e.global);
    }
    CHECK(objects == kReleaseFamilyObjectCount && objects == 16);
    CHECK(dwords == kReleaseFamilyDwordCount && dwords == 40);
    // slot extents: 17 slots, ascending slot numbers, contiguous tiles
    std::size_t cursor = 0;
    for (std::size_t p = 0; p != kReleaseFamilySlotCount; ++p) {
        const ReleaseFamilySlot& d = kReleaseFamilySlots[p];
        CHECK(d.slot == kSlots[p]);
        CHECK(d.first == cursor);
        for (std::uint32_t i = d.first; i != d.first + d.count; ++i) CHECK(kExp[i].slot == d.slot);
        cursor += d.count;
    }
    CHECK(cursor == kExpCount);
    // pinned shapes: object slots hold only N rows, hash slots only S / C / Z rows
    for (unsigned slot : {371u, 372u, 405u, 433u, 470u, 472u, 481u, 483u, 484u, 485u, 487u, 529u})
        for (const Exp& e : kExp) if (e.slot == slot) CHECK(e.kind == 'N');
    for (unsigned slot : {434u, 435u, 438u})
        for (const Exp& e : kExp) if (e.slot == slot) CHECK(e.kind == 'S');
    for (unsigned slot : {431u, 447u})
        for (const Exp& e : kExp) if (e.slot == slot) CHECK(e.kind == 'C' || e.kind == 'Z');
    // spot checks that pin concrete facts from the byte oracle
    CHECK(release_family_slot_position(405) == 2 && release_family_slot_position(306) == kReleaseFamilySlotCount);
    CHECK(kReleaseFamilySlots[release_family_slot_position(371)].count == 2);
    const unsigned counts[][2] = {{371, 2}, {372, 1}, {405, 2}, {431, 27}, {433, 1}, {434, 2}, {435, 2}, {438, 1},
                                  {447, 8}, {470, 2}, {472, 2}, {481, 1}, {483, 1}, {484, 1}, {485, 1}, {487, 1}, {529, 1}};
    for (const auto& c : counts) CHECK(kReleaseFamilySlots[release_family_slot_position(c[0])].count == c[1]);
}

void check_object(const ReleaseFamilyState& st, std::size_t i) {
    const Exp& e = kExp[i];
    const ReleaseFamilyNameObject& o = st.objects[obj_ordinal(i)];
    CHECK(o.vptr == e.vptr);
    CHECK(o.name.size == static_cast<std::int64_t>(std::strlen(e.text)));
    CHECK(o.name.aux == 0 && o.name.ctl == nullptr);   // borrowed view, no control block
    CHECK(o.name.data != nullptr);
    CHECK(std::memcmp(o.name.data, e.text, std::strlen(e.text) + 1) == 0);   // NUL-terminated literal
    CHECK(o.tail == 0);
}

void section2_full_run() {
    ReleaseFamilyState st;
    StartupAtexitLog log;
    ReleaseFamilyHooks hooks;
    hooks.release_view = counting_release;
    g_release_calls = 0;
    g_fail_at_call = -1;
    std::size_t atexit_total = 0, release_total = 0;
    for (unsigned slot : kSlots) {
        const ReleaseFamilyResult r = init_release_family_slot(slot, st, log, hooks);
        CHECK(r.ok && r.slot == slot && r.stage == ReleaseFamilyStage::none);
        CHECK(r.blocked_step == 0xffffffffu);
        const ReleaseFamilySlot& d = kReleaseFamilySlots[release_family_slot_position(slot)];
        CHECK(r.built_count == d.count);
        std::uint32_t objs = 0, spans = 0;
        for (std::uint32_t i = d.first; i != d.first + d.count; ++i) {
            objs += kExp[i].kind == 'N';
            spans += kExp[i].kind == 'S';
        }
        CHECK(r.atexit_records == objs);
        CHECK(r.releases == objs + spans);   // f590 only on the stack views; none in the cstr slots
        atexit_total += objs;
        release_total += objs + spans;
        CHECK(st.results[release_family_slot_position(slot)].ok);
    }
    CHECK(atexit_total == 16 && release_total == 16 + 5);
    CHECK(static_cast<std::size_t>(g_release_calls) == release_total);

    // atexit log: one record per object, in table order, dtor tag + the object's address
    CHECK(log.size() == 16);
    std::size_t k = 0;
    for (std::size_t i = 0; i != kExpCount; ++i) {
        const Exp& e = kExp[i];
        CHECK(st.built[i]);
        if (e.kind == 'N') {
            check_object(st, i);
            CHECK(log.at(k).dtor_rva == e.dtor);
            CHECK(log.at(k).object == &st.objects[obj_ordinal(i)]);
            CHECK(st.atexit_notes[obj_ordinal(i)].dtor_rva == e.dtor);
            ++k;
        } else if (e.kind == 'Z') {
            CHECK(st.dwords[dw_ordinal(i)] == 0);
        } else {
            CHECK(st.dwords[dw_ordinal(i)] == expected_hash(e));
        }
    }
    CHECK(k == 16);
    // concrete anchors: empty span returns the seed; FNV-1a("logo_texture"); slot 431 literal zero first
    for (std::size_t i = 0; i != kExpCount; ++i) {
        if (kExp[i].slot == 435 && kExp[i].text[0] == 0) CHECK(st.dwords[dw_ordinal(i)] == 0x811c9dc5u);
        if (kExp[i].slot == 431 && kExp[i].kind == 'Z') CHECK(st.dwords[dw_ordinal(i)] == 0);
        if (kExp[i].slot == 438) CHECK(st.dwords[dw_ordinal(i)] == fnv("logo_texture", 12));
        if (kExp[i].slot == 447 && std::strcmp(kExp[i].text, "UI_PHOTO_MODE_SHADOW_LABEL") == 0)
            CHECK(st.dwords[dw_ordinal(i)] == fnv("UI_PHOTO_MODE_SHADOW_LABEL", 26));
    }
    // default hooks (null release_view -> lifted inert release) give the same result on a fresh state
    ReleaseFamilyState st2;
    StartupAtexitLog log2;
    for (unsigned slot : kSlots) CHECK(init_release_family_slot(slot, st2, log2, ReleaseFamilyHooks{}).ok);
    CHECK(std::memcmp(st2.dwords.data(), st.dwords.data(), sizeof(st.dwords)) == 0);
    CHECK(log2.size() == 16);
}

void section3_isolation() {
    // each slot alone touches exactly its own steps
    for (unsigned slot : kSlots) {
        ReleaseFamilyState st;
        StartupAtexitLog log;
        const ReleaseFamilyResult r = init_release_family_slot(slot, st, log, ReleaseFamilyHooks{});
        CHECK(r.ok);
        for (std::size_t i = 0; i != kExpCount; ++i) CHECK(st.built[i] == (kExp[i].slot == slot));
        std::size_t own_objs = 0;
        for (const Exp& e : kExp) own_objs += (e.slot == slot && e.kind == 'N');
        CHECK(log.size() == own_objs);
        std::size_t nonzero_obj = 0;
        for (const auto& o : st.objects) nonzero_obj += o.vptr != 0;
        CHECK(nonzero_obj == own_objs);
    }
    // the three disableInput globals are three distinct objects
    ReleaseFamilyState st;
    StartupAtexitLog log;
    for (unsigned slot : {483u, 484u, 485u}) CHECK(init_release_family_slot(slot, st, log, ReleaseFamilyHooks{}).ok);
    CHECK(log.size() == 3 && log.at(0).object != log.at(1).object && log.at(1).object != log.at(2).object);
    // re-running a slot is idempotent for the state bytes (the original would register again)
    CHECK(init_release_family_slot(483, st, log, ReleaseFamilyHooks{}).ok);
    CHECK(log.size() == 4);
}

void section4_fail_closed() {
    // refuse the first f590 of slot 481: object bytes were stored (as the original does before the
    // call) but the slot is blocked at view_release, not built, and atexit is NOT recorded
    {
        ReleaseFamilyState st;
        StartupAtexitLog log;
        ReleaseFamilyHooks hooks;
        hooks.release_view = counting_release;
        g_release_calls = 0;
        g_fail_at_call = 1;
        const ReleaseFamilyResult r = init_release_family_slot(481, st, log, hooks);
        CHECK(!r.ok && r.stage == ReleaseFamilyStage::view_release);
        CHECK(r.built_count == 0 && r.atexit_records == 0 && log.size() == 0);
        const std::size_t pos = release_family_slot_position(481);
        CHECK(r.blocked_step == kReleaseFamilySlots[pos].first);
        CHECK(!st.results[pos].ok);
        for (bool b : st.built) CHECK(!b);
    }
    // two-object slot, refuse the second release: first object fully built + registered, second blocked
    {
        ReleaseFamilyState st;
        StartupAtexitLog log;
        ReleaseFamilyHooks hooks;
        hooks.release_view = counting_release;
        g_release_calls = 0;
        g_fail_at_call = 2;
        const ReleaseFamilyResult r = init_release_family_slot(371, st, log, hooks);
        const std::size_t pos = release_family_slot_position(371);
        CHECK(!r.ok && r.stage == ReleaseFamilyStage::view_release);
        CHECK(r.built_count == 1 && r.atexit_records == 1 && log.size() == 1);
        CHECK(r.blocked_step == kReleaseFamilySlots[pos].first + 1);
        CHECK(st.built[kReleaseFamilySlots[pos].first] && !st.built[kReleaseFamilySlots[pos].first + 1]);
        CHECK(std::strcmp(reinterpret_cast<const char*>(st.objects[0].name.data), "r_WantsHDR_Game") == 0);
    }
    // span-hash slot: refused release leaves the destination dword unwritten (store follows the call)
    {
        ReleaseFamilyState st;
        StartupAtexitLog log;
        ReleaseFamilyHooks hooks;
        hooks.release_view = counting_release;
        g_release_calls = 0;
        g_fail_at_call = 2;
        const ReleaseFamilyResult r = init_release_family_slot(434, st, log, hooks);
        CHECK(!r.ok && r.stage == ReleaseFamilyStage::view_release && r.built_count == 1);
        const std::size_t d0 = release_family_dword_index(kReleaseFamilySlots[release_family_slot_position(434)].first);
        CHECK(st.dwords[d0] == fnv("hdr_curWhiteLevel", 17) && st.dwords[d0 + 1] == 0);
    }
    // a release hook that refuses everything never lets cstr slots fail (they issue no f590)
    {
        ReleaseFamilyState st;
        StartupAtexitLog log;
        ReleaseFamilyHooks hooks;
        hooks.release_view = [](const SharedStr&) noexcept { return false; };
        CHECK(init_release_family_slot(431, st, log, hooks).ok);
        CHECK(init_release_family_slot(447, st, log, hooks).ok);
        CHECK(!init_release_family_slot(438, st, log, hooks).ok);
        CHECK(!init_release_family_slot(529, st, log, hooks).ok);
    }
    // unknown slot numbers are named failures, state untouched
    {
        ReleaseFamilyState st;
        StartupAtexitLog log;
        for (unsigned slot : {0u, 306u, 370u, 373u, 530u}) {
            const ReleaseFamilyResult r = init_release_family_slot(slot, st, log, ReleaseFamilyHooks{});
            CHECK(!r.ok && r.stage == ReleaseFamilyStage::unknown_slot);
        }
        for (bool b : st.built) CHECK(!b);
        CHECK(log.size() == 0);
    }
    // the lifted default really is the inert contract: a view with a control counter is refused
    {
        std::int64_t counter = 1;
        SharedStr live{};
        live.ctl = &counter;
        CHECK(!release_inert_view_f590(live));
        SharedStr inert{};
        CHECK(release_inert_view_f590(inert));
    }
    CHECK(std::strcmp(release_family_stage_name(ReleaseFamilyStage::view_release), "view_release") == 0);
}

}  // namespace

int main() {
    section1_table();
    section2_full_run();
    section3_isolation();
    section4_fail_closed();
    std::puts("OK test_ctor_release_family_slots");
    return 0;
}
