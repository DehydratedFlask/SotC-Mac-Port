#include "ctor_slot_303_a82170.hpp"

namespace sotc {

namespace {

// The original's empty description points at a NUL byte inside rodata (0x12c6bef); the native view
// points at its own NUL. Borrowed view: ctl == nullptr, so no counter is ever touched.
constexpr char kEmpty[] = "";

template <std::size_t N>
SharedStr borrowed(const char (&text)[N]) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(N - 1), 0,
                     nullptr};
}

}  // namespace

Slot303Result init_slot303_a82170(Slot303Objects& objects, ConCmdRegistry& concmd,
                                  DebugMenuRegistry& menu) noexcept {
    Slot303Result out{};

    // ---- A: ProfileCPU (0xa82188..0xa82292) ------------------------------------------------------
    const SharedStr name = borrowed(kSlot303Name);
    const SharedStr description{reinterpret_cast<const std::uint8_t*>(kEmpty), 0, 0, nullptr};
    out.var = init_bool_var_static(objects.var, name, description, kSlot303FnVtbl, concmd,
                                   kSlot303Value, kSlot303Default);

    // ---- B: debug-menu toggle (0xa82297..0xa8230e) -----------------------------------------------
    const SharedStr label = borrowed(kSlot303Label);
    const SharedStr text = borrowed(kSlot303Text);
    const DebugMenuTarget target = debug_menu_target_for(objects.var);
    out.item_ctor_ok = init_debug_menu_item_26ea0(objects.item, label, text, target, kSlot303ItemFlags);
    if (out.item_ctor_ok) {
        out.item_registration = register_debug_menu_item_268b0(objects.item, menu, concmd.atexit_log);
        out.item_views_inert = release_inert_view_f590(text) && release_inert_view_f590(label);  // 0xa822f4 / 0xa822fc
        out.item_atexit_recorded = concmd.atexit_log.record(kDebugMenuItemDtorRva, &objects.item);  // 0xa8230e
    }

    out.ok = out.var.ok && out.item_ctor_ok && out.item_registration.ok && out.item_views_inert &&
             out.item_atexit_recorded;
    return out;
}

}  // namespace sotc
