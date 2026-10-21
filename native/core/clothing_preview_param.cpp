#include "clothing_preview_param.hpp"

#include <cstring>
#include <new>

#include "startup_ctors.hpp"

namespace sotc {

namespace {

ClothingPreviewParam* construct_clothing_preview_param(void* storage,
                                                       ResourceOwner* owner) noexcept {
    if (storage == nullptr) {
        return nullptr;
    }
    return new (storage) ClothingPreviewParam(owner);
}

void destroy_clothing_preview_param(ClothingPreviewParam* object) noexcept {
    if (object != nullptr) {
        object->~ClothingPreviewParam();
    }
}

void* to_pointer(std::uint64_t value) noexcept {
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(value));
}

}  // namespace

ClothingParamBase::ClothingParamBase(ResourceOwner* owner, std::uint64_t field20,
                                     std::uint64_t field28) noexcept
    : owner_(owner),
      field10_(0),
      field18_(0),
      field20_(field20),
      field28_(field28),
      tracked30_(0),
      tracked38_(0),
      flag40_(0),
      flag41_(0),
      flag42_(0),
      reserved43_{} {
    // Original 0x00e1eda0 only zeroes +0x10..+0x1f and +0x30..+0x3f and raises the three gate
    // flags on the field20==0 path. The native model always zero-initializes the untouched words;
    // for field20!=0 the original leaves them caller-provided, which has no native equivalent.
    if (field20 == 0) {
        flag40_ = 1;
        flag41_ = 1;
        flag42_ = 1;
    }
}

ClothingParamBase::~ClothingParamBase() noexcept {
    // Original 0x00e1ede0: release +0x38 then +0x30 through the owner's [vtbl+0xd0] hook.
    if (tracked38_ != 0 && flag42_ != 0 && owner_ != nullptr) {
        owner_->release_base(to_pointer(tracked38_));
        tracked38_ = 0;
    }
    if (tracked30_ != 0 && flag41_ != 0 && owner_ != nullptr) {
        owner_->release_base(to_pointer(tracked30_));
        tracked30_ = 0;
    }
}

ClothingPreviewParam::ClothingPreviewParam(ResourceOwner* owner, std::uint64_t field20,
                                           std::uint64_t field28) noexcept
    : ClothingParamBase(owner, field20, field28),
      scale48_(0.0f),
      block4c_{},
      scale5c_(0.0f),
      block60_{},
      scale70_(0.0f),
      block74_{},
      scale84_(0.0f),
      flag88_(0),
      flag89_(0),
      flag8a_(0),
      reserved8b_{},
      tracked90_(0),
      tracked98_(0),
      owns_resource_(0),
      reservedA1_{},
      stateA4_(0) {
    // Original 0x00f09280 initializes the +0x48..+0xa4 block only on the field20==0 path.
    if (field20 == 0) {
        initialize_derived_state();
    }
}

ClothingPreviewParam::~ClothingPreviewParam() noexcept {
    // Original 0x00f09350: release +0x98 through the owner's [vtbl+0xc0] hook, then run the base
    // destructor. The base destructor runs automatically after this body.
    if (owns_resource_ != 0 && tracked98_ != 0 && owner() != nullptr) {
        owner()->release_derived(to_pointer(tracked98_));
    }
}

void ClothingPreviewParam::reset() noexcept {
    // Original 0x00f09d80.
    if (owns_resource_ != 0 && tracked98_ != 0 && owner() != nullptr) {
        owner()->release_derived(to_pointer(tracked98_));
    }
    initialize_derived_state();
}

void ClothingPreviewParam::initialize_derived_state() noexcept {
    scale48_ = 1.0f;
    std::memset(block4c_, 0, sizeof block4c_);
    scale5c_ = 1.0f;
    std::memset(block60_, 0, sizeof block60_);
    scale70_ = 1.0f;
    std::memset(block74_, 0, sizeof block74_);
    scale84_ = 1.0f;
    flag88_ = 0;
    flag89_ = 0;
    flag8a_ = 0;
    tracked90_ = 0;
    tracked98_ = 0;
    owns_resource_ = 1;
    stateA4_ = kRecoveredStateA4;
}

const ClothingPreviewParamDescriptor kClothingPreviewParamDescriptor{
    ClothingPreviewParam::kTypeName,
    ClothingPreviewParam::kObjectSize,
    ClothingPreviewParam::kObjectAlignment,
    &construct_clothing_preview_param,
    &destroy_clothing_preview_param,
};

bool init_clothing_preview_descriptor(StartupState& state) noexcept {
    // Original 0x00f09e60 builds and tears down one stack-local instance before publishing.
    {
        ClothingPreviewParam probe;
        (void)probe;
    }

    state.descriptor = &kClothingPreviewParamDescriptor;

    // The original allocates a 16-byte handle with the constant loaded from RVA 0x12d2d1c, then
    // initializes a recursive + priority-inherit mutex (attribute values 2 and 1) and registers an
    // atexit cleanup. StartupState owns the native mutex through RAII instead.
    if (!state.descriptor_mutex_initialized) {
        const int result = state.descriptor_mutex.initialize();
        if (result != 0) {
            state.init_failed = true;
            return false;
        }
        state.descriptor_mutex_initialized = true;
    }
    return true;
}

}  // namespace sotc
