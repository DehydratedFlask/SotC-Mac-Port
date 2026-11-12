// CPU-01-AOT-CFG-SCALE-01-BATCH20: table-driven replay of a "hash a string, store the dword" run (startup slot 445, RVA 0x4cfe80).
//
// The original constructor runs 79 times `lea rdi, <rodata C string> ; call 0xa2ddc0 ; mov [rbx+disp], eax`: 0xa2ddc0 is `mov esi, 0x811c9dc5 ; jmp 0x162020`, the
// path-folding FNV hash, modelled natively as fnv_cstr_a2ddc0 (documented ASCII upper-case policy for the imported _Getptoupper). The generator tools/lift/startup_gen
// decodes the raw instruction bytes twice (objdump text and a byte decoder, they must agree) and emits one record per site; the checker re-derives the table from the ELF.
// This header is the NATIVE application of such a table to an image:
//   - nothing here executes, loads or calls an original RVA; `call_rva` / `store_rva` are identity tags of the instructions that produced the record;
//   - the replay validates EVERY record before the first byte is written and refuses (returns false, image untouched) on a malformed table;
//   - bytes of the image that no record covers are never written (the original does not write them either); later records overwrite earlier ones in order.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

struct HashStoreRecord {
    std::uint32_t call_rva;    // original `call 0xa2ddc0` (identity only)
    std::uint32_t store_rva;   // original `mov [rbx+disp], eax` (identity only)
    std::uint32_t offset;      // byte offset of the destination dword inside the image (4-byte aligned)
    std::uint32_t string;      // index into the string pool
};
static_assert(sizeof(HashStoreRecord) == 16);

struct HashStoreSpec {
    const HashStoreRecord* records = nullptr;
    std::size_t count = 0;
    const char* string_blob = nullptr;                 // concatenated NUL-terminated strings
    std::size_t string_blob_size = 0;
    const std::uint32_t* string_offset = nullptr;      // string_count entries: byte offset of each string inside the blob
    std::size_t string_count = 0;
};

// True when every record is well formed and was applied in order. False (image untouched) otherwise.
[[nodiscard]] bool replay_hash_stores(std::uint8_t* image, std::size_t image_size, const HashStoreSpec& spec) noexcept;

}  // namespace sotc
