#include "init_5d5410_body.hpp"

#include <cstring>

namespace sotc {
namespace {

std::uint32_t load_u32(const void* p, std::size_t offset) noexcept {
    std::uint32_t v = 0;
    std::memcpy(&v, static_cast<const std::uint8_t*>(p) + offset, sizeof v);
    return v;
}

void store_u32(void* p, std::size_t offset, std::uint32_t v) noexcept {
    std::memcpy(static_cast<std::uint8_t*>(p) + offset, &v, sizeof v);
}

void store_u64(void* p, std::size_t offset, std::uint64_t v) noexcept {
    std::memcpy(static_cast<std::uint8_t*>(p) + offset, &v, sizeof v);
}

std::uint32_t low_byte(std::uint32_t v) noexcept {
    return v & 0xffu;
}

// 0x8666d0: pack six low-bit fields into a 24-bit format word.
std::uint32_t pack_8666d0(std::uint32_t rdi, std::uint32_t rsi, std::uint32_t rdx,
                          std::uint32_t rcx, std::uint32_t r8, std::uint32_t r9) noexcept {
    return (rdi & 0xffu) | ((rsi & 0x0fu) << 8) | ((rdx & 0x07u) << 12) |
           ((rcx & 0x07u) << 15) | ((r8 & 0x07u) << 18) | ((r9 & 0x07u) << 21);
}

// The common final packing in 0x8652d0 / 0x865500 / 0x865410. The high bits are 0x20000000.
std::uint32_t format_bits_865(std::uint32_t d) noexcept {
    std::uint32_t r15 = 0;
    if ((d & 0x800u) == 0) {
        const std::uint32_t low = d & 0xffu;
        if ((low - 1u) <= 0x0du) {
            const std::uint32_t candidate = (d & 0x0fu) << 15;
            const std::uint32_t low_even = d & 0xfeu;
            if ((d & 0xf00u) == 0x700u) r15 = candidate;
            if (low_even != 6u) r15 = candidate;
        }
    }
    return ((d >> 12) & 0xfffu) | ((d << 4) & 0x7000u) | r15 | 0x20000000u;
}

// 0x8652d0(object, rsi, edx, ecx, r8).
void setter_8652d0(void* object, std::uint64_t rsi, std::uint32_t edx, std::uint32_t ecx,
                   std::uint32_t r8) noexcept {
    store_u32(object, 0x0c, 0x20000000u);
    store_u32(object, 0x00, static_cast<std::uint32_t>(rsi));
    const std::uint32_t high = static_cast<std::uint32_t>(rsi >> 32) & 0xfffu;
    store_u32(object, 0x04, ((ecx << 16) & 0x3fff0000u) | high);
    store_u32(object, 0x0c, format_bits_865(edx));
    store_u32(object, 0x08, r8);
}

// 0x865360(object, esi).
void setter_865360(void* object, std::uint32_t esi) noexcept {
    const std::uint32_t old4 = load_u32(object, 0x04);
    const std::uint32_t oldc = load_u32(object, 0x0c);
    const std::uint32_t ecx = (esi & 0x03u) << 14;
    const std::uint32_t edx = (esi & 0x1cu) << 25;
    const std::uint32_t bits = ((esi & 0x60u) << 7) | ecx | (old4 & 0xffff0fffu);
    store_u32(object, 0x0c, edx | (oldc & 0xc7ffffffu));
    store_u32(object, 0x04, bits);
}

// 0x865500(object, rsi, edx, ecx).
void setter_865500(void* object, std::uint64_t rsi, std::uint32_t edx,
                   std::uint32_t ecx) noexcept {
    store_u32(object, 0x0c, 0x20000000u);
    store_u32(object, 0x00, static_cast<std::uint32_t>(rsi));
    const std::uint32_t high = static_cast<std::uint32_t>(rsi >> 32) & 0xfffu;
    store_u32(object, 0x04, ((edx << 16) & 0x3fff0000u) | high);
    const std::uint32_t packed = pack_8666d0(4, 7, 4, 0, 0, 1);
    store_u32(object, 0x0c, format_bits_865(packed));
    store_u32(object, 0x08, ecx);
}

// 0x8655d0(object, rsi, edx).
void setter_8655d0(void* object, std::uint64_t rsi, std::uint32_t edx) noexcept {
    store_u32(object, 0x00, static_cast<std::uint32_t>(rsi));
    store_u32(object, 0x04,
              (static_cast<std::uint32_t>(rsi >> 32) & 0xfffu) | 0x00100000u);
    store_u32(object, 0x0c, 0x20077facu);
    store_u32(object, 0x08, (edx + 0x0fu) >> 4);
}

// 0x865600(object, rsi, edx).
void setter_865600(void* object, std::uint64_t rsi, std::uint32_t edx) noexcept {
    store_u32(object, 0x00, static_cast<std::uint32_t>(rsi));
    store_u32(object, 0x04, static_cast<std::uint32_t>(rsi >> 32) & 0xfffu);
    store_u32(object, 0x0c, 0x2000c004u);
    store_u32(object, 0x08, edx);
}

// 0x87c390(d, esi): full low byte shifted to bit 30.
void set_87c390(std::uint32_t& d, std::uint32_t esi) noexcept {
    d = (d & 0xbfffffffu) | (low_byte(esi) << 30);
}

// 0x87c3b0(d, esi, edx, ecx).
void set_87c3b0(std::uint32_t& d, std::uint32_t esi, std::uint32_t edx,
                std::uint32_t ecx) noexcept {
    d = (d & 0xffffe000u) | (esi & 0x1fu) | ((edx & 0x07u) << 5) |
        ((ecx & 0x1fu) << 8);
}

// 0x87c3d0(d, esi, edx, ecx).
void set_87c3d0(std::uint32_t& d, std::uint32_t esi, std::uint32_t edx,
                std::uint32_t ecx) noexcept {
    d = (d & 0xe000ffffu) | ((esi & 0x1fu) << 16) | ((edx & 0x07u) << 21) |
        ((ecx & 0x1fu) << 24);
}

// 0x87c400(d, esi): full low byte shifted to bit 29.
void set_87c400(std::uint32_t& d, std::uint32_t esi) noexcept {
    d = (d & 0xdfffffffu) | (low_byte(esi) << 29);
}

// 0x87cba0(d, esi, edx).
void set_87cba0(std::uint32_t& d, std::uint32_t esi, std::uint32_t edx) noexcept {
    const std::uint32_t field = ((edx << 4) & 0x70u) + ((esi & 1u) << 2);
    d = (d & 0xffffff8bu) | field;
}

// 0x87cc00(d, esi): (d & ~2) + byte*2; the add may carry above bit 1.
void set_87cc00(std::uint32_t& d, std::uint32_t esi) noexcept {
    d = (d & 0xfffffffdu) + (low_byte(esi) << 1);
}

// 0x87cc10(d, esi): (d & ~1) | byte; bits 1..7 of the byte are preserved.
void set_87cc10(std::uint32_t& d, std::uint32_t esi) noexcept {
    d = (d & 0xfffffffeu) | low_byte(esi);
}

// 0x87cc20(d, esi): byte shifted to bit 7.
void set_87cc20(std::uint32_t& d, std::uint32_t esi) noexcept {
    d = (d & 0xffffff7fu) | (low_byte(esi) << 7);
}

// 0x87cc40(d, esi): (d & ~8) + byte*8; the add may carry above bit 3.
void set_87cc40(std::uint32_t& d, std::uint32_t esi) noexcept {
    d = (d & 0xfffffff7u) + (low_byte(esi) << 3);
}

// 0x87cbc0(d, esi).
void set_87cbc0(std::uint32_t& d, std::uint32_t esi) noexcept {
    d = (d & 0xfffff8ffu) | ((esi & 0x07u) << 8);
}

// 0x87cbe0(d, esi).
void set_87cbe0(std::uint32_t& d, std::uint32_t esi) noexcept {
    d = (d & 0xff8fffffu) | ((esi & 0x07u) << 20);
}

// 0x87cd80(d, esi, edx, ecx).
void set_87cd80(std::uint32_t& d, std::uint32_t esi, std::uint32_t edx,
                std::uint32_t ecx) noexcept {
    d = (d & 0xfffff000u) | (esi & 0x0fu) | ((edx & 0x0fu) << 4) |
        ((ecx & 0x0fu) << 8);
}

// 0x87cda0(d, esi, edx, ecx).
void set_87cda0(std::uint32_t& d, std::uint32_t esi, std::uint32_t edx,
                std::uint32_t ecx) noexcept {
    d = (d & 0xff000fffu) | ((esi & 0x0fu) << 12) | ((edx & 0x0fu) << 16) |
        ((ecx & 0x0fu) << 20);
}

constexpr std::uint32_t kFormatTable[0x1a] = {
    0x00204704u, 0x0022c70bu, 0x003ac70du, 0x00fac70eu, 0x00004404u, 0x0002c40bu,
    0x001ac40du, 0x00fac40eu, 0x00004504u, 0x0002c50bu, 0x001ac50du, 0x00fac50eu,
    0x0022c705u, 0x00fac70cu, 0x00004402u, 0x0002c405u, 0x00fac40cu, 0x00004401u,
    0x00fac009u, 0x00fac40au, 0x00fac00au, 0x00fac90au, 0x00fac50au, 0x00fac10au,
    0x00004402u, 0x00004404u,
};

}  // namespace

Init8be60Result run_init_8be60_body(void* object, std::uint32_t a1, std::uint32_t a2,
                                    std::uint32_t a3, std::uint32_t a4, void* a5,
                                    std::uint64_t stack0,
                                    const Init8be60Hooks& hooks) noexcept {
    Init8be60Result out{};
    if (object == nullptr) {
        out.blocked = "object_missing";
        return out;
    }

    const std::uint32_t product = a3 * a2;
    store_u32(object, 0x18, a4);
    store_u32(object, 0x1c, a1);
    store_u32(object, 0x20, product);

    // 0x8be9e jumps to 0x8bf5e before `xor r15d,r15d` at 0x8bea4. r15 is uninitialized on
    // that path; the model refuses instead of inventing a pointer.
    if ((a4 & 0x800u) != 0) {
        out.blocked = "r15_uninitialized_path";
        return out;
    }

    void* r15 = nullptr;
    auto allocate = [&](std::uint32_t size, std::uint32_t tag) noexcept -> bool {
        if (hooks.alloc_12820 == nullptr) {
            out.blocked = "alloc_12820_missing";
            return false;
        }
        ++out.alloc_calls;
        const InitPostCallOutcome alloc = hooks.alloc_12820(size, tag, 0x10u, 0u);
        if (!alloc.executed || !alloc.has_value || alloc.value == 0) {
            out.blocked = alloc.blocker != nullptr ? alloc.blocker : "alloc_12820_failed";
            return false;
        }
        r15 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(alloc.value));
        return true;
    };

