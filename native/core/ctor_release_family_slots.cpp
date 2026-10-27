#include "ctor_release_family_slots.hpp"

#include "fnv_path_cstr_162020.hpp"
#include "fnv_span_161f80.hpp"

namespace sotc {

namespace {

struct Ordinals {
    std::array<std::uint16_t, kReleaseFamilyStepCount> object{};
    std::array<std::uint16_t, kReleaseFamilyStepCount> dword{};
    std::size_t objects = 0;
    std::size_t dwords = 0;
};

constexpr Ordinals compute_ordinals() noexcept {
    Ordinals o{};
    for (std::size_t i = 0; i != kReleaseFamilyStepCount; ++i) {
        if (kReleaseFamilySteps[i].kind == ReleaseFamilyKind::name_object) {
            o.object[i] = static_cast<std::uint16_t>(o.objects++);
        } else {
            o.dword[i] = static_cast<std::uint16_t>(o.dwords++);
        }
    }
    return o;
}

constexpr Ordinals kOrdinals = compute_ordinals();
static_assert(kOrdinals.objects == kReleaseFamilyObjectCount, "object count pin");
static_assert(kOrdinals.dwords == kReleaseFamilyDwordCount, "dword count pin");

constexpr bool slots_cover_steps() noexcept {
    std::size_t cursor = 0;
    for (const ReleaseFamilySlot& s : kReleaseFamilySlots) {
        if (s.first != cursor) return false;
        cursor += s.count;
    }
    return cursor == kReleaseFamilyStepCount;
}
static_assert(slots_cover_steps(), "slot extents must tile the step table in order");

// `call 0xf590` on the stack view: through the hook, or the lifted inert release.
bool release_view(const ReleaseFamilyHooks& hooks, const SharedStr& view) noexcept {
    if (hooks.release_view != nullptr) return hooks.release_view(view);
    return release_inert_view_f590(view);
}

ReleaseFamilyResult block(ReleaseFamilyResult r, std::uint32_t step, ReleaseFamilyStage stage) noexcept {
    r.ok = false;
    r.blocked_step = step;
    r.stage = stage;
    return r;
}

}  // namespace

const char* release_family_stage_name(ReleaseFamilyStage stage) noexcept {
    switch (stage) {
        case ReleaseFamilyStage::none: return "none";
        case ReleaseFamilyStage::unknown_slot: return "unknown_slot";
        case ReleaseFamilyStage::bad_table: return "bad_table";
        case ReleaseFamilyStage::view_release: return "view_release";
        case ReleaseFamilyStage::atexit_record: return "atexit_record";
    }
    return "?";
}

std::size_t release_family_object_index(std::size_t step) noexcept {
    return step < kReleaseFamilyStepCount ? kOrdinals.object[step] : kReleaseFamilyObjectCount;
}

std::size_t release_family_dword_index(std::size_t step) noexcept {
    return step < kReleaseFamilyStepCount ? kOrdinals.dword[step] : kReleaseFamilyDwordCount;
}

std::size_t release_family_slot_position(unsigned slot) noexcept {
    for (std::size_t i = 0; i != kReleaseFamilySlotCount; ++i) {
        if (kReleaseFamilySlots[i].slot == slot) return i;
    }
    return kReleaseFamilySlotCount;
}

ReleaseFamilyResult init_release_family_slot(unsigned slot, ReleaseFamilyState& state,
                                             StartupAtexitLog& atexit_log,
                                             const ReleaseFamilyHooks& hooks) noexcept {
    ReleaseFamilyResult r{};
    r.slot = static_cast<std::uint16_t>(slot);
    const std::size_t pos = release_family_slot_position(slot);
    if (pos == kReleaseFamilySlotCount) {
        r = block(r, 0xffffffffu, ReleaseFamilyStage::unknown_slot);
        return r;
    }
    const ReleaseFamilySlot& desc = kReleaseFamilySlots[pos];
    if (desc.first + desc.count > kReleaseFamilyStepCount) {
        r = block(r, desc.first, ReleaseFamilyStage::bad_table);
        state.results[pos] = r;
        return r;
    }

    for (std::uint32_t i = desc.first; i != desc.first + desc.count; ++i) {
        const ReleaseFamilyStep& step = kReleaseFamilySteps[i];
        // The stack view every shape builds: {text, len, aux = 0, ctl = nullptr}.
        SharedStr view{};
        view.data = reinterpret_cast<const std::uint8_t*>(step.text);
        view.size = step.length;
        view.aux = 0;
        view.ctl = nullptr;

        switch (step.kind) {
            case ReleaseFamilyKind::name_object: {
                const std::size_t k = release_family_object_index(i);
                ReleaseFamilyNameObject& obj = state.objects[k];
                obj.vptr = step.aux_rva;           // G+0x00
                obj.name = view;                   // G+0x08 .. G+0x27 (borrowed copy, ctl null)
                obj.tail = 0;                      // G+0x28
                ++r.releases;
                if (!release_view(hooks, view)) {
                    state.results[pos] = r = block(r, i, ReleaseFamilyStage::view_release);
                    return r;
                }
                if (!atexit_log.record(step.dtor_rva, &obj)) {
                    state.results[pos] = r = block(r, i, ReleaseFamilyStage::atexit_record);
                    return r;
                }
                state.atexit_notes[k] = {step.dtor_rva, &obj};
                ++r.atexit_records;
                break;
            }
            case ReleaseFamilyKind::span_hash: {
                const ByteSpan span{view.data, view.size};
                const std::uint32_t h = fnv_span_161f80(span, step.seed);
                ++r.releases;
                if (!release_view(hooks, view)) {
                    state.results[pos] = r = block(r, i, ReleaseFamilyStage::view_release);
                    return r;
                }
                state.dwords[release_family_dword_index(i)] = h;
                break;
            }
            case ReleaseFamilyKind::cstr_hash:
                state.dwords[release_family_dword_index(i)] = fnv_cstr_a2ddc0(step.text);
                break;
            case ReleaseFamilyKind::const_zero:
                state.dwords[release_family_dword_index(i)] = 0;
                break;
        }
        state.built[i] = true;
        ++r.built_count;
    }
    r.ok = r.built_count == desc.count;
    r.stage = r.ok ? ReleaseFamilyStage::none : ReleaseFamilyStage::bad_table;
    state.results[pos] = r;
    return r;
}

}  // namespace sotc
