#include "vtable_15e2028_slot2a0_e21420_tail.hpp"

#include <cstring>

namespace sotc {

const char* e21420_tail_status_name(E21420TailStatus status) noexcept {
    switch (status) {
        case E21420TailStatus::ok:
            return "ok";
        case E21420TailStatus::no_provider:
            return "no_provider";
        case E21420TailStatus::lookup_missing:
            return "lookup_missing";
        case E21420TailStatus::tail_missing:
            return "tail_missing";
    }
    return "unknown";
}

E21420FoundResult e21420_found(const E21420FoundCells& cells,
                               const E21420FoundHooks& hooks) noexcept {
    E21420FoundResult out{};
    if (cells.provider == nullptr || hooks.lookup == nullptr) {
        out.status = E21420TailStatus::no_provider;
        out.code = kE21420StatusMissing;
        return out;
    }
    out.lookup_called = true;
    void* object = hooks.lookup(cells.provider, cells.name);
    if (object == nullptr) {
        // `r15 = 0x1b` then `test rax,rax; je` back to the shared epilogue.
        out.status = E21420TailStatus::lookup_missing;
        out.code = kE21420StatusMissing;
        out.ok = true;
        return out;
    }
    if (hooks.tail == nullptr) {
        out.status = E21420TailStatus::tail_missing;
        out.code = kE21420StatusMissing;
        return out;
    }
    out.tail_called = true;
    out.code = hooks.tail(cells.context, cells.arg2, object, cells.arg4_byte);
    out.status = E21420TailStatus::ok;
    out.ok = true;
    return out;
}

E21420MissingResult e21420_missing(const E21420MissingCells& cells,
                                   const E21420MissingHooks& hooks) noexcept {
    E21420MissingResult out{};
    if (cells.provider == nullptr || cells.record == nullptr || hooks.allocate == nullptr) {
        out.status = E21420TailStatus::no_provider;
        return out;
    }

    void* record = hooks.allocate(cells.provider, kE21420AllocationSize);
    if (record == nullptr) {
        // The retail body computes the stores only on the non-null path but keeps r15 = 0x1b.
        out.status = E21420TailStatus::ok;
        out.ok = true;
        out.code = kE21420StatusMissing;
        return out;
    }

    // The record is constructed in the caller's storage so the fixture can inspect it byte for byte.
    std::uint8_t* bytes = static_cast<std::uint8_t*>(cells.record);
    std::memset(bytes, 0, kE21420AllocationSize);
    const std::uint64_t vtable = kE21420MissingVtable;
    const std::uint64_t owner = reinterpret_cast<std::uint64_t>(cells.provider);
    std::memcpy(bytes + kE21420RecordVtable, &vtable, sizeof(vtable));
    std::memcpy(bytes + kE21420RecordOwner, &owner, sizeof(owner));
    // The body zeroes 32 bytes at +0x10 (`vmovups %ymm0`) and 16 at +0x30 (`vmovups %xmm0`):
    // the second store is an xmm width, not a ymm one, so the record stays inside 0x48 bytes.
    std::memset(bytes + kE21420RecordZeroA, 0, 0x20);
    std::memset(bytes + kE21420RecordZeroB, 0, 0x10);
    bytes[kE21420RecordFlags] = 1;
    bytes[kE21420RecordFlags + 1] = 1;
    bytes[kE21420RecordFlags + 2] = 1;
    out.record_constructed = true;
    out.code = kE21420StatusMissing;

    if (cells.name == nullptr) {
        // `testq %r12,%r12; je 0xe215ce` — a distinct sub-tree, reported rather than run.
        out.null_name_subtree = true;
        if (hooks.null_name_subtree != nullptr) {
            (void)hooks.null_name_subtree(record);
        }
        out.status = E21420TailStatus::ok;
        out.ok = true;
        return out;
    }

    // Second scan: provider->slot78() elements compared with strcmp against the name.
    if (hooks.count == nullptr || hooks.name_at == nullptr) {
        out.status = E21420TailStatus::no_provider;
        return out;
    }
    const std::int32_t count = hooks.count(cells.provider);
    for (std::int32_t index = 0; index < count; ++index) {
        ++out.scanned;
        const char* candidate = hooks.name_at(cells.provider, index);
        if (candidate != nullptr && std::strcmp(candidate, cells.name) == 0) {
            out.rescan_matched = true;
            break;
        }
    }
    if (!out.rescan_matched) {
        // Falls through to `provider->vtable[0xc0](record)` with r15 = 0x14.
        if (hooks.release != nullptr) {
            hooks.release(record);
            out.release_called = true;
        }
        out.code = kE21420StatusScanExhausted;
    }
    out.status = E21420TailStatus::ok;
    out.ok = true;
    return out;
}

}  // namespace sotc
