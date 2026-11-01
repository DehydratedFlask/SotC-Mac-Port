#include "../platform/pm4_state.hpp"

#include <cstdio>
#include <cstdint>
#include <string_view>
#include <vector>

namespace {

using sotc::pm4::PacketType;
using sotc::pm4::RegisterFile;
using sotc::pm4::StateStatus;
using sotc::pm4::StateTrackingSink;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

constexpr std::uint32_t register_write_header(std::uint16_t index, std::uint32_t payload_dwords) {
    return (1u << 30) | (((payload_dwords - 1u) & 0x3FFFu) << 16) | index;
}

void test_empty() {
    RegisterFile regs;
    const auto value = regs.read(0);
    check("empty absent", !value.present);
    check("empty writes", regs.write_count() == 0);
    check("empty distinct", regs.distinct_written() == 0);
    check("empty delta", regs.delta().empty());
    check("empty digest", sotc::pm4::state_digest(regs) == 0xcbf29ce484222325ULL);
}

void test_single_write_and_read() {
    RegisterFile regs;
    const std::uint32_t values[] = {0xDEADBEEF};
    check("single write ok", regs.write(0x0123, values, 1));
    const auto value = regs.read(0x0123);
    check("single present", value.present);
    check("single value", value.value == 0xDEADBEEF);
    check("single writes", regs.write_count() == 1);
    check("single distinct", regs.distinct_written() == 1);
    check("single delta", regs.delta().size() == 1 && regs.delta()[0] == 0x0123);
    check("single missing neighbour", !regs.read(0x0124).present);
}

void test_overwrite_keeps_one_delta() {
    RegisterFile regs;
    const std::uint32_t first[] = {1};
    const std::uint32_t second[] = {2};
    check("overwrite first", regs.write(0x10, first, 1));
    check("overwrite second", regs.write(0x10, second, 1));
    check("overwrite value", regs.read(0x10).value == 2);
    check("overwrite writes", regs.write_count() == 2);
    check("overwrite distinct", regs.distinct_written() == 1);
    check("overwrite delta once", regs.delta().size() == 1);
    regs.clear_delta();
    check("delta cleared", regs.delta().empty());
    check("write after clear", regs.write(0x10, second, 1) && regs.delta().size() == 1);
}

void test_multi_write() {
    RegisterFile regs;
    const std::uint32_t values[] = {10, 11, 12, 13};
    check("multi ok", regs.write(0x0100, values, 4));
    check("multi first", regs.read(0x0100).value == 10);
    check("multi last", regs.read(0x0103).value == 13);
    check("multi distinct", regs.distinct_written() == 4);
    check("multi delta", regs.delta().size() == 4);
}

void test_out_of_range_write() {
    RegisterFile regs;
    const std::uint32_t values[] = {7, 8};
    const auto before = sotc::pm4::state_digest(regs);
    check("range rejected", !regs.write(0xFFFF, values, 2));
    check("range unchanged", sotc::pm4::state_digest(regs) == before);
    check("range no writes", regs.write_count() == 0);
    check("range no delta", regs.delta().empty());
    const std::uint32_t single[] = {9};
    check("range boundary ok", regs.write(0xFFFF, single, 1));
}

void test_null_values() {
    RegisterFile regs;
    check("null values rejected", !regs.write(0, nullptr, 1));
    check("zero count ok", regs.write(0, nullptr, 0));
}

void test_digest_sensitivity() {
    RegisterFile a;
    RegisterFile b;
    const std::uint32_t one[] = {1};
    check("digest a write", a.write(0x20, one, 1));
    check("digest b write", b.write(0x20, one, 1));
    check("digest equal", sotc::pm4::state_digest(a) == sotc::pm4::state_digest(b));
    const std::uint32_t two[] = {2};
    check("digest b rewrite", b.write(0x20, two, 1));
    check("digest differs by value", sotc::pm4::state_digest(a) != sotc::pm4::state_digest(b));
    RegisterFile c;
    check("digest c write", c.write(0x21, one, 1));
    check("digest differs by index", sotc::pm4::state_digest(a) != sotc::pm4::state_digest(c));
}

void test_sink_applies_register_packets() {
    RegisterFile regs;
    StateTrackingSink sink(regs);
    const std::uint32_t words[] = {register_write_header(0x0030, 2), 0xAAAA5555, 0x12345678,
                                   0,  // nop
                                   register_write_header(0x0040, 1), 0x99};
    const auto result = sotc::pm4::decode(words, 6, sink);
    check("sink decode ok", result.status == sotc::pm4::Status::ok);
    check("sink applied", sink.applied_writes() == 2);
    check("sink status", sink.status() == StateStatus::ok);
    check("sink value 0x30", regs.read(0x0030).value == 0xAAAA5555);
    check("sink value 0x31", regs.read(0x0031).value == 0x12345678);
    check("sink value 0x40", regs.read(0x0040).value == 0x99);
    check("sink packets", result.stats.register_writes == 2 && result.stats.nops == 1);
}

void test_sink_rejects_overflow() {
    RegisterFile regs;
    StateTrackingSink sink(regs);
    const std::uint32_t words[] = {register_write_header(0xFFFF, 2), 1, 2};
    const auto result = sotc::pm4::decode(words, 3, sink);
    check("overflow decode status", result.status == sotc::pm4::Status::sink_rejected);
    check("overflow sink status", sink.status() == StateStatus::write_out_of_range);
    check("overflow rejected count", sink.rejected_writes() == 1);
    check("overflow no writes", regs.write_count() == 0);
}

void test_status_names() {
    check("state name ok", std::string_view(sotc::pm4::state_status_name(StateStatus::ok)) == "ok");
    check("state name range",
          std::string_view(sotc::pm4::state_status_name(StateStatus::write_out_of_range)) ==
              "write_out_of_range");
    check("state name unknown",
          std::string_view(sotc::pm4::state_status_name(static_cast<StateStatus>(200))) == "unknown");
}

}  // namespace

int main() {
    test_empty();
    test_single_write_and_read();
    test_overwrite_keeps_one_delta();
    test_multi_write();
    test_out_of_range_write();
    test_null_values();
    test_digest_sensitivity();
    test_sink_applies_register_packets();
    test_sink_rejects_overflow();
    test_status_names();

    if (failures == 0) {
        std::printf("OK test_pm4_state\n");
        return 0;
    }
    std::fprintf(stderr, "test_pm4_state: %d failure(s)\n", failures);
    return 1;
}