    if ((a4 & 2u) == 0) {
        if ((a4 & 0x18u) != 0) {
            if (!allocate(product + 4u, hooks.tag_2dd9f84)) return out;
            store_u32(r15, product, 0);
        } else {
            const std::uint32_t tag = (a4 & 1u) != 0 ? hooks.tag_15fe920 : hooks.tag_2dd9f84;
            if (!allocate(product, tag)) return out;
        }
        store_u64(object, 0x10, reinterpret_cast<std::uintptr_t>(r15));
    }

    if (a5 != nullptr) {
        // 0x8bf25: the fallback allocation is not stored into object+0x10, matching retail.
        if (r15 == nullptr) {
            if (!allocate(product, hooks.tag_2ed1f54)) return out;
        }
        if (product != 0) std::memcpy(r15, a5, product);
        ++out.copy_calls;
    }

    void* const r14 = r15;
    const std::uint32_t masked = a4 & 0x3cu;
    switch (a1) {
        case 0x1a:
            setter_865600(object, reinterpret_cast<std::uintptr_t>(r14), product);
            break;
        case 0x1b:
            setter_865500(object, reinterpret_cast<std::uintptr_t>(r14), a2, a3);
            break;
        case 0x1c:
            setter_8655d0(object, reinterpret_cast<std::uintptr_t>(r14), product);
            break;
        default: {
            const std::uint32_t table = a1 <= 0x19u ? kFormatTable[a1] : 0u;
            std::uint32_t ecx = a2;
            std::uint32_t r8 = a3;
            if ((a4 & 0x100u) != 0) {
                r8 = product;
                ecx = 0;
            }
            setter_8652d0(object, reinterpret_cast<std::uintptr_t>(r14), table, ecx, r8);
            break;
        }
    }

