#include "../core/vtable_15e1ee0_slot10_e19440.hpp"
#include "../core/vtable_15e2028_slot18_e19cb0.hpp"
#include "../core/vtable_15e2028_slot38_e19f70.hpp"
#include "../core/vtable_15e2028_slot40_e1a040.hpp"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <vector>

namespace {

using sotc::AllocDispatchContract;
using sotc::Vtable15e1ee0Slot10_e19440Cells;
using sotc::Vtable15e1ee0Slot10_e19440Hooks;
using sotc::Vtable15e1ee0Slot10_e19440Result;
using sotc::Vtable15e2028Slot18_e19cb0Cells;
using sotc::Vtable15e2028Slot18_e19cb0Hooks;
using sotc::Vtable15e2028Slot18_e19cb0Result;
using sotc::Vtable15e2028Slot38_e19f70Cells;
using sotc::Vtable15e2028Slot38_e19f70Hooks;
using sotc::Vtable15e2028Slot38_e19f70Result;
using sotc::Vtable15e2028Slot40_e1a040Cells;
using sotc::Vtable15e2028Slot40_e1a040Hooks;
using sotc::Vtable15e2028Slot40_e1a040Result;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

struct Element {
    const char* name;
    std::int32_t id;
};

const char* element_name(void* element) {
    return static_cast<Element*>(element)->name;
}

std::int32_t element_id(void* element) {
    return static_cast<Element*>(element)->id;
}

void test_constants() {
    check("e1a040 rva", sotc::kVtable15e2028Slot40_e1a040Rva == 0xe1a040);
    check("e1a040 size", sotc::kVtable15e2028Slot40_e1a040Size == 0x81);
    check("e1a040 reloc", sotc::kVtable15e2028Slot40_e1a040RelocAddress == 0x15e2068);
    check("e1a040 slots", sotc::kVtable15e2028Slot40_e1a040NameSlot == 0x10 &&
                              sotc::kVtable15e2028Slot40_e1a040IdSlot == 0x18);
    check("e19f70 rva", sotc::kVtable15e2028Slot38_e19f70Rva == 0xe19f70);
    check("e19f70 size", sotc::kVtable15e2028Slot38_e19f70Size == 0xc5);
    check("e19f70 reloc", sotc::kVtable15e2028Slot38_e19f70RelocAddress == 0x15e2060);
    check("e19cb0 rva", sotc::kVtable15e2028Slot18_e19cb0Rva == 0xe19cb0);
    check("e19cb0 size", sotc::kVtable15e2028Slot18_e19cb0Size == 0x1c);
    check("e19cb0 reloc", sotc::kVtable15e2028Slot18_e19cb0RelocAddress == 0x15e2040);
    check("e19cb0 pre", sotc::kVtable15e2028Slot18_e19cb0PreCallRva == 0xe1b1b0);
    check("e19440 rva", sotc::kVtable15e1ee0Slot10_e19440Rva == 0xe19440);
    check("e19440 size", sotc::kVtable15e1ee0Slot10_e19440Size == 0x8f);
    check("e19440 reloc", sotc::kVtable15e1ee0Slot10_e19440RelocAddress == 0x15e1ef0);
    check("e19440 stamps", sotc::kVtable15e1ee0Slot10_e19440StampA == 0x15e1f30 &&
                               sotc::kVtable15e1ee0Slot10_e19440StampB == 0x15e1f88 &&
                               sotc::kVtable15e1ee0Slot10_e19440StampC == 0x15e1fe0);
}

void test_name_id_lookup() {
    const Vtable15e2028Slot40_e1a040Result null_key = sotc::vtable_15e2028_slot40_e1a040();
    check("e1a040 null key early", null_key.ok && null_key.early_return);
    check("e1a040 null key zero", null_key.code == 0);

    std::uint32_t count = 0;
    Vtable15e2028Slot40_e1a040Cells empty{};
    empty.count = &count;
    const Vtable15e2028Slot40_e1a040Result empty_result =
        sotc::vtable_15e2028_slot40_e1a040(empty);
    check("e1a040 empty early", empty_result.ok && empty_result.early_return);
    check("e1a040 empty zero", empty_result.code == 0);

    Element a{"alpha", 3};
    Element b{"beta", 4};
    void* elements[] = {&a, &b};
    count = 2;
    Vtable15e2028Slot40_e1a040Cells cells{};
    cells.elements = elements;
    cells.count = &count;
    cells.key = "beta";
    cells.value = 4;
    Vtable15e2028Slot40_e1a040Hooks hooks{};
    hooks.element_name = element_name;
    hooks.element_id = element_id;

    const Vtable15e2028Slot40_e1a040Result hit = sotc::vtable_15e2028_slot40_e1a040(cells, hooks);
    check("e1a040 hit ok", hit.ok);
    check("e1a040 hit code", hit.code == 1);
    check("e1a040 hit index", hit.found_index == 1);

    cells.value = 9;
    const Vtable15e2028Slot40_e1a040Result miss = sotc::vtable_15e2028_slot40_e1a040(cells, hooks);
    check("e1a040 miss ok", miss.ok);
    check("e1a040 miss code", miss.code == 0);
    check("e1a040 miss scans", miss.elements_scanned == 2);

    Vtable15e2028Slot40_e1a040Hooks no_id{};
    no_id.element_name = element_name;
    const Vtable15e2028Slot40_e1a040Result missing_id =
        sotc::vtable_15e2028_slot40_e1a040(cells, no_id);
    check("e1a040 missing id fails", !missing_id.ok);
}

void test_max_id_scan() {
    Element a{"same", 3};
    Element b{"same", 9};
    Element c{"same", 5};
    Element d{"other", 100};
    void* elements[] = {&a, &d, &b, &c};
    std::uint32_t count = 4;

    Vtable15e2028Slot38_e19f70Cells cells{};
    cells.elements = elements;
    cells.count = &count;
    cells.key = "same";
    Vtable15e2028Slot38_e19f70Hooks hooks{};
    hooks.element_name = element_name;
    hooks.element_id = element_id;

    const Vtable15e2028Slot38_e19f70Result result =
        sotc::vtable_15e2028_slot38_e19f70(cells, hooks);
    check("e19f70 ok", result.ok);
    check("e19f70 code", result.code == 1);
    check("e19f70 found", result.found);
    check("e19f70 matches", result.name_matches == 3);
    check("e19f70 best index", result.best_index == 2);  // id 9 beats 3 and 5
    check("e19f70 best id", result.best_id == 9);
    check("e19f70 id calls", result.id_calls == 4);      // two comparisons, two reads each

    cells.key = "missing";
    const Vtable15e2028Slot38_e19f70Result miss =
        sotc::vtable_15e2028_slot38_e19f70(cells, hooks);
    check("e19f70 miss ok", miss.ok);
    check("e19f70 miss code", miss.code == 0);
    check("e19f70 miss not found", !miss.found);

    cells.key = "same";
    Vtable15e2028Slot38_e19f70Hooks no_id{};
    no_id.element_name = element_name;
    const Vtable15e2028Slot38_e19f70Result missing_id =
        sotc::vtable_15e2028_slot38_e19f70(cells, no_id);
    check("e19f70 missing id fails", !missing_id.ok);
}

struct FreeTrace {
    std::vector<void*> freed;
    std::vector<void*> pre_calls;
};

FreeTrace g_free;

void free_pre(void* self) { g_free.pre_calls.push_back(self); }

bool deallocate(void* pointer, std::uint32_t) {
    g_free.freed.push_back(pointer);
    return true;
}

bool deallocate_fail(void*, std::uint32_t) { return false; }

void test_pre_call_free() {
    g_free = FreeTrace{};
    int object = 0;
    AllocDispatchContract contract{};
    contract.deallocate = deallocate;
    Vtable15e2028Slot18_e19cb0Cells cells{};
    cells.self = &object;
    Vtable15e2028Slot18_e19cb0Hooks hooks{};
    hooks.pre_call_e1b1b0 = free_pre;
    hooks.alloc = &contract;
    const Vtable15e2028Slot18_e19cb0Result result =
        sotc::vtable_15e2028_slot18_e19cb0(cells, hooks);
    check("e19cb0 ok", result.ok);
    check("e19cb0 pre", result.pre_call_modelled && g_free.pre_calls.size() == 1);
    check("e19cb0 freed", result.freed && g_free.freed.size() == 1);
    check("e19cb0 tag", result.free_tag == 1);

    g_free = FreeTrace{};
    contract.deallocate = deallocate_fail;
    const Vtable15e2028Slot18_e19cb0Result failed =
        sotc::vtable_15e2028_slot18_e19cb0(cells, hooks);
    check("e19cb0 free failure visible", !failed.ok && !failed.freed);
}

struct AllocTrace {
    std::vector<std::size_t> sizes;
    std::vector<void*> nodes;
};

AllocTrace g_alloc;
std::uint64_t g_pool[3] = {};

void* allocate(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    g_alloc.sizes.push_back(size);
    check("e19440 alloc tag", tag == 1);
    check("e19440 alloc align", align == 8);
    check("e19440 alloc flags", flags == 1);
    void* pointer = &g_pool[g_alloc.sizes.size() - 1];
    g_alloc.nodes.push_back(pointer);
    return pointer;
}

std::int32_t dispatch_stub(void* target, void* node) {
    check("e19440 dispatch target", target == reinterpret_cast<void*>(0x1234));
    g_alloc.nodes.push_back(node);
    return 5;
}

void test_binding_node_dispatch() {
    g_alloc = AllocTrace{};
    g_pool[0] = g_pool[1] = g_pool[2] = 0;

    const Vtable15e1ee0Slot10_e19440Result null_target =
        sotc::vtable_15e1ee0_slot10_e19440();
    check("e19440 null target fails", !null_target.ok);

    Vtable15e1ee0Slot10_e19440Cells cells{};
    cells.target = reinterpret_cast<void*>(0x1234);
    const Vtable15e1ee0Slot10_e19440Result no_contract =
        sotc::vtable_15e1ee0_slot10_e19440(cells);
    check("e19440 no contract fails", !no_contract.ok);

    AllocDispatchContract contract{};
    contract.allocate = allocate;
    Vtable15e1ee0Slot10_e19440Hooks hooks{};
    hooks.alloc = &contract;
    hooks.dispatch_slot10 = dispatch_stub;
    const Vtable15e1ee0Slot10_e19440Result result =
        sotc::vtable_15e1ee0_slot10_e19440(cells, hooks);

    check("e19440 ok", result.ok);
    check("e19440 three allocs", g_alloc.sizes.size() == 3);
    check("e19440 alloc size", g_alloc.sizes.size() == 3 && g_alloc.sizes[0] == 8);
    check("e19440 stamped", result.stamped);
    check("e19440 node a stamp", g_pool[0] == 0x15e1f30);
    check("e19440 node b stamp", g_pool[1] == 0x15e1f88);
    check("e19440 node c stamp", g_pool[2] == 0x15e1fe0);
    check("e19440 two calls", result.calls == 2);
    check("e19440 tail", result.tail_dispatched);
    check("e19440 code", result.code == 5);
    check("e19440 dispatch order", g_alloc.nodes.size() == 6 &&
                                        g_alloc.nodes[0] == &g_pool[0] &&
                                        g_alloc.nodes[1] == &g_pool[1] &&
                                        g_alloc.nodes[2] == &g_pool[2] &&
                                        g_alloc.nodes[3] == &g_pool[0] &&
                                        g_alloc.nodes[4] == &g_pool[1] &&
                                        g_alloc.nodes[5] == &g_pool[2]);
}

}  // namespace

int main() {
    test_constants();
    test_name_id_lookup();
    test_max_id_scan();
    test_pre_call_free();
    test_binding_node_dispatch();

    if (failures == 0) {
        std::printf("OK test_vtable_containers_e1\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_containers_e1: %d failure(s)\n", failures);
    return 1;
}
