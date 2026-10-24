#!/usr/bin/env python3
"""ELF-tie Init `0x531d60` alternate path `0x5327bd..0x532904` (327 B / 70 units).

Private: needs local/re/eboot.elf.  Pins every one of the 70 consecutive
instructions, the four direct-call rel32 targets, the five indirect-call sites,
the ten RIP-relative load targets, the vucomiss float compare, the gate byte
at `[0x2dfce70]+0x60`, and every branch target and join point.

Does NOT claim any callee body, vtable slot resolution, the values held by the
pinned globals, or the stack frame layout.
"""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
START = 0x5327BD
SIZE = 327
END = 0x532904
REGION_SHA256 = "34b848a9426ffd79d2cebc433472d3d8eba3c6b0686fdd9a8dfc5f0d4c7c0206"

# Exit stub
EXIT_STUB = 0x5327B6
EXIT_STUB_SIZE = 7

# Direct-call rel32 targets
CALL_AC5260 = 0xAC5260
CALL_3E7E0 = 0x3E7E0
CALL_4C4950 = 0x4C4950

# RIP-relative lea targets
LEA_2DFCE70 = 0x2DFCE70
LEA_3305568 = 0x3305568
LEA_2EA5170 = 0x2EA5170
LEA_1014DB9 = 0x1014DB9
LEA_1014DED = 0x1014DED
LEA_1014D71 = 0x1014D71
LEA_2EFE318 = 0x2EFE318

# RIP-relative mov targets
MOV_2EFE318 = 0x2EFE318
MOV_2EFE340 = 0x2EFE340

# vucomiss float const
FLOAT_1014C40 = 0x1014C40

# Branch targets
BR_JNE_5328CF = 0x5328CF
BR_JBE_5328CF = 0x5328CF
BR_JE_532864 = 0x532864
BR_JE_5328BC = 0x5328BC
BR_JMP_532904 = 0x532904

