// CPU-01-STARTUP-SLOT-552: see ctor_slot_552_8c880.hpp.
#include "ctor_slot_552_8c880.hpp"

namespace sotc {

const char* slot552_stage_name(Slot552Stage stage) noexcept {
    switch (stage) {
        case Slot552Stage::none: return "none";
        case Slot552Stage::flags: return "flags";
        case Slot552Stage::records: return "records";
        case Slot552Stage::page_ctor: return "page_ctor";
        case Slot552Stage::page_stores: return "page_stores";
        case Slot552Stage::page_atexit: return "page_atexit";
    }
    return "";
}

const char* slot552_page_name(std::uint32_t page) noexcept {
    switch (page) {
        case 0: return kSlot552Name0;
        case 1: return kSlot552Name1;
        case 2: return kSlot552Name2;
        case 3: return kSlot552Name3;
        case 4: return kSlot552Name4;
        case 5: return kSlot552Name5;
        case 6: return kSlot552Name6;
        case 7: return kSlot552Name7;
        default: return "";
    }
}

std::size_t slot552_page_name_len(std::uint32_t page) noexcept {
    switch (page) {
        case 0: return sizeof kSlot552Name0 - 1;
        case 1: return sizeof kSlot552Name1 - 1;
        case 2: return sizeof kSlot552Name2 - 1;
        case 3: return sizeof kSlot552Name3 - 1;
        case 4: return sizeof kSlot552Name4 - 1;
        case 5: return sizeof kSlot552Name5 - 1;
        case 6: return sizeof kSlot552Name6 - 1;
        case 7: return sizeof kSlot552Name7 - 1;
        default: return 0;
    }
}

namespace {

// 0x475c0: the 87-byte page base ctor. Every store of the original is reproduced; the fields it
// never touches (gap0a, gap5c, gap68, gap78) are left exactly as the caller supplied them.
void page_base_475c0(Slot552PageBase& base, const SharedStr& name) noexcept {
    base.vptr = kSlot552PageCtorVptr;                      // 0x475c0 lea 0x15958f0 + 0x10
    base.byte08 = 0;                                       // 0x475d6 movb 0,[rdi+8]
    base.byte09 = 1;                                       // 0x475da movb 1,[rdi+9]
    base.name = reinterpret_cast<std::uint64_t>(name.data);  // 0x475de mov [rdi+0x10],rsi
    base.word18 = 0;                                       // 0x475e2 mov [rdi+0x18],edx (0)
    std::memset(base.zero1c, 0, sizeof base.zero1c);       // 0x475e5 vmovups [rdi+0x1c],ymm0
    base.word3c = 0;                                       // 0x475ea movl $0,[rdi+0x3c]
    const auto self = reinterpret_cast<std::uint64_t>(&base);
    base.link40 = self + 0x38;                             // 0x475f1 [rdi+0x40] = rdi+0x38
    base.link48 = self + 0x38;                             // 0x475f5 [rdi+0x48] = rdi+0x38
    base.word50 = 0;                                       // 0x475f9
    base.word58 = 0;                                       // 0x47601
    base.word60 = 0;                                       // 0x4760a xchg [rdi+0x60],rax (rax = 0)
    base.word70 = 0;                                       // 0x4760e
}

bool exact_borrowed_name(const SharedStr& name, std::uint32_t page) noexcept {
    if (name.data == nullptr || name.ctl != nullptr) return false;
    const std::size_t len = slot552_page_name_len(page);
    if (name.size != static_cast<std::int64_t>(len)) return false;
    return std::memcmp(name.data, slot552_page_name(page), len) == 0;
}

bool in_range(std::size_t value, std::size_t begin, std::size_t end) noexcept {
    return value >= begin && value < end;
}

}  // namespace

Slot552PageResult init_slot552_page_475c0(void* page, const SharedStr& name) noexcept {
    Slot552PageResult out{};
    if (page == nullptr || name.data == nullptr || name.ctl != nullptr) return out;
    page_base_475c0(*static_cast<Slot552PageBase*>(page), name);
    out.ok = true;
    return out;
}

Slot552Hooks default_slot552_hooks() noexcept {
    Slot552Hooks hooks{};
    hooks.page_ctor = &init_slot552_page_475c0;
    return hooks;
}

void init_slot552_prefix(Slot552Objects& objects) noexcept {
    // 0x8c8b0..0x8c8f7: seven dwords set to 1, in this order: +0x04, +0x08, +0x00, +0x0c, +0x10,
    // +0x14, +0x18. The order does not change the result, and every one of the seven is written.
    for (std::uint32_t& flag : objects.flags.value) flag = 1;
    // 0x8c8fd..0x8ca2a: seventeen records, each a dword 0 followed by sixteen zero bytes.
    for (Slot552Record& record : objects.records) {
        record.word = 0;
        std::memset(record.zero8, 0, sizeof record.zero8);
    }
}

void init_slot552_page_a_stores(Slot552PageA& page) noexcept {
    // 0x8ca34/0x8ca6c: the derived vptr.
    page.base.vptr = kSlot552PageAVptr;
    // 0x8ca73 qword [page+0x80] = 0; 0x8ca7e dword [page+0x88] = 0; 0x8ca88 ymm [page+0x90] = 0.
    page.word80 = 0;
    page.word88 = 0;
    std::memset(page.zero90, 0, sizeof page.zero90);
    // 0x8ca90..0x8cbfe: twelve self-referential nodes. Each is followed by a 32-byte zero store at
    // node+0x20 that the next node's own {next, prev} store overwrites; nodes 0..10 therefore keep
    // their sixteen zero bytes and the page's last sixteen bytes are never written.
    for (std::size_t i = 0; i < page.nodes.size(); ++i) {
        Slot552Node& node = page.nodes[i];
        node.next = &node;
        node.prev = &node;
        std::memset(node.zero20, 0, sizeof node.zero20);
    }
    Slot552NodeTail& last = page.last_node;
    last.next = &last;
    last.prev = &last;
}

void init_slot552_page_b_stores(Slot552PageB& page) noexcept {
    // 0x8d550/0x8d568: the derived vptr.
    page.base.vptr = kSlot552PageBVptr;
    // 0x8d576 qword [page+0xa8] = 0; 0x8d581..0x8d591 ymm [page+0x80] = {p,p,p,p}; 0x8d59d
    // qword [page+0xa0] = p, where p = page + 0x80. 0x8d5a4/0x8d5ac zero +0xd0 and +0xb8.
    const auto head = reinterpret_cast<std::uint64_t>(&page) + 0x80;
    for (std::uint64_t& slot : page.head) slot = head;
    page.word_a8 = 0;
    std::memset(page.zero_b8, 0, sizeof page.zero_b8);
}

void init_slot552_page_c_stores(Slot552PageC& page) noexcept {
    // 0x8d5f0: the derived vptr; 0x8d5f7 xmm [page+0x80] = 0; 0x8d5ff qword [page+0x90] = 0.
    page.base.vptr = kSlot552PageCVptr;
    std::memset(page.zero80, 0, sizeof page.zero80);
    page.word90 = 0;
}

bool slot552_byte_written(std::size_t offset) noexcept {
    if (offset >= kSlot552BlockSize) return false;
    if (offset < 0x1c) return true;                       // the seven flag dwords
    if (offset < 0x20) return false;                      // 0x2ed1f64
    if (offset < 0x1b8) {                                 // the seventeen 0x18-byte records
        const std::size_t index = (offset - 0x20) / kSlot552RecordStride;
        const std::size_t within = (offset - 0x20) % kSlot552RecordStride;
        return index < kSlot552RecordCount && (within < 4 || within >= 8);
    }
    if (offset < 0x1d8) return false;                     // 0x2ed2100..0x2ed2120
    if (offset < 0x1318) {                                // the six class-A pages
        const std::size_t within = (offset - 0x1d8) % kSlot552PageASize;
        if (in_range(within, 0x0a, 0x10) || in_range(within, 0x5c, 0x60) ||
            in_range(within, 0x68, 0x70) || in_range(within, 0x78, 0x80) ||
            in_range(within, 0x8c, 0x90) || in_range(within, 0x2d0, 0x2e0)) return false;
        // Each full node is {next, prev} followed by sixteen bytes the original never writes.
        if (within >= kSlot552NodesOffset && within < 0x2d0) {
            const std::size_t node_offset = (within - kSlot552NodesOffset) % kSlot552NodeStride;
            if (node_offset >= 0x10 && node_offset < 0x20) return false;
        }
        return true;
    }
    if (offset < 0x1408) {                                // class-B page
        const std::size_t within = offset - 0x1318;
        if (in_range(within, 0x0a, 0x10) || in_range(within, 0x5c, 0x60) ||
            in_range(within, 0x68, 0x70) || in_range(within, 0x78, 0x80) ||
            in_range(within, 0xb0, 0xb8)) return false;
        return true;
    }
    const std::size_t within = offset - 0x1408;           // class-C page
    if (in_range(within, 0x0a, 0x10) || in_range(within, 0x5c, 0x60) ||
        in_range(within, 0x68, 0x70) || in_range(within, 0x78, 0x80)) return false;
    return true;
}

Slot552Result init_slot552_8c880(Slot552Objects& objects, ConCmdRegistry& concmd,
                                 const Slot552Hooks& hooks) noexcept {
    Slot552Result out{};

    // 0x8c8b0..0x8ca2a: the flags and the zeroed records precede the first page ctor.
    out.blocked = Slot552Stage::flags;
    init_slot552_prefix(objects);
    out.flags_written = kSlot552FlagCount;
    out.records_written = kSlot552RecordCount;

    out.blocked = Slot552Stage::page_ctor;
    for (std::uint32_t page = 0; page < kSlot552PageACount; ++page) {
        out.blocked_page = page;
        Slot552PageA& target = objects.pages_a[page];
        if (hooks.page_ctor == nullptr) return out;
        const SharedStr name = l2::measured_view(slot552_page_name(page), slot552_page_name_len(page));
        if (!exact_borrowed_name(name, page)) {
            out.names_borrowed = false;
            return out;
        }
        const Slot552PageResult built = hooks.page_ctor(&target.base, name);
        if (!built.ok) return out;

        out.blocked = Slot552Stage::page_stores;
        init_slot552_page_a_stores(target);

        out.blocked = Slot552Stage::page_atexit;
        if (!concmd.atexit_log.record(kSlot552PageADtorRva, &target)) return out;
        ++out.atexit_records;
        ++out.pages_built;
        out.blocked = Slot552Stage::page_ctor;
    }

    // Page B (0x2ed3260): ctor, stores, atexit at 0x8d5b4.
    out.blocked = Slot552Stage::page_ctor;
    out.blocked_page = 6;
    if (hooks.page_ctor == nullptr) return out;
    {
        const SharedStr name = l2::measured_view(slot552_page_name(6), slot552_page_name_len(6));
        if (!exact_borrowed_name(name, 6)) {
            out.names_borrowed = false;
            return out;
        }
        const Slot552PageResult built = hooks.page_ctor(&objects.page_b.base, name);
        if (!built.ok) return out;
        out.blocked = Slot552Stage::page_stores;
        init_slot552_page_b_stores(objects.page_b);
        out.blocked = Slot552Stage::page_atexit;
        if (!concmd.atexit_log.record(kSlot552PageBDtorRva, &objects.page_b)) return out;
        ++out.atexit_records;
        ++out.pages_built;
    }

    // Page C (0x2ed3350): ctor, stores, then the tail `jmp __cxa_atexit` at 0x8d612.
    out.blocked = Slot552Stage::page_ctor;
    out.blocked_page = 7;
    if (hooks.page_ctor == nullptr) return out;
    {
        const SharedStr name = l2::measured_view(slot552_page_name(7), slot552_page_name_len(7));
        if (!exact_borrowed_name(name, 7)) {
            out.names_borrowed = false;
            return out;
        }
        const Slot552PageResult built = hooks.page_ctor(&objects.page_c.base, name);
        if (!built.ok) return out;
        out.blocked = Slot552Stage::page_stores;
        init_slot552_page_c_stores(objects.page_c);
        out.blocked = Slot552Stage::page_atexit;
        if (!concmd.atexit_log.record(kSlot552PageCDtorRva, &objects.page_c)) return out;
        ++out.atexit_records;
        ++out.pages_built;
    }

    out.blocked = Slot552Stage::none;
    out.ok = true;
    return out;
}

}  // namespace sotc
