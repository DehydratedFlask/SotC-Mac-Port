#!/usr/bin/env python3
"""ELF byte-oracle for the red-black named registry used by startup slot 446 (call 0x3be2d0), batch 21.

Six bodies are hash-pinned (whole body) and every semantic constant the native model uses is additionally pinned as exact instruction bytes:
  0x3be2d0 (192 B)  register_entry: lower_bound on the key at entry+0x60 (lea rsi,[r15+0x60]), cmovs fallback to the head, mapped (+0x40) test,
                    else call 0x3c0840 and store the entry at the returned slot
  0x3c0840 (368 B)  index: lower_bound, a 0x48 B node from 0x17070, color/isnil word zeroed, 0x20 B key copy, `lock incq (ctl)`, mapped = 0,
                    hint insert 0x3c31b0, result + 0x40
  0x3c31b0 (692 B)  hint insert (empty tree / leftmost / head / interior neighbours / unique-insert fallback)
  0x3c3710 (544 B)  unique insert (descent mode, predecessor check, duplicate: 0xf590 on node+0x20, 0x170b0 on the node)
  0x3c3470 (662 B)  link + rebalance (size bound 0x38e38e38e38e38d, head bookkeeping, rotations, root black)
  0x3c3930 (32 B)   node destroy (0xf590 on node+0x20, tail 0x170b0)
The comparator 0xf8f500 is the libc import `strcasecmp` (NID AV6ipCNa4Rw); the two throw stubs 0xf8fc10 / 0xf8f5f0 are pinned by NID string
(tQIo+GIPklo / eT2UsmTewbU: their names are not resolved, the model treats both as a failure). The allocator thunks 0x17070 / 0x170b0 and the
shared release 0xf590 are hash-pinned (HOOKS / modelled elsewhere).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA.
Usage: python3 -I tools/lift/check_rb_registry_3be2d0.py [--no-mutate] [--tamper elf|native]
"""

import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402
import bool_page_lib as bpl  # noqa: E402
from slot_pin_util import require  # noqa: E402

lib.COMMON_CALLEES.update({
    "rb_index_3c0840": (0x3c0840, 368, "708e207021a8a6cff7fc48a7b81b1e5fd2270c44b1058bd72cec02deb0dd7a11"),
    "rb_hint_insert_3c31b0": (0x3c31b0, 692, "71ffc6f61b8db6ac36942eb26b600476ab96f49ab80b314d20237f02e335cb5a"),
    "rb_unique_insert_3c3710": (0x3c3710, 544, "197d870f9cbfbe89369754fdd89b767ee03b18c4bda248a0e8e44fe8b1a16cb1"),
    "rb_insert_at_3c3470": (0x3c3470, 662, "b48b3714a93949bb2eb29aecb42b4f93a924269184ef7f6e317dc277dc2a0e6a"),
    "rb_node_destroy_3c3930": (0x3c3930, 32, "9032065af49bafdca45cddc19d61c52dff68971ba8777a054051324c840e7d68"),
    "alloc_tagged_17070": (0x17070, 20, "9c9cdfa9ec1727f5e2903512bc08086d93c439323bee3a9212eb4aabfe4ba148"),
    "free_tagged_170b0": (0x170b0, 10, "bd6d6a49a0a57e9f8f358cd4c02a743796b0e138a0098a26091b0c4c95df3928"),
})

