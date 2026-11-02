#!/usr/bin/env python3
"""ELF tie for the prerequisites of a native Mermaid/Kraken decoder (STATIC_ANALYSIS_VERIFIED).

Offline and read-only. Bytes of the pinned private ELF (`local/re/eboot.elf`, sha256 d0723afb...5861d3)
are read through `slot_pin_util.Elf`; nothing is loaded, relocated or executed. Skips cleanly (exit 0)
when the private ELF is absent; a present-but-different ELF is a FAIL.

Pinned facts (see docs/checkpoints/2026-11-01-oodle-prereqs.md for the observation/inference split):

  1. Decoder-memory cells. 0x2ee6720 (pointer), 0x2ee6740 (init flag), 0x2ee6748 (size), 0x2ee6728 (24 B
     profile stopwatch, NOT a lock) and 0x2ee6750 (bytes-decoded counter) are .bss (not file-backed). A
     whole-image census (E8/E9/Jcc/lea/rip-disp/q64/d32/RELA) attributes every reference to an FDE:
     the only LIVE initialiser is the inline copy at 0x1aa2cf..0x1aa31e inside bpe::Compression::Decompress;
     0x1aa0f0 and 0x1aa160 carry the same code and have zero references. The size is
     0x89b980(-1,-1) = leaf(0x90c100)=0x4d18 + 0xa8 + 0x40000 + 0x10 = 0x44dd0, derived from the bytes.
  2. Decode-type routing. Header decode type (byte 1 of the 2-byte block header, ctx+0x34, <= 0xc) indexes
     the dword table 0x1295090 -> compressor id -> name table 0x15ced00. Type 6 = Kraken -> 0x8c1e60 (ONLY),
     type 10 = Mermaid -> 0x8db8a0, type 12 = Leviathan -> 0x8cddc0, decoded from the jump table 0x1294528.
  3. Size constants: 0x20000 quantum cap, 0x40000 chunk window / scratch cap, 0x420 scratch slop, 0x7ffff
     19-bit compLen, 0x3ffff 18-bit lengths / 256 KiB chunk offset mask, 0x4000 non-newLZ quantum,
     0xa8 newLZ state base, 0x10/0x100 buffer slack, 0x80000 threaded minimum, 0x1000 alloca threshold.
  4. A function map (rva, FDE extent, sha256) of 55 functions of the newLZ family and its wrapper chain.

Mutation probes (`--no-mutate` skips them): every recorded ELF byte pin and every recorded relocation pin has
its first and last byte (or addend) perturbed and the check must fail; the whole-image census is re-run once
with synthetic references injected into padding and must reject them; and a small synthetic image proves each
detector class fires. `--tamper` runs on a deliberately altered input and therefore exits 1.

Optional (`--archives`): cross-tab of the Decompress tag against the first two bytes of every `.csdr` member
of the private PSARCs (counts only; no member bytes are stored or printed).
"""

from __future__ import annotations

import bisect
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from slot_pin_util import Elf, ROOT, digest, require  # noqa: E402
import check_csdr_oodle_dispatch as base  # noqa: E402

EH_FRAME_HDR = base.EH_FRAME_HDR

CELL_PTR, CELL_LOCK, CELL_FLAG, CELL_SIZE, CELL_STATS = 0x2EE6720, 0x2EE6728, 0x2EE6740, 0x2EE6748, 0x2EE6750
UNIVERSAL_CELL_SIZE = 0x44DD0
TYPE_TO_ID = 0x1295090
NAME_TABLE = 0x15CED00
DISPATCH_JT = 0x1294528
SIZE_JT = 0x12944A8
HOOKS = {0x2DCE810: 0x93A8D0, 0x2DCE818: 0x93A8E0, 0x2DCE820: 0x93A8F0, 0x2DCE828: 0x93A9B0}
ALLOCA_THRESHOLD_CELL = 0x2DCE800

