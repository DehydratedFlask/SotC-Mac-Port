// Function `0x31900` (0xb1 B): poll the object's outstanding Fios op and, once it is done, fold the
// result into the object and delete the op.
//
// Real target of the withdrawn `vptr+0xa8` calls (`0x312ce`, `0x3133e`) and of the direct call at
// `0x312b7`/`0x31327`: `0x15954b0` holds `0x31900` (one type-8 RELA,
// `tools/lift/check_frontier_target_models.py`). Characterised from the ELF bytes (E1100): 0xb1 B,
// 52 instructions, `push rbp` frame, 4 direct PLT calls (`sceFiosOpIsDone` 0xf8f7c0,
// `sceFiosOpGetError` 0xf8f7b0, `sceFiosOpGetActualCount` 0xf8f7a0, `sceFiosOpDelete` 0xf8f790), 1
// indirect call (`vptr+0x50` at `0x319a8`, read as `0x316a0` -> `sceFiosFHGetSize([this+0x10])` for the
// `0x1595408` table, a 8 B body), and one 4-entry jump table at `0xf96ec8` (`movslq` relative
// entries, bounded by `dec ecx; cmp ecx,3; ja`). Object fields (all dword unless noted):
//   +0x08 mode (written as dword 1 at 0x31995, as qword 0 at 0x31969), +0x0c byte tested `& 8`,
//   +0x10 file handle, +0x18 qword cursor/size, +0x50 op handle, +0x54 op state, +0x58 actual count,
//   +0x5c error. State 4 is the one `0x31870`/`0x318b0` store (`0xFFFFFFFF00000004` at +0x54): a data
//   transfer; the meaning of states 1/2/3 below is inferred from what each arm does, not named by
//   the bytes.
//
//   bool f(Object* this) {
//       if (this->state != 0) {                              // 0x31909
//           if (sceFiosOpIsDone(this->op)) {
//               this->error = sceFiosOpGetError(this->op);   // +0x5c
//               this->count = sceFiosOpGetActualCount(this->op);   // +0x58, eax kept for cdqe
//               switch (this->state) {                       // dec; cmp 3; ja -> 0x31971
//                 case 1: if (this->error != 0) { *(u32*)(this+0x10) = 0; break; }
//                         *(u32*)(this+0x8) = 1;
//                         if (*(u8*)(this+0xc) & 8) *(u64*)(this+0x18) = this->vtable[0x50](this);
//                         break;
//                 case 2: *(u32*)(this+0x10) = 0; *(u64*)(this+0x8) = 0; break;
//                 case 3: case 4: *(i64*)(this+0x18) += (i64)(i32)count; break;   // 0x3194b
//                 default: break;                              // 5.. and "negative" states
//               }
//               this->state = 0;                                // 0x31971
//               sceFiosOpDelete(this->op);
//               this->op = 0;
//           }
//       }
//       return this->state == 0;                             // 0x31987; reloaded
//   }
//
// Fail-visible choices: every service is a hook. A hook that would be reached is checked before the
// first object write, so a missing hook leaves the object untouched and reports `ok == false` with
// `missing_hook` naming it. The `+0x50`/`+0x54` fields are re-read where the body re-reads them.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kFrontier31900Rva = 0x31900;
inline constexpr std::size_t kFrontier31900Size = 0xb1;
inline constexpr std::uint64_t kFrontier31900EndAddress = 0x319b1;
inline constexpr std::uint64_t kFrontier31900SlotAddress = 0x15954b0;   // 0x15953f8 + 0x10 + 0xa8
inline constexpr std::uint64_t kFrontier31900JumpTable = 0xf96ec8;
inline constexpr std::uint32_t kFrontier31900JumpTableEntries = 4;
inline constexpr std::uint64_t kFrontier31900IsDonePlt = 0xf8f7c0;          // sceFiosOpIsDone
inline constexpr std::uint64_t kFrontier31900GetErrorPlt = 0xf8f7b0;        // sceFiosOpGetError
inline constexpr std::uint64_t kFrontier31900GetActualCountPlt = 0xf8f7a0;  // sceFiosOpGetActualCount
inline constexpr std::uint64_t kFrontier31900DeletePlt = 0xf8f790;          // sceFiosOpDelete
inline constexpr std::uint32_t kFrontier31900SizeVtableOffset = 0x50;       // -> 0x316a0 (FHGetSize)
inline constexpr std::uint32_t kFrontier31900ModeOffset = 0x08;
inline constexpr std::uint32_t kFrontier31900FlagsOffset = 0x0c;
inline constexpr std::uint8_t kFrontier31900FlagsSizeMask = 0x08;
inline constexpr std::uint32_t kFrontier31900HandleOffset = 0x10;
inline constexpr std::uint32_t kFrontier31900CursorOffset = 0x18;
inline constexpr std::uint32_t kFrontier31900OpOffset = 0x50;
inline constexpr std::uint32_t kFrontier31900StateOffset = 0x54;
inline constexpr std::uint32_t kFrontier31900CountOffset = 0x58;
inline constexpr std::uint32_t kFrontier31900ErrorOffset = 0x5c;

struct Frontier31900Cells {
    std::uint8_t* object = nullptr;   // at least 0x60 bytes; null is fail-visible
};

struct Frontier31900Hooks {
    bool (*op_is_done)(std::uint32_t op, void* user) = nullptr;             // sceFiosOpIsDone (al)
    std::int32_t (*op_get_error)(std::uint32_t op, void* user) = nullptr;   // sceFiosOpGetError
    std::int32_t (*op_get_actual_count)(std::uint32_t op, void* user) = nullptr;
    void (*op_delete)(std::uint32_t op, void* user) = nullptr;              // sceFiosOpDelete
    std::uint64_t (*file_size)(void* object, void* user) = nullptr;         // vptr+0x50
    void* user = nullptr;
};

struct Frontier31900Result {
    bool ok = false;
    bool settled = false;           // the returned `al`: state == 0 after the call
    std::uint32_t state_before = 0;
    bool is_done_called = false;
    bool done = false;              // sceFiosOpIsDone reported done
    bool size_called = false;       // the vptr+0x50 call ran
    bool deleted = false;           // sceFiosOpDelete ran
    std::int32_t error = 0;
    std::int32_t count = 0;
    const char* missing_hook = nullptr;
};

[[nodiscard]] Frontier31900Result frontier_31900_fios_poll(
    const Frontier31900Cells& cells = {}, const Frontier31900Hooks& hooks = {}) noexcept;

}  // namespace sotc
