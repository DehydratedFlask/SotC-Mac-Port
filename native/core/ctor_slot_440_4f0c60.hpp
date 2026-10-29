// CPU-01-STARTUP-SLOT-440 (lane m3): native model of startup constructor slot 440 (RVA 0x4f0c60, 362 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 440. Every body fact below was read
// end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_440.py. NOTHING here executes,
// loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded
// (the atexit log) and never run.
//
// Shape, in order: (1) the console command `shadow_GallerySkipToNextAudioTrack` (object 0x2efc0d0, flags 2, handler tag 0x4eeaf0
// never called, description length 0x41) built exactly like the slot 306 / 343 commands (init_fconcmd_object:
// 0xf2f0, 0x20af0, vtable slot +0x20 = 0x20d60, f590 x2, atexit(0x4b0)); (2) four path-folding FNV hashes
// (0xa2ddc0 -> 0x162020, native fnv_cstr_a2ddc0) of the strings UI_OPTION_GALLERY_AUDIO_PLAYLIST_{NONE,LOOP_SINGLE,
// LOOP_ALL,RANDOM}, stored in order to the four dwords at 0x2efc170 (the 0xa0-byte slot after the command object).
// Inference only: the dwords are string ids of the gallery playlist options; their consumer is not identified. The
// native hash uses the documented ASCII-toupper policy for the imported _Getptoupper (see fnv_path_cstr_162020.hpp).
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "fnv_path_cstr_162020.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot440Rva = 0x4f0c60;
inline constexpr std::size_t kSlot440Size = 362;
inline constexpr char kSlot440CommandName[] = "shadow_GallerySkipToNextAudioTrack";
inline constexpr char kSlot440CommandDescription[] = "Makes the gallery jump to the next audio track as soon as it can.";
inline constexpr std::size_t kSlot440CommandDescriptionLength = 0x41;
inline constexpr std::uint64_t kSlot440HandlerTag = 0x4eeaf0;
inline constexpr std::uint64_t kSlot440CommandRva = 0x2efc0d0;
inline constexpr std::uint64_t kSlot440HashesRva = 0x2efc170;
inline constexpr char kSlot440Hash0[] = "UI_OPTION_GALLERY_AUDIO_PLAYLIST_NONE";
inline constexpr char kSlot440Hash1[] = "UI_OPTION_GALLERY_AUDIO_PLAYLIST_LOOP_SINGLE";
inline constexpr char kSlot440Hash2[] = "UI_OPTION_GALLERY_AUDIO_PLAYLIST_LOOP_ALL";
inline constexpr char kSlot440Hash3[] = "UI_OPTION_GALLERY_AUDIO_PLAYLIST_RANDOM";

struct Slot440Objects {
    ConCmdCommand command;           // 0x2efc0d0 (0xa0-byte stride)
    std::uint32_t hashes[4];         // 0x2efc170
};

struct Slot440Result {
    bool ok = false;
    FConCmdBuildResult command{};
    bool hashes_stored = false;
};

[[nodiscard]] Slot440Result init_slot440_4f0c60(Slot440Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
