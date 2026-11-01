#include "../core/guest_memory.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string_view>
#include <vector>

namespace {

using sotc::GuestStatus;
using sotc::GuestWindow;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

void test_empty_window() {
    GuestWindow window;
    check("empty invalid", !window.valid());
    std::uint8_t value = 0;
    check("empty read", window.read_u8(0, value) == GuestStatus::no_window);
    check("empty write", window.write_u8(0, 1) == GuestStatus::no_window);
    std::string_view text;
    check("empty cstr", window.read_cstr(0, 8, text) == GuestStatus::no_window);
}

void test_read_write() {
    std::vector<std::uint8_t> storage(64, 0);
    GuestWindow window(storage.data(), 0x1000, storage.size());
    check("valid", window.valid());
    check("write8", window.write_u8(0x1000, 0xAB) == GuestStatus::ok);
    check("write32", window.write_u32(0x1004, 0x11223344) == GuestStatus::ok);
    check("write64", window.write_u64(0x1008, 0x1122334455667788ULL) == GuestStatus::ok);

    std::uint8_t value8 = 0;
    std::uint32_t value32 = 0;
    std::uint64_t value64 = 0;
    check("read8", window.read_u8(0x1000, value8) == GuestStatus::ok && value8 == 0xAB);
    check("read32", window.read_u32(0x1004, value32) == GuestStatus::ok && value32 == 0x11223344);
    check("read64", window.read_u64(0x1008, value64) == GuestStatus::ok &&
                        value64 == 0x1122334455667788ULL);
}

void test_bounds() {
    std::vector<std::uint8_t> storage(16, 0);
    GuestWindow window(storage.data(), 0x2000, storage.size());
    std::uint64_t value = 0;
    check("below base", window.read_u64(0x1FFF, value) == GuestStatus::out_of_range);
    check("past end", window.read_u64(0x2009, value) == GuestStatus::out_of_range);
    check("exact end ok", window.read_u64(0x2008, value) == GuestStatus::ok);
    check("write past end", window.write_u32(0x200E, 1) == GuestStatus::out_of_range);
    check("host pointer null", window.host_pointer(0x3000, 1) == nullptr);
}

void test_strings() {
    std::vector<std::uint8_t> storage(32, 0);
    std::memcpy(storage.data() + 4, "alpha", 6);
    std::memset(storage.data() + 16, 'x', 16);  // no terminator within range
    GuestWindow window(storage.data(), 0x3000, storage.size());

    std::string_view text;
    check("cstr ok", window.read_cstr(0x3004, 8, text) == GuestStatus::ok && text == "alpha");
    check("cstr limit", window.read_cstr(0x3004, 3, text) == GuestStatus::unterminated_string);
    check("cstr unterminated",
          window.read_cstr(0x3010, 32, text) == GuestStatus::unterminated_string);
    check("cstr out of range", window.read_cstr(0x4000, 8, text) == GuestStatus::out_of_range);
}

void test_status_names() {
    check("name ok", std::string_view(sotc::guest_status_name(GuestStatus::ok)) == "ok");
    check("name range",
          std::string_view(sotc::guest_status_name(GuestStatus::out_of_range)) == "out_of_range");
}

}  // namespace

int main() {
    test_empty_window();
    test_read_write();
    test_bounds();
    test_strings();
    test_status_names();

    if (failures == 0) {
        std::printf("OK test_guest_memory\n");
        return 0;
    }
    std::fprintf(stderr, "test_guest_memory: %d failure(s)\n", failures);
    return 1;
}
