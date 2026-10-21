#include <cstdio>
#include <cstdlib>
#include <new>
#include <string>
#include <type_traits>

#include "core/clothing_preview_param.hpp"
#include "core/startup_ctors.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

struct RecordingOwner final : sotc::ResourceOwner {
    int derived_calls = 0;
    int base_calls = 0;
    void* last_derived = nullptr;
    void* last_base = nullptr;

    void release_derived(void* pointer) noexcept override {
        ++derived_calls;
        last_derived = pointer;
    }
    void release_base(void* pointer) noexcept override {
        ++base_calls;
        last_base = pointer;
    }
};

struct ExposedBase final : sotc::ClothingParamBase {
    using ClothingParamBase::ClothingParamBase;
    using ClothingParamBase::set_tracked30;
    using ClothingParamBase::set_tracked38;
};

}  // namespace

int main() {
    using namespace sotc;

    static_assert(sizeof(ClothingParamBase) == ClothingParamBase::kBaseObjectSize,
                  "base subobject must stay 0x48 bytes");
    static_assert(sizeof(ClothingPreviewParam) == ClothingPreviewParam::kObjectSize,
                  "ClothingPreviewParam must stay 0xb0 bytes");
    static_assert(alignof(ClothingPreviewParam) == ClothingPreviewParam::kObjectAlignment,
                  "ClothingPreviewParam must stay 8-byte aligned");
    static_assert(std::has_virtual_destructor_v<ClothingPreviewParam>);

    // Default construction reproduces the recovered +0x48..+0xa4 state block.
    {
        ClothingPreviewParam object;
        CHECK(object.scale48() == 1.0f);
        CHECK(object.scale5c() == 1.0f);
        CHECK(object.scale70() == 1.0f);
        CHECK(object.scale84() == 1.0f);
        CHECK(object.stateA4() == ClothingPreviewParam::kRecoveredStateA4);
        CHECK(!object.derived_flag88() && !object.derived_flag89() && !object.derived_flag8a());
        CHECK(object.owns_resource());
        CHECK(object.tracked98() == 0);
        CHECK(object.base_flag40() && object.base_flag41() && object.base_flag42());
        CHECK(object.base_flag40());   // gates remain raised on the field20==0 path
    }

    // The field20!=0 path skips the derived state block, matching the original branch.
    {
        ClothingPreviewParam object(nullptr, 0x11, 0x22);
        CHECK(object.scale48() == 0.0f);
        CHECK(object.stateA4() == 0);
        CHECK(!object.owns_resource());
        CHECK(!object.base_flag40() && !object.base_flag41() && !object.base_flag42());
        CHECK(object.field20() == 0x11);
        CHECK(object.field28() == 0x22);
    }

    // reset() releases +0x98 through the [vtbl+0xc0] hook and restores the state block.
    {
        RecordingOwner owner;
        ClothingPreviewParam object(&owner);
        object.set_tracked98(0x1234);
        object.reset();
        CHECK(owner.derived_calls == 1);
        CHECK(owner.last_derived == reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1234)));
        CHECK(object.tracked98() == 0);
        CHECK(object.scale48() == 1.0f);
        CHECK(object.stateA4() == ClothingPreviewParam::kRecoveredStateA4);
    }

    // Destructor releases +0x98 only once and only when armed.
    {
        RecordingOwner owner;
        {
            ClothingPreviewParam object(&owner);
            object.set_tracked98(0x5678);
        }
        CHECK(owner.derived_calls == 1);
        CHECK(owner.last_derived == reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x5678)));
    }
    {
        RecordingOwner owner;
        {
            ClothingPreviewParam object(&owner);
            object.reset();
        }
        CHECK(owner.derived_calls == 0);
    }

    // Base teardown releases +0x38 then +0x30 through the [vtbl+0xd0] hook, in that order.
    {
        RecordingOwner owner;
        {
            ExposedBase base(&owner, 0, 0);
            base.set_tracked38(0x38);
            base.set_tracked30(0x30);
            CHECK(base.base_flag41() && base.base_flag42());
        }
        CHECK(owner.base_calls == 2);
        CHECK(owner.last_base == reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x30)));
    }
    {
        RecordingOwner owner;
        {
            ExposedBase base(&owner, 0, 0);
            base.set_tracked38(0x38);
        }
        CHECK(owner.base_calls == 1);
    }

    // Published descriptor metadata matches the recovered factory constants.
    {
        const ClothingPreviewParamDescriptor& descriptor = kClothingPreviewParamDescriptor;
        CHECK(std::string(descriptor.type_name) == ClothingPreviewParam::kTypeName);
        CHECK(descriptor.object_size == 0xb0);
        CHECK(descriptor.object_alignment == 8);
        CHECK(descriptor.construct != nullptr);
        CHECK(descriptor.destroy != nullptr);
        CHECK(std::string(descriptor.type_name) == "ClothingPreviewParam");
    }

    // The factory path mirrors original RVA 0x00ed59a0: 0xb0-byte storage, alignment 8, construct.
    {
        RecordingOwner owner;
        alignas(8) unsigned char storage[ClothingPreviewParam::kObjectSize];
        ClothingPreviewParam* object =
            kClothingPreviewParamDescriptor.construct(storage, &owner);
        CHECK(object == reinterpret_cast<ClothingPreviewParam*>(storage));
        CHECK(reinterpret_cast<std::uintptr_t>(object) % 8 == 0);
        CHECK(object->scale48() == 1.0f);
        object->set_tracked98(0x9abc);
        kClothingPreviewParamDescriptor.destroy(object);
        CHECK(owner.derived_calls == 1);
        CHECK(owner.last_derived == reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x9abc)));
    }

    // Slot-10 publication is idempotent for the descriptor and initializes the owned mutex once.
    {
        StartupState state;
        CHECK(state.descriptor == nullptr);
        CHECK(!state.descriptor_mutex_initialized);
        CHECK(init_clothing_preview_descriptor(state));
        CHECK(state.descriptor == &kClothingPreviewParamDescriptor);
        CHECK(state.descriptor_mutex_initialized);
        CHECK(!state.init_failed);
        CHECK(init_clothing_preview_descriptor(state));
        CHECK(state.descriptor_mutex_initialized);
        CHECK(state.descriptor_mutex.lock() == 0);
        CHECK(state.descriptor_mutex.lock() == 0);   // recursive
        CHECK(state.descriptor_mutex.unlock() == 0);
        CHECK(state.descriptor_mutex.unlock() == 0);
    }

    std::puts("clothing_preview_param: all checks passed");
    return 0;
}
