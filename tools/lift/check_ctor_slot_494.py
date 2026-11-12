#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 494 (RVA 0x26f7c0, 84 bytes), batch 20.

Two pool registries: builder 0x26e340 (0x18-byte nodes) on 0x2eef038 with __cxa_atexit(0x269590), builder 0x26e770 (0x20-byte nodes) on
0x2eef058 with the tail-called __cxa_atexit(0x269610). The builders, their node-vector reserve helpers (0x26e5f0 / 0x26ea10), their
pointer-vector grow helpers (0x26e4d0 / 0x26e8f0), the four length_error throw stubs and the two destructors are hash-pinned (whole
body) and every semantic constant the native model uses (0x28 block, 0x1000 nodes, the node initialisation stores, the 0xfff * stride
free-list chain, the 0x20 head store, the push_back store, the dtor frees) is additionally pinned as exact instruction bytes. The
allocator thunks 0x17070 / 0x170b0 are pinned as the `mov esi,1 ; jmp 0x12820/0x12af0` tails they are (hooks in the model).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA.
Usage: python3 -I tools/lift/check_ctor_slot_494.py [--no-mutate] [--tamper elf|native]
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402
import bool_page_lib as bpl  # noqa: E402
from slot_pin_util import require  # noqa: E402

lib.COMMON_CALLEES.update({
    "pool_builder_26e340": (0x26e340, 390, "41174f9da8714e9aaec0a30a04c7826b990dba1e53f403354acb9fc2e37190c1"),
    "pool_builder_26e770": (0x26e770, 375, "ab356c41a47d1c137133791b1290859502c9c6f2da01421066a6d352d30f437c"),
    "node_reserve_26e5f0": (0x26e5f0, 337, "7c78c9faa3aa0ce172fdda79ec8834eb36b3a4a17798c4f84344a409b542d48e"),
    "node_reserve_26ea10": (0x26ea10, 279, "0667db5fcb7a8ef32fcb1d21fe57d4c86b65926781e076867aceb6e3e6a5f1a8"),
    "ptr_grow_26e4d0": (0x26e4d0, 245, "68a79fc7f42f07c4ffe0b6dc4a61a26c60385202ccb005137abf84317c33a66e"),
    "ptr_grow_26e8f0": (0x26e8f0, 245, "dfd97635c85f8d50f8fc8a42fe2cda60c1834b16ee89b35d7e6c723db17c5f51"),
    "registry_dtor_269590": (0x269590, 124, "53939c1173e94ac8caa1b91a8776350b4e69749d58f1e282ae25b40bda0cda27"),
    "registry_dtor_269610": (0x269610, 124, "996ead0f96c5713e895a3dc46b439eaec86de57ec3d5e29eb93a2cfdd0788aed"),
    "length_error_26e5d0": (0x26e5d0, 18, "a3e5ed0f2df2550a5118514ba082d0f361896f465f88b017a0a8e6021e6f599f"),
    "length_error_26e750": (0x26e750, 18, "4d67650abc5d2a6e5a8d1b5204698eeb0c5063bec99aeb4f627231f9984780c0"),
    "length_error_26e9f0": (0x26e9f0, 18, "19649249006ac743a8c1415626247fe2620f0731c1b714b02848b037bfee0bb9"),
    "length_error_26eb30": (0x26eb30, 18, "dfdceaca89b7ef88d79cecbb4da862feec13b7a62584f3989824653a95eecc94"),
    "alloc_tagged_17070": (0x17070, 20, "9c9cdfa9ec1727f5e2903512bc08086d93c439323bee3a9212eb4aabfe4ba148"),
    "free_tagged_170b0": (0x170b0, 10, "bd6d6a49a0a57e9f8f358cd4c02a743796b0e138a0098a26091b0c4c95df3928"),
})

