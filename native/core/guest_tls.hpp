// Guest thread / TLS model for the retail call layer.
//
// Evidence (pinned executable + saved disassembly, FDE-covered code only):
//   * **38** instructions read the FS segment, and every one of them is `movq %fs:0x0, %reg` —
//     the thread pointer. No other FS offset appears in real code.
//   * A raw `grep %fs:` over the full `-D` disassembly finds 10,096 hits with 171 distinct offsets
//     (0x6f, 0x67, 0x31 … 0x5f). Those are data bytes that objdump disassembled as instructions:
//     every one of them lies outside an FDE range, and the "offsets" are ASCII codes ('o', 'g',
//     '1', '_' …). They are not TLS accesses and are excluded by construction here.
//   * The thread pointer is then dereferenced at offsets **0x8**, **0x10** and **0x400** in the
//     five instructions after the load (6, 4 and 2 occurrences).
//
// What this models: a checked block of host storage per guest thread id, addressed as
// (thread, offset), with the thread pointer handed out as a guest address from a caller-supplied
// base. What it does NOT model: the PS4 kernel's real TLS layout, thread creation/exit, the
// allocator's free-heap hint, or any per-thread register state. The block size is a policy choice
// that covers the largest observed offset (0x400), not a bound proven from the binary.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

enum class TlsStatus : std::uint8_t {
    ok = 0,
    no_thread,
    out_of_range,
    already_exists,
    too_many_threads,
};

const char* tls_status_name(TlsStatus status) noexcept;

class GuestThreads {
public:
    // Covers every observed `thread_pointer + offset` access (max 0x400) plus one qword.
    static constexpr std::uint32_t kBlockSize = 0x800;
    static constexpr std::uint32_t kMaxThreads = 64;
    // Host-side base of the window the thread pointers are handed out from. Distinct blocks make
    // "two threads never alias" structural rather than a test-only property.
    static constexpr std::uint64_t kDefaultBase = 0x7f00'0000'0000ULL;

    explicit GuestThreads(std::uint64_t guest_base = kDefaultBase) noexcept;

    [[nodiscard]] TlsStatus create(std::uint32_t thread_id) noexcept;
    [[nodiscard]] TlsStatus destroy(std::uint32_t thread_id) noexcept;
    [[nodiscard]] bool alive(std::uint32_t thread_id) const noexcept;
    [[nodiscard]] std::uint32_t live_count() const noexcept { return count_; }

    // `%fs:0x0` for this thread. Fails for an unknown id; the value is unique per live thread.
    [[nodiscard]] TlsStatus thread_pointer(std::uint32_t thread_id,
                                           std::uint64_t& out) const noexcept;

    [[nodiscard]] TlsStatus read_u8(std::uint32_t thread_id, std::uint32_t offset,
                                    std::uint8_t& out) const noexcept;
    [[nodiscard]] TlsStatus write_u8(std::uint32_t thread_id, std::uint32_t offset,
                                     std::uint8_t value) noexcept;
    [[nodiscard]] TlsStatus read_u32(std::uint32_t thread_id, std::uint32_t offset,
                                     std::uint32_t& out) const noexcept;
    [[nodiscard]] TlsStatus write_u32(std::uint32_t thread_id, std::uint32_t offset,
                                      std::uint32_t value) noexcept;
    [[nodiscard]] TlsStatus read_u64(std::uint32_t thread_id, std::uint32_t offset,
                                     std::uint64_t& out) const noexcept;
    [[nodiscard]] TlsStatus write_u64(std::uint32_t thread_id, std::uint32_t offset,
                                      std::uint64_t value) noexcept;

private:
    [[nodiscard]] int index_of(std::uint32_t thread_id) const noexcept;
    [[nodiscard]] std::uint8_t* block(int index) noexcept;
    [[nodiscard]] const std::uint8_t* block(int index) const noexcept;
    [[nodiscard]] TlsStatus check(std::uint32_t thread_id, std::uint32_t offset,
                                  std::size_t bytes) const noexcept;

    std::uint64_t guest_base_ = kDefaultBase;
    std::uint32_t ids_[kMaxThreads] = {};
    bool used_[kMaxThreads] = {};
    std::uint32_t count_ = 0;
    alignas(8) std::uint8_t storage_[kMaxThreads][kBlockSize] = {};
};

}  // namespace sotc
