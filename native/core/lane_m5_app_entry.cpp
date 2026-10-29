#include "lane_m5_app_entry.hpp"

namespace sotc::lane_m5 {

EntryRegisterResult register_entry_4b560(AppInfoQueueEntry& entry, AppInfoQueue& queue) noexcept {
    EntryRegisterResult out{};
    if (entry.registered != 0) return out;                  // 0x4b574: cmp byte [r12],0; jne -> ret
    entry.registered = 1;                                   // 0x4b586
    if (queue.singleton != nullptr) {
        out.path = EntryRegisterPath::singleton_present;    // tree lookup / insert_unique: not modelled
        return out;
    }
    entry.next = queue.head;                                // 0x4b5ec..0x4b637: [entry+0x28] = old head
    queue.head = &entry;                                    //                   [0x2ea8980]  = entry
    out.path = EntryRegisterPath::queued;
    out.ok = true;
    return out;
}

NamedDelegateResult init_named_delegate_entry(AppInfoQueueEntry& entry, const NamedDelegateSpec& spec,
                                              AppInfoQueue& queue, StartupAtexitLog& atexit_log) noexcept {
    NamedDelegateResult out{};
    if (spec.handler_tag == 0) return out;                  // the delegate self-pointer rule is not modelled for 0
    entry.registered = 0;                                   // mov byte [entry], 0
    entry.name = spec.name;                                 // 32 B copy ...
    concmd_acquire_view(entry.name);                        // ... `lock inc [ctl]` when ctl != 0
    entry.next = nullptr;                                   // xmm zero store at +0x28
    entry.delegate.object = reinterpret_cast<std::uint64_t>(&entry.delegate);
    entry.delegate.fn = kDelegateInvokerRva;                // value of the relocated cell 0x15fe800
    entry.delegate.adj = 0;
    entry.delegate.handler = spec.handler_tag;
    out.registration = register_entry_4b560(entry, queue);  // call 0x4b560
    if (!out.registration.ok) return out;
    out.view_inert = release_inert_view_f590(spec.name);    // call 0xf590 on the stack view
    if (!out.view_inert) return out;
    out.atexit_recorded = atexit_log.record(kEntryDtorRva, &entry);
    out.ok = out.atexit_recorded;
    return out;
}

}  // namespace sotc::lane_m5