# (rva, FDE address_range, sha256 of the FDE extent, name, role, label: O observed / I inferred / - not examined)
MAP = [
    (0x1AA070, 0x24, 'f67f55d2defadf18a66fa8bedfe10cdd46995bc027bfa8b5b0f57cd26bcae887', 'Decoder-memory cell dtor (atexit)', 'frees [0x2ee6720] if non-null, clears it; registered by 0x1aa510', 'O'),
    (0x1AA0F0, 0x5C, '5e0f73ffe2ce0de31ac19fb3d093066375d1d3686f4469ed5eae2a8ccaa149b7', 'Decoder-memory Init() [UNREFERENCED]', 'same lazy init as inline in 0x1aa2a0; zero references in the whole-image census', 'O'),
    (0x1AA160, 0x9F, 'b0f30cf64225ea6f1361f1316c6b1e21cbdc01aca001f0d44de7272ee5d811bf', 'Size/bound helper by tag [UNREFERENCED]', 'same lazy init, then tag 2..5 -> 0x89ed10, 1 -> 0x1b4130, 0 -> identity (GetCompressedBound-like, by inference)', 'O/I'),
    (0x1AA2A0, 0x1CA, 'fe721ccebe16e9c3923ecad8bb1c6e8fe7e2780b5a7229de2b175097f0583734', 'bpe::Compression::Decompress', 'tag switch; the ONLY live initialiser of cells 0x2ee6720/40/48; calls 0x89d3b0 once', 'O'),
    (0x1AA490, 0x1D, 'ba0fb2faaac7f2fc4ddd646404da11572cc554b0c9f053ee325fe854b98c5859', 'Decode stats reset', 'stopwatch reset(0x2ee6728) + [0x2ee6750]=0 (callers 0x144320, 0x149390)', 'O'),
    (0x1AA4B0, 0x5A, '29bbe0d3f06553241c33df7959b547c3a79825202e2e1820d84a962761d0dd17', 'Decode throughput getter', 'bytes [0x2ee6750] / stopwatch time (caller 0x1451a0; profiler UI, by inference)', 'O/I'),
    (0x1AA510, 0x38, '063b368346241adb85d70d658ccc1045725e82a451c1f4c41fd043e5d835766f', 'Static ctor for the cells', '[0x2ee6720]=0; __cxa_atexit(0x1aa070,&cell); stopwatch ctor(0x2ee6728,0); pointer in a ctor table at 0x2dd60b8', 'O'),
    (0x17090, 0x14, 'f26d9b99d1ad4395bc0dd389d33874229cce9e8ad4ec0dd359fabf714b97ad11', 'Alloc wrapper', 'esi=1 edx=8 ecx=2 ; jmp 0x12820 (engine allocator)', 'O'),
    (0x170C0, 0xA, '39cd562d64a27bc6e432aaf827156a2b3551b6d1ae1c44831c2a2f5feeace372', 'Free wrapper', 'esi=2 ; jmp 0x12af0 (engine free)', 'O'),
    (0x89B980, 0x10, '996dc7a23f41c5e49bc6a70c7b737ab055e855b8c6244512d24745e8ddc51ae3', 'DecoderMemorySize(type,rawLen)', '0x89b990(type,rawLen,NULL)+0x10; called with (-1,-1) at 0x1aa2ea', 'O'),
    (0x89B990, 0x11F, 'e43328fc7612d0f2b433a37685ba49309f30f32375b2613a48770d7bb058373c', 'DecoderStateSize(type,rawLen,*outState)', 'per-compressor state + scratch; newLZ ids add min(2*rawLen+0x420,0x40000)', 'O'),
    (0x89BCB0, 0x16D1, '2973d6ad31819e0237950feb97ccd915bbdb196e3a0178a365c9065899857bbe', 'Decode driver (chunk/quantum loop)', 'oodlelzcompressors.cpp; header parse, 0x40000 chunk window, switch to the codec by decode type', 'O'),
    (0x89D3B0, 0x68C, 'd633fb9445d0166d13fae98c13213315238ac8069f414821a5f7729e44fd0f1c', 'OodleLZ_Decompress wrapper (14 args)', 'single call site 0x1aa426; validates fuzz-safe mask, sizes/aligns decoder memory, loops 0x89bcb0', 'O'),
    (0x89DAD0, 0x14, '80614b1869123f53ac69e11ec34a2a88c5318c33e3c67c2413554dcbb076d239', 'DecodeBufferSize(rawLen,flag)', 'rawLen + (flag ? 0x100 : 0x10); FDE-only (Ghidra has no function here)', 'O'),
    (0x89F410, 0x6, 'ece74a3abf7f9404faefe63cbc71943f536077bf8a3d712a3753215e02027948', 'threaded-phase minimum', 'returns 0x80000; reached only when threadPhase is 1/2', 'O'),
    (0x8AC710, 0x30A, 'ef24535a354c52e595b024dce5e7580c87462ff7f64151a995a3959626398de3', 'Block-header parse (2-byte)', 'reads byte0 (low nibble 0xc) and byte1; fills ctx+0x30..0x44', 'O'),
    (0x8ACB90, 0x1A6, '7c4ab3918361432cebaab79c1ac85f9bf3da60b8f71997c95c89cf9f24e6f442', 'Non-newLZ quantum-header parse', 'used for ids outside {8,9,12}; raw cap 0x4000', 'O'),
    (0x8ACE10, 0x1AA, 'ec2b489ee13bfc467d16a876b442e9cc537ea7f3558edd65df4bc08f3d46218c', 'newLZ chunk quantum-header parse (3 bytes)', '18-bit compLen-1, special value 0x3ffff, optional 24-bit crc', 'O'),
    (0x89FC90, 0x7B0, 'e6870e958d180ac28ca667b6c040fe2e7530a47646cbfb1a4abf8173f4de47ed', '24-bit quantum checksum', 'compared with the QH crc field when checkCRC != 0 (role by inference)', 'I'),
    (0x8C1780, 0xA6, '8b4bb02735c46983c52dfd4046da4af3b2f67a3af3c07ec254b3ed1d5c3b42e8', 'Zero-fill (ptr,size)', 'called with 0x10/0x20/0x88/0x98 on contexts', 'O'),
    (0x8C1AA0, 0x19, '9bfe57ec9f0cd795d4f56de996af51c681d76c0ca73afcf5a95e32d2e4515622', 'tiny guard', '25 B leaf (role not examined)', '-'),
    (0x8C1AC0, 0x12, 'aacbe7391154b8d4d3fa300fa69da9ff6aa59028db0bcfdd14abd6872d6580ad', 'tiny guard', '18 B leaf, called at wrapper exit with the context pointer (role not examined)', '-'),
    (0x8C1B20, 0x14, '5f0b01b741f26ff46fbbfe0ab212a429ce6ae62cd2f25b0483e844353d9dcf08', 'assert-fail handler', '(file,line,func,expr) -> 0x93a9b0; nonzero result -> int 0x41', 'O'),
    (0x8C1E60, 0x30F6, 'd1e7286d7fcf681eaa8de257256a36eb29cb6ed37cfbbf6bcd74fb50dd753fe9', 'Kraken chunk decoder', 'newlz.cpp; decode type 6 / compressor id 8; sole caller 0x89c57e', 'O'),
    (0x8CB610, 0x719, '3a7619875d67af7039ff80dff8d93890325c00b79329eb264ab32f1a546f094b', 'Kraken phase-2 (mode 1)', 'newLZ_decode_chunk_phase2; call 0x8c47f7', 'O'),
    (0x8CBD30, 0x746, '7a81867b421e592e4d538047dca95a5299c6dca4f788a3ace379aa07b559ce3f', 'Kraken phase-2 (mode 0)', 'newLZ_decode_chunk_phase2; call 0x8c4844', 'O'),
    (0x8CDDC0, 0x3410, 'ade9fb30b22bbc9efecdc459241eec3a3fcd4a566c2a72a2266df8ac038c58ac', 'Leviathan chunk decoder', 'newlz.cpp; decode type 12 / id 13; sole caller 0x89c443; no .csdr member starts with a type-12 header', 'O'),
    (0x8D6E00, 0x5E3, 'f7ede3aa8095187c6b6a6804c277fb65b689962e94e5a7a99af4532141eaca22', 'Leviathan helper', 'leaf called only by 0x8cddc0 (role not examined)', '-'),
    (0x8D73F0, 0x5F2, '88b465c311c6bea7c160c103de335e3bc53681f3d1a4f27769817539b1655b0d', 'Leviathan helper', 'leaf called only by 0x8cddc0', '-'),
    (0x8D79F0, 0x661, '60471f29ce33b2e95888a23096b1638d1cd523c7eea41a958b215d17e0d0ecc9', 'Leviathan helper', 'leaf called only by 0x8cddc0', '-'),
    (0x8D8060, 0x11D9, '20b46d495b71b3320fe1ec7439856275350db35e195517ef5207b89a3728447c', 'Leviathan helper', '4,523 B leaf called only by 0x8cddc0', '-'),
    (0x8D9240, 0x49A, 'b5852157cdcc09b4fef366e1d9ad0cc3efe3f36c110074255be3aa88ff0f6673', 'Leviathan helper', 'leaf called only by 0x8cddc0', '-'),
    (0x8DB8A0, 0x153F, 'a59d1d949dffe2c14e78fcdeee08adddba95a9ed82d867f3b213e4760e58ad2a', 'Mermaid chunk decoder', 'newlzf.cpp; decode type 10 / compressor id 9; sole caller 0x89c64c', 'O'),
    (0x8EE200, 0x1792, 'cb38f2000464e8e9cd45782e024f14ad73086b7f74d81dba02d84ee01944e4a8', 'Mermaid parse loop variant', 'newlzf_decode_parse_outer.inl; local_d0 != 0 and sub-mode != 1', 'O'),
    (0x8EF9A0, 0x168F, 'ab21da25de3eae4a15c1e3c54f78dead6292d8fe905caf3509ef261408d8725b', 'Mermaid parse loop variant', 'as above; local_d0 != 0 and sub-mode == 1', 'O'),
    (0x8F1030, 0x163E, '7affd0d05dfb6204706c3f5cc6a3ff6801800268974a67db3a26d6de39e2fe71', 'Mermaid parse loop variant', "'mp >= window_base' check; local_d0 == 0 and sub-mode != 1", 'O'),
    (0x8F2670, 0x14CB, '01a7a00ee39d2d7a8bc6a26da8461480d4ea6e157f2b41ec8f0a93957cb5815f', 'Mermaid parse loop variant', "'mp >= window_base' check; local_d0 == 0 and sub-mode == 1", 'O'),
    (0x8F3B40, 0x24B, 'fef1399664407f437500d23f86f5c83041cffd47f2b79966f5b9f5c8e82df3ae', 'Mermaid off24 reader', 'newlzf_sse4.cpp; 2 calls from 0x8db8a0', 'O'),
    (0x8F47D0, 0xCB3, '00e32a3e903ea582b1c90b726d5f5614a272d162fef3e4f701112e46ae127bf5', 'Shared entropy sub-stream decoder', 'called by Kraken (6x), Leviathan (7x) and Mermaid (6x); arg includes the nibble==4 flag', 'O'),
    (0x8A04C0, 0x12A, '456ee914fe3b88f686f22b8ac66cebc264a940e1e151dff952c02c62b5ae2892', 'Entropy primitive', 'callee of 0x8f47d0 only (not examined)', '-'),
    (0x8A1700, 0x1C3A, 'df557f2da2dc752ff932d3c1d9ccbb88cde7138d92a2f7fde35c6e5f9dde070b', 'Entropy primitive', '7,150 B callee of 0x8f47d0 only (not examined)', '-'),
    (0x8A0480, 0x3B, 'd225b41179769c5fa99b90dfcc3654f9d504bd91bf0bdf8f8ce17ef6211ac433', 'Size leaf for ids 0/1', 'used by 0x89b990 (not examined)', '-'),
    (0x90C100, 0x6, '342d53e43cb9dcf77a3272bca60062ac22600b573722505d3baee8ebf737fcac', 'State size leaf, id 6', 'mov eax,0x4d18 ; ret', 'O'),
    (0x927C00, 0x6, 'b57724a38722579d01a6b0ba3c00fec98c893354fcae658e9e84923ab6f7c2fa', 'State size leaf, id 7', 'mov eax,0x2cdc ; ret', 'O'),
    (0x9359D0, 0x6, '48852b6977b80c325438f87ddf0c3dc68224048723debf0df7fc4cfe836b4941', 'State size leaf, id 10', 'mov eax,0x3ea8 ; ret', 'O'),
    (0x93A7D0, 0x12, '6b8c01a3ea3967ea9b3af86f18f019a4fcc15e3705776a829ac3026dcf1a7c34', '18 B leaf beside the hooks', 'role not examined', '-'),
    (0x93A7F0, 0x12, '2356528567459aa08b90dd9b0ff684485d6337ec4d5adb8c6d7f49b056c5760b', '18 B leaf beside the hooks', 'role not examined', '-'),
    (0x93A8D0, 0xE, 'c9abd635e8d9e2c64ef980b2a4ed102bd7a755f37769e2a692ecb76c669021c0', 'Alloc hook', 'target of the cell 0x2dce810', 'O'),
    (0x93A8F0, 0xB2, 'aadf765cecfb6c497cb94a9476bf62da7148e3a06c6c4afa2b5fc88174f2c444', 'Log hook', 'target of the cell 0x2dce820 (level,file,line,msg)', 'O'),
    (0x93A9B0, 0x28, '9e9dadbc59ae76cca3a00395600d4dc2ff64bf3af146241d0fb6c332a81f3caa', 'Assert hook', 'target of the cell 0x2dce828', 'O'),
    (0x326A0, 0x4F, '215ded26febcf396f98d8ecbfcf659271c455e5be1a5581930bb0622eb356dad', 'Stopwatch ctor', 'byte tag + rdtsc start', 'O'),
    (0x326F0, 0x16, '273f0f8430a0df9505dba36215ed110d883f94a0dd54a17870700a8748edef93', 'Stopwatch start', 'rdtsc', 'O'),
    (0x32710, 0x27, 'fbf935fe7e0903702e0a7757d37b94650cffdb80fdd49a601447f78556ac9ccb', 'Stopwatch stop', 'rdtsc, accumulates', 'O'),
    (0x32740, 0xA5, 'de59056e180dbba815fbee58b3d4e0cfab3233d2fc6de57f7d36364b981fd5f2', 'Stopwatch read', '', 'O'),
    (0x32A00, 0x16, '9365e0f430568b00f71af00827020b9ef07d508cb9758f183a924eb4fe365a14', 'Stopwatch reset', '', 'O'),
]

