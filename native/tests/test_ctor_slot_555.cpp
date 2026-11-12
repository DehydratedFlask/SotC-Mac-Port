// CPU-01-STARTUP-SLOT-555: fixture for startup constructor index 555 (0x527f0): string variable `smtp_server`
// (0x257a0), unbounded int variable `smtp_server_port` (0x3a980 through the family wrapper, functor tag 0x1595e60,
// value 25) and string variable `smtp_from`. Batch 6.
#include "l4_test_util.hpp"

#include "core/ctor_slot_555_527f0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot555Rva == 0x527f0 && kSlot555Size == 585 && kSlot555IntFnTag == 0x1595e60);
    CHECK(sizeof kSlot555ServerName - 1 == 11 && sizeof kSlot555ServerInit - 1 == 0x1f);
    CHECK(sizeof kSlot555PortName - 1 == 0x10 && sizeof kSlot555FromName - 1 == 9 && sizeof kSlot555FromInit - 1 == 0x21);
    CHECK(kSlot555PortRva - kSlot555ServerRva == offsetof(Slot555Objects, port));
    CHECK(kSlot555FromRva - kSlot555ServerRva == offsetof(Slot555Objects, from));
    CHECK(kSlot555PortValue == 25 && is_int_functor_tag(kSlot555IntFnTag));
}

void section_alone() {
    ConCmdRegistry creg;
    Slot555Objects o;
    poison(o);
    const Slot555Result r = init_slot555_527f0(o, creg);
    CHECK(r.ok && r.server.ok && r.server_views_inert && r.server_atexit_recorded);
    CHECK(r.port.ok && r.port.refusal == HelperRefusal::none && r.port.ctor_ok && r.port.functor_destroy_ok &&
          r.port.views_inert && r.port.atexit_recorded && r.port.registration.ok);
    CHECK(r.from.ok && r.from_views_inert && r.from_atexit_recorded);

    const ConCmdStringVar& s = o.server;
    CHECK(s.base.vptr == 0x1594ef0 && s.base.registered == 1 && s.base.flags == 1);
    CHECK(same_view(s.base.name, "smtp_server", 11));
    CHECK(same_view(s.default_value, "smtp-relay.bluepointgames.local", 0x1f) &&
          same_view(s.value, "smtp-relay.bluepointgames.local", 0x1f));
    CHECK(s.value_ref == &s.value);

    const ConCmdRangedVar& p = o.port;
    CHECK(p.base.vptr == 0x1594c08 && p.base.registered == 1 && p.base.flags == 1);
    CHECK(same_view(p.base.name, "smtp_server_port", 0x10) && empty_view(p.base.description));
    CHECK(p.value_ref == &p.value && p.value == 25 && p.default_value == 25);
    CHECK(p.min_engaged == 0 && p.max_engaged == 0 && p.fn_vptr == 0x15944a0 && p.fn_payload == 0 && p.fn_aux == 0);
    for (std::uint64_t q : p.args) CHECK(q == 0);

    const ConCmdStringVar& f = o.from;
    CHECK(f.base.vptr == 0x1594ef0 && f.base.registered == 1 && f.base.flags == 1);
    CHECK(same_view(f.base.name, "smtp_from", 9));
    CHECK(same_view(f.default_value, "bpe_ps4-mailer@bluepointgames.com", 0x21) &&
          same_view(f.value, "bpe_ps4-mailer@bluepointgames.com", 0x21));

    CHECK(creg.pending.size() == 3 && creg.pending.first() == &s.base.pending);
    CHECK(s.base.pending.next == &p.base.pending && p.base.pending.next == &f.base.pending &&
          f.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 3);
    check_atexit(creg, 0, 0x23c90, &o.server);
    check_atexit(creg, 1, 0x18a60, &o.port);
    check_atexit(creg, 2, 0x23c90, &o.from);
}

void section_tag_refusal() {
    ConCmdRegistry creg;
    ConCmdRangedVar v;
    poison(v);
    const HelperIntSiteSpec bad{kSlot555PortName, 0x10, "", 0, 0, 0, 1, 0x1234567, 25, 25};
    const HelperIntSiteResult r = helper_init_unbounded_int_var_site(v, bad, creg);
    CHECK(!r.ok && !r.ctor_ok && r.refusal == HelperRefusal::unadmitted_arg_functor);
    CHECK(creg.pending.size() == 0 && creg.atexit_log.size() == 0);
    const auto* b = reinterpret_cast<const std::uint8_t*>(&v);
    for (std::size_t i = 8; i < sizeof v; ++i) CHECK(b[i] == kPoison);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot555Objects o;
    poison(o.port);
    const Slot555Result r = init_slot555_527f0(o, creg);
    CHECK(!r.ok && !r.server.ok && creg.pending.size() == 0 && creg.atexit_log.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_tag_refusal();
    section_failures();
    std::puts("OK test_ctor_slot_555");
    return 0;
}