# (rva, size, sha256, kind, target)
#   kind: call=rel32, vcall=indirect, lea_rip=RIP-relative, mov_rip=mov [rip],
#         vucomiss=vucomiss [rip], jcc/jmp, setcc, pin=opcode/shape only
UNITS = [
    (0x5327BD, 7, "67d6656f09e77a668d9a2aaecd077c7f8b6d673a55a66aedfabd587de40904f3", "lea_rip", LEA_2DFCE70),
    (0x5327C4, 4, "2e886acf7022f26436056b8080c5ce424d76ae1beaefbe576186b5a6c776e253", "pin", None),
    (0x5327C8, 3, "a47fe7f6ec040a966cb24a4799930b575501c9fc0592cc5f2bb577ee74f7cdda", "pin", None),
    (0x5327CB, 6, "ef0085eda3604ab06f4c6621dbe58b93ffda74170befc592c9b030316ae46184", "jcc", BR_JNE_5328CF),
    (0x5327D1, 7, "86b717c63f25a247946e64503cae409995e85ecea87350c35944eabc6d8f3dcb", "lea_rip", LEA_3305568),
    (0x5327D8, 10, "68ec645461f4f2d67111ac576e39ca839324404e6fef5870c42b92446b1191f7", "pin", None),
    (0x5327E2, 5, "b65a7a82519a18f3d63caff87ac4c6bb87a49d7e49697649e3d417906af20128", "pin", None),
    (0x5327E7, 5, "9035771e6f4de89686c8ffc9749f8633ab5f8add83dab1b40a0588fd59519004", "pin", None),
    (0x5327EC, 5, "3dea137c35ad95a61ffbe26f7f85dd150675ab1bf5099b193473369a110d4d0e", "pin", None),
    (0x5327F1, 8, "09f33521741d5f767abdd289302338977561135ce65fa337f81e1db4ea764408", "pin", None),
    (0x5327F9, 3, "c9bff944b3e4886138ef80377469b1e6c420a80eeda2bb01002ee9165394e9e5", "pin", None),
    (0x5327FC, 5, "7908c3ae9ab9cd2631cd32ed61c8c1301875d7c39abc9040f00057cc3255ebe7", "call", CALL_AC5260),
    (0x532801, 3, "c9bff944b3e4886138ef80377469b1e6c420a80eeda2bb01002ee9165394e9e5", "pin", None),
    (0x532804, 3, "81c7e25bd38dde9dde06d8a46d8bb57256fcbc780400d7e09baf1a8585b5b409", "pin", None),
    (0x532807, 3, "9db50a8f32bcf398b5946159f1fa2e20280b166c33ef974cbd251a988ecb70ab", "vcall", None),
    (0x53280A, 3, "c9bff944b3e4886138ef80377469b1e6c420a80eeda2bb01002ee9165394e9e5", "pin", None),
    (0x53280D, 2, "2e2f89076640f42236c01bdc8663b50e0001b061170a88fd2ae0a99e0c7b1675", "pin", None),
    (0x53280F, 3, "81c7e25bd38dde9dde06d8a46d8bb57256fcbc780400d7e09baf1a8585b5b409", "pin", None),
    (0x532812, 3, "a37eeb5761d97112a4b5d99429af7e636c638ac3e39d137f2d96a26ec041f1d7", "vcall", None),
    (0x532815, 3, "c9bff944b3e4886138ef80377469b1e6c420a80eeda2bb01002ee9165394e9e5", "pin", None),
    (0x532818, 3, "81c7e25bd38dde9dde06d8a46d8bb57256fcbc780400d7e09baf1a8585b5b409", "pin", None),
    (0x53281B, 3, "76e65c840e73736ff53074c7156f6a65b42a73f6fecf882bbc652c62f38dbede", "vcall", None),
    (0x53281E, 3, "082370799dd74ac6b2f7a51526597980cc9a3f999fab5d8dacd223855e0f70bb", "pin", None),
    (0x532821, 8, "e251b5b0c5b345a3c3af5c99873f55a6d8763e145cfcc0de3cd66301ed6e97b0", "pin", None),
    (0x532829, 2, "7647777c227d53512d0ca616b868156458792b05694ebae60f82d4396cc5e7e7", "pin", None),
    (0x53282B, 3, "9b004760a32fd1a808c96a215e7da56714596d9e939aa5e7b816592fd45cab69", "pin", None),
    (0x53282E, 3, "78c44094f6bcab1688c4b7f0a06794ac042160250d3bc74918762f5283bea50f", "vcall", None),
    (0x532831, 9, "76362c4c07df9df7e94eec2684033badda111887a3c81e6da6110de27213998e", "pin", None),
    (0x53283A, 8, "09c1cf29859deb7da2192cfad511f122f8d35a3322a1f1110dee6e54f812b166", "vucomiss", FLOAT_1014C40),
    (0x532842, 6, "95f7491825cdb4f753745e3e1e95030bc5f153f4db40f8d51091907dcdca2806", "jcc", BR_JBE_5328CF),
    (0x532848, 7, "6ba2e976107925b3d425b656d6a8ee0391d6f139840499790608907fb931cb04", "lea_rip", LEA_2EA5170),
    (0x53284F, 3, "151d32d4e1e6ca5699d939bbc6112ec17a1353964867118ce8ee5a28bb8c125c", "pin", None),
    (0x532852, 4, "1194a03969a3d325dffe7b722c8c930b97c0dd099eea504506c49bb98d46ce6e", "pin", None),
    (0x532856, 2, "57a97cadea0452df11971965cf92e9e279b06b4b5b2806b94dc1174a4fa06871", "jcc", BR_JE_532864),
    (0x532858, 7, "8447da898dd6f9430e9227d4cc2aa8d357753fa2b473e205622215cbc9f3124d", "pin", None),
    (0x53285F, 3, "a47fe7f6ec040a966cb24a4799930b575501c9fc0592cc5f2bb577ee74f7cdda", "pin", None),
    (0x532862, 2, "2abdfeb9239d372e9083240f0ec724cff29673073be43d47153f2e5a1f543bbf", "jcc", BR_JE_5328BC),
    (0x532864, 7, "025b6bfe7ae8030d5156be7ec218d6511885136461e340138b8718c01a00b560", "lea_rip", LEA_1014DB9),
    (0x53286B, 7, "e3f1cbfbfde9f35fe6cf9f8f87faf5efd77de1c7f0d0dc9a9fcde008c9a0906c", "lea_rip", LEA_1014DED),
    (0x532872, 8, "890649d0acb882da13a440bc388e21c25cad199984529b90c345e6aea2da2b31", "pin", None),
    (0x53287A, 5, "f7e051a6d6eb2f09dcad3b45e2d523abb559e6d07859450e8b00dc1edde4558c", "pin", None),
    (0x53287F, 5, "bce257e988d5cf159b57a7b1e2b2a4d627f25434f74ae88d7e83a2d9560a38b9", "pin", None),
    (0x532884, 2, "07da0b176e80afd9779a094eb848879d2a9616427d8b594a261bcad1c56bb18d", "pin", None),
    (0x532886, 6, "58cbbaba5a9b49d32bcb8c7ffb07e7d49e264157cc4bb43ad2ba0c13a59c6f6e", "pin", None),
    (0x53288C, 5, "19df2df817a7ca0ec68eefa3200e4246cda6bbc66ed1a599b5da1905a3bcd97d", "pin", None),
    (0x532891, 7, "c9fb9757dcc91bcf5cfceda545f103ea891910fa404ef03810e79eaf432da808", "lea_rip", LEA_1014D71),
    (0x532898, 5, "42ea0118bcf665636232c490aa1572abdfca31c0c5b4ef252d89f6308657497c", "pin", None),
    (0x53289D, 2, "8359c8f2ab7af5c7569d43a68ce42c771d8e5c6e1349821ed39c1c0234f3785d", "pin", None),
    (0x53289F, 4, "5e310891d38bd1406bd43d58e3fa29d0083a9d8e7b4bbb4658f2ad26f15025d6", "pin", None),
    (0x5328A3, 9, "c650cf096f5d046a83e0ac9e17e9c4ec6775ecf985334df375ea649992accd63", "pin", None),
    (0x5328AC, 11, "738bf3af243c9acc0723895fdc1a3b26d864f2240391928f235e4e8f237d6be5", "pin", None),
    (0x5328B7, 5, "80fc728a26b78316ecc76e73f309754fda05b790619c0de246f723597e96846f", "call", CALL_3E7E0),
    (0x5328BC, 2, "ea63ec2753e75a486bcfb90e99e21daed745b64507f96ae77cead1fd85b00784", "pin", None),
    (0x5328BE, 3, "32bd539e9a830cf5b33ed4e3a9f0133b37c2c2d0745c4bb5f0304ab229f4e3c9", "pin", None),
    (0x5328C1, 5, "bc40cd63763c2484d8a7fb293bff0b10324403400345e142d4702f3d53a84ad3", "call", CALL_4C4950),
    (0x5328C6, 4, "4c1648caac4a2a956dff05c0ed8149ebf8310fb0108beceee7f5b3e6d7f28aaf", "pin", None),
    (0x5328CA, 3, "558681fe421c74c7b3a27974f59cddcc2d03b814d5f57165c1baabd2f884c044", "pin", None),
    (0x5328CD, 2, "f72bb6d714bb27c33c8ff49920934df92bbe4248f353952aa7d630a041b4471a", "jmp", BR_JMP_532904),
    (0x5328CF, 5, "b9d7f44bf15b9548c00ba1847adfd13bedfcfeac25bd22374209ef912e52ee18", "pin", None),
    (0x5328D4, 5, "ede52ecb67befc6c0cf327182583244f2501eeb99c6d95bca92031bf029846ef", "call", CALL_4C4950),
    (0x5328D9, 4, "4c1648caac4a2a956dff05c0ed8149ebf8310fb0108beceee7f5b3e6d7f28aaf", "pin", None),
    (0x5328DD, 3, "883647eeb03616e71d2601359c439ce949f3aec6e3fa3b21a608b219913b9e44", "pin", None),
    (0x5328E0, 7, "4af907794663712b799ee312bb260c0f4da3b3df981a0429876b59d4497fb47c", "mov_rip", MOV_2EFE318),
    (0x5328E7, 7, "820dea855d415588dc7e9e034f20ec0e07f7fb10c14f3215af9e3990f1a1ac38", "lea_rip", LEA_2EFE318),
    (0x5328EE, 2, "2e2f89076640f42236c01bdc8663b50e0001b061170a88fd2ae0a99e0c7b1675", "pin", None),
    (0x5328F0, 2, "6e7c72ff1c0aef12b52b1cab58166f527b87f2b37842ad6e82a9c81524c33bff", "vcall", None),
    (0x5328F2, 7, "2f819d30f4f3dce7956712bdc61458e9d5fadb37405ccd7077d60d7241d1a0ce", "mov_rip", MOV_2EFE340),
    (0x5328F9, 4, "2e886acf7022f26436056b8080c5ce424d76ae1beaefbe576186b5a6c776e253", "pin", None),
    (0x5328FD, 3, "a47fe7f6ec040a966cb24a4799930b575501c9fc0592cc5f2bb577ee74f7cdda", "pin", None),
    (0x532900, 4, "b9f56af5c8bbd9d2d71b0ba437816ea6beab0dc54cc1428262f290b42f119107", "setcc", None),
]

