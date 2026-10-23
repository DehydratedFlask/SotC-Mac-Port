#include "report_tree.hpp"

#include <utility>

namespace sotc {

ReportItem::ReportItem(std::string text, ReportKind kind)
    : text_(std::move(text)), kind_(kind) {}

ReportItem::~ReportItem() = default;

void ReportItem::set_property(std::string key, std::string value) {
    // The original compares key bytes (`0xf90150` memcmp) and overwrites an existing entry.
    properties_.insert_or_assign(std::move(key), std::move(value));
}

const std::string* ReportItem::property(std::string_view key) const {
    const auto it = properties_.find(key);
    return it == properties_.end() ? nullptr : &it->second;
}

ReportItem* ReportItem::append(std::unique_ptr<ReportItem> child) {
    if (child == nullptr) {
        return nullptr;
    }
    ReportItem* const raw = child.get();
    raw->parent_ = this;
    raw->prev_sibling_ = last_child_;
    raw->next_sibling_ = nullptr;
    if (last_child_ != nullptr) {
        last_child_->next_sibling_ = raw;
    } else {
        first_child_ = raw;
    }
    last_child_ = raw;
    children_.push_back(std::move(child));
    return raw;
}

std::unique_ptr<ReportItem> report_node_new(std::string type_name) {
    return std::make_unique<ReportItem>(std::move(type_name), ReportKind::kNode);
}

std::unique_ptr<ReportItem> report_value_new(std::string text) {
    return std::make_unique<ReportItem>(std::move(text), ReportKind::kStringValue);
}

ReportItem* report_link_child(ReportItem& parent, std::unique_ptr<ReportItem> child) {
    return parent.append(std::move(child));
}

void report_set_property(ReportItem& node, std::string key, std::string value) {
    node.set_property(std::move(key), std::move(value));
}

}  // namespace sotc