# (address, bytes, what): literal instruction bytes
PINS = [
    (0x1AA070, "554889e553504889fb488b3b4885ff740ce83ad0e6ff48c703000000004883c4085b5dc3", '0x1aa070 atexit dtor of the decoder-memory cell: if([cell]) {free([cell]); [cell]=0}'),
    (0x1AA0F6, "488d0543c6d3028038007543bfffffffff48c7c6ffffffffc60001e86a186f004863f848893d28c6d302e86bcfe6ff488b3df4c5d3024889c34839df74114885ff7405e882cfe6ff48891ddbc5d3024883c4085b5dc3", '0x1aa0f0 Init() body: lazy init of flag 0x2ee6740 / size 0x2ee6748 / pointer 0x2ee6720 (unreferenced)'),
    (0x1AA16A, "488d05cfc5d30289f34989fe8038007543bfffffffff48c7c6ffffffffc60001e8f1176f004863f848893dafc5d302e8f2cee6ff488b3d7bc5d3024989c74c39ff74114885ff7405e809cfe6ff4c893d62c5d302", '0x1aa160 prologue + the same lazy init (unreferenced)'),
    (0x1AA1BE, "89d931c080c1fe80f90473124c89f74883c4085b415e415f5de9344b6f0080fb01740984db750f4c89f0eb0a4489f7e83e9f000089c04883c4085b415e415f5dc3", '0x1aa160 tag switch: tags 2..5 -> 0x89ed10, tag 1 -> 0x1b4130, tag 0 -> identity (unreferenced)'),
    (0x1AA2CF, "488d056ac4d3028038007543bfffffffff48c7c6ffffffffc60001e891166f004863f848893d4fc4d302e892cde6ff488b3d1bc4d3024889c34839df74114885ff7405e8a9cde6ff48891d02c4d302", '0x1aa2a0 inline lazy init (the only live initialiser): flag, size = 0x89b980(-1,-1), ptr = alloc, free old'),
    (0x1AA404, "004c89ff4c89e64889d96a03ff3532c3d302ff3504c3d3026a006a006a006a006a00e8852f6f004883c440", '0x1aa2a0 Oodle stack args: push 3 ; push [0x2ee6748] ; push [0x2ee6720] ; push 0 x5 ; call 0x89d3b0'),
    (0x1AA490, "554889e5488d3d8dc2d302e86085e8ff48c705a5c2d302000000005dc3", '0x1aa490 ResetStats: stopwatch reset(0x2ee6728) ; [0x2ee6750] = 0'),
    (0x1AA4B0, "554889e5488d3d6dc2d302e88082e8ffc5fa1015a038e000c5f057c9488b057dc2d302c5fac2c900c5f055c0c5f054d289c183e1014885c0c5f856c27807", '0x1aa4b0 Throughput head: stopwatch read(0x2ee6728) ; loads [0x2ee6750]'),
    (0x1AA510, "554889e5488d3d55fbffff488d15f69c4501488d35f7c1d30248c705ecc1d30200000000e8574ede00488d3de8c1d30231f65de95881e8", '0x1aa510 static ctor: [0x2ee6720]=0 ; atexit(0x1aa070,&cell) ; stopwatch ctor(0x2ee6728,0)'),
    (0x17090, "be01000000ba08000000b902000000e97cb7ffff", '0x17090 alloc(size=rdi, align... ): esi=1 edx=8 ecx=2 ; jmp 0x12820'),
    (0x170C0, "be02000000e926baffff", '0x170c0 free: esi=2 ; jmp 0x12af0'),
    (0x89B980, "554889e531d2e80500000083c0105dc3", '0x89b980 DecoderMemorySize(type,rawLen): 0x89b990(type,rawLen,NULL)+0x10'),
    (0x89B99E, "83ffff4989d5ba060000004989f6b1060f94c083ff0d0f9fc308c30f44d7750289f941bf003b0000b8a800000041d3ef4183e7010f858f000000", '0x89b990 type clamp (-1 or >13 -> 6), mask 0x3b00 newLZ test, base 0xa8'),
    (0x89B9D8, "b9cbf7ffff0fa3d10f838100000089d183fa0a777a488d15b48a9f0048630c8a4801d1ffe1", '0x89b990 mask 0xfffff7cb, jump table 0x12944a8 for ids 0/1/6/7/10'),
    (0x89BA4F, "e8ac060700eb0ce8a5c10800eb05e86e9f090005a8000000", '0x89b990 id 6/7/10 leaf calls 0x90c100/0x927c00/0x9359d0 then +0xa8'),
    (0x89BA67, "4d85ed7404418945004585ff0f95c108cbf6c3017423b9000004004d85f67e174b8d943620040000b9000004004881fa000004000f4cca01c8", '0x89b990 optional out-param, newLZ/unknown scratch: ecx=0x40000 ; rawLen>0 -> min(2*rawLen+0x420, 0x40000)'),
    (0x89DAD0, "85f6b910000000b800010000480f44c14801f8c3", '0x89dad0 DecodeBufferSize(rawLen,flag): rawLen + (flag ? 0x100 : 0x10)'),
    (0x90C100, "b8184d0000c3", '0x90c100 mov eax,0x4d18 ; ret'),
    (0x927C00, "b8dc2c0000c3", '0x927c00 mov eax,0x2cdc ; ret'),
    (0x9359D0, "b8a83e0000c3", '0x9359d0 mov eax,0x3ea8 ; ret'),
    (0x89BD01, "3de70300000f8f460c0000", '0x89bcb0 recursion/depth guard: ++ctx[0x28] ; > 999 -> fail'),
    (0x89BD35, "41bd000004004881f9000004004c0f4ee9", '0x89bcb0 chunk window: 0x40000 cap: min(0x40000, srcLen-pos)'),
    (0x89BD61, "89c14963f541bd000004004c89f281e1ffff03004829c24929cd4c39ee", '0x89bcb0 chunk offset mask 0x3ffff and 0x40000 - offset'),
    (0x89BE12, "48634934bfffffffff4883f90c770a488d1568929f008b3c8a83fe017538b9133f00000fa3f9", '0x89bcb0 header decode type ctx[0x34] (<=0xc) -> table 0x1295090 ; fuzz-safe mask 0x3f13'),
    (0x89C24B, "8d4bfa48895598b25148895d904c8bb568ffffff4589cf4c89c783f9070f92c0d2eab90040000020c2410f45cd4181fd00400000410f4ccd84d2488d9540ffffff4189cd", '0x89bcb0 newLZ chunk-type test 0x51>>(type-6) (types 6,10,12) ; non-newLZ quantum 0x4000'),
    (0x89C39C, "89d083fa0c0f87af0200004c8d057a819f00488b55b04c2b6b18496304804c01c04a8d3422ffe0", '0x89bcb0 decode-type switch: >0xc -> error, jump table 0x1294528'),
    (0x89C440, "ff7350e878190300", '0x89bcb0 type 12: 3 stack args + call 0x8cddc0 (Leviathan)'),
    (0x89C578, "ff7358ff7350e8dd580200", '0x89bcb0 type 6: 3 stack args + call 0x8c1e60 (Kraken)'),
    (0x89C646, "ff7358ff7350e84ff20300", '0x89bcb0 type 10: 3 stack args + call 0x8db8a0 (Mermaid)'),
    (0x89D5E6, "b8003b00004489f1d3e84d85e40f841804000083e0010f840f040000e8091e0000", '0x89d3b0 mask 0x3b00 + call 0x89f410 (threaded-phase branch only)'),
    (0x89D621, "488d55b44489f7e863e3ffff", '0x89d3b0 call 0x89b990(type, rawLen, &stateSize)'),
    (0x89D62D, "4d85e4741683c01049c7c5ffffffff48984839d87e27", '0x89d3b0 supplied decoder memory must hold stateSize+0x10'),
    (0x89D648, "488d0db11153023b017d3983c0104989e44863d8488d430f4883e0f04929c44c89e4", '0x89d3b0 no decoder memory: alloca below [0x2dce800] else hook [0x2dce810]'),
    (0x89D66A, "448b75b44983c40fbe980000004883c3f04983e4f04c89e7", '0x89d3b0 memory realign: (ptr+0xf)&-16 ; size-0x10'),
    (0x89F410, "b800000800c3", '0x89f410 returns 0x80000 (threaded minimum)'),
    (0x8C1E60, "554889e54157415641554154534881ecb8010000", '0x8c1e60 prologue (8 instrs, sub rsp,0x1b8)'),
    (0x8C1EF0, "418b042489c20fca85d20f88", '0x8c1e60 quantum header: mov eax,[r12]; bswap; test edx,edx ; js (bit 31 = compressed)'),
    (0x8C1F0D, "89d1c1e91c84c90f842304000080f904740980f9020f856f2f0000", '0x8c1e60 non-compressed header: top nibble 0 / 4 / 2 dispatch'),
    (0x8C1F3F, "81e1ffff0300", '0x8c1e60 5-byte header: compLen = (be32(hdr+1)) & 0x3ffff'),
    (0x8C1F58, "b812120000c4e2f8f7da", '0x8c1e60 5-byte header: bextr 0x1212 (start 18, len 18) = rawLen-1'),
    (0x8C1F80, "4189d341c1eb084181e3ffff07004b8d441c03", '0x8c1e60 compressed 3-byte header: (be32>>8) & 0x7ffff = compLen ; data at +3'),
    (0x8C1FA0, "b81b0400004d8d7c2403c46278f7f2", '0x8c1e60 mode = bextr(0x41b): bits 27..30 of the big-endian dword'),
    (0x8C1FBF, "4b8d8c1220040000b800000400488b5d884881f901000400480f4dc8", '0x8c1e60 scratch = min(2*rawQuantum+0x420, 0x40000)'),
    (0x8C233D, "b808120000c4e278f7da4c39d30f8f472b0000", '0x8c1e60 stored quantum: bextr 0x1208 (start 8, len 18) = rawLen; data at +3'),
    (0x8C47F2, "89cb4929c0e8146e0000", '0x8c1e60 phase2 mode 1: call 0x8cb610'),
    (0x8C483F, "89d34d89dce8e7740000", '0x8c1e60 phase2 mode 0: call 0x8cbd30'),
    (0x8C48F0, "4989f2b800000200498d54240441bdffffffff4d29ca4981fa000002004c0f4fd04c39c2", '0x8c1e60 quantum loop: raw = min(0x20000, end-pos) ; need 4 bytes of header'),
    (0x8DB8A0, "554889e54157415641554154534881ec68010000", '0x8db8a0 prologue (8 instrs, sub rsp,0x168)'),
    (0x8DB979, "81e1ffff03004901ce4d39d6", '0x8db8a0 5-byte header compLen mask 0x3ffff'),
    (0x8DB9C0, "4189d6b8130400004181e6ffff0700c46278f7e2", '0x8db8a0 compressed header: 24-bit value & 0x7ffff = compLen ; bextr 0x413 = mode (bits 19..22)'),
    (0x8DB9FA, "4b8d8c1b20040000b8000004004881f901000400", '0x8db8a0 scratch = min(2*rawQuantum+0x420, 0x40000)'),
    (0x8DCAD0, "4c89c6b800000200498d5504bfffffffff4c29fe4881fe000002004989f3", '0x8db8a0 quantum loop: raw = min(0x20000, end-pos)'),
    (0x8DC8F1, "04244989f4e8755d0100", '0x8db8a0 mode A call 0x8f2670'),
]

