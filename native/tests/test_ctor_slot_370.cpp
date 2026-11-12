// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 370 (0x720b50): 36 bool and 131 bounded float variables and 43 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include "core/ctor_slot_370_720b50.hpp"

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
    const void* ext;      // identity pointer of an external slider target (target == -1), otherwise nullptr
};

Row rows_of(Slot370Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'l', &o.r00, 0, -1, -1, -1, -1, nullptr};
    out[1] = Row{'b', &o.r01, 0, -1, -1, -1, -1, nullptr};
    out[2] = Row{'b', &o.r02, 1, -1, -1, -1, -1, nullptr};
    out[3] = Row{'f', &o.r03, 0, -1, -1, -1, -1, nullptr};
    out[4] = Row{'f', &o.r04, 1, -1, -1, -1, -1, nullptr};
    out[5] = Row{'b', &o.r05, 2, -1, -1, -1, -1, nullptr};
    out[6] = Row{'b', &o.r06, 3, -1, -1, -1, -1, nullptr};
    out[7] = Row{'f', &o.r07, 2, -1, -1, -1, -1, nullptr};
    out[8] = Row{'f', &o.r08, 3, -1, -1, -1, -1, nullptr};
    out[9] = Row{'f', &o.r09, 4, -1, -1, -1, -1, nullptr};
    out[10] = Row{'f', &o.r10, 5, -1, -1, -1, -1, nullptr};
    out[11] = Row{'f', &o.r11, 6, -1, -1, -1, -1, nullptr};
    out[12] = Row{'f', &o.r12, 7, -1, -1, -1, -1, nullptr};
    out[13] = Row{'f', &o.r13, 8, -1, -1, -1, -1, nullptr};
    out[14] = Row{'b', &o.r14, 4, -1, -1, -1, -1, nullptr};
    out[15] = Row{'f', &o.r15, 9, -1, -1, -1, -1, nullptr};
    out[16] = Row{'f', &o.r16, 10, -1, -1, -1, -1, nullptr};
    out[17] = Row{'f', &o.r17, 11, -1, -1, -1, -1, nullptr};
    out[18] = Row{'f', &o.r18, 12, -1, -1, -1, -1, nullptr};
    out[19] = Row{'f', &o.r19, 13, -1, -1, -1, -1, nullptr};
    out[20] = Row{'f', &o.r20, 14, -1, -1, -1, -1, nullptr};
    out[21] = Row{'f', &o.r21, 15, -1, -1, -1, -1, nullptr};
    out[22] = Row{'f', &o.r22, 16, -1, -1, -1, -1, nullptr};
    out[23] = Row{'b', &o.r23, 5, -1, -1, -1, -1, nullptr};
    out[24] = Row{'f', &o.r24, 17, -1, -1, -1, -1, nullptr};
    out[25] = Row{'f', &o.r25, 18, -1, -1, -1, -1, nullptr};
    out[26] = Row{'f', &o.r26, 19, -1, -1, -1, -1, nullptr};
    out[27] = Row{'f', &o.r27, 20, -1, -1, -1, -1, nullptr};
    out[28] = Row{'f', &o.r28, 21, -1, -1, -1, -1, nullptr};
    out[29] = Row{'f', &o.r29, 22, -1, -1, -1, -1, nullptr};
    out[30] = Row{'f', &o.r30, 23, -1, -1, -1, -1, nullptr};
    out[31] = Row{'f', &o.r31, 24, -1, -1, -1, -1, nullptr};
    out[32] = Row{'b', &o.r32, 6, -1, -1, -1, -1, nullptr};
    out[33] = Row{'f', &o.r33, 25, -1, -1, -1, -1, nullptr};
    out[34] = Row{'f', &o.r34, 26, -1, -1, -1, -1, nullptr};
    out[35] = Row{'f', &o.r35, 27, -1, -1, -1, -1, nullptr};
    out[36] = Row{'f', &o.r36, 28, -1, -1, -1, -1, nullptr};
    out[37] = Row{'f', &o.r37, 29, -1, -1, -1, -1, nullptr};
    out[38] = Row{'f', &o.r38, 30, -1, -1, -1, -1, nullptr};
    out[39] = Row{'f', &o.r39, 31, -1, -1, -1, -1, nullptr};
    out[40] = Row{'f', &o.r40, 32, -1, -1, -1, -1, nullptr};
    out[41] = Row{'b', &o.r41, 7, -1, -1, -1, -1, nullptr};
    out[42] = Row{'b', &o.r42, 8, -1, -1, -1, -1, nullptr};
    out[43] = Row{'f', &o.r43, 33, -1, -1, -1, -1, nullptr};
    out[44] = Row{'f', &o.r44, 34, -1, -1, -1, -1, nullptr};
    out[45] = Row{'f', &o.r45, 35, -1, -1, -1, -1, nullptr};
    out[46] = Row{'f', &o.r46, 36, -1, -1, -1, -1, nullptr};
    out[47] = Row{'f', &o.r47, 37, -1, -1, -1, -1, nullptr};
    out[48] = Row{'f', &o.r48, 38, -1, -1, -1, -1, nullptr};
    out[49] = Row{'f', &o.r49, 39, -1, -1, -1, -1, nullptr};
    out[50] = Row{'b', &o.r50, 9, -1, -1, -1, -1, nullptr};
    out[51] = Row{'f', &o.r51, 40, -1, -1, -1, -1, nullptr};
    out[52] = Row{'f', &o.r52, 41, -1, -1, -1, -1, nullptr};
    out[53] = Row{'f', &o.r53, 42, -1, -1, -1, -1, nullptr};
    out[54] = Row{'f', &o.r54, 43, -1, -1, -1, -1, nullptr};
    out[55] = Row{'f', &o.r55, 44, -1, -1, -1, -1, nullptr};
    out[56] = Row{'f', &o.r56, 45, -1, -1, -1, -1, nullptr};
    out[57] = Row{'f', &o.r57, 46, -1, -1, -1, -1, nullptr};
    out[58] = Row{'b', &o.r58, 10, -1, -1, -1, -1, nullptr};
    out[59] = Row{'f', &o.r59, 47, -1, -1, -1, -1, nullptr};
    out[60] = Row{'f', &o.r60, 48, -1, -1, -1, -1, nullptr};
    out[61] = Row{'f', &o.r61, 49, -1, -1, -1, -1, nullptr};
    out[62] = Row{'f', &o.r62, 50, -1, -1, -1, -1, nullptr};
    out[63] = Row{'f', &o.r63, 51, -1, -1, -1, -1, nullptr};
    out[64] = Row{'f', &o.r64, 52, -1, -1, -1, -1, nullptr};
    out[65] = Row{'f', &o.r65, 53, -1, -1, -1, -1, nullptr};
    out[66] = Row{'b', &o.r66, 11, -1, -1, -1, -1, nullptr};
    out[67] = Row{'f', &o.r67, 54, -1, -1, -1, -1, nullptr};
    out[68] = Row{'f', &o.r68, 55, -1, -1, -1, -1, nullptr};
    out[69] = Row{'f', &o.r69, 56, -1, -1, -1, -1, nullptr};
    out[70] = Row{'f', &o.r70, 57, -1, -1, -1, -1, nullptr};
    out[71] = Row{'f', &o.r71, 58, -1, -1, -1, -1, nullptr};
    out[72] = Row{'f', &o.r72, 59, -1, -1, -1, -1, nullptr};
    out[73] = Row{'f', &o.r73, 60, -1, -1, -1, -1, nullptr};
    out[74] = Row{'b', &o.r74, 12, -1, -1, -1, -1, nullptr};
    out[75] = Row{'f', &o.r75, 61, -1, -1, -1, -1, nullptr};
    out[76] = Row{'f', &o.r76, 62, -1, -1, -1, -1, nullptr};
    out[77] = Row{'f', &o.r77, 63, -1, -1, -1, -1, nullptr};
    out[78] = Row{'f', &o.r78, 64, -1, -1, -1, -1, nullptr};
    out[79] = Row{'f', &o.r79, 65, -1, -1, -1, -1, nullptr};
    out[80] = Row{'f', &o.r80, 66, -1, -1, -1, -1, nullptr};
    out[81] = Row{'b', &o.r81, 13, -1, -1, -1, -1, nullptr};
    out[82] = Row{'f', &o.r82, 67, -1, -1, -1, -1, nullptr};
    out[83] = Row{'f', &o.r83, 68, -1, -1, -1, -1, nullptr};
    out[84] = Row{'f', &o.r84, 69, -1, -1, -1, -1, nullptr};
    out[85] = Row{'b', &o.r85, 14, -1, -1, -1, -1, nullptr};
    out[86] = Row{'b', &o.r86, 15, -1, -1, -1, -1, nullptr};
    out[87] = Row{'b', &o.r87, 16, -1, -1, -1, -1, nullptr};
    out[88] = Row{'f', &o.r88, 70, -1, -1, -1, -1, nullptr};
    out[89] = Row{'f', &o.r89, 71, -1, -1, -1, -1, nullptr};
    out[90] = Row{'f', &o.r90, 72, -1, -1, -1, -1, nullptr};
    out[91] = Row{'f', &o.r91, 73, -1, -1, -1, -1, nullptr};
    out[92] = Row{'f', &o.r92, 74, -1, -1, -1, -1, nullptr};
    out[93] = Row{'f', &o.r93, 75, -1, -1, -1, -1, nullptr};
    out[94] = Row{'f', &o.r94, 76, -1, -1, -1, -1, nullptr};
    out[95] = Row{'f', &o.r95, 77, -1, -1, -1, -1, nullptr};
    out[96] = Row{'f', &o.r96, 78, -1, -1, -1, -1, nullptr};
    out[97] = Row{'b', &o.r97, 17, -1, -1, -1, -1, nullptr};
    out[98] = Row{'u', &o.r98, 0, -1, -1, -1, -1, nullptr};
    out[99] = Row{'u', &o.r99, 1, -1, -1, -1, -1, nullptr};
    out[100] = Row{'u', &o.r100, 2, -1, -1, -1, -1, nullptr};
    out[101] = Row{'f', &o.r101, 79, -1, -1, -1, -1, nullptr};
    out[102] = Row{'f', &o.r102, 80, -1, -1, -1, -1, nullptr};
    out[103] = Row{'f', &o.r103, 81, -1, -1, -1, -1, nullptr};
    out[104] = Row{'f', &o.r104, 82, -1, -1, -1, -1, nullptr};
    out[105] = Row{'l', &o.r105, 1, -1, -1, -1, -1, nullptr};
    out[106] = Row{'f', &o.r106, 83, -1, -1, -1, -1, nullptr};
    out[107] = Row{'f', &o.r107, 84, -1, -1, -1, -1, nullptr};
    out[108] = Row{'f', &o.r108, 85, -1, -1, -1, -1, nullptr};
    out[109] = Row{'b', &o.r109, 18, -1, -1, -1, -1, nullptr};
    out[110] = Row{'f', &o.r110, 86, -1, -1, -1, -1, nullptr};
    out[111] = Row{'b', &o.r111, 19, -1, -1, -1, -1, nullptr};
    out[112] = Row{'f', &o.r112, 87, -1, -1, -1, -1, nullptr};
    out[113] = Row{'f', &o.r113, 88, -1, -1, -1, -1, nullptr};
    out[114] = Row{'b', &o.r114, 20, -1, -1, -1, -1, nullptr};
    out[115] = Row{'f', &o.r115, 89, -1, -1, -1, -1, nullptr};
    out[116] = Row{'f', &o.r116, 90, -1, -1, -1, -1, nullptr};
    out[117] = Row{'f', &o.r117, 91, -1, -1, -1, -1, nullptr};
    out[118] = Row{'b', &o.r118, 21, -1, -1, -1, -1, nullptr};
    out[119] = Row{'b', &o.r119, 22, -1, -1, -1, -1, nullptr};
    out[120] = Row{'f', &o.r120, 92, -1, -1, -1, -1, nullptr};
    out[121] = Row{'f', &o.r121, 93, -1, -1, -1, -1, nullptr};
    out[122] = Row{'b', &o.r122, 23, -1, -1, -1, -1, nullptr};
    out[123] = Row{'b', &o.r123, 24, -1, -1, -1, -1, nullptr};
    out[124] = Row{'b', &o.r124, 25, -1, -1, -1, -1, nullptr};
    out[125] = Row{'f', &o.r125, 94, -1, -1, -1, -1, nullptr};
    out[126] = Row{'f', &o.r126, 95, -1, -1, -1, -1, nullptr};
    out[127] = Row{'f', &o.r127, 96, -1, -1, -1, -1, nullptr};
    out[128] = Row{'f', &o.r128, 97, -1, -1, -1, -1, nullptr};
    out[129] = Row{'f', &o.r129, 98, -1, -1, -1, -1, nullptr};
    out[130] = Row{'u', &o.r130, 3, -1, -1, -1, -1, nullptr};
    out[131] = Row{'u', &o.r131, 4, -1, -1, -1, -1, nullptr};
    out[132] = Row{'b', &o.r132, 26, -1, -1, -1, -1, nullptr};
    out[133] = Row{'b', &o.r133, 27, -1, -1, -1, -1, nullptr};
    out[134] = Row{'f', &o.r134, 99, -1, -1, -1, -1, nullptr};
    out[135] = Row{'f', &o.r135, 100, -1, -1, -1, -1, nullptr};
    out[136] = Row{'f', &o.r136, 101, -1, -1, -1, -1, nullptr};
    out[137] = Row{'f', &o.r137, 102, -1, -1, -1, -1, nullptr};
    out[138] = Row{'b', &o.r138, 28, -1, -1, -1, -1, nullptr};
    out[139] = Row{'b', &o.r139, 29, -1, -1, -1, -1, nullptr};
    out[140] = Row{'f', &o.r140, 103, -1, -1, -1, -1, nullptr};
    out[141] = Row{'b', &o.r141, 30, -1, -1, -1, -1, nullptr};
    out[142] = Row{'f', &o.r142, 104, -1, -1, -1, -1, nullptr};
    out[143] = Row{'f', &o.r143, 105, -1, -1, -1, -1, nullptr};
    out[144] = Row{'f', &o.r144, 106, -1, -1, -1, -1, nullptr};
    out[145] = Row{'f', &o.r145, 107, -1, -1, -1, -1, nullptr};
    out[146] = Row{'f', &o.r146, 108, -1, -1, -1, -1, nullptr};
    out[147] = Row{'f', &o.r147, 109, -1, -1, -1, -1, nullptr};
    out[148] = Row{'f', &o.r148, 110, -1, -1, -1, -1, nullptr};
    out[149] = Row{'f', &o.r149, 111, -1, -1, -1, -1, nullptr};
    out[150] = Row{'f', &o.r150, 112, -1, -1, -1, -1, nullptr};
    out[151] = Row{'f', &o.r151, 113, -1, -1, -1, -1, nullptr};
    out[152] = Row{'f', &o.r152, 114, -1, -1, -1, -1, nullptr};
    out[153] = Row{'f', &o.r153, 115, -1, -1, -1, -1, nullptr};
    out[154] = Row{'f', &o.r154, 116, -1, -1, -1, -1, nullptr};
    out[155] = Row{'b', &o.r155, 31, -1, -1, -1, -1, nullptr};
    out[156] = Row{'b', &o.r156, 32, -1, -1, -1, -1, nullptr};
    out[157] = Row{'b', &o.r157, 33, -1, -1, -1, -1, nullptr};
    out[158] = Row{'b', &o.r158, 34, -1, -1, -1, -1, nullptr};
    out[159] = Row{'c', &o.r159, -1, -1, 0, -1, -1, nullptr};
    out[160] = Row{'c', &o.r160, -1, -1, 1, -1, -1, nullptr};
    out[161] = Row{'e', &o.r161, -1, 157, 2, 3, -1, nullptr};
    out[162] = Row{'e', &o.r162, -1, 134, 2, 4, -1, nullptr};
    out[163] = Row{'e', &o.r163, -1, 135, 2, 5, -1, nullptr};
    out[164] = Row{'e', &o.r164, -1, 136, 2, 6, -1, nullptr};
    out[165] = Row{'e', &o.r165, -1, 137, 2, 7, -1, nullptr};
    out[166] = Row{'t', &o.r166, -1, 139, 2, -1, -1, nullptr};
    out[167] = Row{'t', &o.r167, -1, 118, 2, -1, -1, nullptr};
    out[168] = Row{'t', &o.r168, -1, 119, 2, -1, -1, nullptr};
    out[169] = Row{'t', &o.r169, -1, 120, 2, -1, -1, nullptr};
    out[170] = Row{'t', &o.r170, -1, 121, 2, -1, -1, nullptr};
    out[171] = Row{'t', &o.r171, -1, 141, 2, -1, -1, nullptr};
    out[172] = Row{'t', &o.r172, -1, 122, 2, -1, -1, nullptr};
    out[173] = Row{'t', &o.r173, -1, 142, 2, -1, -1, nullptr};
    out[174] = Row{'t', &o.r174, -1, 143, 2, -1, -1, nullptr};
    out[175] = Row{'e', &o.r175, -1, 105, 2, 8, -1, nullptr};
    out[176] = Row{'t', &o.r176, -1, 152, 9, -1, -1, nullptr};
    out[177] = Row{'t', &o.r177, -1, 153, 9, -1, -1, nullptr};
    out[178] = Row{'s', &o.r178, -1, 154, 9, -1, 0, nullptr};
    out[179] = Row{'t', &o.r179, -1, 155, 9, -1, -1, nullptr};
    out[180] = Row{'t', &o.r180, -1, 123, 10, -1, -1, nullptr};
    out[181] = Row{'t', &o.r181, -1, 124, 10, -1, -1, nullptr};
    out[182] = Row{'s', &o.r182, -1, 125, 10, -1, 1, nullptr};
    out[183] = Row{'s', &o.r183, -1, 126, 10, -1, 2, nullptr};
    out[184] = Row{'s', &o.r184, -1, 127, 10, -1, 3, nullptr};
    out[185] = Row{'s', &o.r185, -1, 128, 10, -1, 4, nullptr};
    out[186] = Row{'s', &o.r186, -1, 129, 10, -1, 5, nullptr};
    out[187] = Row{'s', &o.r187, -1, 145, 11, -1, 6, nullptr};
    out[188] = Row{'s', &o.r188, -1, 144, 11, -1, 7, nullptr};
    out[189] = Row{'s', &o.r189, -1, 146, 11, -1, 8, nullptr};
    out[190] = Row{'s', &o.r190, -1, 147, 11, -1, 9, nullptr};
    out[191] = Row{'s', &o.r191, -1, 148, 11, -1, 10, nullptr};
    out[192] = Row{'s', &o.r192, -1, 149, 11, -1, 11, nullptr};
    out[193] = Row{'s', &o.r193, -1, 150, 11, -1, 12, nullptr};
    out[194] = Row{'s', &o.r194, -1, 151, 11, -1, 13, nullptr};
    out[195] = Row{'t', &o.r195, -1, 156, 12, -1, -1, nullptr};
    out[196] = Row{'t', &o.r196, -1, 158, 13, -1, -1, nullptr};
    out[197] = Row{'b', &o.r197, 35, -1, -1, -1, -1, nullptr};
    out[198] = Row{'c', &o.r198, -1, -1, 14, -1, -1, nullptr};
    out[199] = Row{'c', &o.r199, -1, -1, 15, -1, -1, nullptr};
    out[200] = Row{'m', &o.r200, 0, -1, -1, -1, -1, nullptr};
    out[201] = Row{'c', &o.r201, -1, -1, 17, -1, -1, nullptr};
    out[202] = Row{'m', &o.r202, 1, -1, -1, -1, -1, nullptr};
    out[203] = Row{'m', &o.r203, 2, -1, -1, -1, -1, nullptr};
    out[204] = Row{'z', &o.r204, 0, -1, 20, -1, -1, nullptr};
    out[205] = Row{'h', &o.r205, 0, -1, 21, -1, -1, nullptr};
    out[206] = Row{'f', &o.r206, 117, -1, -1, -1, -1, nullptr};
    out[207] = Row{'f', &o.r207, 118, -1, -1, -1, -1, nullptr};
    out[208] = Row{'f', &o.r208, 119, -1, -1, -1, -1, nullptr};
    out[209] = Row{'f', &o.r209, 120, -1, -1, -1, -1, nullptr};
    out[210] = Row{'f', &o.r210, 121, -1, -1, -1, -1, nullptr};
    out[211] = Row{'f', &o.r211, 122, -1, -1, -1, -1, nullptr};
    out[212] = Row{'f', &o.r212, 123, -1, -1, -1, -1, nullptr};
    out[213] = Row{'t', &o.r213, -1, 206, 2, -1, -1, nullptr};
    out[214] = Row{'t', &o.r214, -1, 207, 2, -1, -1, nullptr};
    out[215] = Row{'t', &o.r215, -1, 208, 2, -1, -1, nullptr};
    out[216] = Row{'t', &o.r216, -1, 209, 2, -1, -1, nullptr};
    out[217] = Row{'t', &o.r217, -1, 210, 2, -1, -1, nullptr};
    out[218] = Row{'t', &o.r218, -1, 211, 2, -1, -1, nullptr};
    out[219] = Row{'t', &o.r219, -1, 212, 2, -1, -1, nullptr};
    out[220] = Row{'c', &o.r220, -1, -1, 22, -1, -1, nullptr};
    out[221] = Row{'c', &o.r221, -1, -1, 23, -1, -1, nullptr};
    out[222] = Row{'c', &o.r222, -1, -1, 24, -1, -1, nullptr};
    out[223] = Row{'c', &o.r223, -1, -1, 25, -1, -1, nullptr};
    out[224] = Row{'c', &o.r224, -1, -1, 26, -1, -1, nullptr};
    out[225] = Row{'f', &o.r225, 124, -1, -1, -1, -1, nullptr};
    out[226] = Row{'f', &o.r226, 125, -1, -1, -1, -1, nullptr};
    out[227] = Row{'f', &o.r227, 126, -1, -1, -1, -1, nullptr};
    out[228] = Row{'f', &o.r228, 127, -1, -1, -1, -1, nullptr};
    out[229] = Row{'f', &o.r229, 128, -1, -1, -1, -1, nullptr};
    out[230] = Row{'f', &o.r230, 129, -1, -1, -1, -1, nullptr};
    out[231] = Row{'f', &o.r231, 130, -1, -1, -1, -1, nullptr};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

