#include "registry_object_2efd160.hpp"

namespace sotc {

void init_registry_object_header_52a220(RegistryObject2efd160& obj, void* input_table) noexcept {
    obj.table_a_begin = nullptr;
    obj.table_a_end = nullptr;
    obj.table_a_cap = nullptr;
    obj.table_b_begin = nullptr;
    obj.table_b_end = nullptr;
    obj.table_b_cap = nullptr;
    obj.listeners_begin = nullptr;
    obj.listeners_end = nullptr;
    obj.listeners_cap = nullptr;
    obj.input_table = input_table;
    obj.float_5 = kRegistryObjectFloat5;
    obj.float_60 = kRegistryObjectFloat60;
}

}  // namespace sotc