    const std::uint32_t format = (a4 & 1u) != 0 ? 0x6eu : (masked != 0 ? 0x6du : 0x10u);
    setter_865360(object, format);

    if (stack0 != 0) {
        if (hooks.tail_87f3e0 == nullptr) {
            out.blocked = "tail_87f3e0_missing";
            return out;
        }
        const std::uint32_t field = (load_u32(object, 0x04) >> 16) & 0x3fffu;
        const std::uint32_t multiplier = field != 0 ? field : 1u;
        const std::uint32_t count = multiplier * load_u32(object, 0x08);
        const std::uint64_t packed = static_cast<std::uint64_t>(load_u32(object, 0x00)) |
                                     (static_cast<std::uint64_t>(
                                          load_u32(object, 0x04) & 0xfffu)
                                      << 32);
        ++out.tail_calls;
        const InitPostCallOutcome tail = hooks.tail_87f3e0(
            static_cast<std::uint8_t*>(object) + 0x24, hooks.tag_2ed37a4, packed, count,
            stack0, 4u, 0u);
        if (!tail.executed) {
            out.blocked = tail.blocker != nullptr ? tail.blocker : "tail_87f3e0_blocked";
            return out;
        }
    }

    out.format = format;
    out.allocation = r15;
    out.object_word0 = load_u32(object, 0x00);
    out.object_word4 = load_u32(object, 0x04);
    out.object_word8 = load_u32(object, 0x08);
    out.object_word_c = load_u32(object, 0x0c);
    out.ok = true;
    return out;
}

