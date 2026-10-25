// Fixture for the three trivial 0x2569d0 constructors: 0xa1cc0, 0xfafb0, 0x26f820.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/owner_simple_ctors.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
int g_base = 0;
int g_sub = 0;
bool g_fail_base = false;
bool g_fail_sub = false;

bool base_ctor(void* object) {
    ++g_base;
    if (g_fail_base) {
        return false;
    }
    static_cast<std::uint8_t*>(object)[0x10] = 0xAA;
    return true;
}

bool sub_ctor(void* object) {
    ++g_sub;
    if (g_fail_sub) {
        return false;
    }
    *static_cast<std::uint8_t*>(object) = 0xBB;
    return true;
}
}  // namespace

int main() {
    CHECK(kOwnerA1cc0Rva == 0xa1cc0);
    CHECK(kOwnerA1cc0Size == 1);
    CHECK(kOwnerFafb0Rva == 0xfafb0);
    CHECK(kOwnerFafb0Size == 92);
    CHECK(kOwnerFafb0ObjectSize == 0x82028);
    CHECK(kOwner26f820Rva == 0x26f820);
    CHECK(kOwner26f820Size == 57);
    CHECK(kOwner26f820ObjectSize == 0x4028);

    // ---- 1. 0xa1cc0 is a no-op. ----
    {
        std::uint8_t byte = 0x5A;
        init_owner_a1cc0(&byte);
        CHECK(byte == 0x5A);
    }

    // ---- 2. 0xfafb0 layout and zero fills. ----
    {
        std::vector<std::byte> object(kOwnerFafb0ObjectSize, std::byte{0xCC});
        int vptr = 0;
        init_owner_fafb0(object.data(), &vptr);
        auto* const base = object.data();
        CHECK(*reinterpret_cast<void**>(base) == &vptr);
        CHECK(*reinterpret_cast<void**>(base + 0x2008) == base + 0x2028);
        CHECK(*reinterpret_cast<void**>(base + 0x2010) == base + kOwnerFafb0ObjectSize);
        CHECK(*reinterpret_cast<void**>(base + 0x2018) == base + kOwnerFafb0ObjectSize);
        for (std::size_t i = 0x08; i < 0x2008; ++i) {
            CHECK(base[i] == std::byte{0});
        }
        for (std::size_t i = 0x2028; i < kOwnerFafb0ObjectSize; ++i) {
            CHECK(base[i] == std::byte{0});
        }
        // The original leaves +0x2020..+0x2027 untouched; the native model zeroes it.
        for (std::size_t i = 0x2020; i < 0x2028; ++i) {
            CHECK(base[i] == std::byte{0});
        }
    }

    // ---- 3. 0x26f820 order, fields and failure surfaces. ----
    {
        std::vector<std::byte> object(kOwner26f820ObjectSize, std::byte{0xCC});
        int vptr = 0;
        Owner26f820Hooks hooks{};
        hooks.base_ctor = base_ctor;
        hooks.sub_ctor = sub_ctor;
        g_base = g_sub = 0;
        CHECK(init_owner_26f820(object.data(), &vptr, hooks));
        CHECK(g_base == 1);
        CHECK(g_sub == 1);
        CHECK(*reinterpret_cast<void**>(object.data()) == &vptr);
        CHECK(*reinterpret_cast<void**>(object.data() + 0x18) == nullptr);
        CHECK(object[0x4020] == std::byte{0xBB});
        CHECK(object[0x10] == std::byte{0xAA});

        // Base failure stops before the vptr/store or the sub ctor.
        std::vector<std::byte> second(kOwner26f820ObjectSize, std::byte{0xCC});
        g_base = g_sub = 0;
        g_fail_base = true;
        CHECK(!init_owner_26f820(second.data(), &vptr, hooks));
        CHECK(g_base == 1 && g_sub == 0);
        CHECK(second[0] == std::byte{0xCC});
        g_fail_base = false;

        // Missing hooks are visible failures.
        Owner26f820Hooks missing{};
        CHECK(!init_owner_26f820(second.data(), &vptr, missing));
        missing.base_ctor = base_ctor;
        CHECK(!init_owner_26f820(second.data(), &vptr, missing));
    }

    return 0;
}
