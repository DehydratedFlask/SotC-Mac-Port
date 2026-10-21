// Native semantic model of the class whose original vtable address point is RVA 0x15fb7c0.
//
// Recovered original facts (ELF sha256 d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3):
//   constructor            0x00f09280 (sets the derived vptr and the +0x48..+0xa4 state block)
//   base constructor       0x00e1eda0 (sets the base vptr 0x15e2d70 and the +0x08..+0x42 block)
//   complete-object dtor   0x00f09350
//   deleting dtor          0x00f093f0
//   reset                  0x00f09d80
//   factory                0x00ed59a0 (allocates 0xb0 bytes at alignment 8, then constructs)
//   second construct path  0x00ed5a90 (reads the published descriptor pointer at global 0x3356678)
//   type name              "ClothingPreviewParam" (error literal at RVA 0x1322b68)
//
// This is a semantic native model, not a binary-compatible reconstruction of the original vtable.
// Only the recovered virtual surface (destructor and reset) is present. The original table has a
// much larger unrecovered method surface; no original address is stored or dispatched from here.

#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

struct StartupState;

// The original object holds an owner/allocator interface at +0x08 and releases resources through
// two different virtual slots: [vtbl+0xc0] for the +0x98 resource and [vtbl+0xd0] for the +0x30
// and +0x38 resources. Both hooks are modelled explicitly instead of collapsing them.
struct ResourceOwner {
    virtual ~ResourceOwner() noexcept = default;
    virtual void release_derived(void* pointer) noexcept = 0;   // original [vtbl+0xc0]
    virtual void release_base(void* pointer) noexcept = 0;      // original [vtbl+0xd0]
};

// Base subobject: original RVAs 0x00e1eda0 (ctor), 0x00e1ede0 (dtor), vptr 0x15e2d70.
class ClothingParamBase {
public:
    static constexpr std::size_t kBaseObjectSize = 0x48;

    ClothingParamBase(ResourceOwner* owner, std::uint64_t field20, std::uint64_t field28) noexcept;
    virtual ~ClothingParamBase() noexcept;

    ClothingParamBase(const ClothingParamBase&) = delete;
    ClothingParamBase& operator=(const ClothingParamBase&) = delete;
    ClothingParamBase(ClothingParamBase&&) = delete;
    ClothingParamBase& operator=(ClothingParamBase&&) = delete;

    [[nodiscard]] ResourceOwner* owner() const noexcept { return owner_; }
    [[nodiscard]] std::uint64_t field20() const noexcept { return field20_; }
    [[nodiscard]] std::uint64_t field28() const noexcept { return field28_; }
    [[nodiscard]] std::uint64_t tracked30() const noexcept { return tracked30_; }
    [[nodiscard]] std::uint64_t tracked38() const noexcept { return tracked38_; }
    [[nodiscard]] bool base_flag40() const noexcept { return flag40_; }
    [[nodiscard]] bool base_flag41() const noexcept { return flag41_; }
    [[nodiscard]] bool base_flag42() const noexcept { return flag42_; }

protected:
    void set_tracked30(std::uint64_t value) noexcept { tracked30_ = value; }
    void set_tracked38(std::uint64_t value) noexcept { tracked38_ = value; }

private:
    ResourceOwner* owner_;
    [[maybe_unused]] std::uint64_t field10_;
    [[maybe_unused]] std::uint64_t field18_;
    std::uint64_t field20_;
    std::uint64_t field28_;
    std::uint64_t tracked30_;
    std::uint64_t tracked38_;
    std::uint8_t flag40_;
    std::uint8_t flag41_;
    std::uint8_t flag42_;
    [[maybe_unused]] std::uint8_t reserved43_[5];
};

// Derived object: original allocation size 0xb0, alignment 8.
class ClothingPreviewParam : public ClothingParamBase {
public:
    static constexpr std::size_t kObjectSize = 0xb0;
    static constexpr std::size_t kObjectAlignment = 8;
    static constexpr const char* kTypeName = "ClothingPreviewParam";
    static constexpr std::uint64_t kRecoveredStateA4 = 0x40;

    ClothingPreviewParam(ResourceOwner* owner = nullptr, std::uint64_t field20 = 0,
                         std::uint64_t field28 = 0) noexcept;
    ~ClothingPreviewParam() noexcept override;

    ClothingPreviewParam(const ClothingPreviewParam&) = delete;
    ClothingPreviewParam& operator=(const ClothingPreviewParam&) = delete;
    ClothingPreviewParam(ClothingPreviewParam&&) = delete;
    ClothingPreviewParam& operator=(ClothingPreviewParam&&) = delete;

    // Original RVA 0x00f09d80: releases the +0x98 resource and restores the +0x48..+0xa4 state.
    virtual void reset() noexcept;

    [[nodiscard]] float scale48() const noexcept { return scale48_; }
    [[nodiscard]] float scale5c() const noexcept { return scale5c_; }
    [[nodiscard]] float scale70() const noexcept { return scale70_; }
    [[nodiscard]] float scale84() const noexcept { return scale84_; }
    [[nodiscard]] std::uint64_t stateA4() const noexcept { return stateA4_; }
    [[nodiscard]] bool derived_flag88() const noexcept { return flag88_; }
    [[nodiscard]] bool derived_flag89() const noexcept { return flag89_; }
    [[nodiscard]] bool derived_flag8a() const noexcept { return flag8a_; }
    [[nodiscard]] bool owns_resource() const noexcept { return owns_resource_; }
    [[nodiscard]] std::uint64_t tracked98() const noexcept { return tracked98_; }

    // Fixture hook that models the original +0x98 resource field.
    void set_tracked98(std::uint64_t value) noexcept { tracked98_ = value; }

private:
    void initialize_derived_state() noexcept;

    float scale48_;
    std::uint8_t block4c_[16];
    float scale5c_;
    std::uint8_t block60_[16];
    float scale70_;
    std::uint8_t block74_[16];
    float scale84_;
    std::uint8_t flag88_;
    std::uint8_t flag89_;
    std::uint8_t flag8a_;
    [[maybe_unused]] std::uint8_t reserved8b_[5];
    [[maybe_unused]] std::uint64_t tracked90_;
    std::uint64_t tracked98_;
    std::uint8_t owns_resource_;
    [[maybe_unused]] std::uint8_t reservedA1_[3];
    std::uint64_t stateA4_;
};

// Native counterpart of the published original descriptor at global 0x3356678. The original global
// stored the vtable address point; the native model publishes ordinary metadata plus native
// construction hooks, so no original executable address participates in dispatch.
struct ClothingPreviewParamDescriptor {
    const char* type_name;
    std::size_t object_size;
    std::size_t object_alignment;
    ClothingPreviewParam* (*construct)(void* storage, ResourceOwner* owner) noexcept;
    void (*destroy)(ClothingPreviewParam* object) noexcept;
};

extern const ClothingPreviewParamDescriptor kClothingPreviewParamDescriptor;

// Original RVA 0x00f09e60 (constructor slot 10): constructs and destroys a stack-local instance,
// publishes the descriptor, then creates the once-guarded recursive/priority-inherit mutex.
// The original also registers an atexit cleanup for that mutex; StartupState owns it through RAII
// instead (settled user decision). Returns false only when native mutex initialization fails.
[[nodiscard]] bool init_clothing_preview_descriptor(StartupState& state) noexcept;

}  // namespace sotc
