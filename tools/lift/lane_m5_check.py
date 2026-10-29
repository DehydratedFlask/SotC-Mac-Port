"""Lane M5 add-on for the l4_slot_lib byte-oracle checkers (startup constructor slots 491 492 499 502 509 516 535
540 543). Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an
original RVA.

It (1) registers the extra shared callees the M5 slots reuse (hash + size pinned, always), (2) lets a SPEC carry
`elf_extra`, a list of functions `fn(e: Elf)` run after the standard ELF checks inside the same mutation harness
(every byte they read is a recorded pin that the self-test flips), and (3) provides those extras.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402
from slot_pin_util import require  # noqa: E402

lib.COMMON_CALLEES.update({
    # handle zero / cleanup / SharedStr destructor (slot 491)
    "handle_zero_aaf040": (0xAAF040, 8, "4e7704d36f166b2a455d03d9b50b56f57967922360c110a597d252af775f6e1e"),
    "handle_dtor_aaf050": (0xAAF050, 52, "5d5e787b76415b2ed8c34f8aaf90e0cf8c45170230ec4570f299e251dc4621f9"),
    "sharedstr_dtor_f7b0": (0xF7B0, 73, "1e7da61ccdc2bc47c92c291047e615af7bbdc1b131ed4e0f5ff1836119dbc676"),
    # string variable (slot 516), same bodies the slot 310 checker pins
    "string_ctor": (0x257A0, 0x14C, "9ac816cf6bebd628d2a9b2941571869d4f1d08994081f5c61db7d165270115f2"),
    "string_inner": (0x258F0, 0x157, "5555aff241162d36f3c442755b6342ac872e92bbdf296839033b3a2b086d6950"),
    "string_dtor_23c90": (0x23C90, 131, "d4323e9e0d8802b2deca4dcfe9430826b3561ed5cc7715f4daf272ab1ba55214"),
    "empty_ctor_f180": (0xF180, 0x1C, "bd33279f6055c44aae01e0f9cdcb392acdadfcf4749dc6fe6e6113f5940f714c"),
    "sfn_copy": (0x25A50, 0x14, "73be203a7605ec0af252ac3df43ce59dc067315828552214dff5a1160c06fb6e"),
    "sfn_clone": (0x25A70, 0x14, "5400d3e5266832c0e4cde2131420856e018aeac1fa816f68d2e5487d98496897"),
    "sfn_move": (0x25A90, 0x9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    "sfn_destroy": (0x25AA0, 0x1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
    "str_slot3": (0x246A0, 0x6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
    "str_slot6": (0x24780, 0x6, "e36623525b7c35b277173e56616b2d865cf6515aa2be1d12d6814ef725cecffb"),
    "str_dtor_mid": (0x24610, 0x28, "60ae896a5c15d364a5e59c48c944d2c75d2e61a9a795ab73ae8f47299005fcc6"),
    # named delegate entry (slots 492, 543)
    "name_view_f2f0": (0xF2F0, 40, "0173e3a3f11a8aa020b87731d2eef5026621d6f1fa0967581b0dab3df646973b"),
    "entry_register_4b560": (0x4B560, 233, "abd6d52ca9e1933507fb1a3f8d82a22e66d52a47a1348ebc33ef82acb3a7b6c6"),
    "entry_dtor_4b660": (0x4B660, 9, "b97017385f4063c288a65875294606be7cda52205dde185961236f6c8b3ee46b"),
    "delegate_invoker_4cca0": (0x4CCA0, 9, "32c3c754106f2957c8685381f096b14cdf51a7404d82a5207ebd925eb0652557"),
    # slot 540
    "entry_writer_c1c70": (0xC1C70, 7, "ce1839d005df30c632573ba3f4aa75d80fdc8d2c5099b6dbcf4f5c5e103616bf"),
    "store_value_clear_158340": (0x158340, 7, "6b0cd5b8120614f50ebd4029d19c6fa245c24d932d4d31d5906c9b8164db3b07"),
    "name_object_dtor_8ef00": (0x8EF00, 23, "32799b7bccf74b604c28f179e15a8bfb5b928f37bfacc3efa336f33d022e6c86"),
    "list_free_101ea0": (0x101EA0, 58, "e45313657c4369cc9fced7b148514f30cb0252cf04c9bca0356abd118e8aa48e"),
    # slot 535
    "critsec_ctor_3b310": (0x3B310, 106, "262dbcd4b170dd06ab8fa9f8c1d30839f93abe91f4c9825dbe65699901cbf1c1"),
    "critsec_dtor_3b380": (0x3B380, 5, "de79f6373ce5a48f02b6ea56b3fb54c88d603d639900604b1b51ee6f78e3e565"),
    "buffer_dtor_138370": (0x138370, 50, "41907dd271afd70188690dc636aff546240fc21cc1579684879096d3669decbe"),
})

_standard_run_elf_checks = lib.run_elf_checks


def _run_with_extras(e, spec: dict) -> None:
    _standard_run_elf_checks(e, spec)
    for fn in spec.get("elf_extra", ()):
        fn(e)


lib.run_elf_checks = _run_with_extras


# ---------------------------------------------------------------------------------------------- extras
def string_var_extra(e) -> None:
    """String-variable class facts (same contract as the slot 310 checker)."""
    require(e.read(0xF96388, 1) == b"\0", "string-var description view points at a NUL byte")
    require(e.reloc_target(0x1594EF0) == 0x23C90, "string class slot 0 = 0x23c90")
    require(e.reloc_target(0x1594EF0 + 0x10) == 0x20D60, "string class slot 2 = 0x20d60")
    require(e.reloc_target(0x1594EF0 + 0x18) == 0x246A0, "string class slot 3 = 0x246a0")
    require(e.reloc_target(0x1594EF0 + 0x30) == 0x24780, "string class slot 6 = 0x24780")
    e.bytes_at(0x246A0, "b801000000c3", "string slot 3: mov eax,1; ret")
    e.bytes_at(0x24780, "b804000000c3", "string slot 6: mov eax,4; ret")
    for i, body in enumerate((0x25A50, 0x25A70, 0x25A90, 0x25AA0)):
        require(e.reloc_target(0x1595068 + 8 * i) == body, f"functor class 0x1595068 slot {i}")
    require(e.rip(0x25A54, 7, 3) == 0x1595068 and e.rip(0x25A70, 7, 3) == 0x1595068, "functor stores its class tag")
    require(e.rip(0x257BF, 7, 3) == 0x1594EE0 and e.rip(0x25921, 7, 3) == 0x1594F58, "string constructor vtables")


def app_entry_extra(e) -> None:
    """The 0x4b560 queue contract, the 0x4b660 entry destructor and the 0x4cca0 invoker cell."""
    e.bytes_at(0x4B574, "41803c2400" "0f85bb000000", "0x4b560 register-once gate (cmp byte [r12],0; jne)")
    require(e.rip(0x4B57F, 7, 3) == 0x2EA8978, "0x4b560 reads the singleton cell 0x2ea8978")
    e.bytes_at(0x4B586, "41c6042401", "0x4b560 registered = 1")
    e.bytes_at(0x4B58B, "4c8b30" "4d85f6" "7459", "0x4b560 singleton null test")
    require(e.rip(0x4B5EC, 7, 3) == 0x2EA8980, "0x4b560 null path reads the queue head cell 0x2ea8980")
    e.bytes_at(0x4B5F3, "488b08" "49894c2428" "eb3a", "0x4b560 entry.next = head")
    e.bytes_at(0x4B637, "4c8920", "0x4b560 head = entry")
    e.bytes_at(0x4B660, "4883c708", "0x4b660 add rdi,8")
    require(e.jmp(0x4B664) == 0xF590, "0x4b660 tail-jumps to the shared release 0xf590")
    e.bytes_at(0x4CCA0, "4889f8" "4889f7" "ff6018", "0x4cca0 invoker: mov rax,rdi; mov rdi,rsi; jmp [rax+0x18]")
    require(e.reloc_target(0x15FE800) == 0x4CCA0, "invoker cell 0x15fe800 relocates to 0x4cca0")
    require(e.call(0xF2FF) == 0xF8F3B0, "0xf2f0 measures the name with the strlen import 0xf8f3b0")


def critsec_extra(e) -> None:
    """The 0x3b310 critical-section shim: four pthread imports and the \"CritSec\" name (settype constant 2)."""
    require(e.call(0x3B335) == 0xF8F920, "0x3b310 attr init import")
    e.bytes_at(0x3B33A, "be02000000", "0x3b310 settype argument 2")
    require(e.call(0x3B342) == 0xF8F930, "0x3b310 attr settype import")
    require(e.rip(0x3B347, 7, 3) == 0xF97DEB, "0x3b310 name string")
    e.cstring(0xF97DEB, b"CritSec")
    require(e.call(0x3B354) == 0xF8F8D0, "0x3b310 mutex init import")
    require(e.call(0x3B35C) == 0xF8F910, "0x3b310 attr destroy import")
    require(e.jmp(0x3B380) == 0xF8F8C0, "0x3b380 jumps to the mutex destroy import")


def name_object_extra(e) -> None:
    """Slot 540: the two 7-byte leaves and the rand import thunk (0x8ef00 is the atexit target, not class slot 0)."""
    e.bytes_at(0xC1C70, "8937" "48895708" "c3", "0xc1c70 mov [rdi],esi; mov [rdi+8],rdx; ret")
    e.bytes_at(0x158340, "8937" "c6470400" "c3", "0x158340 mov [rdi],esi; mov byte [rdi+4],0; ret")
    require(e.call(0x10E49B) == 0xF90130, "0x10e390 calls the rand import thunk 0xf90130")


main = lib.main
