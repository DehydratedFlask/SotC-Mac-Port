// CPU-01-AOT-CFG-SCALE-01-BATCH18: table-driven replay of a long straight-line store run (startup slot 424, RVA 0x551140).
//
// The original constructor writes ~2000 immediates / registers into ONE static object with plain stores (no call between them). The generator tools/lift/startup_gen
// decodes the raw instruction bytes twice (objdump text and a byte decoder, they must agree) and emits one record per store instruction in program order; the
// checker re-derives the same table from the ELF bytes. This header is the NATIVE application of such a table to an object image:
//   - nothing here executes, loads or calls an original RVA; `insn_rva` is an identity tag of the store instruction that produced the record;
//   - a record lane is a constant, a pointer into the image itself, a pointer to a native NUL-terminated string copy, or an IDENTITY TAG (the original address of
//     a function or of a vtable-like object; recorded as the integer, never called);
//   - the replay validates EVERY record before the first byte is written and refuses (returns false, image untouched) on a malformed table.
// Bytes of the image that no record covers are never written (the original does not write them either). Later records overwrite earlier ones in order.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

enum class StoreLaneKind : std::uint8_t {
    constant = 0,   // value is stored as is (width 4: low 32 bits)
    self = 1,       // value is a byte offset inside the image; the lane stores the NATIVE address image + value
    string = 2,     // value is an index into the string pool; the lane stores the NATIVE address of that NUL-terminated copy
    tag = 3,        // value is an original address kept as an IDENTITY TAG (function / vtable-like); stored as the integer, never followed
};

struct StoreTableRecord {
    std::uint32_t insn_rva;    // original store instruction (identity only)
    std::uint32_t offset;      // byte offset inside the image
    std::uint8_t width;        // 4, 8 or 16
    std::uint8_t kind0;        // StoreLaneKind of lane 0 (the only lane of width 4 / 8)
    std::uint8_t kind1;        // StoreLaneKind of lane 1 (width 16 only; 0 otherwise)
    std::uint8_t reserved;     // must be 0
    std::uint64_t value0;
    std::uint64_t value1;      // 0 unless width == 16
};
static_assert(sizeof(StoreTableRecord) == 32);

struct StoreTableSpec {
    const StoreTableRecord* records = nullptr;
    std::size_t count = 0;
    const char* string_blob = nullptr;                 // concatenated NUL-terminated strings
    std::size_t string_blob_size = 0;
    const std::uint32_t* string_offset = nullptr;      // string_count entries: byte offset of each string inside the blob
    std::size_t string_count = 0;
};

// True when every record is well formed and was applied in order. False (image untouched) otherwise.
[[nodiscard]] bool replay_store_table(std::uint8_t* image, std::size_t image_size, const StoreTableSpec& spec) noexcept;

}  // namespace sotc