# (site, target, kind, what)
CALLS = [
    (0x1AA111, 0x89B980, "call", "Init(): DecoderMemorySize(-1,-1)"),
    (0x1AA18A, 0x89B980, "call", "0x1aa160: DecoderMemorySize(-1,-1)"),
    (0x1AA2EA, 0x89B980, "call", "Decompress inline init: DecoderMemorySize(-1,-1)"),
    (0x1AA120, 0x17090, "call", "Init(): alloc"),
    (0x1AA2F9, 0x17090, "call", "Decompress inline init: alloc"),
    (0x1AA139, 0x170C0, "call", "Init(): free old"),
    (0x1AA312, 0x170C0, "call", "Decompress inline init: free old"),
    (0x1AA426, 0x89D3B0, "call", "Decompress -> OodleLZ_Decompress"),
    (0x1AA290, 0x89DAD0, "jmp", "DecodedSize tag 5 tail-jumps to the decode-buffer-size helper"),
    (0x1AA49B, 0x32A00, "call", "ResetStats: stopwatch reset"),
    (0x1AA4BB, 0x32740, "call", "Throughput: stopwatch read"),
    (0x1AA543, 0x326A0, "jmp", "static ctor tail-jumps to the stopwatch ctor"),
    (0x89B986, 0x89B990, "call", "0x89b980 wrapper"),
    (0x89D628, 0x89B990, "call", "wrapper sizes the state"),
    (0x89BE73, 0x89B990, "call", "driver sizes the state"),
    (0x89D602, 0x89F410, "call", "wrapper threaded-phase minimum"),
    (0x89BA4F, 0x90C100, "call", "state size id 6"),
    (0x89BA56, 0x927C00, "call", "state size id 7"),
    (0x89BA5D, 0x9359D0, "call", "state size id 10"),
    (0x89D7D6, 0x89BCB0, "call", "wrapper -> driver (first)"),
    (0x89D8BF, 0x89BCB0, "call", "wrapper -> driver (second)"),
    (0x89C57E, 0x8C1E60, "call", "driver type 6 -> Kraken"),
    (0x89C64C, 0x8DB8A0, "call", "driver type 10 -> Mermaid"),
    (0x89C443, 0x8CDDC0, "call", "driver type 12 -> Leviathan"),
    (0x8C47F7, 0x8CB610, "call", "Kraken phase-2 mode 1"),
    (0x8C4844, 0x8CBD30, "call", "Kraken phase-2 mode 0"),
    (0x8DC8F6, 0x8F2670, "call", "Mermaid parse variant (window null, sub-mode 1)"),
    (0x8DC992, 0x8F1030, "call", "Mermaid parse variant (window null, sub-mode != 1)"),
    (0x8DC895, 0x8EF9A0, "call", "Mermaid parse variant (window set, sub-mode 1)"),
    (0x8DC918, 0x8EE200, "call", "Mermaid parse variant (window set, sub-mode != 1)"),
    (0x8DC6E7, 0x8F3B40, "call", "Mermaid off24 reader"),
]

