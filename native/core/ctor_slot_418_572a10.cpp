#include "ctor_slot_418_572a10.hpp"

#include <cstring>

namespace sotc {

namespace {

// The 0x27120 slider: ctor(item, label, variable, 0x30, lo, hi) (text = the variable's own name view), 0x268b0, 0xf590(label), __cxa_atexit(0x272d0, item).
[[maybe_unused]] bool slider_item(DebugMenuItem& item, const char* label, std::size_t label_length, const ConCmdRangedVar& var, std::uint32_t lo_bits,
                 std::uint32_t hi_bits, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {
    float lo = 0.0f;
    float hi = 0.0f;
    std::memcpy(&lo, &lo_bits, sizeof lo);
    std::memcpy(&hi, &hi_bits, sizeof hi);
    const SharedStr view = family_view(label, label_length);
    if (!init_debug_menu_item_27120(item, view, var.base, kSlot418ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}









// `[cell] = name; cell->hash = 0x52a990(name)`; the four bytes at +0x0c are never written.
bool name_cell(NameHashCell& cell, const char* name) noexcept {
    cell.name = name;
    cell.hash = fnv_cstr_52a990(name);
    return true;
}

[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept { return debug_menu_target_for(var); }
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept { return lane_m6::ranged_float_menu_target(var); }

}  // namespace

Slot418Result init_slot418_572a10(Slot418Objects& objects, ConCmdRegistry&) noexcept {
    Slot418Result out{};

    if (!name_cell(objects.r00, kSlot418MenuStrings[0])) return out;
    ++out.rows_done;   // row 00 cell
    if (!name_cell(objects.r01, kSlot418MenuStrings[1])) return out;
    ++out.rows_done;   // row 01 cell
    if (!name_cell(objects.r02, kSlot418MenuStrings[2])) return out;
    ++out.rows_done;   // row 02 cell
    if (!name_cell(objects.r03, kSlot418MenuStrings[3])) return out;
    ++out.rows_done;   // row 03 cell
    if (!name_cell(objects.r04, kSlot418MenuStrings[4])) return out;
    ++out.rows_done;   // row 04 cell
    if (!name_cell(objects.r05, kSlot418MenuStrings[5])) return out;
    ++out.rows_done;   // row 05 cell
    if (!name_cell(objects.r06, kSlot418MenuStrings[6])) return out;
    ++out.rows_done;   // row 06 cell
    if (!name_cell(objects.r07, kSlot418MenuStrings[7])) return out;
    ++out.rows_done;   // row 07 cell
    if (!name_cell(objects.r08, kSlot418MenuStrings[8])) return out;
    ++out.rows_done;   // row 08 cell
    if (!name_cell(objects.r09, kSlot418MenuStrings[9])) return out;
    ++out.rows_done;   // row 09 cell
    if (!name_cell(objects.r10, kSlot418MenuStrings[10])) return out;
    ++out.rows_done;   // row 10 cell
    if (!name_cell(objects.r11, kSlot418MenuStrings[11])) return out;
    ++out.rows_done;   // row 11 cell
    if (!name_cell(objects.r12, kSlot418MenuStrings[12])) return out;
    ++out.rows_done;   // row 12 cell
    if (!name_cell(objects.r13, kSlot418MenuStrings[13])) return out;
    ++out.rows_done;   // row 13 cell
    if (!name_cell(objects.r14, kSlot418MenuStrings[14])) return out;
    ++out.rows_done;   // row 14 cell
    if (!name_cell(objects.r15, kSlot418MenuStrings[15])) return out;
    ++out.rows_done;   // row 15 cell
    if (!name_cell(objects.r16, kSlot418MenuStrings[16])) return out;
    ++out.rows_done;   // row 16 cell
    if (!name_cell(objects.r17, kSlot418MenuStrings[17])) return out;
    ++out.rows_done;   // row 17 cell
    if (!name_cell(objects.r18, kSlot418MenuStrings[18])) return out;
    ++out.rows_done;   // row 18 cell
    if (!name_cell(objects.r19, kSlot418MenuStrings[19])) return out;
    ++out.rows_done;   // row 19 cell
    if (!name_cell(objects.r20, kSlot418MenuStrings[20])) return out;
    ++out.rows_done;   // row 20 cell
    if (!name_cell(objects.r21, kSlot418MenuStrings[21])) return out;
    ++out.rows_done;   // row 21 cell
    if (!name_cell(objects.r22, kSlot418MenuStrings[22])) return out;
    ++out.rows_done;   // row 22 cell
    if (!name_cell(objects.r23, kSlot418MenuStrings[23])) return out;
    ++out.rows_done;   // row 23 cell
    if (!name_cell(objects.r24, kSlot418MenuStrings[24])) return out;
    ++out.rows_done;   // row 24 cell
    if (!name_cell(objects.r25, kSlot418MenuStrings[25])) return out;
    ++out.rows_done;   // row 25 cell
    if (!name_cell(objects.r26, kSlot418MenuStrings[26])) return out;
    ++out.rows_done;   // row 26 cell
    if (!name_cell(objects.r27, kSlot418MenuStrings[27])) return out;
    ++out.rows_done;   // row 27 cell
    if (!name_cell(objects.r28, kSlot418MenuStrings[28])) return out;
    ++out.rows_done;   // row 28 cell
    if (!name_cell(objects.r29, kSlot418MenuStrings[29])) return out;
    ++out.rows_done;   // row 29 cell
    if (!name_cell(objects.r30, kSlot418MenuStrings[30])) return out;
    ++out.rows_done;   // row 30 cell
    if (!name_cell(objects.r31, kSlot418MenuStrings[31])) return out;
    ++out.rows_done;   // row 31 cell
    if (!name_cell(objects.r32, kSlot418MenuStrings[32])) return out;
    ++out.rows_done;   // row 32 cell
    if (!name_cell(objects.r33, kSlot418MenuStrings[33])) return out;
    ++out.rows_done;   // row 33 cell
    if (!name_cell(objects.r34, kSlot418MenuStrings[34])) return out;
    ++out.rows_done;   // row 34 cell
    if (!name_cell(objects.r35, kSlot418MenuStrings[35])) return out;
    ++out.rows_done;   // row 35 cell
    if (!name_cell(objects.r36, kSlot418MenuStrings[36])) return out;
    ++out.rows_done;   // row 36 cell
    if (!name_cell(objects.r37, kSlot418MenuStrings[37])) return out;
    ++out.rows_done;   // row 37 cell
    if (!name_cell(objects.r38, kSlot418MenuStrings[38])) return out;
    ++out.rows_done;   // row 38 cell
    if (!name_cell(objects.r39, kSlot418MenuStrings[39])) return out;
    ++out.rows_done;   // row 39 cell
    if (!name_cell(objects.r40, kSlot418MenuStrings[40])) return out;
    ++out.rows_done;   // row 40 cell
    if (!name_cell(objects.r41, kSlot418MenuStrings[41])) return out;
    ++out.rows_done;   // row 41 cell
    if (!name_cell(objects.r42, kSlot418MenuStrings[42])) return out;
    ++out.rows_done;   // row 42 cell
    if (!name_cell(objects.r43, kSlot418MenuStrings[43])) return out;
    ++out.rows_done;   // row 43 cell
    if (!name_cell(objects.r44, kSlot418MenuStrings[44])) return out;
    ++out.rows_done;   // row 44 cell
    if (!name_cell(objects.r45, kSlot418MenuStrings[45])) return out;
    ++out.rows_done;   // row 45 cell
    if (!name_cell(objects.r46, kSlot418MenuStrings[46])) return out;
    ++out.rows_done;   // row 46 cell
    if (!name_cell(objects.r47, kSlot418MenuStrings[47])) return out;
    ++out.rows_done;   // row 47 cell
    if (!name_cell(objects.r48, kSlot418MenuStrings[48])) return out;
    ++out.rows_done;   // row 48 cell
    if (!name_cell(objects.r49, kSlot418MenuStrings[49])) return out;
    ++out.rows_done;   // row 49 cell
    if (!name_cell(objects.r50, kSlot418MenuStrings[50])) return out;
    ++out.rows_done;   // row 50 cell
    if (!name_cell(objects.r51, kSlot418MenuStrings[51])) return out;
    ++out.rows_done;   // row 51 cell
    if (!name_cell(objects.r52, kSlot418MenuStrings[52])) return out;
    ++out.rows_done;   // row 52 cell
    if (!name_cell(objects.r53, kSlot418MenuStrings[53])) return out;
    ++out.rows_done;   // row 53 cell
    if (!name_cell(objects.r54, kSlot418MenuStrings[54])) return out;
    ++out.rows_done;   // row 54 cell
    if (!name_cell(objects.r55, kSlot418MenuStrings[55])) return out;
    ++out.rows_done;   // row 55 cell
    if (!name_cell(objects.r56, kSlot418MenuStrings[56])) return out;
    ++out.rows_done;   // row 56 cell
    if (!name_cell(objects.r57, kSlot418MenuStrings[57])) return out;
    ++out.rows_done;   // row 57 cell
    if (!name_cell(objects.r58, kSlot418MenuStrings[58])) return out;
    ++out.rows_done;   // row 58 cell
    if (!name_cell(objects.r59, kSlot418MenuStrings[59])) return out;
    ++out.rows_done;   // row 59 cell
    if (!name_cell(objects.r60, kSlot418MenuStrings[60])) return out;
    ++out.rows_done;   // row 60 cell
    if (!name_cell(objects.r61, kSlot418MenuStrings[61])) return out;
    ++out.rows_done;   // row 61 cell
    if (!name_cell(objects.r62, kSlot418MenuStrings[62])) return out;
    ++out.rows_done;   // row 62 cell
    if (!name_cell(objects.r63, kSlot418MenuStrings[63])) return out;
    ++out.rows_done;   // row 63 cell
    if (!name_cell(objects.r64, kSlot418MenuStrings[64])) return out;
    ++out.rows_done;   // row 64 cell
    if (!name_cell(objects.r65, kSlot418MenuStrings[65])) return out;
    ++out.rows_done;   // row 65 cell
    if (!name_cell(objects.r66, kSlot418MenuStrings[66])) return out;
    ++out.rows_done;   // row 66 cell
    if (!name_cell(objects.r67, kSlot418MenuStrings[67])) return out;
    ++out.rows_done;   // row 67 cell
    if (!name_cell(objects.r68, kSlot418MenuStrings[68])) return out;
    ++out.rows_done;   // row 68 cell
    if (!name_cell(objects.r69, kSlot418MenuStrings[69])) return out;
    ++out.rows_done;   // row 69 cell
    if (!name_cell(objects.r70, kSlot418MenuStrings[70])) return out;
    ++out.rows_done;   // row 70 cell
    if (!name_cell(objects.r71, kSlot418MenuStrings[71])) return out;
    ++out.rows_done;   // row 71 cell
    if (!name_cell(objects.r72, kSlot418MenuStrings[72])) return out;
    ++out.rows_done;   // row 72 cell
    if (!name_cell(objects.r73, kSlot418MenuStrings[73])) return out;
    ++out.rows_done;   // row 73 cell
    if (!name_cell(objects.r74, kSlot418MenuStrings[74])) return out;
    ++out.rows_done;   // row 74 cell
    if (!name_cell(objects.r75, kSlot418MenuStrings[75])) return out;
    ++out.rows_done;   // row 75 cell
    if (!name_cell(objects.r76, kSlot418MenuStrings[76])) return out;
    ++out.rows_done;   // row 76 cell
    if (!name_cell(objects.r77, kSlot418MenuStrings[77])) return out;
    ++out.rows_done;   // row 77 cell
    if (!name_cell(objects.r78, kSlot418MenuStrings[78])) return out;
    ++out.rows_done;   // row 78 cell
    if (!name_cell(objects.r79, kSlot418MenuStrings[79])) return out;
    ++out.rows_done;   // row 79 cell
    if (!name_cell(objects.r80, kSlot418MenuStrings[80])) return out;
    ++out.rows_done;   // row 80 cell
    if (!name_cell(objects.r81, kSlot418MenuStrings[81])) return out;
    ++out.rows_done;   // row 81 cell
    if (!name_cell(objects.r82, kSlot418MenuStrings[82])) return out;
    ++out.rows_done;   // row 82 cell
    if (!name_cell(objects.r83, kSlot418MenuStrings[83])) return out;
    ++out.rows_done;   // row 83 cell
    if (!name_cell(objects.r84, kSlot418MenuStrings[84])) return out;
    ++out.rows_done;   // row 84 cell
    if (!name_cell(objects.r85, kSlot418MenuStrings[85])) return out;
    ++out.rows_done;   // row 85 cell
    if (!name_cell(objects.r86, kSlot418MenuStrings[86])) return out;
    ++out.rows_done;   // row 86 cell
    if (!name_cell(objects.r87, kSlot418MenuStrings[87])) return out;
    ++out.rows_done;   // row 87 cell
    if (!name_cell(objects.r88, kSlot418MenuStrings[88])) return out;
    ++out.rows_done;   // row 88 cell
    if (!name_cell(objects.r89, kSlot418MenuStrings[89])) return out;
    ++out.rows_done;   // row 89 cell
    if (!name_cell(objects.r90, kSlot418MenuStrings[90])) return out;
    ++out.rows_done;   // row 90 cell
    if (!name_cell(objects.r91, kSlot418MenuStrings[91])) return out;
    ++out.rows_done;   // row 91 cell
    if (!name_cell(objects.r92, kSlot418MenuStrings[92])) return out;
    ++out.rows_done;   // row 92 cell
    if (!name_cell(objects.r93, kSlot418MenuStrings[93])) return out;
    ++out.rows_done;   // row 93 cell
    if (!name_cell(objects.r94, kSlot418MenuStrings[94])) return out;
    ++out.rows_done;   // row 94 cell
    if (!name_cell(objects.r95, kSlot418MenuStrings[95])) return out;
    ++out.rows_done;   // row 95 cell
    if (!name_cell(objects.r96, kSlot418MenuStrings[96])) return out;
    ++out.rows_done;   // row 96 cell
    if (!name_cell(objects.r97, kSlot418MenuStrings[97])) return out;
    ++out.rows_done;   // row 97 cell
    if (!name_cell(objects.r98, kSlot418MenuStrings[98])) return out;
    ++out.rows_done;   // row 98 cell
    if (!name_cell(objects.r99, kSlot418MenuStrings[99])) return out;
    ++out.rows_done;   // row 99 cell
    if (!name_cell(objects.r100, kSlot418MenuStrings[100])) return out;
    ++out.rows_done;   // row 100 cell
    if (!name_cell(objects.r101, kSlot418MenuStrings[101])) return out;
    ++out.rows_done;   // row 101 cell
    if (!name_cell(objects.r102, kSlot418MenuStrings[102])) return out;
    ++out.rows_done;   // row 102 cell
    if (!name_cell(objects.r103, kSlot418MenuStrings[103])) return out;
    ++out.rows_done;   // row 103 cell
    if (!name_cell(objects.r104, kSlot418MenuStrings[104])) return out;
    ++out.rows_done;   // row 104 cell
    if (!name_cell(objects.r105, kSlot418MenuStrings[105])) return out;
    ++out.rows_done;   // row 105 cell
    if (!name_cell(objects.r106, kSlot418MenuStrings[106])) return out;
    ++out.rows_done;   // row 106 cell
    if (!name_cell(objects.r107, kSlot418MenuStrings[107])) return out;
    ++out.rows_done;   // row 107 cell
    if (!name_cell(objects.r108, kSlot418MenuStrings[108])) return out;
    ++out.rows_done;   // row 108 cell
    if (!name_cell(objects.r109, kSlot418MenuStrings[109])) return out;
    ++out.rows_done;   // row 109 cell
    if (!name_cell(objects.r110, kSlot418MenuStrings[110])) return out;
    ++out.rows_done;   // row 110 cell
    if (!name_cell(objects.r111, kSlot418MenuStrings[111])) return out;
    ++out.rows_done;   // row 111 cell
    if (!name_cell(objects.r112, kSlot418MenuStrings[112])) return out;
    ++out.rows_done;   // row 112 cell
    if (!name_cell(objects.r113, kSlot418MenuStrings[113])) return out;
    ++out.rows_done;   // row 113 cell
    if (!name_cell(objects.r114, kSlot418MenuStrings[114])) return out;
    ++out.rows_done;   // row 114 cell
    if (!name_cell(objects.r115, kSlot418MenuStrings[115])) return out;
    ++out.rows_done;   // row 115 cell
    if (!name_cell(objects.r116, kSlot418MenuStrings[116])) return out;
    ++out.rows_done;   // row 116 cell
    if (!name_cell(objects.r117, kSlot418MenuStrings[117])) return out;
    ++out.rows_done;   // row 117 cell
    if (!name_cell(objects.r118, kSlot418MenuStrings[118])) return out;
    ++out.rows_done;   // row 118 cell
    if (!name_cell(objects.r119, kSlot418MenuStrings[119])) return out;
    ++out.rows_done;   // row 119 cell
    if (!name_cell(objects.r120, kSlot418MenuStrings[120])) return out;
    ++out.rows_done;   // row 120 cell
    if (!name_cell(objects.r121, kSlot418MenuStrings[121])) return out;
    ++out.rows_done;   // row 121 cell
    if (!name_cell(objects.r122, kSlot418MenuStrings[122])) return out;
    ++out.rows_done;   // row 122 cell
    if (!name_cell(objects.r123, kSlot418MenuStrings[123])) return out;
    ++out.rows_done;   // row 123 cell
    if (!name_cell(objects.r124, kSlot418MenuStrings[124])) return out;
    ++out.rows_done;   // row 124 cell
    if (!name_cell(objects.r125, kSlot418MenuStrings[125])) return out;
    ++out.rows_done;   // row 125 cell
    if (!name_cell(objects.r126, kSlot418MenuStrings[126])) return out;
    ++out.rows_done;   // row 126 cell
    if (!name_cell(objects.r127, kSlot418MenuStrings[127])) return out;
    ++out.rows_done;   // row 127 cell
    if (!name_cell(objects.r128, kSlot418MenuStrings[128])) return out;
    ++out.rows_done;   // row 128 cell
    if (!name_cell(objects.r129, kSlot418MenuStrings[129])) return out;
    ++out.rows_done;   // row 129 cell
    if (!name_cell(objects.r130, kSlot418MenuStrings[130])) return out;
    ++out.rows_done;   // row 130 cell
    if (!name_cell(objects.r131, kSlot418MenuStrings[131])) return out;
    ++out.rows_done;   // row 131 cell
    if (!name_cell(objects.r132, kSlot418MenuStrings[132])) return out;
    ++out.rows_done;   // row 132 cell
    if (!name_cell(objects.r133, kSlot418MenuStrings[133])) return out;
    ++out.rows_done;   // row 133 cell
    if (!name_cell(objects.r134, kSlot418MenuStrings[134])) return out;
    ++out.rows_done;   // row 134 cell
    if (!name_cell(objects.r135, kSlot418MenuStrings[135])) return out;
    ++out.rows_done;   // row 135 cell
    if (!name_cell(objects.r136, kSlot418MenuStrings[136])) return out;
    ++out.rows_done;   // row 136 cell
    if (!name_cell(objects.r137, kSlot418MenuStrings[137])) return out;
    ++out.rows_done;   // row 137 cell
    if (!name_cell(objects.r138, kSlot418MenuStrings[138])) return out;
    ++out.rows_done;   // row 138 cell
    if (!name_cell(objects.r139, kSlot418MenuStrings[139])) return out;
    ++out.rows_done;   // row 139 cell
    if (!name_cell(objects.r140, kSlot418MenuStrings[140])) return out;
    ++out.rows_done;   // row 140 cell
    if (!name_cell(objects.r141, kSlot418MenuStrings[141])) return out;
    ++out.rows_done;   // row 141 cell
    if (!name_cell(objects.r142, kSlot418MenuStrings[142])) return out;
    ++out.rows_done;   // row 142 cell
    if (!name_cell(objects.r143, kSlot418MenuStrings[143])) return out;
    ++out.rows_done;   // row 143 cell
    if (!name_cell(objects.r144, kSlot418MenuStrings[144])) return out;
    ++out.rows_done;   // row 144 cell
    if (!name_cell(objects.r145, kSlot418MenuStrings[145])) return out;
    ++out.rows_done;   // row 145 cell
    if (!name_cell(objects.r146, kSlot418MenuStrings[146])) return out;
    ++out.rows_done;   // row 146 cell
    if (!name_cell(objects.r147, kSlot418MenuStrings[147])) return out;
    ++out.rows_done;   // row 147 cell
    if (!name_cell(objects.r148, kSlot418MenuStrings[148])) return out;
    ++out.rows_done;   // row 148 cell
    if (!name_cell(objects.r149, kSlot418MenuStrings[149])) return out;
    ++out.rows_done;   // row 149 cell
    if (!name_cell(objects.r150, kSlot418MenuStrings[150])) return out;
    ++out.rows_done;   // row 150 cell
    if (!name_cell(objects.r151, kSlot418MenuStrings[151])) return out;
    ++out.rows_done;   // row 151 cell
    if (!name_cell(objects.r152, kSlot418MenuStrings[152])) return out;
    ++out.rows_done;   // row 152 cell
    if (!name_cell(objects.r153, kSlot418MenuStrings[153])) return out;
    ++out.rows_done;   // row 153 cell
    if (!name_cell(objects.r154, kSlot418MenuStrings[154])) return out;
    ++out.rows_done;   // row 154 cell
    if (!name_cell(objects.r155, kSlot418MenuStrings[155])) return out;
    ++out.rows_done;   // row 155 cell
    if (!name_cell(objects.r156, kSlot418MenuStrings[156])) return out;
    ++out.rows_done;   // row 156 cell
    if (!name_cell(objects.r157, kSlot418MenuStrings[157])) return out;
    ++out.rows_done;   // row 157 cell
    if (!name_cell(objects.r158, kSlot418MenuStrings[158])) return out;
    ++out.rows_done;   // row 158 cell
    if (!name_cell(objects.r159, kSlot418MenuStrings[159])) return out;
    ++out.rows_done;   // row 159 cell
    if (!name_cell(objects.r160, kSlot418MenuStrings[160])) return out;
    ++out.rows_done;   // row 160 cell
    if (!name_cell(objects.r161, kSlot418MenuStrings[161])) return out;
    ++out.rows_done;   // row 161 cell
    if (!name_cell(objects.r162, kSlot418MenuStrings[162])) return out;
    ++out.rows_done;   // row 162 cell
    if (!name_cell(objects.r163, kSlot418MenuStrings[163])) return out;
    ++out.rows_done;   // row 163 cell
    if (!name_cell(objects.r164, kSlot418MenuStrings[164])) return out;
    ++out.rows_done;   // row 164 cell
    if (!name_cell(objects.r165, kSlot418MenuStrings[165])) return out;
    ++out.rows_done;   // row 165 cell
    if (!name_cell(objects.r166, kSlot418MenuStrings[166])) return out;
    ++out.rows_done;   // row 166 cell
    if (!name_cell(objects.r167, kSlot418MenuStrings[167])) return out;
    ++out.rows_done;   // row 167 cell
    if (!name_cell(objects.r168, kSlot418MenuStrings[168])) return out;
    ++out.rows_done;   // row 168 cell
    if (!name_cell(objects.r169, kSlot418MenuStrings[169])) return out;
    ++out.rows_done;   // row 169 cell
    if (!name_cell(objects.r170, kSlot418MenuStrings[170])) return out;
    ++out.rows_done;   // row 170 cell
    if (!name_cell(objects.r171, kSlot418MenuStrings[171])) return out;
    ++out.rows_done;   // row 171 cell
    if (!name_cell(objects.r172, kSlot418MenuStrings[172])) return out;
    ++out.rows_done;   // row 172 cell
    if (!name_cell(objects.r173, kSlot418MenuStrings[173])) return out;
    ++out.rows_done;   // row 173 cell
    if (!name_cell(objects.r174, kSlot418MenuStrings[174])) return out;
    ++out.rows_done;   // row 174 cell
    if (!name_cell(objects.r175, kSlot418MenuStrings[175])) return out;
    ++out.rows_done;   // row 175 cell
    if (!name_cell(objects.r176, kSlot418MenuStrings[176])) return out;
    ++out.rows_done;   // row 176 cell
    if (!name_cell(objects.r177, kSlot418MenuStrings[177])) return out;
    ++out.rows_done;   // row 177 cell
    if (!name_cell(objects.r178, kSlot418MenuStrings[178])) return out;
    ++out.rows_done;   // row 178 cell
    out.ok = true;
    return out;
}

}  // namespace sotc