SPEC = {
    "slot": 494,
    "base": 0x26f7c0,
    "size": 84,
    "body_sha256": "eaa5bc16e6a476c97998d80c34d5bc195eefe608d4ca1a36fbe195bcb105ef6d",
    "pad": "909090909090909090909090",
    "callees": ['pool_builder_26e340', 'pool_builder_26e770', 'node_reserve_26e5f0', 'node_reserve_26ea10', 'ptr_grow_26e4d0',
                'ptr_grow_26e8f0', 'registry_dtor_269590', 'registry_dtor_269610', 'length_error_26e5d0', 'length_error_26e750',
                'length_error_26e9f0', 'length_error_26eb30', 'alloc_tagged_17070', 'free_tagged_170b0'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [],
    "lengths": [],
    "pins": [
        # the slot body
        ("rip", 0x26f7c7, 7, 3, 0x2eef038),
        ("call", 0x26f7d1, 0x26e340),
        ("rip", 0x26f7d6, 7, 3, 0x1604218),
        ("rip", 0x26f7dd, 7, 3, 0x269590),
        ("call", 0x26f7ea, 0xf8f390),
        ("rip", 0x26f7ef, 7, 3, 0x2eef058),
        ("call", 0x26f7f9, 0x26e770),
        ("rip", 0x26f7fe, 7, 3, 0x269610),
        ("jmp", 0x26f80f, 0xf8f390),
        # builder 0x26e340: allocate(0x28), zero the registry vector, reserve 0x1000, node init {0,-1,0}, stride 0x18, chain, head, push
        ("imm", 0x26e359, "bf28000000"),
        ("imm", 0x26e369, "c4c17811460849c7461800000000"),
        ("call", 0x26e377, 0x17070),
        ("imm", 0x26e383, "be0010000041bc00100000"),
        ("call", 0x26e39e, 0x26e5f0),
        ("imm", 0x26e3e0, "48c70600000000c74608ffffffff48c74610000000004883c61848ffc175"),
        ("imm", 0x26e3ff, "4b8d3464498d4e08bf00100000488d14f231f6"),
        ("imm", 0x26e416, "488d90e87f010048c74320000000"),
        ("imm", 0x26e430, "c74208000000004889324889d64883c2e848ffcf7fea488943"),
        ("imm", 0x26e446, "48894320"),
        ("call", 0x26e471, 0x26e4d0),
        ("call", 0x26e491, 0x26e4d0),
        ("imm", 0x26e4a2, "4889184983461008"),
        ("call", 0x26e4c0, 0xf8f3a0),
        # builder 0x26e770: same shape with 0x20-byte nodes (extra dword -1 at +0x18), 0x1ffe0 = 0xfff * 0x20
        ("imm", 0x26e789, "bf28000000"),
        ("imm", 0x26e799, "c4c17811460849c7461800000000"),
        ("call", 0x26e7a7, 0x17070),
        ("imm", 0x26e7b3, "be00100000bb001000004c89e7"),
        ("call", 0x26e7d0, 0x26ea10),
        ("imm", 0x26e800, "48c70600000000c74608ffffffff48c7461000000000c74618ffffffff4883"),
        ("imm", 0x26e83d, "488d90e0ff010049c74424200000"),
        ("imm", 0x26e850, "c74208000000004889324889d64883c2e048ffcf7fea49"),
        ("imm", 0x26e866, "4989442420"),
        ("call", 0x26e892, 0x26e8f0),
        ("call", 0x26e8b2, 0x26e8f0),
        ("imm", 0x26e8c3, "4c89204983461008"),
        ("call", 0x26e8e1, 0xf8f3a0),
        # node-vector reserve helpers: allocate, release, length_error / bad_alloc throws
        ("call", 0x26e6a0, 0x17070),
        ("call", 0x26e6fc, 0x170b0),
        ("call", 0x26e733, 0x26e750),
        ("call", 0x26e73a, 0xf8f5f0),
        ("call", 0x26ea9f, 0x17070),
        ("call", 0x26eaee, 0x170b0),
        ("call", 0x26eb19, 0x26eb30),
        ("call", 0x26eb20, 0xf8f5f0),
        # pointer-vector grow helpers: allocate, memmove, release, throws
        ("call", 0x26e55f, 0x17070),
        ("call", 0x26e57b, 0xf8f480),
        ("call", 0x26e58c, 0x170b0),
        ("call", 0x26e5b7, 0x26e5d0),
        ("call", 0x26e5be, 0xf8f5f0),
        ("call", 0x26e97f, 0x17070),
        ("call", 0x26e99b, 0xf8f480),
        ("call", 0x26e9ac, 0x170b0),
        ("call", 0x26e9d7, 0x26e9f0),
        ("call", 0x26e9de, 0xf8f5f0),
        # destructors: node vector, pool block, pointer vector (all 0x170b0)
        ("call", 0x2695c1, 0x170b0),
        ("call", 0x2695c9, 0x170b0),
        ("call", 0x2695eb, 0x170b0),
        ("call", 0x269641, 0x170b0),
        ("call", 0x269649, 0x170b0),
        ("call", 0x26966b, 0x170b0),
        # allocator thunks: mov esi,1 ; (edx = 8 ; ecx = 1 for allocate) ; jmp dispatch
        ("imm", 0x17070, "be01000000ba08000000b901000000"),
        ("jmp", 0x1707f, 0x12820),
        ("imm", 0x170b0, "be01000000"),
        ("jmp", 0x170b5, 0x12af0),
    ],
    "native": {
        'header': 'native/core/ctor_slot_494_26f7c0.hpp', 'source': 'native/core/ctor_slot_494_26f7c0.cpp',
        'test': 'native/tests/test_ctor_slot_494.cpp', 'cmake': 'native/CMakeLists.txt',
        'consts': {'kSlot494PoolsRva': 0x26f7c0, 'kSlot494PoolsSize': 84, 'kSlot494PoolsObj0Rva': 0x2eef038, 'kSlot494PoolsObj1Rva': 0x2eef058,
                   'kSlot494PoolsCtor0Rva': 0x26e340, 'kSlot494PoolsCtor1Rva': 0x26e770, 'kSlot494PoolsDtor0Rva': 0x269590,
                   'kSlot494PoolsDtor1Rva': 0x269610, 'kSlot494PoolReserve0Rva': 0x26e5f0, 'kSlot494PoolReserve1Rva': 0x26ea10,
                   'kSlot494PtrGrow0Rva': 0x26e4d0, 'kSlot494PtrGrow1Rva': 0x26e8f0, 'kSlot494PoolBlockSize': 0x28,
                   'kSlot494NodeCount': 0x1000, 'kSlot494Node0Size': 0x18, 'kSlot494Node1Size': 0x20,
                   'kSlot494NodeStateInit': 0xffffffff},
        'header_tokens': ['static_assert(sizeof(Slot494PoolBlock) == 0x28);', 'static_assert(sizeof(Slot494Registry) == 0x20);'],
        'source_order': ['kMaxPointers = 0x1fffffffffffffffull', 'kMaxNodes24 = 0x0aaaaaaaaaaaaaaaull',
                         'kMaxNodes32 = 0x07ffffffffffffffull', 'grown = capacity + (capacity >> 1)',
                         'node->state = kSlot494NodeStateInit;', 'node->state2 = kSlot494NodeStateInit;',
                         'pool->free_head = nullptr;', 'first[i].state = 0;', 'pool->free_head = pool->nodes_begin;',
                         'slot494_build_registry(objects.r0, 0, objects.hooks)', 'record(kSlot494PoolsDtor0Rva, &objects.r0)',
                         'slot494_build_registry(objects.r1, 1, objects.hooks)', 'record(kSlot494PoolsDtor1Rva, &objects.r1)'],
        'test_tokens': ['kSlot494PoolsRva == 0x26f7c0', 'section_alone()', 'section_failures()', 'g_calls[1].size == 0x1000 * 0x18',
                        'g_calls[4].size == 0x1000 * 0x20'],
        'cmake_tokens': ['core/ctor_slot_494_26f7c0.cpp', 'add_executable(test_ctor_slot_494 tests/test_ctor_slot_494.cpp)',
                         'add_test(NAME ctor_slot_494 COMMAND test_ctor_slot_494)'],
    },
}


def import_checks(e) -> None:
    """PLT tails of the imports the slot, the builders and the grow helpers call (NID decoded from the JMPREL records)."""
    bpl._bp_check_imports(e, (
        (0xF8F390, 0x15FFFB8, "__cxa_atexit"),
        (0xF8F3A0, 0x15FFFC0, "__stack_chk_fail"),
        (0xF8F480, 0x1600030, "memmove"),
    ))


_run_elf_checks = lib.run_elf_checks


def run_elf_checks(e, spec) -> None:
    _run_elf_checks(e, spec)
    import_checks(e)


lib.run_elf_checks = run_elf_checks

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'two pool registries (0x18 / 0x20-byte nodes, 0x1000 each) with atexit(0x269590 / 0x269610), builders / '
                                    'reserve / grow helpers / destructors / throw stubs / allocator thunks hash-pinned'))