TYPE_ID_EXPECT = [1, 2, 4, 5, 6, 7, 8, 0, -1, -1, 9, 10, 13]
ID_NAMES = ["LZH", "LZHLW", "LZNIB", "None", "LZB16", "LZBLW", "LZA", "LZNA", "Kraken", "Mermaid", "BitKnit",
            "Selkie", "Hydra", "Leviathan"]
ERROR_BLOCK = 0x89C656
# 0x1294528: 13 rel32 entries indexed by header decode type; 0x12944a8: 11 entries indexed by compressor id
DISPATCH_JT_EXPECT = [0x89C3C3, 0x89C53A, 0x89C50A, 0x89C4E1, 0x89C44D, 0x89C4A2, 0x89C563, 0x89C588, 0x89C656,
                      0x89C656, 0x89C631, 0x89C5B5, 0x89C428]
SIZE_JT_EXPECT = [0x89B9FD, 0x89BA0E, 0x89BA67, 0x89BA67, 0x89BA67, 0x89BA67, 0x89BA4F, 0x89BA56, 0x89BA67, 0x89BA67,
                  0x89BA5D]

# Whole-image census expectations (E8, E9, Jcc, lea, q64, d32, RELA) per function address.
CENSUS_FUN = {
    0x1AA0F0: ([], [], [], [], [], [], []),
    0x1AA160: ([], [], [], [], [], [], []),
    0x1AA070: ([], [], [], [0x1aa517], [], [], []),
    0x1AA490: ([0x144348], [0x1493b7], [], [], [], [], []),
    0x1AA4B0: ([0x1454e2], [], [], [], [], [], []),
    0x1AA510: ([], [], [], [], [0x2dd60b8], [0x2dd60b8], [0x2dd60b8]),
    0x89DAD0: ([], [0x1aa290], [], [], [], [], []),
    0x89B980: ([0x1aa111, 0x1aa18a, 0x1aa2ea], [], [], [], [], [0x4677ea], []),
    0x89B990: ([0x89b986, 0x89bacb, 0x89be73, 0x89d628], [], [], [], [], [], []),
    0x89BCB0: ([0x89d7d6, 0x89d8bf], [], [], [], [], [], []),
    0x89D3B0: ([0x1aa426], [], [], [], [], [], []),
    0x89F410: ([0x89d602], [], [], [], [], [], []),
    0x8C1E60: ([0x89c57e], [], [], [], [], [], []),
    0x8DB8A0: ([0x89c64c], [], [], [], [], [], []),
    0x8CDDC0: ([0x89c443], [], [], [], [], [], []),
    0x8CB610: ([0x8c47f7], [], [], [], [], [], []),
    0x8CBD30: ([0x8c4844], [], [], [], [], [], []),
    0x8F47D0: ([0x8c21e7, 0x8c2302, 0x8c242e, 0x8c2650, 0x8c280e, 0x8c2969, 0x8ce1da, 0x8ce321, 0x8ce442, 0x8ce5cd, 0x8ce806, 0x8ce9d4, 0x8ceb37, 0x8dbc01, 0x8dbcf3, 0x8dbe40, 0x8dc0bd, 0x8dc2f7, 0x8dc464], [], [], [], [], [], []),
    0x8EE200: ([0x8dc918, 0x8dc96d, 0x8dca4b], [], [], [], [], [], []),
    0x8EF9A0: ([0x8dc895, 0x8dc8ca, 0x8dca20], [], [], [], [], [], []),
    0x8F1030: ([0x8dc992, 0x8dcaae], [], [], [], [], [], []),
    0x8F2670: ([0x8dc8f6, 0x8dc94b], [], [], [], [], [], []),
    0x8F3B40: ([0x8dc6e7, 0x8dc717], [], [], [], [], [], []),
}
CENSUS_CELLS = {
    0x2EE6720: ([0x1aa525], [], [], [], [(0x1aa128, 0), (0x1aa141, 0), (0x1aa1a1, 0), (0x1aa1ba, 0), (0x1aa301, 0), (0x1aa31a, 0), (0x1aa418, 0), (0x1aa52c, 4)], []),
    0x2EE6728: ([0x1aa324, 0x1aa432, 0x1aa497, 0x1aa4b7, 0x1aa53c], [], [], [], [], []),
    0x2EE6740: ([0x1aa0f9, 0x1aa16d, 0x1aa2d2], [], [], [], [], []),
    0x2EE6748: ([], [], [], [], [(0x1aa11c, 0), (0x1aa195, 0), (0x1aa2f5, 0), (0x1aa412, 0)], []),
    0x2EE6750: ([], [], [], [], [(0x1aa445, 0), (0x1aa4a3, 4), (0x1aa4cf, 0)], []),
}
# (lea, q64, d32, RELA, loose[(disp position, trailing imm bytes)], rel-branch) per cell
CELL_FDES = {
    CELL_PTR: {0x1AA0F0: 2, 0x1AA160: 2, 0x1AA2A0: 3, 0x1AA510: 2},
    CELL_LOCK: {0x1AA2A0: 2, 0x1AA490: 1, 0x1AA4B0: 1, 0x1AA510: 1},
    CELL_FLAG: {0x1AA0F0: 1, 0x1AA160: 1, 0x1AA2A0: 1},
    CELL_SIZE: {0x1AA0F0: 1, 0x1AA160: 1, 0x1AA2A0: 2},
    CELL_STATS: {0x1AA2A0: 1, 0x1AA490: 1, 0x1AA4B0: 1},
}


