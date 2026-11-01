#include "pm4_state.hpp"

namespace sotc::pm4 {

const char* state_status_name(StateStatus status) noexcept {
    switch (status) {
        case StateStatus::ok:
            return "ok";
        case StateStatus::write_out_of_range:
            return "write_out_of_range";
        case StateStatus::read_out_of_range:
            return "read_out_of_range";
        case StateStatus::missing_register:
            return "missing_register";
    }
    return "unknown";
}

RegisterFile::RegisterFile()
    : registers_(kRegisterIndexSpace), in_delta_(kRegisterIndexSpace) {}

bool RegisterFile::write(std::uint16_t index, const std::uint32_t* values,
                         std::uint16_t count) noexcept {
    if (count == 0) {
        return true;
    }
    if (values == nullptr) {
        return false;
    }
    const std::size_t start = index;
    const std::size_t end = start + count;
    if (end > kRegisterIndexSpace) {
        return false;
    }
    for (std::size_t i = start; i < end; ++i) {
        StateValue& slot = registers_[i];
        if (!slot.present) {
            slot.present = true;
            ++distinct_written_;
        }
        slot.value = values[i - start];
        if (!in_delta_[i]) {
            in_delta_[i] = true;
            delta_.push_back(static_cast<std::uint16_t>(i));
        }
    }
    ++write_count_;
    return true;
}

StateValue RegisterFile::read(std::uint16_t index) const noexcept {
    // The 16-bit header field cannot address outside kRegisterIndexSpace by construction.
    return registers_[static_cast<std::size_t>(index)];
}

bool StateTrackingSink::on_packet(const Packet& packet, const std::uint32_t* payload) {
    if (packet.type != PacketType::register_write) {
        return true;
    }
    if (!registers_.write(packet.reg_index, payload, packet.count)) {
        status_ = StateStatus::write_out_of_range;
        ++rejected_writes_;
        return false;
    }
    ++applied_writes_;
    return true;
}

std::uint64_t state_digest(const RegisterFile& registers) {
    // FNV-1a, 64-bit.
    std::uint64_t hash = 0xcbf29ce484222325ULL;
    for (std::size_t index = 0; index < kRegisterIndexSpace; ++index) {
        const StateValue value = registers.read(static_cast<std::uint16_t>(index));
        if (!value.present) {
            continue;
        }
        const std::uint64_t mixed = (static_cast<std::uint64_t>(index) << 32) | value.value;
        for (int byte = 0; byte < 8; ++byte) {
            hash ^= (mixed >> (byte * 8)) & 0xFFu;
            hash *= 0x100000001b3ULL;
        }
    }
    return hash;
}

}  // namespace sotc::pm4