SPEC = {
    "slot": 446,
    "label": "rb_registry_3be2d0",
    "no_table": True,
    "base": 0x3be2d0,
    "size": 192,
    "body_sha256": "97e528177a856833ce387cb31b4bef859f8c3ef5814387b4b88a96698fe5f8ce",
    "pad": "554889e5",   # the next function's prologue (no alignment padding between these bodies)
    "callees": ["rb_index_3c0840", "rb_hint_insert_3c31b0", "rb_unique_insert_3c3710", "rb_insert_at_3c3470", "rb_node_destroy_3c3930",
                "release_f590", "alloc_tagged_17070", "free_tagged_170b0"],
    "bool_tags": [],
    "float_tags": [],
    "strings": [],
    "lengths": [],
    "pins": [
        # 0x3be2d0 register_entry
        ("imm", 0x3be2ec, "498d7760"),                       # lea rsi,[r15+0x60]: the key SharedStr inside the entry
        ("imm", 0x3be2f0, "498b45084c89e980"),               # root = head->parent ; rcx = head ; cmpb $0,0x19(rax) [isnil]
        ("imm", 0x3be308, "488b1e"),                         # rbx = key.data
        ("imm", 0x3be310, "498b7e204889de"),                 # rdi = node.key.data ; rsi = key.data
        ("call", 0x3be317, 0xf8f500),                        # strcasecmp(node.key, key)
        ("imm", 0x3be31c, "85c07910"),                       # test eax,eax ; jns (node.key >= key goes left)
        ("imm", 0x3be330, "498b06"),
        ("imm", 0x3be351, "498b76204889df"),                 # rsi = candidate.key.data ; rdi = key.data
        ("call", 0x3be358, 0xf8f500),                        # strcasecmp(key, candidate.key)
        ("imm", 0x3be361, "85c04d0f48f5"),                   # test eax,eax ; cmovs r14,r13 (candidate = head when key < candidate.key)
        ("imm", 0x3be36a, "4c39e9"),                         # cmp rcx,r13 (== head)
        ("imm", 0x3be36f, "4883794000750b"),                 # cmpq $0,0x40(rcx) ; jne (mapped != 0 -> already registered)
        ("call", 0x3be379, 0x3c0840),                        # index(tree, key)
        ("imm", 0x3be37e, "4c8938"),                         # *slot = entry
        # 0x3c0840 index
        ("imm", 0x3c0865, "4d8b6c24"),
        ("imm", 0x3c086a, "498b4508807819000f84"),           # root = head->parent ; isnil test
        ("imm", 0x3c0880, "bf48000000"),                     # edi = 0x48: the data node
        ("call", 0x3c088e, 0x17070),
        ("imm", 0x3c0893, "498b4c2408"),
        ("imm", 0x3c0898, "488d5020488908488948084889481066"),   # left = parent = right = head
        ("imm", 0x3c08a7, "66c740180000"),                   # color = isnil = 0
        ("imm", 0x3c08ad, "c4c1781007c5f8114020"),           # key words 0/1
        ("imm", 0x3c08b7, "498b4f1048894830498b4f18488948384885c9"),   # key words 2/3 (ctl at +0x38), test ctl
        ("imm", 0x3c08cc, "f048ff01"),                       # lock incq (ctl)
        ("imm", 0x3c08d0, "48c7404000000000"),               # mapped = 0
        ("call", 0x3c08f5, 0x3c31b0),
        ("imm", 0x3c0940, "488b7b204c89e6e8b4ebbc00"),       # rdi = node.key.data ; rsi = key.data ; call strcasecmp
        ("call", 0x3c0947, 0xf8f500),
        ("imm", 0x3c094c, "85c07910"),
        ("imm", 0x3c0976, "498b3f488b7320"),                 # rdi = key.data ; rsi = candidate.key.data
        ("call", 0x3c097d, 0xf8f500),
        ("imm", 0x3c098d, "85c0"),
        ("imm", 0x3c0915, "4883c340"),                       # result + 0x40 = &node->mapped
        ("imm", 0x3c0919, "4889d8"),
        # 0x3c3470 insert_at
        ("imm", 0x3c3470, "4c8b4610"),
        ("imm", 0x3c3474, "48b88de3388ee3388e03"),           # movabs rax,0x38e38e38e38e38d
        ("imm", 0x3c347e, "4939c00f8362020000"),             # cmp r8,rax ; jae throw
        ("imm", 0x3c3487, "49ffc04c894610498949084d8d4108"), # size++ ; store size ; node->parent = where
        ("imm", 0x3c34d4, "4d8b1841807b18000f85f3"),         # parent color test (0 = red)
        ("imm", 0x3c3540, "c60201c6401801498b00"),           # uncle-red recolor
        ("imm", 0x3c36d5, "488b4608488b4008c64018014889f84c894f10c3"),   # root->color = 1 ; result.node = new node
        ("call", 0x3c36f3, 0x3c3930),
        ("call", 0x3c36ff, 0xf8fc10),
        # 0x3c3710 unique insert
        ("imm", 0x3c3770, "4889cb4c89f6488b7b20e881"),       # descent (dl != 0): rdi = node.key ; rsi = key
        ("call", 0x3c377a, 0xf8f500),
        ("imm", 0x3c377f, "c1e81f488d4b1084c048"),           # sign bit selects left / right
        ("imm", 0x3c37a0, "4889cb4c89f7488b7320e851"),       # descent (dl == 0): rdi = key ; rsi = node.key
        ("call", 0x3c37aa, 0xf8f500),
        ("imm", 0x3c37af, "c1e81f488d4b1084c048"),
        ("call", 0x3c381c, 0x3c3470),
        ("imm", 0x3c3894, "498b7e20498b37e860bcbc0085c0"),   # predecessor.key vs key
        ("call", 0x3c389b, 0xf8f500),
        ("imm", 0x3c38a0, "85c07831"),                       # js -> insert
        ("imm", 0x3c38a4, "488b5d80488d7b20"),               # duplicate: rdi = &new_node->key
        ("call", 0x3c38ac, 0xf590),
        ("call", 0x3c38b4, 0x170b0),
        ("call", 0x3c38eb, 0x3c3470),
        # 0x3c31b0 hint insert
        ("imm", 0x3c31dc, "49837f1000"),                     # cmpq $0,0x10(r15): size == 0
        ("call", 0x3c3219, 0xf8f500),
        ("imm", 0x3c3213, "4889df4c89e6"),
        ("call", 0x3c3248, 0x3c3470),
        ("imm", 0x3c3256, "498b38488b73204d89"),
        ("call", 0x3c3260, 0xf8f500),
        ("imm", 0x3c3275, "4d8b6510498b304c89c3498b7c24"),
        ("call", 0x3c3284, 0xf8f500),
        ("imm", 0x3c32fb, "498b7e204889dee8f9c1bc0085"),
        ("call", 0x3c3302, 0xf8f500),
        ("call", 0x3c3319, 0xf8f500),
        ("imm", 0x3c3326, "85c0796c"),
        ("imm", 0x3c3381, "488b7320488b7d90e872"),
        ("call", 0x3c3389, 0xf8f500),
        ("call", 0x3c33a6, 0x3c3710),
        ("call", 0x3c33e8, 0x3c3470),
        ("call", 0x3c3444, 0x3c3470),
        # 0x3c3930 node destroy
        ("imm", 0x3c3936, "4889f3488d7b20"),                 # rdi = &node->key (node is the SECOND argument)
        ("call", 0x3c393d, 0xf590),
        ("imm", 0x3c3942, "4889df"),
        ("jmp", 0x3c394b, 0x170b0),
        # allocator thunks: mov esi,1 ; (edx = 8 ; ecx = 1 for allocate) ; jmp dispatch
        ("imm", 0x17070, "be01000000ba08000000b901000000"),
        ("jmp", 0x1707f, 0x12820),
        ("imm", 0x170b0, "be01000000"),
        ("jmp", 0x170b5, 0x12af0),
    ],
    "native": {
        'header': 'native/core/rb_registry_3be2d0.hpp', 'source': 'native/core/rb_registry_3be2d0.cpp',
        'test': 'native/tests/test_rb_registry_3be2d0.cpp', 'cmake': 'native/CMakeLists.txt',
        'consts': {'kRbRegisterEntryRva': 0x3be2d0, 'kRbRegisterEntrySize': 192, 'kRbIndexRva': 0x3c0840, 'kRbIndexSize': 368,
                   'kRbHintInsertRva': 0x3c31b0, 'kRbHintInsertSize': 692, 'kRbUniqueInsertRva': 0x3c3710, 'kRbUniqueInsertSize': 544,
                   'kRbInsertAtRva': 0x3c3470, 'kRbInsertAtSize': 662, 'kRbNodeDestroyRva': 0x3c3930, 'kRbNodeDestroySize': 32,
                   'kRbStrcasecmpPltRva': 0xf8f500, 'kRbHeadNodeBytes': 0x30, 'kRbDataNodeBytes': 0x48,
                   'kRbMaxSize': 0x038e38e38e38e38d, 'kRbEntryKeyOffset': 0x60},
        'header_tokens': ['static_assert(sizeof(RbNode) == kRbDataNodeBytes', 'static_assert(sizeof(RbMap) == 0x18'],
        'source_order': ['bool rb_node_destroy_3c3930', 'RbInsertResult rb_insert_at_3c3470(', 'if (map.size >= kRbMaxSize)',
                         'RbInsertResult rb_unique_insert_3c3710(', 'RbInsertResult rb_hint_insert_3c31b0(',
                         'RbIndexResult rb_index_3c0840(', 'alloc_17070(kRbDataNodeBytes, hooks.alloc)', 'fetch_add(1, std::memory_order_seq_cst)',
                         'RbRegisterResult rb_register_entry_3be2d0('],
        'test_tokens': ['kRbMaxSize == 0x038e38e38e38e38dull', 'section_hints()', 'section_failures()', 'section_differential(zig, "zigzag")',
                        'rb_check_invariants(m).ok'],
        'cmake_tokens': ['core/rb_registry_3be2d0.cpp', 'add_executable(test_rb_registry_3be2d0 tests/test_rb_registry_3be2d0.cpp)',
                         'add_test(NAME rb_registry_3be2d0 COMMAND test_rb_registry_3be2d0)'],
    },
}


