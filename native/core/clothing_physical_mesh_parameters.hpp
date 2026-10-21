// Native semantic model of the class whose original vtable address point is RVA 0x15fa780
// (vtable symbol base 0x15fa770): "ClothingPhysicalMeshParameters", constructor-table slot 11.
//
// Recovered original facts (ELF sha256 d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3;
// docs/checkpoints/2026-10-20-root-cpu01-ctor11-structural-analysis.md and
// 2026-10-21-agentG-ctor11-reflection-scope.md; private packets under local/re/evidence/):
//   slot 11 body            0x00f09160 (281 bytes)
//   class constructor       0x00f06410 (1021 bytes; base ctor 0x00e1eda0, vptr 0x15fa780)
//   complete-object dtor    0x00f069f0 (-> block release 0x00f06a40, base dtor 0x00e1ede0)
//   reset (vslot 3)         0x00f08f00
//   factory create / finish 0x00ed5750 (allocates 0x1b8 bytes, align 8) / 0x00ed5880
//   published vptr global   0x3356068; mutex handle 0x3356078; lazy-schema flag/table 0x3356070/0x3356080
//   type name               "ClothingPhysicalMeshParameters" (error literal at RVA 0x1322b23)
//
// Object layout: base part 0x48 bytes (ClothingParamBase) followed by a 0x170-byte data struct at +0x48
// holding scalars and TWELVE dynamic-array blocks {ptr, u8 owns, u32 stride, u32 count} (0x18 bytes each).
// Block bases/strides are byte-verified against the constructor stores; the element sizes match, in
// order, the array nodes of the reflection schema. The meaning (names) of individual scalars and arrays
// is NOT recovered here and is deliberately not invented: fields are named by original object offset.
//
// This is a semantic native model, not a binary-compatible reconstruction of the original vtable. The
// original 74-record reflection schema (built lazily by 0xf06cd0) is NOT modelled; see
// LazyParamDefTree, whose get() reports schema_not_available.
#pragma once

#include <cstddef>
#include <cstdint>

#include "clothing_preview_param.hpp"   // ResourceOwner, ClothingParamBase
#include "lazy_param_def_tree.hpp"

namespace sotc {

struct StartupState;

// One dynamic-array block (original: +0 ptr, +8 u8 owns, +0xc u32 stride, +0x10 u32 count).
struct ParamArrayBlock {
    void* data;                       // caller-provided storage; native code never allocates it
    std::uint8_t owns;                // 1 after default init: data is released through the owner on teardown
    std::uint8_t reserved_pad_[3];
    std::uint32_t stride;             // element size in bytes
    std::uint32_t count;
    std::uint32_t reserved_tail_;
};
static_assert(sizeof(ParamArrayBlock) == 0x18);
static_assert(alignof(ParamArrayBlock) == 8);

// Data struct that starts at object offset +0x48. Member comments give ORIGINAL OBJECT offsets.
struct PhysicalMeshData {
    std::uint32_t w48, w4c;           // zeroed by ctor/reset
    std::uint32_t w50;                // never written by the original ctor/reset (zero here)
    std::uint32_t w54, w58, w5c;      // zeroed (16-byte store 0x54..0x63; granularity inferred)
    ParamArrayBlock b60;              // stride 12
    ParamArrayBlock b78;              // stride 12
    ParamArrayBlock b90;              // stride 12
    ParamArrayBlock ba8;              // stride 12
    ParamArrayBlock bc0;              // stride 2
    ParamArrayBlock bd8;              // stride 4
    ParamArrayBlock bf0;              // stride 1
    std::uint8_t f108, f109;          // zeroed byte flags
    std::uint8_t reserved_10a_[6];
    ParamArrayBlock b110;             // stride 4
    std::uint32_t w128, w12c, w130, w134;   // zeroed (16-byte store at 0x128)
    std::uint16_t h138;               // zeroed
    std::uint8_t reserved_13a_[6];
    ParamArrayBlock b140;             // stride 0x28
    ParamArrayBlock b158;             // stride 0x34
    std::uint32_t w170, w174;         // zeroed
    ParamArrayBlock b178;             // stride 0x28
    ParamArrayBlock b190;             // stride 0x34
    std::uint32_t w1a8, w1ac, w1b0;   // zeroed
    std::uint8_t reserved_1b4_[4];
};

// Original object offset of a data-struct member (data struct begins at +0x48).
inline constexpr std::size_t kPhysicalMeshDataOffset = 0x48;
#define SOTC_PMESH_OBJ_OFFSET(member) (kPhysicalMeshDataOffset + offsetof(PhysicalMeshData, member))
static_assert(sizeof(PhysicalMeshData) == 0x1b8 - 0x48);
static_assert(alignof(PhysicalMeshData) == 8);
static_assert(SOTC_PMESH_OBJ_OFFSET(w48) == 0x48 && SOTC_PMESH_OBJ_OFFSET(w5c) == 0x5c);
static_assert(SOTC_PMESH_OBJ_OFFSET(b60) == 0x60 && SOTC_PMESH_OBJ_OFFSET(b78) == 0x78);
static_assert(SOTC_PMESH_OBJ_OFFSET(b90) == 0x90 && SOTC_PMESH_OBJ_OFFSET(ba8) == 0xa8);
static_assert(SOTC_PMESH_OBJ_OFFSET(bc0) == 0xc0 && SOTC_PMESH_OBJ_OFFSET(bd8) == 0xd8);
static_assert(SOTC_PMESH_OBJ_OFFSET(bf0) == 0xf0 && SOTC_PMESH_OBJ_OFFSET(f108) == 0x108);
static_assert(SOTC_PMESH_OBJ_OFFSET(f109) == 0x109 && SOTC_PMESH_OBJ_OFFSET(b110) == 0x110);
static_assert(SOTC_PMESH_OBJ_OFFSET(w128) == 0x128 && SOTC_PMESH_OBJ_OFFSET(h138) == 0x138);
static_assert(SOTC_PMESH_OBJ_OFFSET(b140) == 0x140 && SOTC_PMESH_OBJ_OFFSET(b158) == 0x158);
static_assert(SOTC_PMESH_OBJ_OFFSET(w170) == 0x170 && SOTC_PMESH_OBJ_OFFSET(w174) == 0x174);
static_assert(SOTC_PMESH_OBJ_OFFSET(b178) == 0x178 && SOTC_PMESH_OBJ_OFFSET(b190) == 0x190);
static_assert(SOTC_PMESH_OBJ_OFFSET(w1a8) == 0x1a8 && SOTC_PMESH_OBJ_OFFSET(w1b0) == 0x1b0);
#undef SOTC_PMESH_OBJ_OFFSET

class ClothingPhysicalMeshParameters : public ClothingParamBase {
public:
    static constexpr std::size_t kObjectSize = 0x1b8;
    static constexpr std::size_t kObjectAlignment = 8;
    static constexpr const char* kTypeName = "ClothingPhysicalMeshParameters";
    static constexpr std::size_t kBlockCount = 12;
    // Original object offsets and element strides of the twelve array blocks, in object order.
    static constexpr std::uint32_t kBlockOffsets[kBlockCount] = {0x60,  0x78,  0x90,  0xa8,  0xc0,  0xd8,
                                                                  0xf0,  0x110, 0x140, 0x158, 0x178, 0x190};
    static constexpr std::uint32_t kBlockStrides[kBlockCount] = {12, 12, 12, 12, 2, 4, 1, 4, 40, 52, 40, 52};

