#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 440 (RVA 0x4f0c60, 362 bytes), lane m3.

Console command `shadow_GallerySkipToNextAudioTrack` (object 0x2efc0d0, flags 2, handler tag 0x4eeaf0, description length 0x41, atexit 0x4b0), then four
calls of 0xa2ddc0 (FNV-1a path-folding hash, native fnv_cstr_a2ddc0) whose results are stored to the dwords at 0x2efc170.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an
original RVA. Pins: constructor-table word, whole-body sha256 and extent, every rip-relative operand, call
target, immediate store and indirect call of the body (generated once from the disassembly, re-verified here
from bytes), the referenced strings, the functor vtables and the hash of every shared callee. A mutation
self-test flips the first/last byte of every pin and perturbs every native token; each mutant must be rejected.
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402

# Callees specific to this slot (rva, size, sha256), registered next to the shared L4 table.
lib.COMMON_CALLEES['fnv_wrapper_a2ddc0'] = (0xa2ddc0, 0xa, "c526d089a4b2d7d1c1acf93955168456e042e061b9f42ec2cfc7fd160fb10765")
lib.COMMON_CALLEES['fnv_body_162020'] = (0x162020, 0x62, "a7d19297baf11957f065c2631731c010abc4d382f344a2125d59a09f65684d57")

SPEC = {
    "slot": 440,
    "base": 0x4f0c60,
    "size": 362,
    "body_sha256": "36067855c714e7334f46d10bb17396f5b887e16efa0c1e6464fb1aff6750206d",
    "pad": "909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'cmd_dtor', 'release_f590', 'fnv_wrapper_a2ddc0', 'fnv_body_162020'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
        (0x100b9c0, b'shadow_GallerySkipToNextAudioTrack'),
        (0x100b9e3, b'Makes the gallery jump to the next audio track as soon as it can.'),
        (0x100ba25, b'UI_OPTION_GALLERY_AUDIO_PLAYLIST_NONE'),
        (0x100ba4b, b'UI_OPTION_GALLERY_AUDIO_PLAYLIST_LOOP_SINGLE'),
        (0x100ba78, b'UI_OPTION_GALLERY_AUDIO_PLAYLIST_LOOP_ALL'),
        (0x100baa2, b'UI_OPTION_GALLERY_AUDIO_PLAYLIST_RANDOM'),
    ],
    "lengths": [
        ('Makes the gallery jump to the next audio track as soon as it can.', 0x4f0cc9, "48c744243841000000"),
    ],
    "pins": [
        ("rip", 0x4f0c74, 7, 3, 0x15fe518),
        ("rip", 0x4f0c80, 7, 3, 0x100b9c0),
        ("call", 0x4f0c94, 0xf2f0),
        ("rip", 0x4f0c99, 7, 3, 0x2efc0d0),
        ("rip", 0x4f0ca0, 8, 4, 0x15fe5e0),
        ("rip", 0x4f0ca8, 7, 3, 0x100b9e3),
        ("imm", 0x4f0cb8, "b902000000"),
        ("imm", 0x4f0cc9, "48c744243841000000"),
        ("rip", 0x4f0cdc, 7, 3, 0x4eeaf0),
        ("call", 0x4f0cf1, 0x20af0),
        ("rip", 0x4f0cf6, 7, 3, 0x1594dc0),
        ("imm", 0x4f0d36, "48c7839000000000000000"),
        ("imm", 0x4f0d41, "ff5020"),
        ("call", 0x4f0d49, 0xf590),
        ("call", 0x4f0d53, 0xf590),
        ("rip", 0x4f0d58, 7, 3, 0x4b0),
        ("rip", 0x4f0d5f, 7, 3, 0x1604218),
        ("call", 0x4f0d69, 0xf8f390),
        ("rip", 0x4f0d6e, 7, 3, 0x100ba25),
        ("call", 0x4f0d75, 0xa2ddc0),
        ("rip", 0x4f0d7a, 7, 3, 0x2efc170),
        ("rip", 0x4f0d81, 7, 3, 0x100ba4b),
        ("imm", 0x4f0d88, "8903"),
        ("call", 0x4f0d8a, 0xa2ddc0),
        ("rip", 0x4f0d8f, 7, 3, 0x100ba78),
        ("imm", 0x4f0d96, "894304"),
        ("call", 0x4f0d99, 0xa2ddc0),
        ("rip", 0x4f0d9e, 7, 3, 0x100baa2),
        ("imm", 0x4f0da5, "894308"),
        ("call", 0x4f0da8, 0xa2ddc0),
        ("imm", 0x4f0dad, "89430c"),
        ("call", 0x4f0dc5, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_440_4f0c60.hpp', 'source': 'native/core/ctor_slot_440_4f0c60.cpp', 'test': 'native/tests/test_ctor_slot_440.cpp', 'cmake': 'native/lanes/m3.cmake', 'consts': {'kSlot440Rva': 5180512, 'kSlot440Size': 362}, 'header_tokens': ['kSlot440HandlerTag = 0x4eeaf0', 'kSlot440CommandDescriptionLength = 0x41', 'kSlot440CommandRva = 0x2efc0d0', 'kSlot440HashesRva = 0x2efc170'], 'source_order': ['init_fconcmd_object(objects.command, spec, concmd)', 'objects.hashes[0] = fnv_cstr_a2ddc0(kSlot440Hash0)', 'objects.hashes[1] = fnv_cstr_a2ddc0(kSlot440Hash1)', 'objects.hashes[2] = fnv_cstr_a2ddc0(kSlot440Hash2)', 'objects.hashes[3] = fnv_cstr_a2ddc0(kSlot440Hash3)'], 'test_tokens': ['kSlot440Rva == 0x4f0c60', 'section_alone()', 'section_failures()', '0x3254564fu', '0x29147ea4u', '0x1a7e8c8bu', '0x45677856u'], 'cmake_tokens': ['core/ctor_slot_440_4f0c60.cpp', 'add_executable(test_ctor_slot_440 tests/test_ctor_slot_440.cpp)', 'add_test(NAME ctor_slot_440 COMMAND test_ctor_slot_440)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'console command `shadow_GallerySkipToNextAudioTrack` (flags 2, handler tag 0x4eeaf0, atexit 0x4b0) + four 0xa2ddc0 hashes stored to dwords at 0x2efc170.'))