[[maybe_unused]] std::uint64_t lint_dtor([[maybe_unused]] std::size_t var) {
    std::uint64_t dtor = 0;
    dtor = find_local_int_var_class(kSlot370LocalIntSites[var].wrapper_rva)->dtor_rva;
    return dtor;
}

void section_constants() {
    CHECK(kSlot370Rva == 0x720b50 && kSlot370Size == 52949 && kSlot370RowCount == 232);
    CHECK(is_known_float_arg_functor(kSlot370FloatFnTag));
    for (const LocalIntSiteSpec& site : kSlot370LocalIntSites) CHECK(find_local_int_var_class(site.wrapper_rva) != nullptr && (find_local_int_var_class(site.wrapper_rva)->fn_vtbl == site.arg_fn_tag || is_local_int_caller_twin_tag(site.arg_fn_tag, find_local_int_var_class(site.wrapper_rva)->fn_vtbl)));   // per-site tag = the wrapper's own class or its proven alias
    CHECK(is_float_functor_tag(kSlot370UFloatFnTag));
    CHECK(sizeof(ConCmdCommand) == 0xa0);
    CHECK(sizeof(ConCmdStringVar) == 0x148);
    for (std::size_t n = 0; n < kSlot370StrVarCount; ++n) CHECK(std::strlen(kSlot370StrNames[n]) == kSlot370StrNameLengths[n]);
    CHECK(offsetof(Slot370Objects, r00) == 0x0);
    CHECK(offsetof(Slot370Objects, r01) == 0xd8);
    CHECK(offsetof(Slot370Objects, r02) == 0x1a0);
    CHECK(offsetof(Slot370Objects, r03) == 0x268);
    CHECK(offsetof(Slot370Objects, r04) == 0x340);
    CHECK(offsetof(Slot370Objects, r05) == 0x418);
    CHECK(offsetof(Slot370Objects, r06) == 0x4e0);
    CHECK(offsetof(Slot370Objects, r07) == 0x5a8);
    CHECK(offsetof(Slot370Objects, r08) == 0x680);
    CHECK(offsetof(Slot370Objects, r09) == 0x758);
    CHECK(offsetof(Slot370Objects, r10) == 0x830);
    CHECK(offsetof(Slot370Objects, r11) == 0x908);
    CHECK(offsetof(Slot370Objects, r12) == 0x9e0);
    CHECK(offsetof(Slot370Objects, r13) == 0xab8);
    CHECK(offsetof(Slot370Objects, r14) == 0xb90);
    CHECK(offsetof(Slot370Objects, r15) == 0xc58);
    CHECK(offsetof(Slot370Objects, r16) == 0xd30);
    CHECK(offsetof(Slot370Objects, r17) == 0xe08);
    CHECK(offsetof(Slot370Objects, r18) == 0xee0);
    CHECK(offsetof(Slot370Objects, r19) == 0xfb8);
    CHECK(offsetof(Slot370Objects, r20) == 0x1090);
    CHECK(offsetof(Slot370Objects, r21) == 0x1168);
    CHECK(offsetof(Slot370Objects, r22) == 0x1240);
    CHECK(offsetof(Slot370Objects, r23) == 0x1318);
    CHECK(offsetof(Slot370Objects, r24) == 0x13e0);
    CHECK(offsetof(Slot370Objects, r25) == 0x14b8);
    CHECK(offsetof(Slot370Objects, r26) == 0x1590);
    CHECK(offsetof(Slot370Objects, r27) == 0x1668);
    CHECK(offsetof(Slot370Objects, r28) == 0x1740);
    CHECK(offsetof(Slot370Objects, r29) == 0x1818);
    CHECK(offsetof(Slot370Objects, r30) == 0x18f0);
    CHECK(offsetof(Slot370Objects, r31) == 0x19c8);
    CHECK(offsetof(Slot370Objects, r32) == 0x1aa0);
    CHECK(offsetof(Slot370Objects, r33) == 0x1b68);
    CHECK(offsetof(Slot370Objects, r34) == 0x1c40);
    CHECK(offsetof(Slot370Objects, r35) == 0x1d18);
    CHECK(offsetof(Slot370Objects, r36) == 0x1df0);
    CHECK(offsetof(Slot370Objects, r37) == 0x1ec8);
    CHECK(offsetof(Slot370Objects, r38) == 0x1fa0);
    CHECK(offsetof(Slot370Objects, r39) == 0x2078);
    CHECK(offsetof(Slot370Objects, r40) == 0x2150);
    CHECK(offsetof(Slot370Objects, r41) == 0x2228);
    CHECK(offsetof(Slot370Objects, r42) == 0x22f0);
    CHECK(offsetof(Slot370Objects, r43) == 0x23b8);
    CHECK(offsetof(Slot370Objects, r44) == 0x2490);
    CHECK(offsetof(Slot370Objects, r45) == 0x2568);
    CHECK(offsetof(Slot370Objects, r46) == 0x2640);
    CHECK(offsetof(Slot370Objects, r47) == 0x2718);
    CHECK(offsetof(Slot370Objects, r48) == 0x27f0);
    CHECK(offsetof(Slot370Objects, r49) == 0x28c8);
    CHECK(offsetof(Slot370Objects, r50) == 0x29a0);
    CHECK(offsetof(Slot370Objects, r51) == 0x2a68);
    CHECK(offsetof(Slot370Objects, r52) == 0x2b40);
    CHECK(offsetof(Slot370Objects, r53) == 0x2c18);
    CHECK(offsetof(Slot370Objects, r54) == 0x2cf0);
    CHECK(offsetof(Slot370Objects, r55) == 0x2dc8);
    CHECK(offsetof(Slot370Objects, r56) == 0x2ea0);
    CHECK(offsetof(Slot370Objects, r57) == 0x2f78);
    CHECK(offsetof(Slot370Objects, r58) == 0x3050);
    CHECK(offsetof(Slot370Objects, r59) == 0x3118);
    CHECK(offsetof(Slot370Objects, r60) == 0x31f0);
    CHECK(offsetof(Slot370Objects, r61) == 0x32c8);
    CHECK(offsetof(Slot370Objects, r62) == 0x33a0);
    CHECK(offsetof(Slot370Objects, r63) == 0x3478);
    CHECK(offsetof(Slot370Objects, r64) == 0x3550);
    CHECK(offsetof(Slot370Objects, r65) == 0x3628);
    CHECK(offsetof(Slot370Objects, r66) == 0x3700);
    CHECK(offsetof(Slot370Objects, r67) == 0x37c8);
    CHECK(offsetof(Slot370Objects, r68) == 0x38a0);
    CHECK(offsetof(Slot370Objects, r69) == 0x3978);
    CHECK(offsetof(Slot370Objects, r70) == 0x3a50);
    CHECK(offsetof(Slot370Objects, r71) == 0x3b28);
    CHECK(offsetof(Slot370Objects, r72) == 0x3c00);
    CHECK(offsetof(Slot370Objects, r73) == 0x3cd8);
    CHECK(offsetof(Slot370Objects, r74) == 0x3db0);
    CHECK(offsetof(Slot370Objects, r75) == 0x3e78);
    CHECK(offsetof(Slot370Objects, r76) == 0x3f50);
    CHECK(offsetof(Slot370Objects, r77) == 0x4028);
    CHECK(offsetof(Slot370Objects, r78) == 0x4100);
    CHECK(offsetof(Slot370Objects, r79) == 0x41d8);
    CHECK(offsetof(Slot370Objects, r80) == 0x42b0);
    CHECK(offsetof(Slot370Objects, r81) == 0x4388);
    CHECK(offsetof(Slot370Objects, r82) == 0x4450);
    CHECK(offsetof(Slot370Objects, r83) == 0x4528);
    CHECK(offsetof(Slot370Objects, r84) == 0x4600);
    CHECK(offsetof(Slot370Objects, r85) == 0x46d8);
    CHECK(offsetof(Slot370Objects, r86) == 0x47a0);
    CHECK(offsetof(Slot370Objects, r87) == 0x4868);
    CHECK(offsetof(Slot370Objects, r88) == 0x4930);
    CHECK(offsetof(Slot370Objects, r89) == 0x4a08);
    CHECK(offsetof(Slot370Objects, r90) == 0x4ae0);
    CHECK(offsetof(Slot370Objects, r91) == 0x4bb8);
    CHECK(offsetof(Slot370Objects, r92) == 0x4c90);
    CHECK(offsetof(Slot370Objects, r93) == 0x4d68);
    CHECK(offsetof(Slot370Objects, r94) == 0x4e40);
    CHECK(offsetof(Slot370Objects, r95) == 0x4f18);
    CHECK(offsetof(Slot370Objects, r96) == 0x4ff0);
    CHECK(offsetof(Slot370Objects, r97) == 0x50c8);
    CHECK(offsetof(Slot370Objects, r98) == 0x5190);
    CHECK(offsetof(Slot370Objects, r99) == 0x5268);
    CHECK(offsetof(Slot370Objects, r100) == 0x5340);
    CHECK(offsetof(Slot370Objects, r101) == 0x5418);
    CHECK(offsetof(Slot370Objects, r102) == 0x54f0);
    CHECK(offsetof(Slot370Objects, r103) == 0x55c8);
    CHECK(offsetof(Slot370Objects, r104) == 0x56a0);
    CHECK(offsetof(Slot370Objects, r105) == 0x5778);
    CHECK(offsetof(Slot370Objects, r106) == 0x5850);
    CHECK(offsetof(Slot370Objects, r107) == 0x5928);
    CHECK(offsetof(Slot370Objects, r108) == 0x5a00);
    CHECK(offsetof(Slot370Objects, r109) == 0x5ad8);
    CHECK(offsetof(Slot370Objects, r110) == 0x5ba0);
    CHECK(offsetof(Slot370Objects, r111) == 0x5c78);
    CHECK(offsetof(Slot370Objects, r112) == 0x5d40);
    CHECK(offsetof(Slot370Objects, r113) == 0x5e18);
    CHECK(offsetof(Slot370Objects, r114) == 0x5ef0);
    CHECK(offsetof(Slot370Objects, r115) == 0x5fb8);
    CHECK(offsetof(Slot370Objects, r116) == 0x6090);
    CHECK(offsetof(Slot370Objects, r117) == 0x6168);
    CHECK(offsetof(Slot370Objects, r118) == 0x6240);
    CHECK(offsetof(Slot370Objects, r119) == 0x6308);
    CHECK(offsetof(Slot370Objects, r120) == 0x63d0);
    CHECK(offsetof(Slot370Objects, r121) == 0x64a8);
    CHECK(offsetof(Slot370Objects, r122) == 0x6580);
    CHECK(offsetof(Slot370Objects, r123) == 0x6648);
    CHECK(offsetof(Slot370Objects, r124) == 0x6710);
    CHECK(offsetof(Slot370Objects, r125) == 0x67d8);
    CHECK(offsetof(Slot370Objects, r126) == 0x68b0);
    CHECK(offsetof(Slot370Objects, r127) == 0x6988);
    CHECK(offsetof(Slot370Objects, r128) == 0x6a60);
    CHECK(offsetof(Slot370Objects, r129) == 0x6b38);
    CHECK(offsetof(Slot370Objects, r130) == 0x6c10);
    CHECK(offsetof(Slot370Objects, r131) == 0x6ce8);
    CHECK(offsetof(Slot370Objects, r132) == 0x6dc0);
    CHECK(offsetof(Slot370Objects, r133) == 0x6e88);
    CHECK(offsetof(Slot370Objects, r134) == 0x6f50);
    CHECK(offsetof(Slot370Objects, r135) == 0x7028);
    CHECK(offsetof(Slot370Objects, r136) == 0x7100);
    CHECK(offsetof(Slot370Objects, r137) == 0x71d8);
    CHECK(offsetof(Slot370Objects, r138) == 0x72b0);
    CHECK(offsetof(Slot370Objects, r139) == 0x7378);
    CHECK(offsetof(Slot370Objects, r140) == 0x7440);
    CHECK(offsetof(Slot370Objects, r141) == 0x7518);
    CHECK(offsetof(Slot370Objects, r142) == 0x75e0);
    CHECK(offsetof(Slot370Objects, r143) == 0x76b8);
    CHECK(offsetof(Slot370Objects, r144) == 0x7790);
    CHECK(offsetof(Slot370Objects, r145) == 0x7868);
    CHECK(offsetof(Slot370Objects, r146) == 0x7940);
    CHECK(offsetof(Slot370Objects, r147) == 0x7a18);
    CHECK(offsetof(Slot370Objects, r148) == 0x7af0);
    CHECK(offsetof(Slot370Objects, r149) == 0x7bc8);
    CHECK(offsetof(Slot370Objects, r150) == 0x7ca0);
    CHECK(offsetof(Slot370Objects, r151) == 0x7d78);
    CHECK(offsetof(Slot370Objects, r152) == 0x7e50);
    CHECK(offsetof(Slot370Objects, r153) == 0x7f28);
    CHECK(offsetof(Slot370Objects, r154) == 0x8000);
    CHECK(offsetof(Slot370Objects, r155) == 0x80d8);
    CHECK(offsetof(Slot370Objects, r156) == 0x81a0);
    CHECK(offsetof(Slot370Objects, r157) == 0x8268);
    CHECK(offsetof(Slot370Objects, r158) == 0x8330);
    CHECK(offsetof(Slot370Objects, r159) == 0x83f8);
    CHECK(offsetof(Slot370Objects, r160) == 0x8408);
    CHECK(offsetof(Slot370Objects, r161) == 0x8418);
    CHECK(offsetof(Slot370Objects, r162) == 0x84c0);
    CHECK(offsetof(Slot370Objects, r163) == 0x8568);
    CHECK(offsetof(Slot370Objects, r164) == 0x8610);
    CHECK(offsetof(Slot370Objects, r165) == 0x86b8);
    CHECK(offsetof(Slot370Objects, r166) == 0x8760);
    CHECK(offsetof(Slot370Objects, r167) == 0x8808);
    CHECK(offsetof(Slot370Objects, r168) == 0x88b0);
    CHECK(offsetof(Slot370Objects, r169) == 0x8958);
    CHECK(offsetof(Slot370Objects, r170) == 0x8a00);
    CHECK(offsetof(Slot370Objects, r171) == 0x8aa8);
    CHECK(offsetof(Slot370Objects, r172) == 0x8b50);
    CHECK(offsetof(Slot370Objects, r173) == 0x8bf8);
    CHECK(offsetof(Slot370Objects, r174) == 0x8ca0);
    CHECK(offsetof(Slot370Objects, r175) == 0x8d48);
    CHECK(offsetof(Slot370Objects, r176) == 0x8df0);
    CHECK(offsetof(Slot370Objects, r177) == 0x8e98);
    CHECK(offsetof(Slot370Objects, r178) == 0x8f40);
    CHECK(offsetof(Slot370Objects, r179) == 0x8fe8);
    CHECK(offsetof(Slot370Objects, r180) == 0x9090);
    CHECK(offsetof(Slot370Objects, r181) == 0x9138);
    CHECK(offsetof(Slot370Objects, r182) == 0x91e0);
    CHECK(offsetof(Slot370Objects, r183) == 0x9288);
    CHECK(offsetof(Slot370Objects, r184) == 0x9330);
    CHECK(offsetof(Slot370Objects, r185) == 0x93d8);
    CHECK(offsetof(Slot370Objects, r186) == 0x9480);
    CHECK(offsetof(Slot370Objects, r187) == 0x9528);
    CHECK(offsetof(Slot370Objects, r188) == 0x95d0);
    CHECK(offsetof(Slot370Objects, r189) == 0x9678);
    CHECK(offsetof(Slot370Objects, r190) == 0x9720);
    CHECK(offsetof(Slot370Objects, r191) == 0x97c8);
    CHECK(offsetof(Slot370Objects, r192) == 0x9870);
    CHECK(offsetof(Slot370Objects, r193) == 0x9918);
    CHECK(offsetof(Slot370Objects, r194) == 0x99c0);
    CHECK(offsetof(Slot370Objects, r195) == 0x9a68);
    CHECK(offsetof(Slot370Objects, r196) == 0x9b10);
    CHECK(offsetof(Slot370Objects, r197) == 0x9bd8);
    CHECK(offsetof(Slot370Objects, r198) == 0x9ca0);
    CHECK(offsetof(Slot370Objects, r199) == 0x9cb0);
    CHECK(offsetof(Slot370Objects, r200) == 0x9cc0);
    CHECK(offsetof(Slot370Objects, r201) == 0x9d60);
    CHECK(offsetof(Slot370Objects, r202) == 0x9d90);
    CHECK(offsetof(Slot370Objects, r203) == 0x9e50);
    CHECK(offsetof(Slot370Objects, r204) == 0x9ef0);
    CHECK(offsetof(Slot370Objects, r205) == 0xa038);
    CHECK(offsetof(Slot370Objects, r206) == 0xa068);
    CHECK(offsetof(Slot370Objects, r207) == 0xa140);
    CHECK(offsetof(Slot370Objects, r208) == 0xa218);
    CHECK(offsetof(Slot370Objects, r209) == 0xa2f0);
    CHECK(offsetof(Slot370Objects, r210) == 0xa3c8);
    CHECK(offsetof(Slot370Objects, r211) == 0xa4a0);
    CHECK(offsetof(Slot370Objects, r212) == 0xa578);
    CHECK(offsetof(Slot370Objects, r213) == 0xa650);
    CHECK(offsetof(Slot370Objects, r214) == 0xa6f8);
    CHECK(offsetof(Slot370Objects, r215) == 0xa7a0);
    CHECK(offsetof(Slot370Objects, r216) == 0xa848);
    CHECK(offsetof(Slot370Objects, r217) == 0xa8f0);
    CHECK(offsetof(Slot370Objects, r218) == 0xa998);
    CHECK(offsetof(Slot370Objects, r219) == 0xaa40);
    CHECK(offsetof(Slot370Objects, r220) == 0xaae8);
    CHECK(offsetof(Slot370Objects, r221) == 0xaaf8);
    CHECK(offsetof(Slot370Objects, r222) == 0xab08);
    CHECK(offsetof(Slot370Objects, r223) == 0xab18);
    CHECK(offsetof(Slot370Objects, r224) == 0xab28);
    CHECK(offsetof(Slot370Objects, r225) == 0xab38);
    CHECK(offsetof(Slot370Objects, r226) == 0xac10);
    CHECK(offsetof(Slot370Objects, r227) == 0xace8);
    CHECK(offsetof(Slot370Objects, r228) == 0xadc0);
    CHECK(offsetof(Slot370Objects, r229) == 0xae98);
    CHECK(offsetof(Slot370Objects, r230) == 0xaf70);
    CHECK(offsetof(Slot370Objects, r231) == 0xb048);
    CHECK(sizeof(Slot370Objects) == 0xb120);
    for (std::size_t n = 0; n < kSlot370BoolCount; ++n) CHECK(std::strlen(kSlot370BoolNames[n]) == kSlot370BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot370FloatCount; ++n) CHECK(std::strlen(kSlot370FloatNames[n]) == kSlot370FloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot370UFloatCount; ++n) CHECK(std::strlen(kSlot370UFloatNames[n]) == kSlot370UFloatNameLengths[n]);
    for (const LocalIntSiteSpec& site : kSlot370LocalIntSites) CHECK(std::strlen(site.name) == site.name_length);
    for (std::size_t n = 0; n < kSlot370CmdCount; ++n) CHECK(std::strlen(kSlot370CmdNames[n]) == kSlot370CmdNameLengths[n] && kSlot370CmdHandler[n] != 0);
    for (std::size_t n = 0; n < kSlot370StringCount; ++n) CHECK(std::strlen(kSlot370MenuStrings[n]) == kSlot370MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            check_bool_with_description(v, kSlot370BoolFnTag, kSlot370BoolValues[row.var], "", 0);
            CHECK(same_view(v.base.name, kSlot370BoolNames[row.var], kSlot370BoolNameLengths[row.var]));
            break;
        }
        case 'l': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            const LocalIntSiteSpec& site = kSlot370LocalIntSites[row.var];
            const LocalIntVarClass* cls = find_local_int_var_class(site.wrapper_rva);
            CHECK(cls != nullptr && (site.arg_fn_tag == cls->fn_vtbl || is_local_int_caller_twin_tag(site.arg_fn_tag, cls->fn_vtbl)));
            CHECK(v.base.vptr == cls->final_vptr && v.base.flags == site.flags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, site.name, site.name_length) && empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(site.value) && dword(v, 0x68) == static_cast<std::uint32_t>(site.value));
            CHECK(dword(v, 0x6c) == static_cast<std::uint32_t>(site.min_value) && v.min_engaged == 1 && dword(v, 0x74) == static_cast<std::uint32_t>(site.max_value) && v.max_engaged == 1);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == cls->fn_vtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'f': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            std::uint32_t lo = 0;
            std::uint32_t hi = 0;
            std::memcpy(&lo, v.min_value, 4);
            std::memcpy(&hi, v.max_value, 4);
            CHECK(v.base.flags == kSlot370VarFlags && v.base.registered == 1 && same_view(v.base.name, kSlot370FloatNames[row.var], kSlot370FloatNameLengths[row.var]));
            CHECK(v.value == kSlot370FloatValueBits[row.var] && v.default_value == kSlot370FloatValueBits[row.var] && v.min_engaged == 1 && v.max_engaged == 1);
            CHECK(lo == kSlot370FloatLoBits[row.var] && hi == kSlot370FloatHiBits[row.var]);
            break;
        }
        case 'u': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kRangedFloatFinalVptr && v.value_ref == &v.value && v.base.registered == 1 && v.base.flags == kSlot370VarFlags);
            CHECK(v.value == kSlot370UFloatValueBits[row.var] && v.default_value == kSlot370UFloatValueBits[row.var] && v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(same_view(v.base.name, kSlot370UFloatNames[row.var], kSlot370UFloatNameLengths[row.var]));
            if (kSlot370UFloatDesc[row.var] < 0) CHECK(empty_view(v.base.description));
            else CHECK(same_view(v.base.description, kSlot370MenuStrings[kSlot370UFloatDesc[row.var]], kSlot370MenuStringLengths[kSlot370UFloatDesc[row.var]]));
            CHECK(v.fn_vptr == kRangedFloatFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'm': {
            const auto& c = *static_cast<const ConCmdCommand*>(row.obj);
            check_command(c, kSlot370CmdNames[row.var], kSlot370MenuStrings[kSlot370CmdDesc[row.var]], kSlot370MenuStringLengths[kSlot370CmdDesc[row.var]], kSlot370CmdHandler[row.var]);
            break;
        }
        case 'z': {
            const auto& v = *static_cast<const ConCmdStringVar*>(row.obj);
            const std::int32_t d = kSlot370StrInit[row.var];
            CHECK(v.base.vptr == kStringVarFinalVptr && v.base.flags == kStringVarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot370StrNames[row.var], kSlot370StrNameLengths[row.var]) && empty_view(v.base.description));
            CHECK(same_view(v.default_value, kSlot370MenuStrings[d], kSlot370MenuStringLengths[d]) && same_view(v.value, kSlot370MenuStrings[d], kSlot370MenuStringLengths[d]));
            CHECK(v.byte88 == 0 && v.bytea8 == 0 && v.byteb0 == 0 && v.byted0 == 0);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == kStringVarFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot370MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot370MenuStrings[row.s0]));
            CHECK(c.unwritten == 0xa5a5a5a5u);   // +0x0c is never written
            break;
        }
        case 'q': {
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            CHECK(h.vptr == kSlot370HolderVptr && h.tail == 0 && same_view(h.name, kSlot370MenuStrings[row.s0], kSlot370MenuStringLengths[row.s0]));
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot370ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot370MenuStrings[row.s0], kSlot370MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot370MenuStrings[row.s1], kSlot370MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = tg.kind == 'b' ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot370MenuStrings[row.s1], kSlot370MenuStringLengths[row.s1]));
            } else {
                CHECK(it.text.data == tb.name.data && it.text.size == tb.name.size && it.text.ctl == nullptr);   // the variable's own name view
            }
            if (row.kind == 's' || row.kind == 'S') {
                CHECK(it.range_min == bits_to_float(kSlot370SliderLoBits[row.slider]) && it.range_max == bits_to_float(kSlot370SliderHiBits[row.slider]));
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
    DebugMenuRegistry menu;
    Slot370Objects o;
    poison(o);
    const Slot370Result r = init_slot370_720b50(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot370RowCount);
    Row rows[kSlot370RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot370RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'i' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w') ++items;
    }
    CHECK(vars == kSlot370BoolCount + kSlot370IntCount + kSlot370LocalIntCount + kSlot370UIntCount + kSlot370CmdCount + kSlot370StrVarCount + kSlot370FloatCount + kSlot370UFloatCount);
    CHECK(menu.root.count == items && items == 43 && menu.root.link.next == &o.r161.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot370CellCount && creg.atexit_log.size() == kSlot370RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot370RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? kSlot370HolderDtorRva : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : 0x272d0;

        if (dtor == 0x272d0 && !root_seen) {
            check_atexit(creg, at, 0x27280, &menu.root);
            root_seen = true;
            check_atexit(creg, at + 1, dtor, rows[i].obj);
        } else {
            check_atexit(creg, at, dtor, rows[i].obj);
        }
    }
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot370Objects o;
        const Slot370Result r = init_slot370_720b50(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot370Objects o;
        const Slot370Result r = init_slot370_720b50(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 161);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_370");
    return 0;
}