# ---------------------------------------------------------------------------------------------------
def fde_index(rows: list[tuple[int, int]]) -> dict[int, int]:
    return {r[0]: i for i, r in enumerate(rows)}


def fde_range(e: Elf, rows, index: dict[int, int], initial: int) -> int:
    """Pins the FDE table entry and the FDE record of `initial` and returns its address_range.
    Tolerant of the record length (FDE sizes vary with the CFI program); the initial location is checked."""
    require(initial in index, f"no FDE table entry for 0x{initial:x}")
    idx = index[initial]
    e.read(EH_FRAME_HDR + 12 + 8 * idx, 8)
    fde = rows[idx][1]
    record = e.read(fde, 16)
    length = struct.unpack("<I", record[0:4])[0]
    require(0x10 <= length <= 0x80 and length % 4 == 0, f"FDE record length 0x{length:x} at 0x{fde:x}")
    require(struct.unpack("<I", record[4:8])[0] != 0, f"FDE CIE pointer at 0x{fde:x}")
    location = fde + 8 + struct.unpack("<i", record[8:12])[0]
    require(location == initial, f"FDE initial_location 0x{location:x} != 0x{initial:x}")
    return struct.unpack("<I", record[12:16])[0]


def imm32(e: Elf, addr: int, opcode: bytes) -> int:
    b = e.read(addr, len(opcode) + 4)
    require(b[:len(opcode)] == opcode, f"expected {opcode.hex()} at 0x{addr:x}")
    return struct.unpack("<I", b[len(opcode):])[0]


def decode_table(e: Elf, table: int, count: int) -> list[int]:
    raw = e.read(table, 4 * count)
    return [table + struct.unpack_from("<i", raw, 4 * i)[0] for i in range(count)]


def not_file_backed(e: Elf, addr: int) -> None:
    """The cell is .bss: inside memsz of the RW segment, beyond its filesz (no initial bytes exist)."""
    inside = [p for p in e.img.loads if p["vaddr"] + p["filesz"] <= addr < p["vaddr"] + p["memsz"]]
    require(len(inside) == 1, f"0x{addr:x} is not in a segment's .bss tail")
    try:
        e.img.read(addr, 1)
    except ValueError:
        return
    raise SystemExit(f"FAIL 0x{addr:x} is file-backed (has initial bytes)")


