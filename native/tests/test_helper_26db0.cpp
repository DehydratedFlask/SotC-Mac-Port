// CPU-01-HELPER-CALLEE-MODELS-01: fixture for the class-aware 0x26db0 front end (named debug-menu item).
// ELF pins: tools/lift/check_helper_26db0.py. FIXTURE_VERIFIED.
#include <bit>

#include "core/helper_26db0_named_item.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

namespace {

bool same_view(const SharedStr& a, const SharedStr& b) {
    return a.data == b.data && a.size == b.size && a.aux == b.aux && a.ctl == b.ctl;
}

void check_item(const DebugMenuItem& item, const SharedStr& parent, const SharedStr& caption, const void* target,
                float lo, float hi, std::uint32_t flags) {
    CHECK(item.link.self == nullptr && item.link.prev == &item.link && item.link.next == &item.link);
    CHECK(item.link.owner == nullptr && item.registered == 0);
    CHECK(same_view(item.label, parent) && same_view(item.text, caption));
    CHECK(item.kind == kDebugMenuItemKind && item.target == target && item.flags == flags);
    CHECK(std::bit_cast<std::uint32_t>(item.range_min) == std::bit_cast<std::uint32_t>(lo));
    CHECK(std::bit_cast<std::uint32_t>(item.range_max) == std::bit_cast<std::uint32_t>(hi));
}

}  // namespace

int main() {
    const SharedStr parent = view("Shadow Misc");
    const SharedStr caption = view("gCaption");

    // bool: {1, 0} -> {1, 10}
    {
        ConCmdBoolVar v;
        poison(v);
        v.base.vptr = kBoolVarFinalVptr;
        v.base.name = caption;
        DebugMenuItem item;
        poison(item);
        CHECK(helper_26db0_named_item_bool(item, parent, v, 0x30) == HelperRefusal::none);
        check_item(item, parent, caption, &v, 1.0f, 10.0f, 0x30);
    }
    // int: {1, 2} -> {1, 10}
    {
        ConCmdRangedVar v;
        poison(v);
        v.base.vptr = kRangedIntFinalVptr;
        v.base.name = caption;
        DebugMenuItem item;
        poison(item);
        CHECK(helper_26db0_named_item_int(item, parent, v, 0x31) == HelperRefusal::none);
        check_item(item, parent, caption, &v, 1.0f, 10.0f, 0x31);
    }
    // float: {1, 3} -> the one narrowing case {0.1, 0.5}
    {
        ConCmdRangedVar v;
        poison(v);
        v.base.vptr = kRangedFloatFinalVptr;
        v.base.name = caption;
        DebugMenuItem item;
        poison(item);
        CHECK(helper_26db0_named_item_float(item, parent, v, 0x32) == HelperRefusal::none);
        check_item(item, parent, caption, &v, 0.1f, 0.5f, 0x32);
    }
    // string: {1, 4} -> {1, 10}
    {
        ConCmdStringVar v;
        poison(v);
        v.base.vptr = kStringVarFinalVptr;
        v.base.name = caption;
        DebugMenuItem item;
        poison(item);
        CHECK(helper_26db0_named_item_string(item, parent, v, 0x33) == HelperRefusal::none);
        check_item(item, parent, caption, &v, 1.0f, 10.0f, 0x33);
    }
    // wrong or unlifted class: refused, item untouched
    {
        ConCmdRangedVar v;
        poison(v);
        v.base.name = caption;
        DebugMenuItem item;
        poison(item);
        v.base.vptr = kRangedIntFinalVptr;   // an int variable offered as a float target
        CHECK(helper_26db0_named_item_float(item, parent, v, 1) == HelperRefusal::unknown_target_class);
        CHECK(all_poison(item));
        v.base.vptr = kRangedFloatFinalVptr;  // and the reverse
        CHECK(helper_26db0_named_item_int(item, parent, v, 1) == HelperRefusal::unknown_target_class);
        CHECK(all_poison(item));
        v.base.vptr = kRangedIntMidVptr;      // a half-built object (mid vptr) is not a lifted final class
        CHECK(helper_26db0_named_item_int(item, parent, v, 1) == HelperRefusal::unknown_target_class);
        CHECK(all_poison(item));
        ConCmdBoolVar b;
        poison(b);
        b.base.vptr = 0;
        CHECK(helper_26db0_named_item_bool(item, parent, b, 1) == HelperRefusal::unknown_target_class);
        CHECK(all_poison(item));
    }
    std::puts("test_helper_26db0: ok");
    return 0;
}
