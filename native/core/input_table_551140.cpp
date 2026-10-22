#include "input_table_551140.hpp"

namespace sotc {

void init_input_table_551140(InputTable551140& table) noexcept {
    for (std::size_t i = 0; i != kInputTable551140Count; ++i) {
        const auto& row = input_table_551140_detail::kRows[i];
        InputTableEntry551140& e = table[i];
        e.name_a = nullptr;
        e.name_b = nullptr;
        e.a = row.a;
        e.b = row.b;
        e.c = row.c;
        e.pad_1c = 0;
        e.callback = nullptr;
        e.fnptr_state = nullptr;
        e.zero_30 = 0;
        e.zero_38 = 0;
        e.self_or_null = nullptr;
        e.zero_48 = 0;
        e.f50 = row.f50;
        e.hash_a = 0;
        e.hash_b = 0;
    }
}

InputTableEntry551140* input_table_551140_base(InputTable551140& table) noexcept {
    return table.data();
}

const InputTableEntry551140* input_table_551140_base(const InputTable551140& table) noexcept {
    return table.data();
}

std::uint32_t input_table_551140_count() noexcept {
    return kInputTable551140CountU32;
}

}  // namespace sotc