def import_checks(e) -> None:
    """PLT tails of the imports the six bodies call (NID decoded from the JMPREL records): strcasecmp, the two throw stubs, the stack check."""
    bpl._bp_check_imports(e, (
        (0xF8F500, 0x1600070, "strcasecmp"),
        (0xF8F3A0, 0x15FFFC0, "__stack_chk_fail"),
    ))
    parsed, _ = bpl._bp_import_context()
    for plt, got, nid in ((0xF8FC10, 0x16003F8, "tQIo+GIPklo"), (0xF8F5F0, 0x16000E8, "eT2UsmTewbU")):
        stub = e.read(plt, 6)
        require(stub[:2] == b"\xff\x25" and e.rip(plt, 6, 2) == got, f"PLT thunk {plt:#x} -> GOT {got:#x}")
        rows = [r for r in parsed["relocs"] if r["table"] == "JMPREL" and r["target"] == got]
        require(len(rows) == 1 and rows[0]["type"] == 7 and rows[0]["addend"] == 0, f"one JUMP_SLOT row for {plt:#x}")
        e.reloc_pins.add(got)
        require(rows[0]["addend"] ^ (1 if got in e.reloc_flip else 0) == 0, f"{plt:#x} JMPREL addend mutation")
        symbol = parsed["symbols"][rows[0]["symbol"]]
        require(symbol["type"] == "FUNC" and not symbol["defined"] and symbol["nid"] == nid, f"{plt:#x} NID {nid}")