# ---------------------------------------------------------------------------------------------------
def check_elf(e: Elf, rows, index) -> dict:
    facts: dict = {}
    # 1. function map: FDE extents (+ body hashes, skipped during mutation)
    for rva, rng, sha, name, role, label in MAP:
        got = fde_range(e, rows, index, rva)
        require(got == rng, f"FDE range of 0x{rva:x} ({name}) is 0x{got:x}, want 0x{rng:x}")
        if not e.skip_hash:
            require(digest(e.raw(rva, rng)) == sha, f"{name} ({rva:#x}) sha256")

    # 2. literal bytes
    for site, hexstr, what in PINS:
        e.bytes_at(site, hexstr, what)

    # 3. direct call/jmp targets
    for site, target, kind, what in CALLS:
        got = e.call(site) if kind == "call" else e.jmp(site)
        require(got == target, f"{what}: {kind} at 0x{site:x} -> 0x{got:x}, want 0x{target:x}")

    # 4. cells: .bss, addresses used by the pinned code
    for cell in (CELL_PTR, CELL_LOCK, CELL_FLAG, CELL_SIZE, CELL_STATS):
        not_file_backed(e, cell)
    require(e.lea(0x1AA2CF) == CELL_FLAG, "inline init flag cell")                  # lea rax,[rip+d]
    require(e.rip(0x1AA2F2, 7, 3) == CELL_SIZE, "inline init size cell")            # mov [rip+d],rdi
    require(e.rip(0x1AA2FE, 7, 3) == CELL_PTR, "inline init reads the pointer cell")
    require(e.rip(0x1AA317, 7, 3) == CELL_PTR, "inline init writes the pointer cell")
    require(e.rip(0x1AA410, 6, 2) == CELL_SIZE and e.rip(0x1AA416, 6, 2) == CELL_PTR, "Oodle stack args")

    # 5. state-size arithmetic derived from the bytes: universal cell size
    leaf6 = imm32(e, 0x90C100, b"\xb8")
    leaf7 = imm32(e, 0x927C00, b"\xb8")
    leaf10 = imm32(e, 0x9359D0, b"\xb8")
    base_state = imm32(e, 0x89B9C6, b"\xb8")                     # mov eax,0xa8
    scratch_cap = imm32(e, 0x89BA7D, b"\xb9")                    # mov ecx,0x40000
    scratch_slop = struct.unpack("<i", e.read(0x89BA87 + 4, 4))[0]   # lea rdx,[r14+r14+0x420]
    slack = e.read(0x89B98B, 3)
    require(slack == bytes.fromhex("83c010"), "0x89b980 adds 0x10")
    require((leaf6, leaf7, leaf10, base_state, scratch_cap, scratch_slop) ==
            (0x4D18, 0x2CDC, 0x3EA8, 0xA8, 0x40000, 0x420), "state-size constants")
    require(leaf7 <= leaf6 and leaf10 <= leaf6, "id 6 (the proxy for type -1) must be the largest leaf")
    universal = leaf6 + base_state + scratch_cap + 0x10
    require(universal == UNIVERSAL_CELL_SIZE, f"universal cell size 0x{universal:x}")
    newlz_max = base_state + scratch_cap + 0x10
    require(newlz_max == 0x400B8 and newlz_max <= universal, "newLZ state + scratch fits the universal cell")
    require(imm32(e, 0x89F410, b"\xb8") == 0x80000, "threaded minimum")
    require(struct.unpack("<I", e.read(ALLOCA_THRESHOLD_CELL, 4))[0] == 0x1000, "alloca threshold cell value")
    # clamp: type -1 (and > 13) -> 6; mask 0x3b00 -> ids 8,9,11,12,13; mask 0xfffff7cb; 0x3f13 fuzz-safe
    require(imm32(e, 0x89B9C0, b"\x41\xbf") == 0x3B00, "newLZ mask 0x3b00")
    require(imm32(e, 0x89B9D8, b"\xb9") == 0xFFFFF7CB, "mask 0xfffff7cb")
    require(imm32(e, 0x89BE30, b"\xb9") == 0x3F13 and imm32(e, 0x89D557, b"\xb8") == 0x3F13, "fuzz-safe mask 0x3f13")
    facts["newlz_ids"] = [i for i in range(14) if 0x3B00 >> i & 1]
    facts["fuzz_safe_ids"] = [i for i in range(14) if 0x3F13 >> i & 1]
    require(facts["newlz_ids"] == [8, 9, 11, 12, 13], "newLZ ids")
    require({8, 9} <= set(facts["fuzz_safe_ids"]), "Kraken and Mermaid are fuzz-safe decodable")
    require(e.read(0x89C252, 2) == b"\xb2\x51", "newLZ chunk-type bitmask 0x51")
    facts["newlz_types"] = [t for t in range(6, 13) if 0x51 >> (t - 6) & 1]
    require(facts["newlz_types"] == [6, 10, 12], "newLZ decode types")
    # size jump table 0x12944a8 (ids 0..10)
    sj = decode_table(e, SIZE_JT, 11)
    require(sj == SIZE_JT_EXPECT, f"size jump table {[hex(x) for x in sj]}")

    # 6. decode type -> compressor id -> name; type -> codec function through the dispatch jump table
    raw = e.read(TYPE_TO_ID, 52)
    t2i = list(struct.unpack("<13i", raw))
    require(t2i == TYPE_ID_EXPECT, f"type->id table {t2i}")
    names = []
    for i in range(14):
        t = e.reloc_target(NAME_TABLE + 8 * i)
        name = ID_NAMES[i].encode()
        e.cstring(t, name)
        names.append(name.decode())
    jt = decode_table(e, DISPATCH_JT, 13)
    require(jt == DISPATCH_JT_EXPECT, f"dispatch jump table {[hex(x) for x in jt]}")
    expect_site = {6: 0x89C57E, 10: 0x89C64C, 12: 0x89C443}
    for typ, site in expect_site.items():
        require(jt[typ] <= site < jt[typ] + 0x30, f"type {typ}: call 0x{site:x} not in block 0x{jt[typ]:x}")
    require(jt[8] == ERROR_BLOCK and jt[9] == ERROR_BLOCK, "types 8 and 9 route to the 'invalid decodeType' block")
    route = {t: (names[t2i[t]] if t2i[t] >= 0 else None) for t in range(13)}
    require(route[6] == "Kraken" and route[10] == "Mermaid" and route[12] == "Leviathan", f"routing {route}")
    facts["route"] = route
    facts["dispatch"] = jt
    return facts


# ---------------------------------------------------------------------------------------------------
def check_census(e: Elf, rows, res: dict) -> None:
    for t, (e8, e9, jcc, lea, q64, d32, reloc) in CENSUS_FUN.items():
        r = res[t]
        got = (r["e8"], r["e9"], r["jcc"], [p for p, _ in r["lea"]], r["q64"], r["d32"], r["reloc"])
        want = (e8, e9, jcc, lea, q64, d32, reloc)
        require(got == want, f"census of 0x{t:x}: got {got}, want {want}")
    for t, (lea, q64, d32, reloc, loose, branches) in CENSUS_CELLS.items():
        r = res[t]
        got = ([p for p, _ in r["lea"]], r["q64"], r["d32"], r["reloc"], list(r["loose"]), r["e8"] + r["e9"] + r["jcc"])
        want = (lea, q64, d32, reloc, loose, branches)
        require(got == want, f"census of cell 0x{t:x}: got {got}, want {want}")
        sites = [p for p, _ in r["lea"]] + [p for p, _ in r["loose"]]
        owners: dict[int, int] = {}
        for s in sites:
            init, rng = base.containing(e, rows, s)
            owners[init] = owners.get(init, 0) + 1
        require(owners == CELL_FDES[t], f"cell 0x{t:x} referenced from FDEs {owners}, want {CELL_FDES[t]}")


def run_census(e: Elf) -> dict:
    return base.census(base.segments(e), e.relocs, list(CENSUS_FUN) + list(CENSUS_CELLS))


def run(e: Elf, rows, index, cache: dict | None = None) -> dict:
    return check_elf(e, rows, index)


