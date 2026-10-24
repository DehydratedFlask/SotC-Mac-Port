// Fixture for the bug-report Frame root: lazy creation, frameNum, adoption and ownership.
#include <cstdio>
#include <cstdlib>
#include <memory>

#include "core/frame_root.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

int main() {
    CHECK(kFrameRootGetRva == 0xa70c30);
    CHECK(kFrameRootGetSize == 220);
    CHECK(kFrameRootAdoptRva == 0xa70d10);
    CHECK(kFrameRootAdoptSize == 32);
    CHECK(kFrameRootAppendRva == 0xab0d40);

    // ---- 1. Lazy creation and frameNum. ----
    {
        FrameRoot root;
        CHECK(root.get() == nullptr);
        ReportItem* const frame = root.node(7);
        CHECK(frame != nullptr);
        CHECK(frame->text() == "Frame");
        CHECK(frame->int_property("frameNum") != nullptr && *frame->int_property("frameNum") == 7);
        CHECK(frame->int_property_count() == 1);
        CHECK(frame->child_count() == 0);
        CHECK(root.node(99) == frame);  // same node; frameNum only on creation
        CHECK(*frame->int_property("frameNum") == 7);
    }

    // ---- 2. Adoption transfers ownership and appends in order. ----
    {
        FrameRoot root;
        ReportItem* const first = root.adopt(report_node_new("BugData"), 3);
        CHECK(first != nullptr);
        CHECK(first->parent() == root.get());
        CHECK(root.get()->text() == "Frame");
        CHECK(*root.get()->int_property("frameNum") == 3);
        CHECK(root.get()->child_count() == 1);

        ReportItem* const second = root.adopt(report_node_new("Second"), 99);
        CHECK(second != nullptr);
        CHECK(second->parent() == root.get());
        CHECK(root.get()->child_count() == 2);
        CHECK(root.get()->first_child() == first);
        CHECK(root.get()->last_child() == second);
        CHECK(first->next_sibling() == second);
        CHECK(*root.get()->int_property("frameNum") == 3);  // unchanged
    }

    // ---- 3. A null root is rejected by append. ----
    {
        FrameRoot root;
        CHECK(root.adopt(nullptr, 1) == nullptr);
        CHECK(root.get() == nullptr);  // no Frame node was created
    }

    return 0;
}