_run_elf_checks = lib.run_elf_checks


def run_elf_checks(e, spec) -> None:
    _run_elf_checks(e, spec)
    import_checks(e)


lib.run_elf_checks = run_elf_checks


def tamper_native(spec) -> int:
    """`--tamper native`: perturb one pinned native constant in memory; the native-token checks must reject it."""
    texts = lib.read_native(spec)
    name, value = sorted(spec["native"]["consts"].items())[0]
    pattern = re.compile(rf"({name} = )(0x[0-9A-Fa-f]+|\d+)(u?;)")
    mutated = dict(texts)
    mutated["header"] = pattern.sub(lambda m: f"{m.group(1)}{value ^ 0x4:#x}{m.group(3)}", texts["header"], count=1)
    require(mutated["header"] != texts["header"], f"constant {name} pattern did not match")
    lib.check_native(spec, mutated)   # raises SystemExit("FAIL ...") (exit 1) when the tamper is detected
    print("native tamper NOT detected", file=sys.stderr)
    return 0


if __name__ == "__main__":
    if "--tamper" in sys.argv[1:] and "native" in sys.argv[1:]:
        raise SystemExit(tamper_native(SPEC))
    raise SystemExit(lib.main(SPEC, 'red-black named registry: 6 bodies hash-pinned, strcasecmp comparator, 0x48 B nodes, lock incq key copy, '
                                    'size bound, throw stubs by NID, allocator thunks'))
