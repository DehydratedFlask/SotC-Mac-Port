// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 418 (0x572a10): 0 bool and 0 bounded float variables and 0 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_418_572a10.hpp"

namespace {
using namespace l4;

struct Row {
    char kind;            // b bool, i/l/n ranged / local / unbounded int, f/u bounded / unbounded float, m console command, t 0x26db0 toggle, e 0x26ea0 toggle, s 0x27120 slider, S 0x27050 slider, g group
    const void* obj;
    int var;              // index into the bool / float tables (variables)
    int target;           // row index of the target variable (items)
    int s0;               // menu string: parent / label
    int s1;               // menu string: explicit text / caption
    int slider;           // slider table index
};

Row rows_of(Slot418Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'c', &o.r00, -1, -1, 0, -1, -1};
    out[1] = Row{'c', &o.r01, -1, -1, 1, -1, -1};
    out[2] = Row{'c', &o.r02, -1, -1, 2, -1, -1};
    out[3] = Row{'c', &o.r03, -1, -1, 3, -1, -1};
    out[4] = Row{'c', &o.r04, -1, -1, 4, -1, -1};
    out[5] = Row{'c', &o.r05, -1, -1, 5, -1, -1};
    out[6] = Row{'c', &o.r06, -1, -1, 6, -1, -1};
    out[7] = Row{'c', &o.r07, -1, -1, 7, -1, -1};
    out[8] = Row{'c', &o.r08, -1, -1, 8, -1, -1};
    out[9] = Row{'c', &o.r09, -1, -1, 9, -1, -1};
    out[10] = Row{'c', &o.r10, -1, -1, 10, -1, -1};
    out[11] = Row{'c', &o.r11, -1, -1, 11, -1, -1};
    out[12] = Row{'c', &o.r12, -1, -1, 12, -1, -1};
    out[13] = Row{'c', &o.r13, -1, -1, 13, -1, -1};
    out[14] = Row{'c', &o.r14, -1, -1, 14, -1, -1};
    out[15] = Row{'c', &o.r15, -1, -1, 15, -1, -1};
    out[16] = Row{'c', &o.r16, -1, -1, 16, -1, -1};
    out[17] = Row{'c', &o.r17, -1, -1, 17, -1, -1};
    out[18] = Row{'c', &o.r18, -1, -1, 18, -1, -1};
    out[19] = Row{'c', &o.r19, -1, -1, 19, -1, -1};
    out[20] = Row{'c', &o.r20, -1, -1, 20, -1, -1};
    out[21] = Row{'c', &o.r21, -1, -1, 21, -1, -1};
    out[22] = Row{'c', &o.r22, -1, -1, 22, -1, -1};
    out[23] = Row{'c', &o.r23, -1, -1, 23, -1, -1};
    out[24] = Row{'c', &o.r24, -1, -1, 24, -1, -1};
    out[25] = Row{'c', &o.r25, -1, -1, 25, -1, -1};
    out[26] = Row{'c', &o.r26, -1, -1, 26, -1, -1};
    out[27] = Row{'c', &o.r27, -1, -1, 27, -1, -1};
    out[28] = Row{'c', &o.r28, -1, -1, 28, -1, -1};
    out[29] = Row{'c', &o.r29, -1, -1, 29, -1, -1};
    out[30] = Row{'c', &o.r30, -1, -1, 30, -1, -1};
    out[31] = Row{'c', &o.r31, -1, -1, 31, -1, -1};
    out[32] = Row{'c', &o.r32, -1, -1, 32, -1, -1};
    out[33] = Row{'c', &o.r33, -1, -1, 33, -1, -1};
    out[34] = Row{'c', &o.r34, -1, -1, 34, -1, -1};
    out[35] = Row{'c', &o.r35, -1, -1, 35, -1, -1};
    out[36] = Row{'c', &o.r36, -1, -1, 36, -1, -1};
    out[37] = Row{'c', &o.r37, -1, -1, 37, -1, -1};
    out[38] = Row{'c', &o.r38, -1, -1, 38, -1, -1};
    out[39] = Row{'c', &o.r39, -1, -1, 39, -1, -1};
    out[40] = Row{'c', &o.r40, -1, -1, 40, -1, -1};
    out[41] = Row{'c', &o.r41, -1, -1, 41, -1, -1};
    out[42] = Row{'c', &o.r42, -1, -1, 42, -1, -1};
    out[43] = Row{'c', &o.r43, -1, -1, 43, -1, -1};
    out[44] = Row{'c', &o.r44, -1, -1, 44, -1, -1};
    out[45] = Row{'c', &o.r45, -1, -1, 45, -1, -1};
    out[46] = Row{'c', &o.r46, -1, -1, 46, -1, -1};
    out[47] = Row{'c', &o.r47, -1, -1, 47, -1, -1};
    out[48] = Row{'c', &o.r48, -1, -1, 48, -1, -1};
    out[49] = Row{'c', &o.r49, -1, -1, 49, -1, -1};
    out[50] = Row{'c', &o.r50, -1, -1, 50, -1, -1};
    out[51] = Row{'c', &o.r51, -1, -1, 51, -1, -1};
    out[52] = Row{'c', &o.r52, -1, -1, 52, -1, -1};
    out[53] = Row{'c', &o.r53, -1, -1, 53, -1, -1};
    out[54] = Row{'c', &o.r54, -1, -1, 54, -1, -1};
    out[55] = Row{'c', &o.r55, -1, -1, 55, -1, -1};
    out[56] = Row{'c', &o.r56, -1, -1, 56, -1, -1};
    out[57] = Row{'c', &o.r57, -1, -1, 57, -1, -1};
    out[58] = Row{'c', &o.r58, -1, -1, 58, -1, -1};
    out[59] = Row{'c', &o.r59, -1, -1, 59, -1, -1};
    out[60] = Row{'c', &o.r60, -1, -1, 60, -1, -1};
    out[61] = Row{'c', &o.r61, -1, -1, 61, -1, -1};
    out[62] = Row{'c', &o.r62, -1, -1, 62, -1, -1};
    out[63] = Row{'c', &o.r63, -1, -1, 63, -1, -1};
    out[64] = Row{'c', &o.r64, -1, -1, 64, -1, -1};
    out[65] = Row{'c', &o.r65, -1, -1, 65, -1, -1};
    out[66] = Row{'c', &o.r66, -1, -1, 66, -1, -1};
    out[67] = Row{'c', &o.r67, -1, -1, 67, -1, -1};
    out[68] = Row{'c', &o.r68, -1, -1, 68, -1, -1};
    out[69] = Row{'c', &o.r69, -1, -1, 69, -1, -1};
    out[70] = Row{'c', &o.r70, -1, -1, 70, -1, -1};
    out[71] = Row{'c', &o.r71, -1, -1, 71, -1, -1};
    out[72] = Row{'c', &o.r72, -1, -1, 72, -1, -1};
    out[73] = Row{'c', &o.r73, -1, -1, 73, -1, -1};
    out[74] = Row{'c', &o.r74, -1, -1, 74, -1, -1};
    out[75] = Row{'c', &o.r75, -1, -1, 75, -1, -1};
    out[76] = Row{'c', &o.r76, -1, -1, 76, -1, -1};
    out[77] = Row{'c', &o.r77, -1, -1, 77, -1, -1};
    out[78] = Row{'c', &o.r78, -1, -1, 78, -1, -1};
    out[79] = Row{'c', &o.r79, -1, -1, 79, -1, -1};
    out[80] = Row{'c', &o.r80, -1, -1, 80, -1, -1};
    out[81] = Row{'c', &o.r81, -1, -1, 81, -1, -1};
    out[82] = Row{'c', &o.r82, -1, -1, 82, -1, -1};
    out[83] = Row{'c', &o.r83, -1, -1, 83, -1, -1};
    out[84] = Row{'c', &o.r84, -1, -1, 84, -1, -1};
    out[85] = Row{'c', &o.r85, -1, -1, 85, -1, -1};
    out[86] = Row{'c', &o.r86, -1, -1, 86, -1, -1};
    out[87] = Row{'c', &o.r87, -1, -1, 87, -1, -1};
    out[88] = Row{'c', &o.r88, -1, -1, 88, -1, -1};
    out[89] = Row{'c', &o.r89, -1, -1, 89, -1, -1};
    out[90] = Row{'c', &o.r90, -1, -1, 90, -1, -1};
    out[91] = Row{'c', &o.r91, -1, -1, 91, -1, -1};
    out[92] = Row{'c', &o.r92, -1, -1, 92, -1, -1};
    out[93] = Row{'c', &o.r93, -1, -1, 93, -1, -1};
    out[94] = Row{'c', &o.r94, -1, -1, 94, -1, -1};
    out[95] = Row{'c', &o.r95, -1, -1, 95, -1, -1};
    out[96] = Row{'c', &o.r96, -1, -1, 96, -1, -1};
    out[97] = Row{'c', &o.r97, -1, -1, 97, -1, -1};
    out[98] = Row{'c', &o.r98, -1, -1, 98, -1, -1};
    out[99] = Row{'c', &o.r99, -1, -1, 99, -1, -1};
    out[100] = Row{'c', &o.r100, -1, -1, 100, -1, -1};
    out[101] = Row{'c', &o.r101, -1, -1, 101, -1, -1};
    out[102] = Row{'c', &o.r102, -1, -1, 102, -1, -1};
    out[103] = Row{'c', &o.r103, -1, -1, 103, -1, -1};
    out[104] = Row{'c', &o.r104, -1, -1, 104, -1, -1};
    out[105] = Row{'c', &o.r105, -1, -1, 105, -1, -1};
    out[106] = Row{'c', &o.r106, -1, -1, 106, -1, -1};
    out[107] = Row{'c', &o.r107, -1, -1, 107, -1, -1};
    out[108] = Row{'c', &o.r108, -1, -1, 108, -1, -1};
    out[109] = Row{'c', &o.r109, -1, -1, 109, -1, -1};
    out[110] = Row{'c', &o.r110, -1, -1, 110, -1, -1};
    out[111] = Row{'c', &o.r111, -1, -1, 111, -1, -1};
    out[112] = Row{'c', &o.r112, -1, -1, 112, -1, -1};
    out[113] = Row{'c', &o.r113, -1, -1, 113, -1, -1};
    out[114] = Row{'c', &o.r114, -1, -1, 114, -1, -1};
    out[115] = Row{'c', &o.r115, -1, -1, 115, -1, -1};
    out[116] = Row{'c', &o.r116, -1, -1, 116, -1, -1};
    out[117] = Row{'c', &o.r117, -1, -1, 117, -1, -1};
    out[118] = Row{'c', &o.r118, -1, -1, 118, -1, -1};
    out[119] = Row{'c', &o.r119, -1, -1, 119, -1, -1};
    out[120] = Row{'c', &o.r120, -1, -1, 120, -1, -1};
    out[121] = Row{'c', &o.r121, -1, -1, 121, -1, -1};
    out[122] = Row{'c', &o.r122, -1, -1, 122, -1, -1};
    out[123] = Row{'c', &o.r123, -1, -1, 123, -1, -1};
    out[124] = Row{'c', &o.r124, -1, -1, 124, -1, -1};
    out[125] = Row{'c', &o.r125, -1, -1, 125, -1, -1};
    out[126] = Row{'c', &o.r126, -1, -1, 126, -1, -1};
    out[127] = Row{'c', &o.r127, -1, -1, 127, -1, -1};
    out[128] = Row{'c', &o.r128, -1, -1, 128, -1, -1};
    out[129] = Row{'c', &o.r129, -1, -1, 129, -1, -1};
    out[130] = Row{'c', &o.r130, -1, -1, 130, -1, -1};
    out[131] = Row{'c', &o.r131, -1, -1, 131, -1, -1};
    out[132] = Row{'c', &o.r132, -1, -1, 132, -1, -1};
    out[133] = Row{'c', &o.r133, -1, -1, 133, -1, -1};
    out[134] = Row{'c', &o.r134, -1, -1, 134, -1, -1};
    out[135] = Row{'c', &o.r135, -1, -1, 135, -1, -1};
    out[136] = Row{'c', &o.r136, -1, -1, 136, -1, -1};
    out[137] = Row{'c', &o.r137, -1, -1, 137, -1, -1};
    out[138] = Row{'c', &o.r138, -1, -1, 138, -1, -1};
    out[139] = Row{'c', &o.r139, -1, -1, 139, -1, -1};
    out[140] = Row{'c', &o.r140, -1, -1, 140, -1, -1};
    out[141] = Row{'c', &o.r141, -1, -1, 141, -1, -1};
    out[142] = Row{'c', &o.r142, -1, -1, 142, -1, -1};
    out[143] = Row{'c', &o.r143, -1, -1, 143, -1, -1};
    out[144] = Row{'c', &o.r144, -1, -1, 144, -1, -1};
    out[145] = Row{'c', &o.r145, -1, -1, 145, -1, -1};
    out[146] = Row{'c', &o.r146, -1, -1, 146, -1, -1};
    out[147] = Row{'c', &o.r147, -1, -1, 147, -1, -1};
    out[148] = Row{'c', &o.r148, -1, -1, 148, -1, -1};
    out[149] = Row{'c', &o.r149, -1, -1, 149, -1, -1};
    out[150] = Row{'c', &o.r150, -1, -1, 150, -1, -1};
    out[151] = Row{'c', &o.r151, -1, -1, 151, -1, -1};
    out[152] = Row{'c', &o.r152, -1, -1, 152, -1, -1};
    out[153] = Row{'c', &o.r153, -1, -1, 153, -1, -1};
    out[154] = Row{'c', &o.r154, -1, -1, 154, -1, -1};
    out[155] = Row{'c', &o.r155, -1, -1, 155, -1, -1};
    out[156] = Row{'c', &o.r156, -1, -1, 156, -1, -1};
    out[157] = Row{'c', &o.r157, -1, -1, 157, -1, -1};
    out[158] = Row{'c', &o.r158, -1, -1, 158, -1, -1};
    out[159] = Row{'c', &o.r159, -1, -1, 159, -1, -1};
    out[160] = Row{'c', &o.r160, -1, -1, 160, -1, -1};
    out[161] = Row{'c', &o.r161, -1, -1, 161, -1, -1};
    out[162] = Row{'c', &o.r162, -1, -1, 162, -1, -1};
    out[163] = Row{'c', &o.r163, -1, -1, 163, -1, -1};
    out[164] = Row{'c', &o.r164, -1, -1, 164, -1, -1};
    out[165] = Row{'c', &o.r165, -1, -1, 165, -1, -1};
    out[166] = Row{'c', &o.r166, -1, -1, 166, -1, -1};
    out[167] = Row{'c', &o.r167, -1, -1, 167, -1, -1};
    out[168] = Row{'c', &o.r168, -1, -1, 168, -1, -1};
    out[169] = Row{'c', &o.r169, -1, -1, 169, -1, -1};
    out[170] = Row{'c', &o.r170, -1, -1, 170, -1, -1};
    out[171] = Row{'c', &o.r171, -1, -1, 171, -1, -1};
    out[172] = Row{'c', &o.r172, -1, -1, 172, -1, -1};
    out[173] = Row{'c', &o.r173, -1, -1, 173, -1, -1};
    out[174] = Row{'c', &o.r174, -1, -1, 174, -1, -1};
    out[175] = Row{'c', &o.r175, -1, -1, 175, -1, -1};
    out[176] = Row{'c', &o.r176, -1, -1, 176, -1, -1};
    out[177] = Row{'c', &o.r177, -1, -1, 177, -1, -1};
    out[178] = Row{'c', &o.r178, -1, -1, 178, -1, -1};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

[[maybe_unused]] std::uint64_t lint_dtor([[maybe_unused]] std::size_t var) {
    std::uint64_t dtor = 0;
    return dtor;
}

void section_constants() {
    CHECK(kSlot418Rva == 0x572a10 && kSlot418Size == 4726 && kSlot418RowCount == 179);
    CHECK(offsetof(Slot418Objects, r00) == 0x0);
    CHECK(offsetof(Slot418Objects, r01) == 0x10);
    CHECK(offsetof(Slot418Objects, r02) == 0x80);
    CHECK(offsetof(Slot418Objects, r03) == 0x90);
    CHECK(offsetof(Slot418Objects, r04) == 0xa0);
    CHECK(offsetof(Slot418Objects, r05) == 0xb0);
    CHECK(offsetof(Slot418Objects, r06) == 0xc0);
    CHECK(offsetof(Slot418Objects, r07) == 0xd0);
    CHECK(offsetof(Slot418Objects, r08) == 0xe0);
    CHECK(offsetof(Slot418Objects, r09) == 0xf0);
    CHECK(offsetof(Slot418Objects, r10) == 0x100);
    CHECK(offsetof(Slot418Objects, r11) == 0x110);
    CHECK(offsetof(Slot418Objects, r12) == 0x120);
    CHECK(offsetof(Slot418Objects, r13) == 0x130);
    CHECK(offsetof(Slot418Objects, r14) == 0x140);
    CHECK(offsetof(Slot418Objects, r15) == 0x150);
    CHECK(offsetof(Slot418Objects, r16) == 0x160);
    CHECK(offsetof(Slot418Objects, r17) == 0x170);
    CHECK(offsetof(Slot418Objects, r18) == 0x180);
    CHECK(offsetof(Slot418Objects, r19) == 0x190);
    CHECK(offsetof(Slot418Objects, r20) == 0x1a0);
    CHECK(offsetof(Slot418Objects, r21) == 0x1b0);
    CHECK(offsetof(Slot418Objects, r22) == 0x1c0);
    CHECK(offsetof(Slot418Objects, r23) == 0x1d0);
    CHECK(offsetof(Slot418Objects, r24) == 0x1e0);
    CHECK(offsetof(Slot418Objects, r25) == 0x1f0);
    CHECK(offsetof(Slot418Objects, r26) == 0x200);
    CHECK(offsetof(Slot418Objects, r27) == 0x210);
    CHECK(offsetof(Slot418Objects, r28) == 0x220);
    CHECK(offsetof(Slot418Objects, r29) == 0x230);
    CHECK(offsetof(Slot418Objects, r30) == 0x240);
    CHECK(offsetof(Slot418Objects, r31) == 0x250);
    CHECK(offsetof(Slot418Objects, r32) == 0x260);
    CHECK(offsetof(Slot418Objects, r33) == 0x270);
    CHECK(offsetof(Slot418Objects, r34) == 0x280);
    CHECK(offsetof(Slot418Objects, r35) == 0x290);
    CHECK(offsetof(Slot418Objects, r36) == 0x2a0);
    CHECK(offsetof(Slot418Objects, r37) == 0x2b0);
    CHECK(offsetof(Slot418Objects, r38) == 0x2c0);
    CHECK(offsetof(Slot418Objects, r39) == 0x2d0);
    CHECK(offsetof(Slot418Objects, r40) == 0x2e0);
    CHECK(offsetof(Slot418Objects, r41) == 0x2f0);
    CHECK(offsetof(Slot418Objects, r42) == 0x300);
    CHECK(offsetof(Slot418Objects, r43) == 0x310);
    CHECK(offsetof(Slot418Objects, r44) == 0x320);
    CHECK(offsetof(Slot418Objects, r45) == 0x330);
    CHECK(offsetof(Slot418Objects, r46) == 0x340);
    CHECK(offsetof(Slot418Objects, r47) == 0x350);
    CHECK(offsetof(Slot418Objects, r48) == 0x360);
    CHECK(offsetof(Slot418Objects, r49) == 0x370);
    CHECK(offsetof(Slot418Objects, r50) == 0x380);
    CHECK(offsetof(Slot418Objects, r51) == 0x390);
    CHECK(offsetof(Slot418Objects, r52) == 0x3a0);
    CHECK(offsetof(Slot418Objects, r53) == 0x3b0);
    CHECK(offsetof(Slot418Objects, r54) == 0x3c0);
    CHECK(offsetof(Slot418Objects, r55) == 0x3d0);
    CHECK(offsetof(Slot418Objects, r56) == 0x3e0);
    CHECK(offsetof(Slot418Objects, r57) == 0x3f0);
    CHECK(offsetof(Slot418Objects, r58) == 0x400);
    CHECK(offsetof(Slot418Objects, r59) == 0x410);
    CHECK(offsetof(Slot418Objects, r60) == 0x420);
    CHECK(offsetof(Slot418Objects, r61) == 0x430);
    CHECK(offsetof(Slot418Objects, r62) == 0x440);
    CHECK(offsetof(Slot418Objects, r63) == 0x450);
    CHECK(offsetof(Slot418Objects, r64) == 0x460);
    CHECK(offsetof(Slot418Objects, r65) == 0x470);
    CHECK(offsetof(Slot418Objects, r66) == 0x480);
    CHECK(offsetof(Slot418Objects, r67) == 0x490);
    CHECK(offsetof(Slot418Objects, r68) == 0x4a0);
    CHECK(offsetof(Slot418Objects, r69) == 0x4b0);
    CHECK(offsetof(Slot418Objects, r70) == 0x4c0);
    CHECK(offsetof(Slot418Objects, r71) == 0x4d0);
    CHECK(offsetof(Slot418Objects, r72) == 0x4e0);
    CHECK(offsetof(Slot418Objects, r73) == 0x4f0);
    CHECK(offsetof(Slot418Objects, r74) == 0x500);
    CHECK(offsetof(Slot418Objects, r75) == 0x510);
    CHECK(offsetof(Slot418Objects, r76) == 0x520);
    CHECK(offsetof(Slot418Objects, r77) == 0x530);
    CHECK(offsetof(Slot418Objects, r78) == 0x540);
    CHECK(offsetof(Slot418Objects, r79) == 0x550);
    CHECK(offsetof(Slot418Objects, r80) == 0x560);
    CHECK(offsetof(Slot418Objects, r81) == 0x570);
    CHECK(offsetof(Slot418Objects, r82) == 0x580);
    CHECK(offsetof(Slot418Objects, r83) == 0x590);
    CHECK(offsetof(Slot418Objects, r84) == 0x5a0);
    CHECK(offsetof(Slot418Objects, r85) == 0x5b0);
    CHECK(offsetof(Slot418Objects, r86) == 0x5c0);
    CHECK(offsetof(Slot418Objects, r87) == 0x5d0);
    CHECK(offsetof(Slot418Objects, r88) == 0x5e0);
    CHECK(offsetof(Slot418Objects, r89) == 0x5f0);
    CHECK(offsetof(Slot418Objects, r90) == 0x600);
    CHECK(offsetof(Slot418Objects, r91) == 0x610);
    CHECK(offsetof(Slot418Objects, r92) == 0x620);
    CHECK(offsetof(Slot418Objects, r93) == 0x630);
    CHECK(offsetof(Slot418Objects, r94) == 0x640);
    CHECK(offsetof(Slot418Objects, r95) == 0x650);
    CHECK(offsetof(Slot418Objects, r96) == 0x660);
    CHECK(offsetof(Slot418Objects, r97) == 0x670);
    CHECK(offsetof(Slot418Objects, r98) == 0x680);
    CHECK(offsetof(Slot418Objects, r99) == 0x690);
    CHECK(offsetof(Slot418Objects, r100) == 0x6a0);
    CHECK(offsetof(Slot418Objects, r101) == 0x6b0);
    CHECK(offsetof(Slot418Objects, r102) == 0x6c0);
    CHECK(offsetof(Slot418Objects, r103) == 0x6d0);
    CHECK(offsetof(Slot418Objects, r104) == 0x6e0);
    CHECK(offsetof(Slot418Objects, r105) == 0x6f0);
    CHECK(offsetof(Slot418Objects, r106) == 0x700);
    CHECK(offsetof(Slot418Objects, r107) == 0x710);
    CHECK(offsetof(Slot418Objects, r108) == 0x720);
    CHECK(offsetof(Slot418Objects, r109) == 0x730);
    CHECK(offsetof(Slot418Objects, r110) == 0x740);
    CHECK(offsetof(Slot418Objects, r111) == 0x750);
    CHECK(offsetof(Slot418Objects, r112) == 0x760);
    CHECK(offsetof(Slot418Objects, r113) == 0x770);
    CHECK(offsetof(Slot418Objects, r114) == 0x780);
    CHECK(offsetof(Slot418Objects, r115) == 0x790);
    CHECK(offsetof(Slot418Objects, r116) == 0x7a0);
    CHECK(offsetof(Slot418Objects, r117) == 0x7b0);
    CHECK(offsetof(Slot418Objects, r118) == 0x7c0);
    CHECK(offsetof(Slot418Objects, r119) == 0x7d0);
    CHECK(offsetof(Slot418Objects, r120) == 0x7e0);
    CHECK(offsetof(Slot418Objects, r121) == 0x7f0);
    CHECK(offsetof(Slot418Objects, r122) == 0x800);
    CHECK(offsetof(Slot418Objects, r123) == 0x810);
    CHECK(offsetof(Slot418Objects, r124) == 0x820);
    CHECK(offsetof(Slot418Objects, r125) == 0x830);
    CHECK(offsetof(Slot418Objects, r126) == 0x840);
    CHECK(offsetof(Slot418Objects, r127) == 0x850);
    CHECK(offsetof(Slot418Objects, r128) == 0x860);
    CHECK(offsetof(Slot418Objects, r129) == 0x870);
    CHECK(offsetof(Slot418Objects, r130) == 0x880);
    CHECK(offsetof(Slot418Objects, r131) == 0x890);
    CHECK(offsetof(Slot418Objects, r132) == 0x8a0);
    CHECK(offsetof(Slot418Objects, r133) == 0x8b0);
    CHECK(offsetof(Slot418Objects, r134) == 0x8c0);
    CHECK(offsetof(Slot418Objects, r135) == 0x8d0);
    CHECK(offsetof(Slot418Objects, r136) == 0x8e0);
    CHECK(offsetof(Slot418Objects, r137) == 0x8f0);
    CHECK(offsetof(Slot418Objects, r138) == 0x900);
    CHECK(offsetof(Slot418Objects, r139) == 0x910);
    CHECK(offsetof(Slot418Objects, r140) == 0x920);
    CHECK(offsetof(Slot418Objects, r141) == 0x930);
    CHECK(offsetof(Slot418Objects, r142) == 0x940);
    CHECK(offsetof(Slot418Objects, r143) == 0x950);
    CHECK(offsetof(Slot418Objects, r144) == 0x960);
    CHECK(offsetof(Slot418Objects, r145) == 0x970);
    CHECK(offsetof(Slot418Objects, r146) == 0x988);
    CHECK(offsetof(Slot418Objects, r147) == 0x998);
    CHECK(offsetof(Slot418Objects, r148) == 0x9a8);
    CHECK(offsetof(Slot418Objects, r149) == 0x9b8);
    CHECK(offsetof(Slot418Objects, r150) == 0x9c8);
    CHECK(offsetof(Slot418Objects, r151) == 0x9d8);
    CHECK(offsetof(Slot418Objects, r152) == 0x9e8);
    CHECK(offsetof(Slot418Objects, r153) == 0x9f8);
    CHECK(offsetof(Slot418Objects, r154) == 0xa08);
    CHECK(offsetof(Slot418Objects, r155) == 0xa18);
    CHECK(offsetof(Slot418Objects, r156) == 0xa28);
    CHECK(offsetof(Slot418Objects, r157) == 0xa38);
    CHECK(offsetof(Slot418Objects, r158) == 0xa48);
    CHECK(offsetof(Slot418Objects, r159) == 0xa58);
    CHECK(offsetof(Slot418Objects, r160) == 0xa68);
    CHECK(offsetof(Slot418Objects, r161) == 0xa78);
    CHECK(offsetof(Slot418Objects, r162) == 0xa88);
    CHECK(offsetof(Slot418Objects, r163) == 0xa98);
    CHECK(offsetof(Slot418Objects, r164) == 0xaa8);
    CHECK(offsetof(Slot418Objects, r165) == 0xab8);
    CHECK(offsetof(Slot418Objects, r166) == 0xac8);
    CHECK(offsetof(Slot418Objects, r167) == 0xad8);
    CHECK(offsetof(Slot418Objects, r168) == 0xae8);
    CHECK(offsetof(Slot418Objects, r169) == 0xb08);
    CHECK(offsetof(Slot418Objects, r170) == 0xb18);
    CHECK(offsetof(Slot418Objects, r171) == 0xb28);
    CHECK(offsetof(Slot418Objects, r172) == 0xb38);
    CHECK(offsetof(Slot418Objects, r173) == 0xb48);
    CHECK(offsetof(Slot418Objects, r174) == 0xb58);
    CHECK(offsetof(Slot418Objects, r175) == 0xb68);
    CHECK(offsetof(Slot418Objects, r176) == 0xb78);
    CHECK(offsetof(Slot418Objects, r177) == 0xb88);
    CHECK(offsetof(Slot418Objects, r178) == 0xbf8);
    CHECK(sizeof(Slot418Objects) == 0xc08);
    for (std::size_t n = 0; n < kSlot418StringCount; ++n) CHECK(std::strlen(kSlot418MenuStrings[n]) == kSlot418MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot418MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot418MenuStrings[row.s0]));
            CHECK(c.unwritten == 0xa5a5a5a5u);   // +0x0c is never written
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            (void)h;
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot418ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot418MenuStrings[row.s0], kSlot418MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot418MenuStrings[row.s1], kSlot418MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = tg.kind == 'b' ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot418MenuStrings[row.s1], kSlot418MenuStringLengths[row.s1]));
            } else {
                CHECK(it.text.data == tb.name.data && it.text.size == tb.name.size && it.text.ctl == nullptr);   // the variable's own name view
            }
            if (row.kind == 's' || row.kind == 'S') {
                CHECK(false);   // no slider rows in this slot
            } else if (is_float) {
                CHECK(it.range_min == 0.1f && it.range_max == 0.5f);   // float class slot3 = 1, slot6 = 3: the one narrowing case
            } else {
                CHECK(it.range_min == 1.0f && it.range_max == 10.0f);
            }
            break;
        }
    }
}