    // Mirrors 0xf06410(this, owner, field20, field28): the default state is installed only on the
    // field20 == 0 path. For field20 != 0 the original leaves the data struct as the caller zeroed it
    // (slot 11's stack probe zeroes it first), so the native object is entirely zero there.
    explicit ClothingPhysicalMeshParameters(ResourceOwner* owner = nullptr, std::uint64_t field20 = 0,
                                            std::uint64_t field28 = 0) noexcept;
    ~ClothingPhysicalMeshParameters() noexcept override;

    ClothingPhysicalMeshParameters(const ClothingPhysicalMeshParameters&) = delete;
    ClothingPhysicalMeshParameters& operator=(const ClothingPhysicalMeshParameters&) = delete;
    ClothingPhysicalMeshParameters(ClothingPhysicalMeshParameters&&) = delete;
    ClothingPhysicalMeshParameters& operator=(ClothingPhysicalMeshParameters&&) = delete;

    // Original vslot 3 = 0x00f08f00: release owned blocks, then restore the default state.
    virtual void reset() noexcept;

    [[nodiscard]] const PhysicalMeshData& data() const noexcept { return data_; }
    // nullptr when index >= kBlockCount.
    [[nodiscard]] const ParamArrayBlock* block(std::size_t index) const noexcept;

    // Fixture hook that models a populated array block (original stores the pointer from elsewhere).
    // Returns false for an out-of-range index.
    bool set_block_data(std::size_t index, void* data, std::uint32_t count) noexcept;

private:
    [[nodiscard]] ParamArrayBlock* mutable_block(std::size_t index) noexcept;
    void release_blocks() noexcept;          // original 0x00f06a40
    void initialize_default_state() noexcept;

    PhysicalMeshData data_;
};

static_assert(sizeof(ClothingPhysicalMeshParameters) == ClothingPhysicalMeshParameters::kObjectSize);
static_assert(alignof(ClothingPhysicalMeshParameters) == ClothingPhysicalMeshParameters::kObjectAlignment);

// Schema builder placeholder for the lazy tree (original builder 0x00f06cd0). The 74 records, their
// names and hints are out of scope: always reports schema_not_available and never allocates.
[[nodiscard]] ParamTreeError build_clothing_physical_mesh_schema(std::unique_ptr<ParamDefTree>& out) noexcept;

// Native counterpart of the published vptr at global 0x3356068 (ordinary metadata plus construction
// hooks). The lazy schema tree is reached through StartupState::physmesh_tree because it needs the
// state-owned mutex; a const global cannot reference it.
struct ClothingPhysicalMeshParametersDescriptor {
    const char* type_name;
    std::size_t object_size;
    std::size_t object_alignment;
    ClothingPhysicalMeshParameters* (*construct)(void* storage, ResourceOwner* owner) noexcept;
    void (*destroy)(ClothingPhysicalMeshParameters* object) noexcept;
};

extern const ClothingPhysicalMeshParametersDescriptor kClothingPhysicalMeshParametersDescriptor;

// Original RVA 0x00f09160 (constructor slot 11): constructs and destroys a stack-local object with
// (owner=null, field20=0x10, field28=0x10), publishes the descriptor and creates the owned recursive +
// priority-inherit mutex through StartupState::physmesh_registration. The lazy schema tree stays
// unbuilt (the original never touches flag 0x3356070 here). Idempotent for the same descriptor.
// Returns false and sets state.init_failed when publication fails.
[[nodiscard]] bool init_clothing_physical_mesh_descriptor(StartupState& state) noexcept;

}  // namespace sotc