# ---------------------------------------------------------------------------------------------------
def mutation_self_test(e: Elf) -> tuple[int, int]:
    def fresh():
        rows = base.fde_table(e)
        return rows, fde_index(rows)
    e.recording = True
    rows, index = fresh()
    run(e, rows, index)
    e.recording = False
    pins = sorted(e.pins)
    relocs = sorted(e.reloc_pins)
    e.skip_hash = True
    table_lo, table_hi = EH_FRAME_HDR, EH_FRAME_HDR + 12 + 8 * 55790
    ran = survivors = 0
    cache_rows, cache_index = fresh()
    for addr, size in pins:
        for byte_addr in sorted({addr, addr + size - 1}):
            e.overlay = {}
            pristine = e.raw(byte_addr, 1)[0]
            e.overlay = {byte_addr: pristine ^ 0x01}
            ran += 1
            try:
                if table_lo <= byte_addr < table_hi:
                    r2, i2 = fresh()
                else:
                    r2, i2 = cache_rows, cache_index
                run(e, r2, i2)
            except (SystemExit, ValueError, struct.error, IndexError, KeyError):
                continue
            survivors += 1
            print(f"mutation survived at 0x{byte_addr:x} (pin 0x{addr:x}+{size})", file=sys.stderr)
    e.overlay = {}
    e.skip_hash = False
    for rva, rng, sha, name, role, label in MAP:           # the whole-body hash must catch its own first/last byte
        for byte_addr in (rva, rva + rng - 1):
            pristine = e.raw(byte_addr, 1)[0]
            e.overlay = {byte_addr: pristine ^ 0x01}
            ran += 1
            try:
                run(e, cache_rows, cache_index)
            except (SystemExit, ValueError, struct.error, IndexError, KeyError):
                continue
            survivors += 1
            print(f"body-hash mutation survived at 0x{byte_addr:x} ({name})", file=sys.stderr)
    e.overlay = {}
    e.skip_hash = True
    rran = 0
    for slot in relocs:
        e.reloc_flip = {slot}
        rran += 1
        try:
            run(e, cache_rows, cache_index)
        except (SystemExit, ValueError, struct.error, IndexError, KeyError):
            continue
        survivors += 1
        print(f"relocation mutation survived at 0x{slot:x}", file=sys.stderr)
    e.reloc_flip = set()
    e.skip_hash = False
    require(survivors == 0, f"{survivors} mutations survived")
    return ran, rran


def injection_probe(e: Elf, rows) -> int:
    """Whole-image re-run with references injected into padding: the census must report exactly them
    and the expectation check must reject them."""
    base_a = 0xBA0
    require(e.raw(base_a, 16) == b"\x90" * 16, "injection site is nop padding")
    overlay = {}
    for i, b in enumerate(b"\xe8" + struct.pack("<i", 0x1AA0F0 - (base_a + 5))):
        overlay[base_a + i] = b
    base_b = base_a + 8
    for i, b in enumerate(b"\x48\x8d\x05" + struct.pack("<i", CELL_FLAG - (base_b + 7))):
        overlay[base_b + i] = b
    e.overlay = overlay
    try:
        res = run_census(e)
    finally:
        e.overlay = {}
    require(res[0x1AA0F0]["e8"] == [base_a], f"injection probe: call to the dead Init() not reported: {res[0x1AA0F0]['e8']}")
    require(base_b + 3 in [p for p, _ in res[CELL_FLAG]["lea"]], "injection probe: lea of the flag cell not reported")
    try:
        check_census(e, rows, res)
    except SystemExit:
        return 2
    raise SystemExit("FAIL injection probe: injected references were accepted by the census check")


# ---------------------------------------------------------------------------------------------------
EXPECT_CROSSTAB = {(3, "8c0a"): 16328, (4, "8c06"): 2992}


def archive_crosstab() -> dict:
    sys.path.insert(0, str(ROOT / "scripts"))
    from psarc import Psarc  # noqa: E402
    root = ROOT / "local/extracted/CUSA08809"
    got: dict = {}
    for name in ("pak0.psarc", "pak1.psarc", "pakN.psarc", "pak2.psarc", "pak3.psarc", "cachedFiles.psarc"):
        path = root / name
        if not path.is_file():
            continue
        ps = Psarc(path)
        for member, idx in ps.members():
            if not member.endswith(".csdr"):
                continue
            data = ps.read(idx)
            if len(data) < 16:
                continue
            typ = int.from_bytes(data[-4:], "little") >> 29
            if typ in (3, 4):
                key = (typ, data[:2].hex())
                got[key] = got.get(key, 0) + 1
    return got


# ---------------------------------------------------------------------------------------------------
def print_map() -> None:
    print("| RVA | FDE bytes | Name | Role | Label |")
    print("|---|---|---|---|---|")
    for rva, rng, sha, name, role, label in MAP:
        print(f"| `0x{rva:x}` | {rng} (0x{rng:x}) | {name} | {role} | {label} |")


def main() -> int:
    arguments = sys.argv[1:]
    if not (ROOT / "local/re/eboot.elf").is_file():
        print("SKIP check_oodle_prereqs_newlz: local/re/eboot.elf not present")
        return 0
    if "--print-map" in arguments:
        print_map()
        return 0
    e = Elf()
    rows = base.fde_table(e)
    index = fde_index(rows)
    if "--tamper" in arguments:
        e.overlay = {0x89DAD1: e.raw(0x89DAD1, 1)[0] ^ 0x40}     # imm32 of `mov ecx,0x10` (the +0x10 slack)
    e.recording = True
    facts = run(e, rows, index)
    e.recording = False
    results = run_census(e)
    check_census(e, rows, results)
    note = ""
    if "--no-mutate" not in arguments and "--tamper" not in arguments:
        ran, rran = mutation_self_test(e)
        probes = base.detector_probes()
        inj = injection_probe(e, rows)
        note = (f"; mutation self-test {ran} byte mutants + {rran} relocation mutants + {probes} detector probes + "
                f"{inj} whole-image injection probes all rejected/detected")
    if "--archives" in arguments:
        got = archive_crosstab()
        require(got == EXPECT_CROSSTAB, f"archive tag x header-prefix cross-tab {got} != {EXPECT_CROSSTAB}")
        note += "; archive cross-tab: tag 3 <-> 8c0a x16328, tag 4 <-> 8c06 x2992, no other pairing (counts only)"
    print(f"OK check_oodle_prereqs_newlz: decoder-memory cells 0x2ee6720/28/40/48/50 are .bss; the only live "
          f"initialiser is the inline copy in 0x1aa2a0 (0x1aa0f0 and 0x1aa160 are unreferenced); universal size "
          f"0x{UNIVERSAL_CELL_SIZE:x}; type 6 -> Kraken 0x8c1e60 only, type 10 -> Mermaid 0x8db8a0, type 12 -> "
          f"Leviathan 0x8cddc0; newLZ ids {facts['newlz_ids']}; {len(MAP)} map functions, {len(PINS)} byte pins, "
          f"{len(CALLS)} call pins; guest bytes NOT executed{note}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
