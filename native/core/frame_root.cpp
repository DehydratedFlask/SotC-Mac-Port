#include "frame_root.hpp"

#include <utility>

namespace sotc {

ReportItem* FrameRoot::node(std::int32_t frame_num) {
    if (node_ == nullptr) {
        node_ = report_node_new(kFrameRootType);
        node_->set_int_property(kFrameRootNumKey, frame_num);
    }
    return node_.get();
}

ReportItem* FrameRoot::adopt(std::unique_ptr<ReportItem> root, std::int32_t frame_num) {
    if (root == nullptr) {
        return nullptr;  // native guard: do not create a Frame node for a null root
    }
    ReportItem* const frame = node(frame_num);
    return frame == nullptr ? nullptr : frame->append(std::move(root));
}

}  // namespace sotc
