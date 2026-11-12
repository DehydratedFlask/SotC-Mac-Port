// CPU-01-AOT-CFG-SCALE-01-BATCH22 (slot 532): fixture for startup constructor index 532 (0x143f50).
// Asserts the 21-object stride, the borrowed-view shape {ptr, strlen, 0, 0}, the empty literals, the
// 21 __cxa_atexit records in body order, and the fail-closed refusal when the caller's table is short.
#include "l4_test_util.hpp"

#include <cstring>

#include "core/ctor_slot_532_143f50.hpp"

namespace {
using namespace l4;

// The digestible ASCII literals, at their real indices. The remaining windows are fragments of
// debug strings that contain non-UTF-8 bytes; the checker pins all 21 against the ELF, and here the
// model is exercised with the real values only where a C string literal can express them exactly.
constexpr const char* kText0 = "EvaluatorAllocator &)";     // 0xfa8d4c, 0x15 B
constexpr const char* kText2 = "rFogDirectionality.cpp";    // 0xfa8dc2, 0x16 B
constexpr const char* kText4 = "orAllocator &)";            // 0xfa8e31, 0x0e B
constexpr const char* kText9 = "&)";                        // 0xfa8f27, 0x02 B
constexpr const char* kText14 = "ies";                      // 0xfa9037, 0x03 B
constexpr const char* kText17 = "<bpe::ParticleSystem> &, bool)";   // 0xfa90e4, 0x1e B

void section_constants() {
    CHECK(kSlot532Rva == 0x143f50 && kSlot532Size == 832);
    CHECK(kSlot532TableWord == 0x2dd70e0 - 8 * 532);
    CHECK(kSlot532TableWord == 0x2dd6040);
    CHECK(kSlot532Count == 21 && kSlot532Stride == sizeof(SharedStr));
    CHECK(kSlot532Object0 + (kSlot532Count - 1) * kSlot532Stride == kSlot532LastObject);
    CHECK(kSlot532LastObject == 0x2ee2cd8);
    CHECK(kSlot532BorrowedStrRva == 0xf2f0 && kSlot532Dtor == 0xf7b0);
    CHECK(kSlot532DsoHandle == 0x1604218 && kSlot532AtexitRva == 0xf8f390);

    // The measured lengths of the literals this fixture can spell out.
    CHECK(kSlot532TextLengths[0] == std::strlen(kText0));
    CHECK(kSlot532TextLengths[2] == std::strlen(kText2));
    CHECK(kSlot532TextLengths[4] == std::strlen(kText4));
    CHECK(kSlot532TextLengths[9] == std::strlen(kText9));
    CHECK(kSlot532TextLengths[14] == std::strlen(kText14));
    CHECK(kSlot532TextLengths[17] == std::strlen(kText17));

    // The five empty windows are the ones with length 0.
    std::size_t empties = 0;
    for (std::size_t i = 0; i < kSlot532Count; ++i) {
        if (kSlot532TextLengths[i] == 0) ++empties;
    }
    CHECK(empties == kSlot532EmptyCount);
    CHECK(kSlot532TextLengths[10] == 0 && kSlot532TextLengths[11] == 0 && kSlot532TextLengths[18] == 0 &&
          kSlot532TextLengths[19] == 0 && kSlot532TextLengths[20] == 0);

    // The literals ascend in body order. Their windows deliberately overlap: they are interior
    // pointers into the same debug strings, so one literal can start inside the previous one.
    for (std::size_t i = 1; i < kSlot532Count; ++i)
        CHECK(kSlot532TextRvas[i] > kSlot532TextRvas[i - 1]);
}

const char* const* text_table() {
    static const char* texts[kSlot532Count] = {
        kText0, "path2", kText2, "prop3", kText4,
        "path5", "prop6", "apply7", "apply8", kText9,
        "", "", "blob12", "blob13", kText14,
        "particle15", "create16", kText17, "", "", "",
    };
    return texts;
}

void run_full() {
    StartupAtexitLog log;
    Slot532Objects o;
    poison(o);
    const char* const* texts = text_table();
    const Slot532Result res = init_slot532_143f50(o, log, texts);

    CHECK(res.ok && !res.refused);
    CHECK(res.views_built == kSlot532Count && res.atexit_records == kSlot532Count);
    CHECK(log.size() == kSlot532Count);

    for (std::size_t i = 0; i < kSlot532Count; ++i) {
        const SharedStr& v = o.views[i];
        CHECK(v.data == reinterpret_cast<const std::uint8_t*>(texts[i]));
        CHECK(v.size == static_cast<std::int64_t>(std::strlen(texts[i])));
        CHECK(v.aux == 0 && v.ctl == nullptr);              // borrowed: nothing allocated
        CHECK(log.at(i).dtor_rva == kSlot532Dtor);             // identity record, in body order
        CHECK(log.at(i).object == &o.views[i]);
    }
}

void run_empty_views() {
    StartupAtexitLog log;
    Slot532Objects o;
    poison(o);
    const Slot532Result res = init_slot532_143f50(o, log, text_table());
    CHECK(res.ok);
    for (std::size_t i : {10u, 11u, 18u, 19u, 20u}) {
        CHECK(o.views[i].size == 0);
        CHECK(o.views[i].data != nullptr && o.views[i].data[0] == 0);   // an empty C string, not null
    }
}

void run_refuses_missing_table() {
    StartupAtexitLog log;
    Slot532Objects o;
    poison(o);
    const Slot532Result res = init_slot532_143f50(o, log, nullptr);
    CHECK(!res.ok && res.refused);
    CHECK(res.views_built == 0 && res.atexit_records == 0);
    CHECK(log.size() == 0);
    CHECK(o.views[0].data == reinterpret_cast<const std::uint8_t*>(static_cast<std::uintptr_t>(0xA5A5A5A5A5A5A5A5ull)));
}

void run_refuses_short_table() {
    StartupAtexitLog log;
    Slot532Objects o;
    poison(o);
    const char* texts[kSlot532Count] = {nullptr};
    texts[5] = "five";
    const Slot532Result res = init_slot532_143f50(o, log, texts);
    CHECK(!res.ok && res.refused);
    CHECK(res.views_built == 0 && log.size() == 0);   // refused before the first store
}


// The header carries the real 21 literals, copied byte for byte from the pinned rodata; assert that
// each one measures to the pinned length, so the model cannot drift from the ELF.
void section_real_literals() {
    for (std::size_t i = 0; i < kSlot532Count; ++i)
        CHECK(std::strlen(kSlot532Texts[i]) == kSlot532TextLengths[i]);
    CHECK(std::strcmp(kSlot532Texts[0], "EvaluatorAllocator &)") == 0);
    CHECK(std::strcmp(kSlot532Texts[2], "rFogDirectionality.cpp") == 0);
    CHECK(std::strcmp(kSlot532Texts[9], "&)") == 0);
    CHECK(std::strcmp(kSlot532Texts[14], "ies") == 0);
    CHECK(std::strcmp(kSlot532Texts[17], "<bpe::ParticleSystem> &, bool)") == 0);
    CHECK(kSlot532Texts[10][0] == 0 && kSlot532Texts[11][0] == 0);
    CHECK(kSlot532Texts[18][0] == 0 && kSlot532Texts[19][0] == 0 && kSlot532Texts[20][0] == 0);
}

}  // namespace

int main() {
    section_constants();
    section_real_literals();
    run_full();
    run_empty_views();
    run_refuses_missing_table();
    run_refuses_short_table();
    std::printf("OK test_ctor_slot_532\n");
    return 0;
}
