#include "init_531d60_trivial_leaves.hpp"

namespace sotc {
namespace {

std::uint8_t g_flag_2efa890 = 0;
std::uint8_t g_flag_2efa891 = 0;

}  // namespace

std::uint8_t& init_flag_cell_2efa890() noexcept { return g_flag_2efa890; }
std::uint8_t& init_flag_cell_2efa891() noexcept { return g_flag_2efa891; }

void set_init_flag_pair_4c4950(std::uint8_t flag) noexcept {
    g_flag_2efa890 = flag;
    g_flag_2efa891 = 1;
}

void set_byte_a52b70(void* obj, std::uint8_t v) noexcept {
    static_cast<std::uint8_t*>(obj)[kSetterOffA52b70] = v;
}

void set_byte_a599e0(void* obj, std::uint8_t v) noexcept {
    static_cast<std::uint8_t*>(obj)[kSetterOffA599e0] = v;
}

void set_byte_a431e0(void* obj, std::uint8_t v) noexcept {
    static_cast<std::uint8_t*>(obj)[kSetterOffA431e0] = v;
}

void ret_nop_56dca0() noexcept {}
void ret_nop_56db70() noexcept {}
void ret_nop_5b3d00() noexcept {}
void ret_nop_5b3cf0() noexcept {}
void ret_nop_56fad0() noexcept {}

void hide_splash_screen_f90310() noexcept {
    // Original: PLT jmp [GOT] → sceSystemServiceHideSplashScreen. No Sony module at runtime.
}

}  // namespace sotc
