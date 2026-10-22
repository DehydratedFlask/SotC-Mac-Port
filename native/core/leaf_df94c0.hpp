// Native recovery of original ctor RVA 0xdf94c0 (ctor index 55; ELF sha256 d0723afb...5861d3).
// Unguarded 34-byte, 9-instruction body: loads the float at rodata 0x1311c3c (bits 0x3d567750,
// 3 degrees in radians; identical bits to the separate rodata copy at 0x1314a8c that ctor 0xe10490 reads), calls the `_FSin` import thunk
// 0xf8fd10 with (x, edi=1 -> cosine, esi=0) and stores the float result to BSS 0x3353330. The
// callee is the one already modelled by sotc::libm::fsin (libm policy L1: <= 1 ULP, no bit-exactness
// claim). The consumer of 0x3353330 is not analysed. Names are ours; the original has no symbols.
#pragma once
#include <cstdint>

namespace sotc {

struct LeafDf94c0 {
    float cos_3_degrees;   // original cell 0x3353330 (4 bytes)   [libm-sensitive]
};

// Rewrites the cell on every call (unguarded in the original).
void init_leaf_df94c0(LeafDf94c0& g) noexcept;

}  // namespace sotc
