#include "owner_members.hpp"

namespace sotc {
namespace {

using Dest = OwnerRowDest;
using Dtor = OwnerRowDestroy;
using Arg = OwnerRowArg;

void* slot_of(OwnerMemberState& state, const OwnerMemberRow& row) noexcept {
    if (row.dest_kind == Dest::kGlobal) {
        return state.globals[row.dest];
    }
    return state.this_fields[static_cast<std::size_t>(row.dest)];
}

void store_slot(OwnerMemberState& state, const OwnerMemberRow& row, void* value) noexcept {
    if (row.dest_kind == Dest::kGlobal) {
        state.globals[row.dest] = value;
    } else {
        state.this_fields[static_cast<std::size_t>(row.dest)] = value;
    }
}

void* resolve_arg(OwnerMemberState& state, const OwnerMemberRow& row) noexcept {
    switch (row.arg_kind) {
        case Arg::kNone:
            return nullptr;
        case Arg::kGlobal:
            return state.globals[row.arg_rva];
        case Arg::kSelfCell:
            return static_cast<void*>(&state.self_cells[row.arg_rva]);
    }
    return nullptr;
}

}  // namespace

const OwnerMemberRow kOwnerMemberRows[kOwnerMemberRowCount] = {
    //  size       new_site    ctor_rva    dest_kind             dest        destroy              arg_kind            arg_rva
    {0x350,     0x256A15,   0x144CA0,   Dest::kGlobal,        0x2EE2D00,  Dtor::kVirtualDtor,  Arg::kNone,         0},
    {0x8,       0x256A49,   0,          Dest::kGlobal,        0x2EE2A50,  Dtor::kVirtualDtor,  Arg::kNone,         0},
    {0x138,     0x256B70,   0x14DB00,   Dest::kGlobal,        0x2EE2CF8,  Dtor::kVirtualDtor,  Arg::kGlobal,       0x2EE2D00},
    {0xAA0,     0x256BAE,   0x8F6F0,    Dest::kSelfRegistered, 0x2EEC4D8, Dtor::kNone,         Arg::kSelfCell,     0x2EEC4D8},
    {0x206050,  0x256BC7,   0x13A9A0,   Dest::kGlobal,        0x2EE2688,  Dtor::kVirtualDtor,  Arg::kNone,         0},
    {0x82028,   0x256BFB,   0xFAFB0,    Dest::kGlobal,        0x2EDEF60,  Dtor::kVirtualDtor,  Arg::kNone,         0},
    {0x1,       0x256C2D,   0xA1CC0,    Dest::kGlobal,        0x2ED37B8,  Dtor::kNone,         Arg::kNone,         0},
    {0x88,      0x256C4C,   0x95A90,    Dest::kSelfRegistered, 0x2EEC4E0, Dtor::kNone,         Arg::kSelfCell,     0x2EEC4E0},
    {0x2203F0,  0x256C65,   0x112DB0,   Dest::kGlobal,        0x2EDF140,  Dtor::kDtorThenFree, Arg::kNone,         0},
    {0xA530,    0x256CC8,   0xA5C850,   Dest::kGlobal,        0x32F8FF0,  Dtor::kDtorThenFree, Arg::kNone,         0},
    {0x178,     0x256D01,   0xA2B960,   Dest::kThisField,     0x3B48,     Dtor::kVirtualDtor,  Arg::kNone,         0},
    {0x1B28,    0x256D36,   0x25FBD0,   Dest::kGlobal,        0x32F4FE0,  Dtor::kDtorThenFree, Arg::kNone,         0},
    {0x740,     0x256D83,   0xA52530,   Dest::kGlobal,        0x32F84F8,  Dtor::kDtorThenFree, Arg::kNone,         0},
    {0x958,     0x256DC1,   0xA597D0,   Dest::kGlobal,        0x32F8B80,  Dtor::kDtorThenFree, Arg::kNone,         0},
    {0x7E0,     0x256DFF,   0xA42C80,   Dest::kGlobal,        0x32F6D18,  Dtor::kDtorThenFree, Arg::kNone,         0},
    {0x4028,    0x256E3D,   0x26F820,   Dest::kThisField,     0x3B50,     Dtor::kVirtualDtor,  Arg::kNone,         0},
    {0x58,      0x256E77,   0x30BEF0,   Dest::kThisField,     0x3B68,     Dtor::kDtorThenFree, Arg::kGlobal,       0x2EDF140},
    {0xB8,      0x256F5C,   0x3C5CB0,   Dest::kThisField,     0x3B70,     Dtor::kDtorThenFree, Arg::kGlobal,       0x2EDF140},
    {0x60,      0x256F9D,   0x1BDB10,   Dest::kGlobal,        0x2EE68C8,  Dtor::kDtorThenFree, Arg::kNone,         0},
    {0x138,     0x257029,   0xA3DA30,   Dest::kGlobal,        0x32F6538,  Dtor::kDtorThenFree, Arg::kNone,         0},
    {0x90,      0x257070,   0x4A690,    Dest::kGlobal,        0x2EA8978,  Dtor::kDtorThenFree, Arg::kNone,         0},
};

OwnerMembersResult init_owner_members(OwnerMemberState& state,
                                      const OwnerMemberHooks& hooks) noexcept {
    OwnerMembersResult out{};
    if (hooks.allocate == nullptr || hooks.deallocate == nullptr || hooks.ctor == nullptr) {
        return out;
    }

    for (std::size_t i = 0; i < kOwnerMemberRowCount; ++i) {
        const OwnerMemberRow& row = kOwnerMemberRows[i];
        const std::size_t row_number = i + 1;
        void* const object = hooks.allocate(row.size);
        if (object == nullptr) {
            out.stopped_row = static_cast<int>(row_number);
            return out;
        }

        void* const arg = resolve_arg(state, row);
        if (!hooks.ctor(row_number, object, arg)) {
            (void)hooks.deallocate(object);
            out.stopped_row = static_cast<int>(row_number);
            return out;
        }

        if (row.dest_kind == Dest::kSelfRegistered) {
            ++out.self_registered;
            continue;
        }

        if (row.destroy != Dtor::kNone) {
            void* const old = slot_of(state, row);
            if (old != nullptr) {
                if (row.destroy == Dtor::kVirtualDtor) {
                    if (hooks.virtual_dtor == nullptr) {
                        (void)hooks.deallocate(object);
                        out.stopped_row = static_cast<int>(row_number);
                        return out;
                    }
                    hooks.virtual_dtor(old);
                    ++out.virtual_destroyed;
                } else {
                    if (hooks.dtor == nullptr) {
                        (void)hooks.deallocate(object);
                        out.stopped_row = static_cast<int>(row_number);
                        return out;
                    }
                    hooks.dtor(old);
                    ++out.dtor_destroyed;
                    (void)hooks.deallocate(old);  // the original ignores the free result
                    ++out.freed;
                }
                ++out.replaced;
            }
        }

        store_slot(state, row, object);
        ++out.installed;
    }

    out.ok = true;
    return out;
}

}  // namespace sotc
