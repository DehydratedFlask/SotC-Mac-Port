// Native reduction of the bug-data report object model (E575).
//
// Recovered shapes (all byte-proven in `local/re/evidence/appinfo-listb-report/report.md`):
//   - report node `0x1a0` B (vtable address point `0x15d4ac0`): base item `0xc8` + property map
//     `0xd8`; allocated by the caller through `0x17070` (tag 1), ctor `0xab1b20`;
//   - value object `0xd0` B (vtable address point `0x15d4ce8`): base item + byte `+0xc8`, ctor
//     `0xab0ad0`, item kind `4`;
//   - base item `0xc8` B: parent `+0x70`, kind `+0x78` (1 = node, 4 = string value), first/last
//     child `+0x80`/`+0x88`, text `+0x90`, prev/next sibling `+0xb8`/`+0xc0`;
//   - property map `0xd8` B: vptr `+0x00`, tree sentinel `+0x198`, key `+0x78`, value `+0xa0`;
//     `0xab2ac0` copies both key and value and is last-write-wins by byte-wise compare.
//
// The native model preserves the observable behaviour and ownership rules, not the guest ABI:
// text is copied into `std::string` (the original deep-copies through `0x1beaa0`), children are
// owned by their parent (the original links raw pointers and destroys them from the parent's
// destructor), and the property map is last-write-wins. It must not borrow registry text: the
// source shared strings are released by the very next list phase.
//
// NOT modelled: the four vtables (the only virtual call in the slice is the deleting destructor),
// the guest SSO layout, the map's balancing rule, and the unproduced `+0x38`/`+0x100`/`+0x148`/
// `+0x170` fields. G1 PARTIAL.
#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace sotc {

// `+0x78` item discriminator. The names are native; the values are recovered.
enum class ReportKind : std::int32_t {
    kNode = 1,         // `0xab1b97`
    kStringValue = 4,  // `0xab0ad0`, called with esi = 4 at `0x1be35d`
};

class ReportItem {
public:
    ReportItem(std::string text, ReportKind kind);

    ReportItem(const ReportItem&) = delete;
    ReportItem& operator=(const ReportItem&) = delete;
    ReportItem(ReportItem&&) = delete;
    ReportItem& operator=(ReportItem&&) = delete;
    ~ReportItem();

    // `+0x90` text: the type name on a node, the value text on a string value.
    [[nodiscard]] const std::string& text() const noexcept { return text_; }
    [[nodiscard]] ReportKind kind() const noexcept { return kind_; }

    // `0xab2ac0` semantics: copy the key and the value; a repeated key overwrites the old value.
    void set_property(std::string key, std::string value);
    [[nodiscard]] const std::string* property(std::string_view key) const;
    [[nodiscard]] std::size_t property_count() const noexcept { return properties_.size(); }

    // `0xab0d40`: O(1) tail append, ownership of `child` transfers to this item.
    [[nodiscard]] ReportItem* append(std::unique_ptr<ReportItem> child);

    [[nodiscard]] ReportItem* parent() const noexcept { return parent_; }
    [[nodiscard]] ReportItem* first_child() const noexcept { return first_child_; }
    [[nodiscard]] ReportItem* last_child() const noexcept { return last_child_; }
    [[nodiscard]] ReportItem* prev_sibling() const noexcept { return prev_sibling_; }
    [[nodiscard]] ReportItem* next_sibling() const noexcept { return next_sibling_; }
    [[nodiscard]] std::size_t child_count() const noexcept { return children_.size(); }

private:
    std::string text_;
    ReportKind kind_;
    std::map<std::string, std::string, std::less<>> properties_;
    ReportItem* parent_ = nullptr;
    ReportItem* first_child_ = nullptr;
    ReportItem* last_child_ = nullptr;
    ReportItem* prev_sibling_ = nullptr;
    ReportItem* next_sibling_ = nullptr;
    std::vector<std::unique_ptr<ReportItem>> children_;
};

// `0x17070(0x1a0)` + `0xab1b20`: a node whose text is the type name.
[[nodiscard]] std::unique_ptr<ReportItem> report_node_new(std::string type_name);

// `0x17070(0xd0)` + `0xab0ad0`: a string value whose text is the copied value.
[[nodiscard]] std::unique_ptr<ReportItem> report_value_new(std::string text);

// `0xab0d40(parent, child)`.
[[nodiscard]] ReportItem* report_link_child(ReportItem& parent, std::unique_ptr<ReportItem> child);

// `0xab2ac0(node, key, value)`.
void report_set_property(ReportItem& node, std::string key, std::string value);

}  // namespace sotc