Init8f520Result run_init_8f520_body(std::uint32_t rsi, std::uint32_t rdx,
                                    std::uint32_t rcx, std::uint32_t r8,
                                    std::uint32_t r9, std::uint32_t s0,
                                    std::uint32_t s1, std::uint32_t s2,
                                    std::uint32_t s3, std::uint32_t s4,
                                    std::uint32_t s5, std::uint32_t s6) noexcept {
    // d0 at rbp-0x40. 0x87cb90 clears it; 0x87cc40(d0, 0) follows.
    std::uint32_t d0 = 0;
    set_87cc40(d0, 0);
    set_87cc00(d0, 1);
    set_87cba0(d0, rsi, rdx);
    set_87cc10(d0, 1);
    set_87cc20(d0, 1);
    set_87cbc0(d0, rcx);
    set_87cbe0(d0, s1);

    // d1 at rbp-0x3c. 0x87cd70 clears it.
    std::uint32_t d1 = 0;
    set_87cd80(d1, r9, r8, s0);
    set_87cda0(d1, s3, s2, s4);

    Init8f520Result out{};
    out.q0 = static_cast<std::uint64_t>(d0) | (static_cast<std::uint64_t>(d1) << 32);

    // The final record at rbp-0x38: ff, s5, s6, ff, 00, s5, s6, ff.
    const std::uint64_t b5 = low_byte(s5);
    const std::uint64_t b6 = low_byte(s6);
    out.q1 = 0xffull | (b5 << 8) | (b6 << 16) | (0xffull << 24) | (b5 << 40) |
             (b6 << 48) | (0xffull << 56);
    return out;
}

