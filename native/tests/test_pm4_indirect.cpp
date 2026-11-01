#include "../platform/pm4_indirect.hpp"
#include "../platform/pm4_state.hpp"

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace {

using sotc::pm4::DrawKind;
using sotc::pm4::GuestReader;
using sotc::pm4::OpcodeRule;
using sotc::pm4::Policy;
using sotc::pm4::TraversalLimits;
using sotc::pm4::TraversalStatus;
using sotc::pm4::traverse;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

constexpr std::uint32_t command(std::uint8_t opcode, std::uint32_t payload_dwords) {
    return (3u << 30) | (((payload_dwords - 1u) & 0x3FFFu) << 16) |
           ((static_cast<std::uint32_t>(opcode) & 0xFFu) << 8);
}

constexpr std::uint32_t nop() { return 0; }

constexpr std::uint8_t kIndirect = 0x10;
constexpr std::uint8_t kDrawAuto = 0x2D;
constexpr std::uint8_t kDrawIndex = 0x2E;

// Synthetic policy: indirect payload = [addr_lo, addr_hi, size_dwords]; draws carry count at 0.
constexpr OpcodeRule kRules[] = {
    {kIndirect, DrawKind::none, 0xFF, true, 0, 2},
    {kDrawAuto, DrawKind::index_auto, 0, false, 0xFF, 0xFF},
    {kDrawIndex, DrawKind::index, 0, false, 0xFF, 0xFF},
};
constexpr Policy kPolicy{kRules, sizeof(kRules) / sizeof(kRules[0])};

struct MapReader {
    const std::vector<std::pair<std::uint64_t, std::vector<std::uint32_t>>>* buffers = nullptr;

    static bool read(void* user, std::uint64_t address, std::uint32_t* out, std::size_t words) {
        const auto* self = static_cast<const MapReader*>(user);
        for (const auto& entry : *self->buffers) {
            if (entry.first != address) {
                continue;
            }
            if (entry.second.size() < words) {
                return false;
            }
            std::memcpy(out, entry.second.data(), words * sizeof(std::uint32_t));
            return true;
        }
        return false;
    }
};

void test_flat_stream() {
    const std::uint32_t root[] = {command(kDrawAuto, 1), 42, nop(), command(kDrawIndex, 1), 7};
    MapReader reader{};
    const std::vector<std::pair<std::uint64_t, std::vector<std::uint32_t>>> buffers{};
    reader.buffers = &buffers;
    const auto result = traverse(root, 5, 0x100, kPolicy, GuestReader{&MapReader::read, &reader});
    check("flat status", result.status == TraversalStatus::ok);
    check("flat draws", result.draws.size() == 2);
    check("flat kinds", result.draws[0].kind == DrawKind::index_auto &&
                            result.draws[1].kind == DrawKind::index);
    check("flat counts", result.draws[0].count == 42 && result.draws[1].count == 7);
    check("flat depth", result.draws[0].depth == 0 && result.draws[1].depth == 0);
    check("flat buffers", result.stats.buffers == 1 && result.stats.indirects == 0);
    check("flat address", result.draws[0].buffer_address == 0x100);
}

void test_one_level_indirect() {
    const std::vector<std::pair<std::uint64_t, std::vector<std::uint32_t>>> buffers{
        {0x300, {command(kDrawIndex, 1), 9}},
    };
    const std::uint32_t root[] = {command(kIndirect, 3), 0x300, 0, 2};
    MapReader reader{};
    reader.buffers = &buffers;
    const auto result = traverse(root, 4, 0x100, kPolicy, GuestReader{&MapReader::read, &reader});
    check("one status", result.status == TraversalStatus::ok);
    check("one draws", result.draws.size() == 1 && result.draws[0].count == 9);
    check("one depth", result.draws[0].depth == 1);
    check("one buffer addr", result.draws[0].buffer_address == 0x300);
    check("one stats", result.stats.buffers == 2 && result.stats.indirects == 1 &&
                           result.stats.max_depth_seen == 1);
}

void test_nested_order() {
    const std::vector<std::pair<std::uint64_t, std::vector<std::uint32_t>>> buffers{
        {0x300, {command(kDrawAuto, 1), 1}},
        {0x400, {command(kDrawAuto, 1), 2}},
    };
    const std::uint32_t root[] = {command(kIndirect, 3), 0x300, 0, 2,
                                  command(kDrawAuto, 1), 99,
                                  command(kIndirect, 3), 0x400, 0, 2};
    MapReader reader{};
    reader.buffers = &buffers;
    const auto result = traverse(root, 10, 0x100, kPolicy, GuestReader{&MapReader::read, &reader});
    check("order status", result.status == TraversalStatus::ok);
    check("order draws", result.draws.size() == 3);
    check("order sequence", result.draws[0].count == 1 && result.draws[1].count == 99 &&
                                result.draws[2].count == 2);
    check("order depths", result.draws[0].depth == 1 && result.draws[1].depth == 0 &&
                              result.draws[2].depth == 1);
}

void test_depth_limit() {
    const std::vector<std::pair<std::uint64_t, std::vector<std::uint32_t>>> buffers{
        {0x200, {command(kIndirect, 3), 0x300, 0, 4}},
        {0x300, {command(kIndirect, 3), 0x400, 0, 4}},
        {0x400, {command(kDrawAuto, 1), 5}},
    };
    const std::uint32_t root[] = {command(kIndirect, 3), 0x200, 0, 4};
    MapReader reader{};
    reader.buffers = &buffers;
    TraversalLimits limits{};
    limits.max_depth = 2;
    const auto result =
        traverse(root, 4, 0x100, kPolicy, GuestReader{&MapReader::read, &reader}, limits);
    check("depth status", result.status == TraversalStatus::depth_exceeded);
    check("depth stop", result.stop_address == 0x300);
    check("depth draws", result.draws.empty());
}

void test_cycle_detected() {
    const std::vector<std::pair<std::uint64_t, std::vector<std::uint32_t>>> buffers{
        {0x200, {command(kIndirect, 3), 0x300, 0, 4}},
        {0x300, {command(kIndirect, 3), 0x200, 0, 4}},
    };
    const std::uint32_t root[] = {command(kIndirect, 3), 0x200, 0, 4};
    MapReader reader{};
    reader.buffers = &buffers;
    const auto result = traverse(root, 4, 0x100, kPolicy, GuestReader{&MapReader::read, &reader});
    check("cycle status", result.status == TraversalStatus::cycle_detected);
    check("cycle stop", result.stop_address == 0x200);
    check("cycle count", result.stats.cycles == 1);
}

void test_unreadable() {
    const std::uint32_t root[] = {command(kIndirect, 3), 0x999, 0, 2};
    MapReader reader{};
    const std::vector<std::pair<std::uint64_t, std::vector<std::uint32_t>>> buffers{};
    reader.buffers = &buffers;
    const auto result = traverse(root, 4, 0x100, kPolicy, GuestReader{&MapReader::read, &reader});
    check("unreadable status", result.status == TraversalStatus::unreadable_indirect);
    check("unreadable stop", result.stop_address == 0x999);
    check("unreadable count", result.stats.unreadable == 1);
}

void test_bad_indirect_payload() {
    const std::uint32_t root[] = {command(kIndirect, 1), 0x300};
    MapReader reader{};
    const std::vector<std::pair<std::uint64_t, std::vector<std::uint32_t>>> buffers{};
    reader.buffers = &buffers;
    const auto result = traverse(root, 2, 0x100, kPolicy, GuestReader{&MapReader::read, &reader});
    check("bad payload status", result.status == TraversalStatus::bad_indirect_payload);
}

void test_reader_missing() {
    const std::uint32_t root[] = {command(kIndirect, 3), 0x300, 0, 2};
    const auto result = traverse(root, 4, 0x100, kPolicy, GuestReader{});
    check("no reader status", result.status == TraversalStatus::reader_missing);
}

void test_invalid_policy_and_empty() {
    const std::uint32_t root[] = {command(kDrawAuto, 1), 1};
    MapReader reader{};
    const std::vector<std::pair<std::uint64_t, std::vector<std::uint32_t>>> buffers{};
    reader.buffers = &buffers;
    const auto empty = traverse(root, 2, 0x100, Policy{}, GuestReader{&MapReader::read, &reader});
    check("policy status", empty.status == TraversalStatus::invalid_policy);
    const auto none = traverse(nullptr, 0, 0x100, kPolicy, GuestReader{&MapReader::read, &reader});
    check("empty status", none.status == TraversalStatus::empty_buffer);
}

void test_draw_cap() {
    std::vector<std::uint32_t> root;
    for (int i = 0; i < 8; ++i) {
        root.push_back(command(kDrawAuto, 1));
        root.push_back(static_cast<std::uint32_t>(i));
    }
    MapReader reader{};
    const std::vector<std::pair<std::uint64_t, std::vector<std::uint32_t>>> buffers{};
    reader.buffers = &buffers;
    TraversalLimits limits{};
    limits.max_draws = 3;
    const auto result =
        traverse(root.data(), root.size(), 0x100, kPolicy, GuestReader{&MapReader::read, &reader},
                 limits);
    check("cap status", result.status == TraversalStatus::decode_error);
    check("cap draws", result.draws.size() == 3);
}

void test_status_names() {
    check("name ok", std::string_view(sotc::pm4::traversal_status_name(TraversalStatus::ok)) == "ok");
    check("name cycle",
          std::string_view(sotc::pm4::traversal_status_name(TraversalStatus::cycle_detected)) ==
              "cycle_detected");
    check("kind name",
          std::string_view(sotc::pm4::draw_kind_name(DrawKind::index_offset)) == "index_offset");
}

// ---------------------------------------------------------------------------------------------
// `--traverse <scenario>`: run a scenario file supplied by tools/lift/check_pm4_indirect.py and
// print a canonical result, so an independent Python traversal can be compared with this one.

std::vector<std::uint32_t> parse_words(std::istream& in, std::size_t count) {
    std::vector<std::uint32_t> words(count, 0);
    std::string token;
    for (std::size_t i = 0; i < count; ++i) {
        if (!(in >> token)) {
            return {};
        }
        words[i] = static_cast<std::uint32_t>(std::strtoul(token.c_str(), nullptr, 16));
    }
    return words;
}

int traverse_mode(const char* path) {
    std::ifstream input(path);
    if (!input) {
        std::fprintf(stderr, "cannot read %s\n", path);
        return 2;
    }
    std::vector<OpcodeRule> rules;
    std::vector<std::pair<std::uint64_t, std::vector<std::uint32_t>>> buffers;
    std::uint64_t root_address = 0;
    std::vector<std::uint32_t> root;

    std::string keyword;
    while (input >> keyword) {
        if (keyword == "policy") {
            std::size_t count = 0;
            input >> count;
            rules.resize(count);
            for (std::size_t i = 0; i < count; ++i) {
                unsigned opcode = 0, kind = 0, count_field = 0, indirect = 0, addr = 0, size = 0;
                input >> opcode >> kind >> count_field >> indirect >> addr >> size;
                rules[i].opcode = static_cast<std::uint16_t>(opcode);
                rules[i].kind = static_cast<DrawKind>(kind);
                rules[i].count_field = static_cast<std::uint8_t>(count_field);
                rules[i].indirect = indirect != 0;
                rules[i].address_field = static_cast<std::uint8_t>(addr);
                rules[i].size_field = static_cast<std::uint8_t>(size);
            }
        } else if (keyword == "buffer") {
            std::uint64_t address = 0;
            std::size_t count = 0;
            input >> std::hex >> address >> std::dec >> count;
            buffers.emplace_back(address, parse_words(input, count));
        } else if (keyword == "root") {
            std::size_t count = 0;
            input >> std::hex >> root_address >> std::dec >> count;
            root = parse_words(input, count);
        } else {
            std::fprintf(stderr, "unknown keyword %s\n", keyword.c_str());
            return 2;
        }
    }

    MapReader reader{};
    reader.buffers = &buffers;
    const Policy policy{rules.data(), rules.size()};
    const auto result =
        traverse(root.data(), root.size(), root_address, policy, GuestReader{&MapReader::read, &reader});

    std::printf("draws %zu\n", result.draws.size());
    for (const auto& draw : result.draws) {
        std::printf("draw %s %u %u %llu\n", sotc::pm4::draw_kind_name(draw.kind), draw.count,
                    static_cast<unsigned>(draw.depth),
                    static_cast<unsigned long long>(draw.buffer_address));
    }
    std::printf("stats buffers=%u indirects=%u draws=%u cycles=%u unreadable=%u maxdepth=%u status=%s\n",
                result.stats.buffers, result.stats.indirects, result.stats.draws,
                result.stats.cycles, result.stats.unreadable,
                static_cast<unsigned>(result.stats.max_depth_seen),
                sotc::pm4::traversal_status_name(result.status));
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc == 3 && std::string_view(argv[1]) == "--traverse") {
        return traverse_mode(argv[2]);
    }
    test_flat_stream();
    test_one_level_indirect();
    test_nested_order();
    test_depth_limit();
    test_cycle_detected();
    test_unreadable();
    test_bad_indirect_payload();
    test_reader_missing();
    test_invalid_policy_and_empty();
    test_draw_cap();
    test_status_names();

    if (failures == 0) {
        std::printf("OK test_pm4_indirect\n");
        return 0;
    }
    std::fprintf(stderr, "test_pm4_indirect: %d failure(s)\n", failures);
    return 1;
}
