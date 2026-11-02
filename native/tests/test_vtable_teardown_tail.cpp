#include "../core/vtable_1595b60_slot18_488d0.hpp"
#include "../core/vtable_15ed2a0_slot10_e52cb0.hpp"
#include "../core/vtable_15f2f08_slot88_ed4860.hpp"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <vector>

namespace {

using sotc::AllocDispatchContract;
using sotc::Vtable1595b60Slot18_488d0Cells;
using sotc::Vtable1595b60Slot18_488d0Hooks;
using sotc::Vtable1595b60Slot18_488d0Result;
using sotc::Vtable15ed2a0Slot10_e52cb0Cells;
using sotc::Vtable15ed2a0Slot10_e52cb0Hooks;
using sotc::Vtable15ed2a0Slot10_e52cb0Result;
using sotc::Vtable15f2f08Slot88_ed4860Cells;
using sotc::Vtable15f2f08Slot88_ed4860Hooks;
using sotc::Vtable15f2f08Slot88_ed4860Result;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

void test_constants() {
    check("e52cb0 rva", sotc::kVtable15ed2a0Slot10_e52cb0Rva == 0xe52cb0);
    check("e52cb0 size", sotc::kVtable15ed2a0Slot10_e52cb0Size == 0xfe);
    check("e52cb0 reloc", sotc::kVtable15ed2a0Slot10_e52cb0RelocAddress == 0x15ed2b0);
    check("e52cb0 table", sotc::kVtable15ed2a0Slot10_e52cb0InstalledTable == 0x15ed2b0);
    check("e52cb0 base", sotc::kVtable15ed2a0Slot10_e52cb0BaseDtorRva == 0xe1ede0);
    check("e52cb0 slots", sotc::kVtable15ed2a0Slot10_e52cb0EntryFreeSlot == 0xd0 &&
                              sotc::kVtable15ed2a0Slot10_e52cb0BufferReleaseSlot == 0xc0 &&
                              sotc::kVtable15ed2a0Slot10_e52cb0ElementDestroySlot == 0x10);
    check("ed4860 rva", sotc::kVtable15f2f08Slot88_ed4860Rva == 0xed4860);
    check("ed4860 size", sotc::kVtable15f2f08Slot88_ed4860Size == 0x5);
    check("ed4860 reloc", sotc::kVtable15f2f08Slot88_ed4860RelocAddress == 0x15f2f90);
    check("ed4860 free", sotc::kVtable15f2f08Slot88_ed4860FreeTargetRva == 0x170b0);
    check("488d0 rva", sotc::kVtable1595b60Slot18_488d0Rva == 0x488d0);
    check("488d0 size", sotc::kVtable1595b60Slot18_488d0Size == 0x1c);
    check("488d0 reloc", sotc::kVtable1595b60Slot18_488d0RelocAddress == 0x1595b78);
    check("488d0 pre", sotc::kVtable1595b60Slot18_488d0PreCallRva == 0x48790);
}

struct TeardownTrace {
    std::vector<void*> freed_entries;
    std::vector<void*> destroyed;
    std::vector<const void*> released;
    std::vector<void*> pre_calls;
    std::vector<void*> base_calls;
    std::vector<std::uint32_t> tags;
};

TeardownTrace g_trace;

void free_entry(void* manager, void* entry) {
    check("e52cb0 manager", manager == reinterpret_cast<void*>(0x1000));
    g_trace.freed_entries.push_back(entry);
}

void release_buffer(void* manager, const void* buffer) {
    check("e52cb0 manager", manager == reinterpret_cast<void*>(0x1000));
    g_trace.released.push_back(buffer);
}

void destroy_element(void* element) { g_trace.destroyed.push_back(element); }

void base_dtor(void* self) { g_trace.base_calls.push_back(self); }

void pre_488d0(void* self) { g_trace.pre_calls.push_back(self); }

bool deallocate(void* pointer, std::uint32_t tag) {
    g_trace.tags.push_back(tag);
    g_trace.freed_entries.push_back(pointer);
    return true;
}

void test_three_owner_teardown() {
    g_trace = TeardownTrace{};

    alignas(8) std::uint8_t records[0x20] = {};
    void* const entry_a = reinterpret_cast<void*>(0xa1);
    void* const entry_b = reinterpret_cast<void*>(0xb2);
    std::memcpy(records + 0x00, &entry_a, sizeof(entry_a));
    records[0x08] = 1;
    std::memcpy(records + 0x10, &entry_b, sizeof(entry_b));
    records[0x18] = 0;  // flag clear: skipped

    void* const elements_array[] = {reinterpret_cast<void*>(0xe1), nullptr};
    std::uint8_t third[0x10] = {};

    void* vtable_slot = nullptr;
    std::uint8_t* entry_base = records;
    void* const* elements_field = elements_array;
    const void* third_value = third;
    void* manager = reinterpret_cast<void*>(0x1000);
    std::int32_t entry_count = 2;
    std::int32_t element_count = 2;
    std::uint8_t elements_owned = 1;
    std::uint8_t entries_owned = 1;
    std::uint8_t third_owned = 1;

    Vtable15ed2a0Slot10_e52cb0Cells cells{};
    cells.self = reinterpret_cast<void*>(0x2000);
    cells.vtable_field = &vtable_slot;
    cells.manager_field = &manager;
    cells.entry_count = &entry_count;
    cells.entry_base_field = &entry_base;
    cells.element_count = &element_count;
    cells.elements_field = &elements_field;
    cells.elements_owned = &elements_owned;
    cells.entries_owned = &entries_owned;
    cells.third_owned = &third_owned;
    cells.third_field = &third_value;

    Vtable15ed2a0Slot10_e52cb0Hooks hooks{};
    hooks.free_entry = free_entry;
    hooks.release_buffer = release_buffer;
    hooks.destroy_element = destroy_element;
    hooks.base_dtor_e1ede0 = base_dtor;

    const Vtable15ed2a0Slot10_e52cb0Result result =
        sotc::vtable_15ed2a0_slot10_e52cb0(cells, hooks);
    check("e52cb0 ok", result.ok);
    check("e52cb0 vtable", result.vtable_written && vtable_slot ==
                                                       reinterpret_cast<void*>(0x15ed2b0));
    check("e52cb0 one entry", result.entries_freed == 1);
    check("e52cb0 entry ptr", g_trace.freed_entries.size() == 1 &&
                                  g_trace.freed_entries[0] == entry_a);
    check("e52cb0 one element", result.elements_destroyed == 1);
    check("e52cb0 element ptr", g_trace.destroyed.size() == 1 &&
                                    g_trace.destroyed[0] == elements_array[0]);
    check("e52cb0 three releases", result.buffers_released == 3);
    check("e52cb0 release order", g_trace.released.size() == 3 &&
                                      g_trace.released[0] == elements_array &&
                                      g_trace.released[1] == records &&
                                      g_trace.released[2] == third_value);
    check("e52cb0 base", result.base_dtor_called && g_trace.base_calls.size() == 1);

    // Base-destructor hook missing: progress is recorded, the result is a visible failure.
    g_trace = TeardownTrace{};
    hooks.base_dtor_e1ede0 = nullptr;
    const Vtable15ed2a0Slot10_e52cb0Result no_base =
        sotc::vtable_15ed2a0_slot10_e52cb0(cells, hooks);
    check("e52cb0 no base fails", !no_base.ok && !no_base.base_dtor_called);
    check("e52cb0 no base progress", no_base.entries_freed == 1 &&
                                         no_base.elements_destroyed == 1 &&
                                         no_base.buffers_released == 3);

    // Everything empty: only the vtable stamp and the base destructor.
    g_trace = TeardownTrace{};
    hooks.base_dtor_e1ede0 = base_dtor;
    entry_count = 0;
    element_count = 0;
    elements_owned = 0;
    entries_owned = 0;
    third_owned = 0;
    const Vtable15ed2a0Slot10_e52cb0Result empty =
        sotc::vtable_15ed2a0_slot10_e52cb0(cells, hooks);
    check("e52cb0 empty ok", empty.ok);
    check("e52cb0 empty counts", empty.entries_freed == 0 && empty.elements_destroyed == 0 &&
                                     empty.buffers_released == 0);
    check("e52cb0 empty base", empty.base_dtor_called && g_trace.base_calls.size() == 1);
}

void test_tail_free() {
    int object = 0;
    AllocDispatchContract contract{};
    contract.deallocate = deallocate;

    g_trace = TeardownTrace{};
    Vtable15f2f08Slot88_ed4860Cells null_cells{};
    Vtable15f2f08Slot88_ed4860Hooks hooks{};
    hooks.alloc = &contract;
    const Vtable15f2f08Slot88_ed4860Result null_result =
        sotc::vtable_15f2f08_slot88_ed4860(null_cells, hooks);
    check("ed4860 null ok", null_result.ok);
    check("ed4860 null no dispatch", g_trace.tags.empty());

    g_trace = TeardownTrace{};
    Vtable15f2f08Slot88_ed4860Cells cells{};
    cells.self = &object;
    const Vtable15f2f08Slot88_ed4860Result result =
        sotc::vtable_15f2f08_slot88_ed4860(cells, hooks);
    check("ed4860 ok", result.ok && result.freed);
    check("ed4860 tag", result.free_tag == 1);
    check("ed4860 pointer", g_trace.freed_entries.size() == 1 &&
                                g_trace.freed_entries[0] == &object);

    g_trace = TeardownTrace{};
    const Vtable15f2f08Slot88_ed4860Result no_contract =
        sotc::vtable_15f2f08_slot88_ed4860(cells);
    check("ed4860 no contract fails", !no_contract.ok && !no_contract.freed);
}

void test_pre_call_and_free() {
    int object = 0;
    AllocDispatchContract contract{};
    contract.deallocate = deallocate;

    g_trace = TeardownTrace{};
    Vtable1595b60Slot18_488d0Cells cells{};
    cells.self = &object;
    Vtable1595b60Slot18_488d0Hooks hooks{};
    hooks.pre_call_48790 = pre_488d0;
    hooks.alloc = &contract;
    const Vtable1595b60Slot18_488d0Result result =
        sotc::vtable_1595b60_slot18_488d0(cells, hooks);
    check("488d0 ok", result.ok);
    check("488d0 pre", result.pre_call_modelled && g_trace.pre_calls.size() == 1 &&
                          g_trace.pre_calls[0] == &object);
    check("488d0 freed", result.freed && g_trace.freed_entries.size() == 1);
    check("488d0 tag", result.free_tag == 1);
}

}  // namespace

int main() {
    test_constants();
    test_three_owner_teardown();
    test_tail_free();
    test_pre_call_and_free();

    if (failures == 0) {
        std::printf("OK test_vtable_teardown_tail\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_teardown_tail: %d failure(s)\n", failures);
    return 1;
}