NATIVE_HPP = ROOT / "native/core/init_531d60_post_base_alt_5327bd.hpp"
NATIVE_CPP = ROOT / "native/core/init_531d60_post_base_alt_5327bd.cpp"
SLICE_HPP = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
SLICE_CPP = ROOT / "native/core/appmain_init_ctor_98a90_slice.cpp"
TEST = ROOT / "native/tests/test_appmain_init_ctor_98a90_slice.cpp"
CMAKE = ROOT / "native/CMakeLists.txt"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(len(UNITS) == 70, "unit count")
    require(START + SIZE == END, "size arithmetic")

    cursor = START
    for rva, size, sha, kind, target in UNITS:
        require(rva == cursor, f"contiguity at 0x{rva:x} (expected 0x{cursor:x})")
        raw = img.read(rva, size)
        require(len(raw) == size, f"short read at 0x{rva:x}")
        got = hashlib.sha256(raw).hexdigest()
        require(got == sha, f"sha mismatch at 0x{rva:x}: {got[:16]}… != {sha[:16]}…")

        if kind == "call":
            require(raw[0] == 0xE8, f"call opcode at 0x{rva:x}")
            rel = struct.unpack("<i", raw[1:5])[0]
            require(rva + 5 + rel == target, f"call target at 0x{rva:x}")
        elif kind == "vcall":
            require(raw[0] == 0xFF, f"vcall opcode at 0x{rva:x}")
            if raw[1] == 0x50:
                require(raw[2] in (0x20, 0x28, 0x48, 0x50),
                        f"vcall slot at 0x{rva:x}")
            elif raw[1] == 0x10:
                # call *(%rax) is exactly two bytes; there is no slot displacement.
                require(len(raw) == 2, f"vcall [rax] must be 2 bytes at 0x{rva:x}")
            else:
                raise SystemExit(
                    f"FAIL unexpected vcall encoding at 0x{rva:x}: {raw.hex()}"
                )
        elif kind == "lea_rip":
            require(raw[0:2] in (bytes([0x48, 0x8D]), bytes([0x4C, 0x8D])),
                    f"lea opcode at 0x{rva:x}")
            require((raw[2] & 0xC7) == 0x05, f"lea rip-rel at 0x{rva:x}")
            rel = struct.unpack("<i", raw[3:7])[0]
            require(rva + 7 + rel == target, f"lea target at 0x{rva:x}")
        elif kind == "mov_rip":
            require(raw[0:2] == bytes([0x48, 0x8B]) and (raw[2] & 0xC7) == 0x05,
                    f"mov rip-rel opcode at 0x{rva:x}")
            rel = struct.unpack("<i", raw[3:7])[0]
            require(rva + 7 + rel == target, f"mov rip target at 0x{rva:x}")
        elif kind == "vucomiss":
            require(raw[0:4] == bytes([0xC5, 0xF8, 0x2E, 0x05]),
                    f"vucomiss opcode at 0x{rva:x}")
            rel = struct.unpack("<i", raw[4:8])[0]
            require(rva + 8 + rel == target, f"vucomiss target at 0x{rva:x}")
        elif kind == "jcc":
            require(raw[0] in (0x0F, 0x74, 0x75), f"jcc opcode at 0x{rva:x}")
            if raw[0] == 0x0F:
                rel = struct.unpack("<i", raw[2:6])[0]
                require(rva + 6 + rel == target, f"jcc32 target at 0x{rva:x}")
            else:
                require(rva + 2 + struct.unpack("<b", raw[1:2])[0] == target,
                        f"jcc8 target at 0x{rva:x}")
        elif kind == "jmp":
            require(raw[0] == 0xEB, f"jmp opcode at 0x{rva:x}")
            rel = struct.unpack("<b", raw[1:2])[0]
            require(rva + 2 + rel == target, f"jmp target at 0x{rva:x}")
        elif kind == "setcc":
            require(raw[0:2] in (bytes([0x41, 0x0F]), bytes([0x0F, 0x94])),
                    f"setcc opcode at 0x{rva:x}")
        cursor = rva + size
    require(cursor == END, "units cover the claimed span")

    require(hashlib.sha256(img.read(START, SIZE)).hexdigest() == REGION_SHA256,
            "region sha")

    # Exit stub verification.
    require(img.read(EXIT_STUB, 2) == bytes([0x31, 0xC0]),
            "exit stub xor eax,eax")
    require(img.read(EXIT_STUB + 2, 5)[0] == 0xE9,
            "exit stub jmp opcode")
    jmp_rel = struct.unpack("<i", img.read(EXIT_STUB + 3, 4))[0]
    require(EXIT_STUB + 7 + jmp_rel == 0x532C6F,
            "exit stub jmp target 0x532c6f")

    # Verify the two bytes after the claimed span (continue_c entry).
    require(img.read(END, 2) == bytes([0x41, 0x0F]),
            "post-block movzx esi,r14b")

    # Native header checks (case-insensitive: the header uses upper-case hex digits).
    nh = NATIVE_HPP.read_text().lower()

    def has(token: str) -> bool:
        return token.lower() in nh

    require(has("init_531d60_post_base_alt_5327bd"), "API")
    require(has("not modelled"), "omission")
    require(has("kInit531d60PostBaseAlt5327bdRva = 0x5327bd"), "start rva")
    require(has("kInit531d60PostBaseAlt5327bdEndRva = 0x532904"), "end rva")
    require(has("kInit531d60PostBaseAlt5327bdUnitCount"), "unit count")
    require(has("kAlt5327bdGateCellRva = 0x2dfce70"), "gate cell pin")
    require(has("kAlt5327bdObj3305568Rva = 0x3305568"), "obj cell pin")
    require(has("kAlt5327bdFloatConstRva = 0x1014c40"), "float const pin")
    require(has("kAlt5327bdCallAc5260Rva = 0xac5260"), "ac5260 pin")
    require(has("kAlt5327bdCell2efe318Rva = 0x2efe318"), "2efe318 pin")
    require(has("kAlt5327bdCell2efe340Rva = 0x2efe340"), "2efe340 pin")
    require(has("0x2ea5170"), "container pin")
    for mnem in ("jne 0x5328cf", "call [rax+0x48]", "jmp 0x532904",
                 "sete r14b", "mov rax, [0x2efe318]"):
        require(has(mnem), f"unit listing {mnem}")

    # Native cpp checks.
    ncp = NATIVE_CPP.read_text()
    require("call_direct_count == 4" in ncp, "4 direct calls")
    require("vcall_count == 5" in ncp, "5 vcalls")
    require("lea_rip_count == 7" in ncp, "7 lea rip")
    require("mov_rip_count == 2" in ncp, "2 mov rip")
    require("float_compare_count == 1" in ncp, "1 vucomiss")
    require("branch_count == 5" in ncp, "5 branches")
    require("setcc_count == 1" in ncp, "1 sete")

    # Slice wiring.
    shpp = SLICE_HPP.read_text()
    require("init_531d60_post_base_alt_5327bd" in shpp, "alt in slice hpp")
    scpp = SLICE_CPP.read_text()
    require("init_531d60_post_base_alt_5327bd" in scpp, "alt in slice cpp")

    # CMake.
    cm = CMAKE.read_text()
    require("init_531d60_post_base_alt_5327bd.cpp" in cm, "alt in CMake")

    # Test.
    tt = TEST.read_text()
    require("alt_5327bd" in tt, "alt in test")

    print(
        f"OK check_f531d60_post_base_alt_5327bd: span=0x{START:x}+{SIZE}B units=70; "
        f"calls ac5260/3e7e0/4c4950×2; vcalls +0x48/+0x28/+0x50/+0x20/[rax]; "
        f"lea 2dfce70/3305568/2ea5170/1014db9/1014ded/1014d71/2efe318; "
        f"mov 2efe318/2efe340; vucomiss 1014c40; "
        f"jne→5328cf jbe→5328cf je→532864 je→5328bc jmp→532904; "
        f"gate [2dfce70]+0x60; join=532904 or sete; "
        f"exit stub 0x5327b6→0x532c6f; "
        f"callee_bodies/vtable_slots/global_values/stack_frame NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
