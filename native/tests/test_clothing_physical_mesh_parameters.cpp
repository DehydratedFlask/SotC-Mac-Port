// ClothingPhysicalMeshParameters (constructor-table slot 11, RVA 0xf09160): object model, descriptor,
// slot-11 init callback, lazy schema tree. Does not claim engine startup.
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <new>
#include <string>
#include <type_traits>

#include "core/clothing_physical_mesh_parameters.hpp"
#include "core/startup_ctors.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

struct RecordingOwner final : sotc::ResourceOwner {
    int derived_calls = 0;
    int base_calls = 0;
    void* released[16] = {};
    void release_derived(void* pointer) noexcept override {
        if (derived_calls < 16) released[derived_calls] = pointer;
        ++derived_calls;
    }
    void release_base(void*) noexcept override { ++base_calls; }
};

}  // namespace

int main() {
    using namespace sotc;
    using Obj = ClothingPhysicalMeshParameters;

    static_assert(sizeof(Obj) == 0x1b8 && alignof(Obj) == 8);
    static_assert(std::has_virtual_destructor_v<Obj>);
    static_assert(sizeof(PhysicalMeshData) == 0x170);

    // Default state: strides installed, owns=1, pointers/counts zero, scalars zero.
    {
        Obj o;
        for (std::size_t i = 0; i < Obj::kBlockCount; ++i) {
            const ParamArrayBlock* b = o.block(i);
            CHECK(b != nullptr);
            CHECK(b->data == nullptr && b->owns == 1 && b->count == 0);
            CHECK(b->stride == Obj::kBlockStrides[i]);
        }
        CHECK(o.block(Obj::kBlockCount) == nullptr);
        const PhysicalMeshData& d = o.data();
        CHECK(d.w48 == 0 && d.w4c == 0 && d.w50 == 0 && d.w54 == 0 && d.w58 == 0 && d.w5c == 0);
        CHECK(d.f108 == 0 && d.f109 == 0 && d.h138 == 0 && d.w170 == 0 && d.w1a8 == 0 && d.w1b0 == 0);
        CHECK(o.base_flag40() && o.base_flag41() && o.base_flag42());   // field20 == 0 base path
    }

    // Block offsets and strides are the verified original values (memo/evidence packet).
    {
        const std::uint32_t offsets[] = {0x60, 0x78, 0x90, 0xa8, 0xc0, 0xd8, 0xf0, 0x110, 0x140, 0x158, 0x178, 0x190};
        const std::uint32_t strides[] = {12, 12, 12, 12, 2, 4, 1, 4, 0x28, 0x34, 0x28, 0x34};
        Obj o;
        const auto* base = reinterpret_cast<const unsigned char*>(&o);
        for (std::size_t i = 0; i < Obj::kBlockCount; ++i) {
            CHECK(Obj::kBlockOffsets[i] == offsets[i] && Obj::kBlockStrides[i] == strides[i]);
            // The actual in-memory position of the block equals the original object offset.
            CHECK(reinterpret_cast<const unsigned char*>(o.block(i)) == base + offsets[i]);
        }
    }

    // field20 != 0 (slot 11 probe): original skips default init; the zeroed storage stays zero.
    {
        Obj o(nullptr, 0x10, 0x10);
        for (std::size_t i = 0; i < Obj::kBlockCount; ++i) {
            const ParamArrayBlock* b = o.block(i);
            CHECK(b->data == nullptr && b->owns == 0 && b->stride == 0 && b->count == 0);
        }
        CHECK(!o.base_flag40() && !o.base_flag41() && !o.base_flag42());
        CHECK(o.field20() == 0x10 && o.field28() == 0x10);
    }

    // Destructor releases owned, non-null blocks through the owner hook in object order, once each.
    {
        RecordingOwner owner;
        int cells[3] = {};
        {
            Obj o(&owner);
            CHECK(o.set_block_data(0, &cells[0], 4));
            CHECK(o.set_block_data(5, &cells[1], 2));
            CHECK(o.set_block_data(11, &cells[2], 1));
            CHECK(!o.set_block_data(Obj::kBlockCount, &cells[0], 1));
        }
        CHECK(owner.derived_calls == 3);
        CHECK(owner.released[0] == &cells[0] && owner.released[1] == &cells[1] && owner.released[2] == &cells[2]);
    }
    // Nothing populated: nothing released (also the slot-11 probe, which has a null owner).
    {
        RecordingOwner owner;
        { Obj o(&owner); }
        CHECK(owner.derived_calls == 0);
    }
    // owns == 0 blocks are not released (field20 != 0 path leaves owns clear).
    {
        RecordingOwner owner;
        int cell = 0;
        {
            Obj o(&owner, 0x10, 0x10);
            CHECK(o.set_block_data(2, &cell, 1));
        }
        CHECK(owner.derived_calls == 0);
    }

    // reset() releases populated blocks, then restores the default state (pointers cleared).
    {
        RecordingOwner owner;
        int cell = 0;
        Obj o(&owner);
        CHECK(o.set_block_data(8, &cell, 7));
        o.reset();
        CHECK(owner.derived_calls == 1 && owner.released[0] == &cell);
        CHECK(o.block(8)->data == nullptr && o.block(8)->count == 0 && o.block(8)->owns == 1);
        CHECK(o.block(8)->stride == 0x28);
        o.reset();
        CHECK(owner.derived_calls == 1);   // nothing left to release
    }

    // Descriptor metadata and construct/destroy hooks.
    {
        const ClothingPhysicalMeshParametersDescriptor& d = kClothingPhysicalMeshParametersDescriptor;
        CHECK(std::string(d.type_name) == "ClothingPhysicalMeshParameters");
        CHECK(d.object_size == 0x1b8 && d.object_alignment == 8);
        CHECK(d.construct != nullptr && d.destroy != nullptr);
        RecordingOwner owner;
        alignas(8) unsigned char storage[Obj::kObjectSize];
        Obj* o = d.construct(storage, &owner);
        CHECK(o == reinterpret_cast<Obj*>(storage));
        CHECK(o->block(4)->stride == 2);
        int cell = 0;
        CHECK(o->set_block_data(4, &cell, 1));
        d.destroy(o);
        CHECK(owner.derived_calls == 1);
        CHECK(d.construct(nullptr, &owner) == nullptr);
        d.destroy(nullptr);   // must be a no-op
    }

    // Schema builder: explicit error, no tree.
    {
        std::unique_ptr<ParamDefTree> out;
        CHECK(build_clothing_physical_mesh_schema(out) == ParamTreeError::schema_not_available);
        CHECK(out == nullptr);
    }

    // Slot-11 init callback: publishes the descriptor, owns an initialized recursive mutex, tree unbuilt.
    {
        StartupState state;
        CHECK(!state.physmesh_registration.published());
        CHECK(state.physmesh_registration.descriptor() == nullptr);
        // Before publication the mutex is not initialized, so the tree reports it explicitly.
        CHECK(state.physmesh_tree.get().error == ParamTreeError::mutex_unavailable);

        CHECK(init_clothing_physical_mesh_descriptor(state));
        CHECK(!state.init_failed);
        CHECK(state.physmesh_registration.published());
        CHECK(state.physmesh_registration.descriptor() == &kClothingPhysicalMeshParametersDescriptor);
        // Slot 10's state is untouched.
        CHECK(state.descriptor == nullptr && !state.descriptor_mutex_initialized);

        // Mutex usable and recursive.
        RecursiveMutex& m = state.physmesh_registration.mutex();
        CHECK(m.lock() == 0 && m.lock() == 0 && m.unlock() == 0 && m.unlock() == 0);

        // The lazy tree stays unbuilt and reports schema_not_available without allocating a tree.
        CHECK(!state.physmesh_tree.built());
        const ParamTreeResult r = state.physmesh_tree.get();
        CHECK(r.tree == nullptr && r.error == ParamTreeError::schema_not_available);
        CHECK(!state.physmesh_tree.built());

        // Second publish is refused by the registration (one-shot) ...
        CHECK(state.physmesh_registration.publish(&kClothingPhysicalMeshParametersDescriptor) ==
              DescriptorPublishError::already_published);
        // ... but re-running the slot callback is idempotent for the same descriptor.
        CHECK(init_clothing_physical_mesh_descriptor(state));
        CHECK(!state.init_failed);
    }

    // A different descriptor already published, or a pre-initialized mutex, makes the callback fail visibly.
    {
        StartupState state;
        static int other = 0;
        CHECK(state.physmesh_registration.publish(&other) == DescriptorPublishError::none);
        CHECK(!init_clothing_physical_mesh_descriptor(state));
        CHECK(state.init_failed);
        CHECK(state.physmesh_registration.descriptor() == &other);
    }
    {
        StartupState state;
        CHECK(state.physmesh_registration.mutex().initialize() == 0);   // publish() will now fail to init
        CHECK(!init_clothing_physical_mesh_descriptor(state));
        CHECK(state.init_failed && !state.physmesh_registration.published());
    }

    // Through the runner: slot 11 completes (slots 12-22 are registered too, 23 rows ran) and the runner
    // stops at the explicit blocker slot 23 / 0xe71990.
    {
        StartupState state;
        const CtorRunResult r = run_constructors(recovered_constructors(), state);
        CHECK(!r.ok() && !r.callback_failed);
        CHECK(r.completed == 23 && r.blocked_on_rva == 0xe71990);
        CHECK(state.physmesh_registration.descriptor() == &kClothingPhysicalMeshParametersDescriptor);
        CHECK(state.physmesh_tree.get().error == ParamTreeError::schema_not_available);
    }

    std::puts("clothing_physical_mesh_parameters: all checks passed");
    return 0;
}
