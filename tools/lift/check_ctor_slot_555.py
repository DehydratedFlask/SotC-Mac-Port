#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 555 (RVA 0x527f0, 585 bytes), batch 6.

String console variable `smtp_server` (0x257a0, initial view `smtp-relay.bluepointgames.local`, atexit 0x23c90), the UNBOUNDED int console variable `smtp_server_port` (0x3a980, flags 1,
value = default = 25, empty description, caller functor tag 0x1595e60, atexit 0x18a60) and the string variable `smtp_from` (initial view `bpe_ps4-mailer@bluepointgames.com`, atexit 0x23c90). No menu item.

Helper-callee tie: the caller functor tag 0x1595e60 is one of the 37 int-family class tables; the native slot reaches 0x3a980 through helper_3a980_unbounded_int and requires
HelperRefusal::none. tools/lift/check_helper_3a980.py pins the same call site. The string class (vtable 0x1594ef0 slot 0/2/3/6, functor class 0x1595068) is tied by the b6_string_class extra.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited driver runs a fail-closed mutation self-test;
--tamper [elf|native] are explicit rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import batch6_lib as bp  # noqa: E402

SPEC = {
    "slot": 555,
    "base": 0x527f0,
    "size": 585,
    "body_sha256": "190562fd7b18b100ac1e02949fecbc9c110d90e525165bd8e46ed2965a2e58d0",
    "pad": "90909090909090",
    "callees": ['concmd_register', 'release_f590'],
    "bool_tags": [],
    "float_tags": [],
    "m6_int_tags": [0x1595e60],
    "strings": [
        (0xf99dd1, b'smtp_server'),
        (0xf99ddd, b'smtp-relay.bluepointgames.local'),
        (0xf99dfd, b'smtp_server_port'),
        (0xf99e0e, b'smtp_from'),
        (0xf99e18, b'bpe_ps4-mailer@bluepointgames.com'),
        (0xf99eb5, b''),
    ],
    "lengths": [
        ('smtp_server', 0x5284b, "48c74424480b000000"),
        ('smtp-relay.bluepointgames.local', 0x52859, "48c74424281f000000"),
        ('smtp_server_port', 0x528ee, "48c744240810000000"),
        ('smtp_from', 0x529c3, "48c744244809000000"),
        ('bpe_ps4-mailer@bluepointgames.com', 0x529d7, "48c744242821000000"),
    ],
    "pins": [
        ("rip", 0x52808, 7, 3, 0x15fe518),
        ("rip", 0x5280f, 7, 3, 0x2ea8b80),
        ("rip", 0x5281b, 7, 3, 0xf99dd1),
        ("rip", 0x5282b, 7, 3, 0xf99ddd),
        ("imm", 0x5284b, "48c74424480b000000"),
        ("imm", 0x52859, "48c74424281f000000"),
        ("call", 0x5286e, 0x257a0),
        ("call", 0x52876, 0xf590),
        ("call", 0x5287e, 0xf590),
        ("rip", 0x52883, 7, 3, 0x23c90),
        ("rip", 0x5288a, 7, 3, 0x1604218),
        ("call", 0x52894, 0xf8f390),
        ("rip", 0x52899, 7, 3, 0x1594bf8),
        ("rip", 0x528a0, 7, 3, 0x2ea8cc8),
        ("rip", 0x528a7, 7, 3, 0xf99dfd),
        ("imm", 0x528bb, "48c744247000000000"),
        ("imm", 0x528cd, "b901000000"),
        ("rip", 0x528db, 7, 3, 0x1595e60),
        ("imm", 0x528ee, "48c744240810000000"),
        ("rip", 0x52905, 7, 3, 0xf99eb5),
        ("imm", 0x5290c, "48c784248800000000000000"),
        ("imm", 0x52923, "48c744243800000000"),
        ("imm", 0x5292c, "4983c710"),
        ("call", 0x5293d, 0x3a980),
        ("call", 0x52945, 0xf590),
        ("imm", 0x52955, "ff5018"),
        ("imm", 0x5295e, "c783d000000019000000"),
        ("imm", 0x52968, "c7436819000000"),
        ("call", 0x5296f, 0x20d60),
        ("call", 0x52977, 0xf590),
        ("rip", 0x5297c, 7, 3, 0x1604218),
        ("rip", 0x52983, 7, 3, 0x18a60),
        ("call", 0x52990, 0xf8f390),
        ("rip", 0x52995, 7, 3, 0x2ea8da0),
        ("rip", 0x5299c, 7, 3, 0xf99e0e),
        ("rip", 0x529b6, 7, 3, 0xf99e18),
        ("imm", 0x529c3, "48c744244809000000"),
        ("imm", 0x529d7, "48c744242821000000"),
        ("call", 0x529e9, 0x257a0),
        ("call", 0x529f1, 0xf590),
        ("call", 0x529f9, 0xf590),
        ("rip", 0x529fe, 7, 3, 0x23c90),
        ("call", 0x52a0b, 0xf8f390),
        ("rip", 0x52a10, 7, 3, 0x15fe518),
        ("call", 0x52a33, 0xf8f3a0),
    ],
    "m6_callees": {
        'int_var_worker_3a980': (0x3a980, 322, "2cbfc2763abb6506a59551b902dd6098325889e5e520d842c0e93da7f43498ca"),
        'int_dtor_18a60': (0x18a60, 54, "4de4f3993b02c84c9c20824690e8ab9d4f7a1b84babfa6cbe0d89c9dd558b50b"),
        'int_fn_copy_c720': (0xc720, 20, "5b63116e7f29674de81acb2a9317619a1c1dcbdbc53f98785a3029c04aa7a914"),
        'int_fn_clone_c740': (0xc740, 20, "3b5a8a9fd4b3e15b1f5f14f73127ac98c5becb3a88fd096f8ccfb2015e991593"),
        'int_fn_move_c760': (0xc760, 9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
        'int_fn_destroy_c770': (0xc770, 1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
        'int_slot3_bf70': (0xbf70, 6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
        'int_slot6_bfc0': (0xbfc0, 6, "7140f35dee6220b79b12aecc27acf5105bf3b77d1588e89fce345de7c16c72b7"),
        'string_ctor_257a0': (0x257a0, 332, "9ac816cf6bebd628d2a9b2941571869d4f1d08994081f5c61db7d165270115f2"),
        'string_inner_258f0': (0x258f0, 343, "5555aff241162d36f3c442755b6342ac872e92bbdf296839033b3a2b086d6950"),
        'empty_ctor_f180': (0xf180, 28, "bd33279f6055c44aae01e0f9cdcb392acdadfcf4749dc6fe6e6113f5940f714c"),
        'sfn_copy_25a50': (0x25a50, 20, "73be203a7605ec0af252ac3df43ce59dc067315828552214dff5a1160c06fb6e"),
        'sfn_clone_25a70': (0x25a70, 20, "5400d3e5266832c0e4cde2131420856e018aeac1fa816f68d2e5487d98496897"),
        'sfn_move_25a90': (0x25a90, 9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
        'sfn_destroy_25aa0': (0x25aa0, 1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
        'str_slot3_246a0': (0x246a0, 6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
        'str_slot6_24780': (0x24780, 6, "e36623525b7c35b277173e56616b2d865cf6515aa2be1d12d6814ef725cecffb"),
    },
    "m6_extras": ['bp_atexit_imports', 'int_site', 'b6_string_class'],
    "native": {'header': 'native/core/ctor_slot_555_527f0.hpp', 'source': 'native/core/ctor_slot_555_527f0.cpp', 'test': 'native/tests/test_ctor_slot_555.cpp', 'shapes_h': 'native/core/helper_int_var_sites.hpp', 'shapes_c': 'native/core/helper_int_var_sites.cpp', 'consts': {'kSlot555Rva': 337904, 'kSlot555Size': 585, 'kSlot555IntFnTag': 22634080, 'kSlot555PortValue': 25, 'kSlot555ServerRva': 48925568, 'kSlot555PortRva': 48925896, 'kSlot555FromRva': 48926112},
        'header_tokens': ['kSlot555ServerName[] = "smtp_server"', 'kSlot555ServerInit[] = "smtp-relay.bluepointgames.local"', 'kSlot555PortName[] = "smtp_server_port"', 'kSlot555FromName[] = "smtp_from"', 'kSlot555FromInit[] = "bpe_ps4-mailer@bluepointgames.com"', 'IDENTITY TAGS'], 'source_order': ['init_string_var_257a0(objects.server, server_name, server_init, concmd)', 'release_inert_view_f590(server_init) && release_inert_view_f590(server_name)', 'concmd.atexit_log.record(kStringVarDtorRva, &objects.server)', 'helper_init_unbounded_int_var_site(objects.port, spec, concmd)', 'init_string_var_257a0(objects.from, from_name, from_init, concmd)', 'release_inert_view_f590(from_init) && release_inert_view_f590(from_name)', 'concmd.atexit_log.record(kStringVarDtorRva, &objects.from)'], 'test_tokens': ['kSlot555Rva == 0x527f0 && kSlot555Size == 585', 'section_alone()', 'section_tag_refusal()', 'section_failures()', 'r.port.refusal == HelperRefusal::none', 'check_atexit(creg, 1, 0x18a60, &o.port)', 'HelperRefusal::unadmitted_arg_functor'], 'shapes_tokens': ['helper_3a980_unbounded_int(var, name, spec.flags, args, description)', 'out.functor_destroy_ok = ctor.arg_destroy_ok;', 'register_concmd_20d60(var.base, registry)']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'string vars smtp_server / smtp_from (0x257a0) + unbounded int smtp_server_port = 25 (tag 0x1595e60 via the 3a980 family wrapper)'))
