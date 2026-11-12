#include "ctor_slot_555_527f0.hpp"

namespace sotc {

Slot555Result init_slot555_527f0(Slot555Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot555Result out{};

    // 0x52843..0x52894: 0x257a0(server, name, init) ; f590(init) ; f590(name) ; __cxa_atexit(0x23c90, server)
    const SharedStr server_name = lane_m4::borrowed_view_of(kSlot555ServerName, sizeof kSlot555ServerName - 1);
    const SharedStr server_init = lane_m4::borrowed_view_of(kSlot555ServerInit, sizeof kSlot555ServerInit - 1);
    out.server = init_string_var_257a0(objects.server, server_name, server_init, concmd);
    if (!out.server.ok) return out;
    out.server_views_inert = release_inert_view_f590(server_init) && release_inert_view_f590(server_name);
    out.server_atexit_recorded = concmd.atexit_log.record(kStringVarDtorRva, &objects.server);

    // 0x528a0..0x52990: 0x3a980 (family wrapper) ; f590(desc) ; argfn destroy ; vptr ; 25 / 25 ; 0x20d60 ; f590(name) ; atexit 0x18a60
    const HelperIntSiteSpec spec{kSlot555PortName, sizeof kSlot555PortName - 1, "", 0, 0, 0, 1, kSlot555IntFnTag,
                                 kSlot555PortValue, kSlot555PortValue};
    out.port = helper_init_unbounded_int_var_site(objects.port, spec, concmd);
    if (!out.port.ok) return out;

    // 0x52995..0x52a0b: 0x257a0(from, name, init) ; f590(init) ; f590(name) ; __cxa_atexit(0x23c90, from)
    const SharedStr from_name = lane_m4::borrowed_view_of(kSlot555FromName, sizeof kSlot555FromName - 1);
    const SharedStr from_init = lane_m4::borrowed_view_of(kSlot555FromInit, sizeof kSlot555FromInit - 1);
    out.from = init_string_var_257a0(objects.from, from_name, from_init, concmd);
    if (!out.from.ok) return out;
    out.from_views_inert = release_inert_view_f590(from_init) && release_inert_view_f590(from_name);
    out.from_atexit_recorded = concmd.atexit_log.record(kStringVarDtorRva, &objects.from);

    out.ok = out.server.ok && out.server_views_inert && out.server_atexit_recorded && out.port.ok && out.from.ok &&
             out.from_views_inert && out.from_atexit_recorded;
    return out;
}

}  // namespace sotc