void section_alone() {
    ConCmdRegistry creg;
    Slot418Objects o;
    poison(o);
    const Slot418Result r = init_slot418_572a10(o, creg);
    CHECK(r.ok && r.rows_done == kSlot418RowCount);
    Row rows[kSlot418RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot418RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'i' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h') ++items;
    }
    CHECK(vars == kSlot418BoolCount + kSlot418IntCount + kSlot418LocalIntCount + kSlot418UIntCount + kSlot418CmdCount + kSlot418FloatCount + kSlot418UFloatCount);
    CHECK(items == 0);
    CHECK(vars == 0 && creg.pending.size() == 0);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot418CellCount && creg.atexit_log.size() == kSlot418RowCount - cells);   // name cells record nothing; no menu root
    bool root_seen = false;
    std::size_t at_base = 0;
    for (std::size_t i = 0; i < kSlot418RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0);
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : 0x272d0;
        check_atexit(creg, at, dtor, rows[i].obj);
    }
}

void section_failures() {
    {
        // Cells-only page: no variable registration exists; the rows are name-hash stores (no manager contract is read).
        ConCmdRegistry creg;
        Slot418Objects o;
        const Slot418Result r = init_slot418_572a10(o, creg);
        CHECK(r.ok && creg.pending.size() == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_418");
    return 0;
}
