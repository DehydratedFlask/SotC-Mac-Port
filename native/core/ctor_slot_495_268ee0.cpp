// CPU-01-AOT-CFG-SCALE-01-BATCH25 (slot 495): see ctor_slot_495_268ee0.hpp.
#include "ctor_slot_495_268ee0.hpp"

#include <cstring>

namespace sotc {

Slot495Result init_slot495_268ee0(Slot495Objects& objects, ConCmdRegistry& registry,
                                  const BlobHooks& blob_hooks) noexcept {
    Slot495Result out{};

    // ---- 1. command object 0x2eeebb0 (0x268ef1..0x269003) -----------------------------------
    const SharedStr name = family_view("network_AllowCrossPlatformPlay", kSlot495NameLength);
    const SharedStr description = family_view("", 0);
    init_concmd_base_20af0(objects.command.base, name, description, kSlot495Flags);
    objects.command.base.vptr = kSlot495Vptr;
    objects.command.tail.region = reinterpret_cast<std::uint64_t>(objects.region60);
    objects.command.tail.flag69 = 0;
    std::memset(objects.command.tail.zero70, 0, sizeof objects.command.tail.zero70);
    objects.command.tail.zeroA0 = 0;
    objects.command.tail.tagB0 = kSlot495TagB0Value;
    objects.command.tail.zeroB8 = 0;
    if (!release_inert_view_f590(description)) return out;
    const ConCmdRegisterResult registered = register_concmd_20d60(objects.command.base, registry);
    if (!registered.ok) return out;
    if (!release_inert_view_f590(name)) return out;
    if (!registry.atexit_log.record(kSlot495CommandDtor, &objects.command)) return out;
    ++out.atexit_records;

    // ---- 2. parsed blob record 0x2eeeca8 (0x269008..0x26902b) -------------------------------
    const BlobParseResult parsed =
        parse_blob_a38960(objects.blob, kSlot495BlobDwords, kSlot495BlobDwordCount, objects.arena,
                          blob_hooks);
    if (!parsed.ok) return out;
    out.blob_parsed = true;
    if (!registry.atexit_log.record(kSlot495BlobDtor, &objects.blob)) return out;
    ++out.atexit_records;

    // ---- 3. twelve items at 0x2eeecd8 + 0x48*i (0x269030..0x26955e) -------------------------
    for (std::size_t i = 0; i < kSlot495ItemCount; ++i) {
        const Slot495ItemSpec& spec = kSlot495Items[i];
        const SharedStr view = family_view(spec.text, spec.length);
        Slot495Item& item = objects.items[i];
        item.text = view;
        item.zero20 = 0;
        item.zero30 = 0;
        item.zero38 = 0;
        item.zero40 = 0;
        if (!release_inert_view_f590(view)) return out;
        if (!registry.atexit_log.record(kSlot495ItemDtor, &item)) return out;
        ++out.atexit_records;
        ++out.items;
    }

    // The original's canary compare has no native counterpart; the two guard words are the same
    // local, so the compare is satisfied by construction.
    out.canary_matched = true;
    out.ok = out.items == kSlot495ItemCount && out.atexit_records == 14 && out.blob_parsed &&
             objects.blob.valid != 0 && objects.blob.a == kSlot495BlobA &&
             objects.blob.size == kSlot495BlobSize && objects.blob.bits == kSlot495BlobBits;
    return out;
}

}  // namespace sotc