std::uint64_t run_init_8f1d0_body(std::uint32_t rsi, std::uint32_t rdx,
                                  std::uint32_t rcx, std::uint32_t r8) noexcept {
    // d0 at rbp-0x38. 0x87c380 clears it.
    std::uint32_t d = 0;
    const std::uint32_t flag = (rdx != 1u || (rsi | rcx) != 0u) ? 1u : 0u;
    set_87c390(d, flag);
    set_87c400(d, 1);
    // 0x87c3b0 / 0x87c3d0 receive (esi, edx, ecx) = (rdx, rsi, rcx).
    set_87c3b0(d, rdx, rsi, rcx);
    set_87c3d0(d, rdx, rsi, rcx);
    return static_cast<std::uint64_t>(d) | (static_cast<std::uint64_t>(r8) << 32);
}

Init5d5410Result run_init_5d5410_body(const Init5d5410Hooks& hooks) noexcept {
    Init5d5410Result out{};
    if (hooks.object_a == nullptr) {
        out.blocked = "object_a_missing";
        return out;
    }
    if (hooks.object_b == nullptr) {
        out.blocked = "object_b_missing";
        return out;
    }
    if (hooks.state == nullptr) {
        out.blocked = "state_missing";
        return out;
    }
    out.owner = hooks.global_owner;

    out.inner_a = run_init_8be60_body(hooks.object_a, 0x1c, 0x50, 1, 2, nullptr, 0,
                                      hooks.be);
    ++out.calls_8be60;
    if (!out.inner_a.ok) {
        ++out.blocked_calls;
        out.blocked = out.inner_a.blocked != nullptr ? out.inner_a.blocked : "8be60_a_blocked";
        return out;
    }
    out.inner_b = run_init_8be60_body(hooks.object_b, 0x1c, 0x50, 1, 2, nullptr, 0,
                                      hooks.be);
    ++out.calls_8be60;
    if (!out.inner_b.ok) {
        ++out.blocked_calls;
        out.blocked = out.inner_b.blocked != nullptr ? out.inner_b.blocked : "8be60_b_blocked";
        return out;
    }

    const Init8f520Result first520 =
        run_init_8f520_body(0, 1, 7, 0, 0, 0, 7, 0, 0, 0, 0xff, 0xff);
    ++out.calls_8f520;
    store_u64(hooks.state, 0x00, first520.q0);
    store_u64(hooks.state, 0x08, first520.q1);

    store_u64(hooks.state, 0x10, run_init_8f1d0_body(0, 1, 0, 8));
    ++out.calls_8f1d0;

    const Init8f520Result second520 =
        run_init_8f520_body(1, 7, 7, 0, 0, 0, 7, 0, 0, 0, 0xff, 0xff);
    ++out.calls_8f520;
    store_u64(hooks.state, 0x18, second520.q0);
    store_u64(hooks.state, 0x20, second520.q1);

    store_u64(hooks.state, 0x28, run_init_8f1d0_body(0, 1, 0, 0));
    ++out.calls_8f1d0;

    const Init8f520Result third520 =
        run_init_8f520_body(0, 4, 7, 0, 0, 0, 7, 0, 0, 0, 0xff, 0xff);
    ++out.calls_8f520;
    store_u64(hooks.state, 0x30, third520.q0);
    store_u64(hooks.state, 0x38, third520.q1);

    store_u64(hooks.state, 0x40, run_init_8f1d0_body(0, 6, 1, 0xffffffffu));
    ++out.calls_8f1d0;

    std::memcpy(out.state.data(), hooks.state, kInit5d5410StateBytes);
    out.ok = true;
    return out;
}

}  // namespace sotc
