#include "helper_8d650_store_one.hpp"

namespace sotc {

void store_one_8d650(std::uint32_t& cell) noexcept {
    cell = kStoreOne8d650Value;   // c7 07 01 00 00 00: mov dword [rdi], 1
}

}  // namespace sotc
