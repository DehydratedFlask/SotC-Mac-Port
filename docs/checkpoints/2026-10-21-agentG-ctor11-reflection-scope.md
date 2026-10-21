# Checkpoint: CPU-01-CTOR11-REFLECTION-SCOPE-01 — slot 11 can be registered with a native lazy-record descriptor

- Time / timezone: 2026-10-21 (America/Chicago), macOS 27.0.1 arm64. Agent G (sole Ghidra/REA user this
  session). Integrator files (`HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `INSTALL_LOG.md`),
  `native/` and `launcher/` were **not** edited; nothing committed or pushed.
- Task / status: CPU-01-CTOR11-REFLECTION-SCOPE-01 **STATIC_ANALYSIS_VERIFIED** (analysis only). 0 failed
  tries, so no escalation file. G0 PARTIAL; G1–G6 NOT RUN. This does not register slot 11 and does not
  claim engine startup.
- Input: private `local/re/eboot.elf` sha256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; constructor table `0x2dd70e0`
  (573 entries, sha256 `5917cb85…3ab28`, re-asserted). Ghidra 12.1.4, persistent project
  `local/ghidra/persistent-20261020`, read-only `-noanalysis`, no re-import.

## Decision

**Slot 11 (`0xf09160`, `ClothingPhysicalMeshParameters`) can be registered with a native lazy-record
descriptor.** Registration needs neither the 74 reflection records nor any PhysX/APEX code. The records
are a parameter *schema* built on first use by a single builder (`0xf06cd0`) that is reachable only
through two virtual getters of the class; slot 11's body, its constructor, and the stack probe's
destructor never touch the records, the built flag, or the table pointer.

There is no missing contract that blocks registration. One non-blocking principal decision is recorded
at the end (provenance of the schema names, only needed if the lazy tree is ever materialised).

## Observations (byte-verified by `verify.py`, 667/667 assertions, plus Ghidra exports)

1. **Records.** The builder allocates `0x2bf0` = 74 × `0x98` through the owning object's allocator
   (`[[this+8]+0xb0]`), runs record ctor `0xe1de80` in one 74-iteration loop, then a straight-line list of
   234 calls: 74 `init(name,type,struct-name)` (`0xe1df60`), 19 hint ctor/init (`0xe1d9c0`/`0xe1da60`),
   19 set-hints (vslot 6), 19 set-children (vslot 34), 12 set-array-size(-1) (vslot 28), 12 set `+0x84`
   (`0xe1e250`, value `0x10`), 19 `__cxa_guard` pairs and 19 `__cxa_atexit` registrations (per-hint
   stubs `0xf08dd0 + 0x10k`), then sets the built flag `0x3356070`.
2. **One 0x98 record:** `+0x00` vptr (`0x15e2c28`), `+0x08` u8 static flag, `+0x10/+0x18` name/long-name
   pointers, `+0x20` struct-name pointer, `+0x28` u32 type, `+0x2c` i32 array size, `+0x30` parent,
   `+0x38/+0x40` child count/array, `+0x48/+0x50` hint count/array, `+0x58..+0x77` two unused
   count/pointer pairs, `+0x78` allocator pointer, `+0x80` long-name-owned flag, `+0x84` u32,
   `+0x88..+0x97` zero.
3. **Tree shape:** root + 73 nodes, depth 4, root has 10 children; 7 struct nodes; 12 dynamic arrays each
   with exactly one element child; type codes {1 array, 2 struct, 3 bool ×4, 11 ×1, 12 ×1, 13 ×25,
   15 ×11, 18 ×13}; 19 hint objects in 3 kinds (16/2/1 uses). Names, struct names and hint strings are
   original content and stay in the private packet.
4. **Schema equals the object's typed field list.** The 12 array element sizes
   `12,12,12,12,2,4,1,4,40,52,40,52` equal, in order, the strides stored by the object ctor `0xf06410` for
   its twelve array blocks (bases `0x60,0x78,0x90,0xa8,0xc0,0xd8,0xf0,0x110,0x140,0x158,0x178,0x190`).
   Object size `0x1b8`, align 8, base part `0x48` (factory create `0xed5750`).
5. **A second, constant structure:** `0x15fae70..0x15fb7b0` = 74 × 32-byte `.data` entries
   (`type | arrayflag<<32, object offset, struct-name ptr, child count`), identical types by index,
   array offsets `+0x48` = the block bases above. No builder, no atexit; consumed by class vslot 124
   (`0xf08cf0` → `0xe36230`). It ends exactly at the next class's vtable header.
6. **Who can reach the builder.** Builder call sites: `0xf06cb0` (in getter `0xf06c80`) and `0xf08bf0`
   (in getter `0xf08bc0`) only. The getters have **no direct callers**; they exist solely as vtable cells
   `0x15fab68` (slot 125) and `0x15fab70` (slot 126) of vtable `0x15fa780`, reached through the shared
   trampolines `0xe205a0`/`0xe205b0` (vtable slots 15/16, `mov rax,[rdi]; jmp [rax+0x3e8|0x3f0]`) by the
   handle helpers `0xe205c0`/`0xe20650` (34 direct callers each; one pair per parameter class is
   inference from the count matching the 34-slot family).
7. **Free path / factory.** `0xf08d30` (destroy 74 nodes, free, clear flag) has one caller, the factory
   vtable entry `0xed5990` (cell `0x15f3258`). Factory vtable `0x15f3218`: create `0xed5750`, finish
   `0xed5880`. The 10 factories are made and registered by `0xed4270` in the runtime module-init chain
   `0x533100 → 0x2569d0 → 0xa3da30 → 0xed1350 → 0xed13e0 → 0xed4270`; none of it is in the direct-call
   closure of the constructor table.
8. **Global-cell isolation (22,624,632-byte rel32/rip-relative scan).** `0x3356068` (vptr) is touched only
   by `0xf09160` and `0xed5880`; `0x3356070/0x3356078/0x3356080` (flag, mutex handle, table pointer) only
   by `0xf09160` (mutex only), `0xf06c80`, `0xf08bc0`, `0xf08d30`, `0xf06cd0`; spare cell `0x3356088`
   by nobody.
9. **Startup closure.** The direct-call closure of `{0xf09160, 0xf06410, 0xf069f0}` is 16 functions and
   contains none of: builder, getters, record ctor/setters, hint code, handle helpers, free routine,
   `0xe368c0`. The direct closure of all 573 constructor entries (1,317 functions) also contains none.
   The slot-11 ctor call passes `rdx = 0x10` (non-zero), so `test r14,r14; jne 0xf06808` skips the
   default-initialisation branch: the probe is the bare base ctor `0xe1eda0` + vptr store, then the slot-0
   destructor (`0xf069f0` → `0xf06a40` + `0xe1ede0`; flags are zero so no allocator is dereferenced).
10. **Locking shape (getters).** Double-checked: test flag; lock handle `0xb309e0`; re-test; build; unlock
    `0xb30a00`; return table pointer. The flag is a plain byte (not atomic) in the original.

Key body hashes (full list in `verification.json`; sizes are Ghidra extents):

| RVA | bytes | role | sha256 |
|---|---|---|---|
| `0xf09160` | 281 | slot 11 | `adb91b08884c8fc861cc37b1653e2496a0aaf8efa883222e2d602e8e6d444ef0` |
| `0xf06410` | 1021 | class ctor | `086c93ce9f3d9ae834aea2d1641bc6e791d5824427002a81176c8d46ee57a5d5` |
| `0xf069f0` | 44 | slot-0 dtor | `d2e9246d8ca47ea76b9b46e68c1e9942e0a0a9d9dccf6fd7666d6d5d0a894e3c` |
| `0xf06cd0` | 7911 | record builder | `afedaa3db9cf32cfd7913486003284278b7624a36aef94997d15c6e5ec5711ac` |
| `0xf06c80` | 79 | getter (slot 125) | `66d60776aed9aafdba1d5c7100b92d05c8e4c60bf6872a35836899a963500390` |
| `0xf08bc0` | 79 | getter (slot 126) | `23f4ed45e2e967c2e23b07e31c36cb04dfd1bc354b13638be4569b077c533703` |
| `0xf08d30` | 145 | table free | `722853c0864dc00cc4cb8684cffac468a10434c6d3df38971e475cba300ede2c` |
| `0xe1de80` | 118 | record ctor | `99a8ff6ff923c1c9e2705632294ef72ebbad3c226f1076aeafd63d5d8d419ec0` |
| `0xed5750` | 289 | factory create | `bd4a7b573b2f6c49955eb847d80dfdb9ca077b100a1c935c4899f8425cb5d1e0` |
| `0xed5880` | 161 | factory finish | `0a4722aaf5a7928ab99f585f07245722a6cbb394070fa6827165c25c9b214145` |

## Inference (not proven)

- The record type is an `NvParameterized::DefinitionImpl`-style node and the 12 setters map to
  `init/setArraySize/setChildren/setHints/setAlignment`; the tree is lazily built per class, like the
  slot-10 accessor `0xf09a90`. Type-code names (array/struct/bool/u8/u16/u32/f32/vec3) come from the
  verified size cross-check, not from a symbol table. The same lazy-schema shape is expected in every
  member of the 34-slot family (slot 10 shares the accessor shape; not re-verified per slot here).
- Whether the shipped game ever calls the getters at runtime is **NOT RUN / unproven** (static
  indirect-call coverage: callers of `[vptr+0x78|0x80]` were not enumerated, the displacement is far too
  common to scan). The registration contract does not depend on it.

## Native interface sketch (proposal; nothing created)

```cpp
// native/core/clothing_physical_mesh_parameters.{hpp,cpp}  -- PROPOSED
namespace sotc {

// Object model (ordinary C++): base ClothingParamBase (0x48 bytes) + embedded data struct at +0x48,
// 12 dynamic-array blocks {ptr,u8 owns,u32 stride,u32 count} at the verified bases/strides above.
class ClothingPhysicalMeshParameters final : public ClothingParamBase {
public:
    static constexpr std::size_t kObjectSize = 0x1b8;
    static constexpr std::size_t kObjectAlignment = 8;
    static constexpr const char* kTypeName = "ClothingPhysicalMeshParameters";
    explicit ClothingPhysicalMeshParameters(ResourceOwner* owner = nullptr, std::uint64_t field20 = 0,
                                            std::uint64_t field28 = 0) noexcept;
    ~ClothingPhysicalMeshParameters() noexcept override;
    virtual void reset() noexcept;                     // original vslot 3 = 0xf08f00
};

// Lazy schema. Registration never calls it.
enum class ParamTreeError : std::uint8_t { none, schema_not_available, allocation_failed };
struct ParamTreeResult { const struct ParamDefTree* tree; ParamTreeError error; };
struct ParamDefTree;                                   // 74 nodes; layout decided with the schema data policy

class LazyParamDefTree final {                         // original: flag 0x3356070, handle 0x3356078,
public:                                                // table 0x3356080, builder 0xf06cd0, free 0xf08d30
    using BuildFn = ParamTreeError (*)(ParamDefTree& out) noexcept;
    LazyParamDefTree(RecursiveMutex& mutex, BuildFn build) noexcept;
    [[nodiscard]] ParamTreeResult get() noexcept;      // getters 0xf06c80/0xf08bc0: DCL, std::atomic<bool> built
    void release() noexcept;                           // 0xf08d30: destroy nodes, built=false
private:
    RecursiveMutex& mutex_;
    BuildFn build_;
    std::atomic<bool> built_{false};
    std::unique_ptr<ParamDefTree> tree_;
};

struct ClothingPhysicalMeshParametersDescriptor {      // native stand-in for the published vptr 0x3356068
    const char* type_name;
    std::size_t object_size, object_alignment;
    ClothingPhysicalMeshParameters* (*construct)(void* storage, ResourceOwner* owner) noexcept;
    void (*destroy)(ClothingPhysicalMeshParameters*) noexcept;
    LazyParamDefTree* tree;                            // may report schema_not_available; never null
};
extern const ClothingPhysicalMeshParametersDescriptor kClothingPhysicalMeshParametersDescriptor;

// StartupState additions:  DescriptorRegistration physmesh_registration;  LazyParamDefTree physmesh_tree;
// Slot 11 (RVA 0xf09160):
[[nodiscard]] bool init_clothing_physical_mesh_descriptor(StartupState& state) noexcept {
    { ClothingPhysicalMeshParameters probe(nullptr, 0x10, 0x10); (void)probe; }   // original: ctor+dtor probe
    return state.physmesh_registration.publish(&kClothingPhysicalMeshParametersDescriptor)
           == DescriptorPublishError::none;       // owns the recursive PI mutex (= original handle)
}   // built flag stays false, tree stays unbuilt, no allocation of records
}
```

`get()` must return `schema_not_available` (a visible error, not an empty tree) until the schema data is
supplied, so nothing fakes success. `startup_ctors.cpp` slot 11 changes from `nullptr` to the callback
above; `check_ctor_slots.py` then expects a non-blocker at index 11.

## Remaining risks / non-blocking principal decision

- **Schema provenance.** The 74 names/struct names/hint strings are executable-derived content (same
  question as the `0xe71990` name table). Needed only if a native consumer of the generic handle/
  serialisation path exists. A native cloth-asset reader that works on typed structs (the object model
  above) never consumes it. Decide before materialising `ParamDefTree`; do not block slot 11 on it.
- Slot 12 is the next unregistered slot in order; its prerequisites are the vtable-`0x15f9ef0` class and
  the descriptor family (see `2026-10-21-root-cpu01-slot12-struct.md`), unchanged by this result.
- Serializer-side callers of the tree getters are not enumerated (coverage gap above).

## Evidence

- Private packet: `local/re/evidence/cpu01-ctor11-reflection-scope-01/` — `report.md`, `verify.py`,
  `verification.json`, `records.private.json` (private names), `mutation_results.json`, `command.txt`,
  `SHA256SUMS`, Ghidra exports `ghidra-q1..q4-*.txt`, scripts `ghidra-scripts/ReflScope{1..4}.java`.
- Logs: `local/checkpoints/agentG-20261021-reflscope-q{1..4}{,-console}.log`.
- Verify: `PYTHONDONTWRITEBYTECODE=1 python3 -I local/re/evidence/cpu01-ctor11-reflection-scope-01/verify.py`
  → exit 0, 667/667. Five negative-control mutants of the verifier all fail (`mutation_results.json`).
- Ghidra: four read-only `-noanalysis` headless runs (`ReflScope1..4.java`), each exit 0.

## Recommended TASKS.md row and next step

`CPU-01-CTOR11-REFLECTION-SCOPE-01 | STATIC_ANALYSIS_VERIFIED | 0 failed tries | decision: slot 11 registrable
with descriptor + mutex only; 74-record schema is lazy/non-blocking | packet local/re/evidence/cpu01-ctor11-reflection-scope-01/`

Next exact native step: **CPU-01-NATIVE-SLOT11-01** — add `ClothingPhysicalMeshParameters` (object model +
descriptor), `init_clothing_physical_mesh_descriptor` on `DescriptorRegistration`, register index 11 in
`startup_ctors.cpp`, extend the ctor checker and add a test that publishes twice (expects
`already_published`), proves the mutex is usable, and asserts `LazyParamDefTree::get()` reports
`schema_not_available` without allocating. Then slot 12 becomes the first blocker.
