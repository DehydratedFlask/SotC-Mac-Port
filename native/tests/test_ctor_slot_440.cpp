// CPU-01-STARTUP-SLOT-440: fixture for startup constructor index 440 (0x4f0c60): the console command
// `shadow_GallerySkipToNextAudioTrack` followed by four path-folding FNV hashes stored to consecutive dwords. Lane m3.
#include "l4_test_util.hpp"

#include "core/ctor_slot_440_4f0c60.hpp"

namespace {
using namespace l4;

// Independent expectation: FNV-1a (seed 0x811c9dc5, prime 0x01000193) over the ASCII-uppercased bytes, '\\' -> '/'.
std::uint32_t reference_hash(const char* s) {
    std::uint32_t h = 0x811c9dc5u;
    for (; *s != 0; ++s) {
        unsigned c = static_cast<unsigned char>(*s);
        if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
        if (c == '\\') c = '/';
        h = (h ^ c) * 0x01000193u;
    }
    return h;
}

void section_constants() {
    CHECK(kSlot440Rva == 0x4f0c60 && kSlot440Size == 362 && kSlot440HandlerTag == 0x4eeaf0);
    CHECK(kSlot440CommandRva == 0x2efc0d0 && kSlot440HashesRva == 0x2efc170);
    CHECK(kSlot440HashesRva == kSlot440CommandRva + kFConCmdObjectSize);
    CHECK(std::strlen(kSlot440CommandName) == 34 && std::strlen(kSlot440CommandDescription) == 0x41);
    CHECK(kSlot440CommandDescriptionLength == 0x41);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot440Objects o;
    poison(o);
    const Slot440Result r = init_slot440_4f0c60(o, creg);
    CHECK(r.ok && r.command.ok && r.command.views_inert && r.hashes_stored);
    check_command(o.command, "shadow_GallerySkipToNextAudioTrack",
                  "Makes the gallery jump to the next audio track as soon as it can.", 0x41, 0x4eeaf0);
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.command.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x4b0, &o.command);

    const char* const texts[4] = {"UI_OPTION_GALLERY_AUDIO_PLAYLIST_NONE", "UI_OPTION_GALLERY_AUDIO_PLAYLIST_LOOP_SINGLE",
                                  "UI_OPTION_GALLERY_AUDIO_PLAYLIST_LOOP_ALL", "UI_OPTION_GALLERY_AUDIO_PLAYLIST_RANDOM"};
    const std::uint32_t expected[4] = {0x3254564fu, 0x29147ea4u, 0x1a7e8c8bu, 0x45677856u};
    for (int i = 0; i < 4; ++i) {
        CHECK(o.hashes[i] == reference_hash(texts[i]));
        CHECK(o.hashes[i] == fnv_cstr_a2ddc0(texts[i]));
        CHECK(o.hashes[i] == expected[i]);
    }
    CHECK(o.hashes[0] != o.hashes[1] && o.hashes[1] != o.hashes[2] && o.hashes[2] != o.hashes[3]);
}

void section_failures() {
    // A manager without an insert contract: the command fails visibly and the hash stage is not reached.
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot440Objects o;
    poison(o);
    const Slot440Result r = init_slot440_4f0c60(o, creg);
    CHECK(!r.ok && !r.command.ok && !r.hashes_stored && creg.pending.size() == 0);
    for (std::uint32_t h : o.hashes) CHECK(h == 0xa5a5a5a5u);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_440");
    return 0;
}
