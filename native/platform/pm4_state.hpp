// PM4 register-file state tracker.
//
// The command processor applies type-1 packets to a register file before a draw reads it. This
// module keeps that state host-side: dense, bounds-checked, with snapshot/delta so a caller can
// compare two points in a stream (the shadow-check that the prior-art command processors use).
//
// The register index space is the PM4 header's 16-bit field. No register *meaning* is assigned
// here; names and semantics are added only with byte-bound evidence.
#pragma once

#include "pm4_decoder.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace sotc::pm4 {

inline constexpr std::size_t kRegisterIndexSpace = 0x10000;  // 16-bit header field

enum class StateStatus : std::uint8_t {
    ok = 0,
    write_out_of_range,
    read_out_of_range,
    missing_register,
};

const char* state_status_name(StateStatus status) noexcept;

struct StateValue {
    bool present = false;
    std::uint32_t value = 0;
};

class RegisterFile {
public:
    RegisterFile();

    // Applies `count` consecutive dwords starting at `index`. All-or-nothing: an out-of-range
    // write changes nothing and returns false.
    [[nodiscard]] bool write(std::uint16_t index, const std::uint32_t* values,
                             std::uint16_t count) noexcept;

    [[nodiscard]] StateValue read(std::uint16_t index) const noexcept;

    // Registers written since construction or since the last clear_delta().
    [[nodiscard]] const std::vector<std::uint16_t>& delta() const noexcept { return delta_; }
    void clear_delta() noexcept {
        for (const std::uint16_t index : delta_) {
            in_delta_[index] = false;
        }
        delta_.clear();
    }

    [[nodiscard]] std::uint64_t write_count() const noexcept { return write_count_; }
    [[nodiscard]] std::uint32_t distinct_written() const noexcept { return distinct_written_; }

private:
    std::vector<StateValue> registers_;
    std::vector<std::uint16_t> delta_;
    std::vector<bool> in_delta_;
    std::uint64_t write_count_ = 0;
    std::uint32_t distinct_written_ = 0;
};

// Sink that applies every type-1 packet to a RegisterFile and ignores the rest.
class StateTrackingSink final : public Sink {
public:
    explicit StateTrackingSink(RegisterFile& registers) noexcept : registers_(registers) {}

    bool on_packet(const Packet& packet, const std::uint32_t* payload) override;

    [[nodiscard]] StateStatus status() const noexcept { return status_; }
    [[nodiscard]] std::uint32_t applied_writes() const noexcept { return applied_writes_; }
    [[nodiscard]] std::uint32_t rejected_writes() const noexcept { return rejected_writes_; }

private:
    RegisterFile& registers_;
    StateStatus status_ = StateStatus::ok;
    std::uint32_t applied_writes_ = 0;
    std::uint32_t rejected_writes_ = 0;
};

// FNV-1a over the (index, value) pairs of a register set in ascending index order. Stable across
// runs and machines; used to compare a shadow parse with a reference parse.
[[nodiscard]] std::uint64_t state_digest(const RegisterFile& registers);

}  // namespace sotc::pm4
