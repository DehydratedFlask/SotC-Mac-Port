#include "pm4_indirect.hpp"

#include <vector>

namespace sotc::pm4 {
namespace {

class PolicySink final : public Sink {
public:
    PolicySink(const Policy& policy, TraversalResult& result, std::uint8_t depth,
               std::uint64_t buffer_address, const std::vector<std::uint64_t>& path,
               const GuestReader& reader, TraversalLimits limits) noexcept
        : policy_(policy),
          result_(result),
          depth_(depth),
          buffer_address_(buffer_address),
          path_(path),
          reader_(reader),
          limits_(limits) {}

    bool on_packet(const Packet& packet, const std::uint32_t* payload) override {
        if (packet.type != PacketType::command) {
            return true;
        }
        const OpcodeRule* rule = find_rule(packet.opcode);
        if (rule == nullptr) {
            return true;
        }

        if (rule->indirect) {
            ++result_.stats.indirects;
            if (!open_nested(*rule, packet, payload)) {
                return false;
            }
            return result_.status == TraversalStatus::ok;
        }

        if (rule->kind == DrawKind::none) {
            return true;
        }
        if (result_.stats.draws >= limits_.max_draws) {
            result_.status = TraversalStatus::decode_error;
            return false;
        }

        DrawCommand draw{};
        draw.kind = rule->kind;
        draw.opcode = packet.opcode;
        draw.depth = depth_;
        draw.header_index = packet.header_index;
        draw.buffer_address = buffer_address_;
        if (rule->count_field != 0xFF && rule->count_field < packet.count) {
            draw.count = payload[rule->count_field];
        }
        result_.draws.push_back(draw);
        ++result_.stats.draws;
        return true;
    }

private:
    [[nodiscard]] const OpcodeRule* find_rule(std::uint16_t opcode) const noexcept {
        for (std::size_t i = 0; i < policy_.count; ++i) {
            if (policy_.rules[i].opcode == opcode) {
                return &policy_.rules[i];
            }
        }
        return nullptr;
    }

    bool open_nested(const OpcodeRule& rule, const Packet& packet, const std::uint32_t* payload) {
        if (rule.address_field == 0xFF || rule.size_field == 0xFF) {
            result_.status = TraversalStatus::invalid_policy;
            result_.stop_address = buffer_address_;
            return false;
        }
        const std::uint8_t high_field = static_cast<std::uint8_t>(rule.address_field + 1);
        if (rule.size_field >= packet.count || high_field >= packet.count) {
            result_.status = TraversalStatus::bad_indirect_payload;
            result_.stop_address = buffer_address_;
            return false;
        }
        if (reader_.read == nullptr) {
            result_.status = TraversalStatus::reader_missing;
            result_.stop_address = buffer_address_;
            return false;
        }
        if (depth_ >= limits_.max_depth) {
            result_.status = TraversalStatus::depth_exceeded;
            result_.stop_address = buffer_address_;
            return false;
        }

        const std::uint64_t address =
            (static_cast<std::uint64_t>(payload[rule.address_field]) |
             (static_cast<std::uint64_t>(payload[high_field]) << 32));
        const std::uint32_t words = payload[rule.size_field];
        if (words == 0) {
            return true;
        }

        for (const std::uint64_t ancestor : path_) {
            if (ancestor == address) {
                ++result_.stats.cycles;
                result_.status = TraversalStatus::cycle_detected;
                result_.stop_address = address;
                return false;
            }
        }
        if (result_.stats.buffers >= limits_.max_buffers) {
            result_.status = TraversalStatus::depth_exceeded;
            result_.stop_address = address;
            return false;
        }

        std::vector<std::uint32_t> nested(words, 0);
        if (!reader_.read(reader_.user, address, nested.data(), words)) {
            ++result_.stats.unreadable;
            result_.status = TraversalStatus::unreadable_indirect;
            result_.stop_address = address;
            return false;
        }

        ++result_.stats.buffers;
        const std::uint8_t child_depth = static_cast<std::uint8_t>(depth_ + 1);
        if (child_depth > result_.stats.max_depth_seen) {
            result_.stats.max_depth_seen = child_depth;
        }

        std::vector<std::uint64_t> child_path = path_;
        child_path.push_back(address);

        PolicySink child(policy_, result_, child_depth, address, child_path, reader_, limits_);
        const Result decoded = decode(nested.data(), nested.size(), child);
        if (decoded.status != Status::ok && result_.status == TraversalStatus::ok) {
            result_.status = TraversalStatus::decode_error;
            result_.stop_address = address;
            return false;
        }
        return result_.status == TraversalStatus::ok;
    }

    const Policy& policy_;
    TraversalResult& result_;
    std::uint8_t depth_;
    std::uint64_t buffer_address_;
    const std::vector<std::uint64_t>& path_;
    GuestReader reader_;
    TraversalLimits limits_;
};

}  // namespace

const char* draw_kind_name(DrawKind kind) noexcept {
    switch (kind) {
        case DrawKind::none:
            return "none";
        case DrawKind::index_auto:
            return "index_auto";
        case DrawKind::index:
            return "index";
        case DrawKind::index_offset:
            return "index_offset";
        case DrawKind::index_indirect:
            return "index_indirect";
        case DrawKind::dispatch:
            return "dispatch";
        case DrawKind::dispatch_indirect:
            return "dispatch_indirect";
    }
    return "unknown";
}

const char* traversal_status_name(TraversalStatus status) noexcept {
    switch (status) {
        case TraversalStatus::ok:
            return "ok";
        case TraversalStatus::empty_buffer:
            return "empty_buffer";
        case TraversalStatus::invalid_policy:
            return "invalid_policy";
        case TraversalStatus::reader_missing:
            return "reader_missing";
        case TraversalStatus::depth_exceeded:
            return "depth_exceeded";
        case TraversalStatus::cycle_detected:
            return "cycle_detected";
        case TraversalStatus::unreadable_indirect:
            return "unreadable_indirect";
        case TraversalStatus::bad_indirect_payload:
            return "bad_indirect_payload";
        case TraversalStatus::decode_error:
            return "decode_error";
    }
    return "unknown";
}

TraversalResult traverse(const std::uint32_t* root, std::size_t root_words,
                         std::uint64_t root_address, const Policy& policy,
                         const GuestReader& reader, TraversalLimits limits) noexcept {
    TraversalResult result{};
    if (root == nullptr || root_words == 0) {
        result.status = TraversalStatus::empty_buffer;
        return result;
    }
    if (policy.rules == nullptr || policy.count == 0) {
        result.status = TraversalStatus::invalid_policy;
        return result;
    }

    result.stats.buffers = 1;
    std::vector<std::uint64_t> path{root_address};
    PolicySink sink(policy, result, 0, root_address, path, reader, limits);
    const Result decoded = decode(root, root_words, sink);
    if (decoded.status != Status::ok && result.status == TraversalStatus::ok) {
        result.status = TraversalStatus::decode_error;
        result.stop_address = root_address;
    }
    return result;
}

}  // namespace sotc::pm4
