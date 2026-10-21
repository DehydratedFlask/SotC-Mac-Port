#include "clothing_physical_mesh_parameters.hpp"

#include <array>
#include <cstring>
#include <new>

#include "startup_ctors.hpp"

namespace sotc {

namespace {

ClothingPhysicalMeshParameters* construct_clothing_physical_mesh_parameters(void* storage,
                                                                           ResourceOwner* owner) noexcept {
    if (storage == nullptr) return nullptr;
    return new (storage) ClothingPhysicalMeshParameters(owner);
}

void destroy_clothing_physical_mesh_parameters(ClothingPhysicalMeshParameters* object) noexcept {
    if (object != nullptr) object->~ClothingPhysicalMeshParameters();
}

template <class Data>
auto block_at(Data& d, std::size_t index) noexcept -> decltype(&d.b60) {
    const auto blocks = std::array{
        &d.b60, &d.b78, &d.b90, &d.ba8, &d.bc0, &d.bd8, &d.bf0, &d.b110, &d.b140, &d.b158, &d.b178, &d.b190};
    static_assert(std::tuple_size_v<decltype(blocks)> == ClothingPhysicalMeshParameters::kBlockCount);
    return index < ClothingPhysicalMeshParameters::kBlockCount ? blocks[index] : nullptr;
}

}  // namespace

ClothingPhysicalMeshParameters::ClothingPhysicalMeshParameters(ResourceOwner* owner, std::uint64_t field20,
                                                               std::uint64_t field28) noexcept
    : ClothingParamBase(owner, field20, field28), data_{} {
    // Original 0xf06410 installs the defaults only when field20 == 0 (the "test r14,r14; jne" skip).
    if (field20 == 0) initialize_default_state();
}

ClothingPhysicalMeshParameters::~ClothingPhysicalMeshParameters() noexcept {
    // Original 0xf069f0: block release (0xf06a40), then the base destructor (runs after this body).
    release_blocks();
}

void ClothingPhysicalMeshParameters::reset() noexcept {
    // Original 0xf08f00.
    release_blocks();
    initialize_default_state();
}

const ParamArrayBlock* ClothingPhysicalMeshParameters::block(std::size_t index) const noexcept {
    return block_at(data_, index);   // nullptr when index >= kBlockCount
}

ParamArrayBlock* ClothingPhysicalMeshParameters::mutable_block(std::size_t index) noexcept {
    return block_at(data_, index);
}

bool ClothingPhysicalMeshParameters::set_block_data(std::size_t index, void* data,
                                                    std::uint32_t count) noexcept {
    ParamArrayBlock* b = mutable_block(index);
    if (b == nullptr) return false;
    b->data = data;
    b->count = count;
    return true;
}

void ClothingPhysicalMeshParameters::release_blocks() noexcept {
    // Original 0xf06a40: for each of the twelve blocks (in object order), when the owns byte is
    // non-zero and the pointer is non-null, release the pointer through the owner's [vtbl+0xc0] hook.
    // The original does not clear the pointer here; callers (reset, ctor) overwrite it afterwards.
    if (owner() == nullptr) return;
    for (std::size_t i = 0; i < kBlockCount; ++i) {
        const ParamArrayBlock* b = block_at(data_, i);
        if (b->owns != 0 && b->data != nullptr) owner()->release_derived(b->data);
    }
}

void ClothingPhysicalMeshParameters::initialize_default_state() noexcept {
    // Mirrors the final stores of 0xf06410 (default path) and 0xf08f00.
    data_.w48 = data_.w4c = 0;
    data_.w54 = data_.w58 = data_.w5c = 0;
    data_.f108 = data_.f109 = 0;
    data_.w128 = data_.w12c = data_.w130 = data_.w134 = 0;
    data_.h138 = 0;
    data_.w170 = data_.w174 = 0;
    data_.w1a8 = data_.w1ac = data_.w1b0 = 0;
    for (std::size_t i = 0; i < kBlockCount; ++i) {
        ParamArrayBlock* b = block_at(data_, i);
        b->data = nullptr;
        b->owns = 1;
        b->stride = kBlockStrides[i];
        b->count = 0;
    }
}

ParamTreeError build_clothing_physical_mesh_schema(std::unique_ptr<ParamDefTree>& out) noexcept {
    // The original builder (0xf06cd0) allocates 74 x 0x98-byte records and fills them with names and
    // hints taken from the executable. Those strings are not supplied to the native build, so nothing
    // is built, nothing is allocated and the failure is explicit.
    out.reset();
    return ParamTreeError::schema_not_available;
}

const ClothingPhysicalMeshParametersDescriptor kClothingPhysicalMeshParametersDescriptor{
    ClothingPhysicalMeshParameters::kTypeName,
    ClothingPhysicalMeshParameters::kObjectSize,
    ClothingPhysicalMeshParameters::kObjectAlignment,
    &construct_clothing_physical_mesh_parameters,
    &destroy_clothing_physical_mesh_parameters,
};

bool init_clothing_physical_mesh_descriptor(StartupState& state) noexcept {
    // Original 0x00f09160 builds and tears down one stack-local instance (zeroed stack, ctor
    // (null, 0x10, 0x10), slot-0 dtor) before publishing. field20 != 0 skips default initialization.
    {
        ClothingPhysicalMeshParameters probe(nullptr, 0x10, 0x10);
        (void)probe;
    }

    const DescriptorPublishError error =
        state.physmesh_registration.publish(&kClothingPhysicalMeshParametersDescriptor);
    if (error == DescriptorPublishError::none) return true;
    if (error == DescriptorPublishError::already_published &&
        state.physmesh_registration.descriptor() == &kClothingPhysicalMeshParametersDescriptor) {
        return true;   // idempotent re-run of the same slot
    }
    state.init_failed = true;
    return false;
}

}  // namespace sotc
