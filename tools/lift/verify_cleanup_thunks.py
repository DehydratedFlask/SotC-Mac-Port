#!/usr/bin/env python3
"""Verify the 15 static-cleanup constructor thunks (CPU-01-STATIC-CLEANUP-THUNKS-01).

Each member is a 33-byte body with no calls, no stack frame and exactly one transfer: a tail call
to the `__cxa_atexit` import thunk. The body zeroes one .bss cell and registers that cell's
destructor with the shared dso handle `0x1604218`:

    lea rsi, [global]        ; slot to clear / destructor argument
    lea rdi, [destructor]    ; cleanup function
    lea rdx, [0x1604218]     ; dso handle
    mov qword [rsi], 0       ; zero the slot
    jmp __cxa_atexit         ; tail call

Re-derives the idiom, body digest, global and destructor for every slot from the pinned ELF and
compares them with the table below. Offline; nothing executed. Exit 0 only when all pass.
"""
import hashlib
import importlib.util
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
sys.path.insert(0, str(ROOT / 'tools/lift'))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
DSO = '0x1604218'
BSS_START = 0x2DD7100
IMAGE_END = 0x3956AA4

# (constructor index, RVA, body sha256, zeroed .bss cell, destructor RVA)
ENTRIES = [
    (515, '0x1c00e0', '37d79ce2907a04a6e207f14054ab58253aea4afd91ecd7ec5d40dacc4cc6e7e8', '0x2ee68c8', '0x1bdae0'),
    (522, '0x16e960', 'a2038a891ab73a82b854a70f6a85c7b1fba775aaf544d4732c6c166ece8fdd84', '0x2ee3e10', '0x16cda0'),
    (523, '0x16cd50', '7dc4c4dda7695e1f83887695cdaec341ddf70626a3335f983bd33241657c5b95', '0x2ee3e08', '0x16b940'),
    (525, '0x166ef0', '36f435a1b776af8ca57de50436d2a4280fa0c8c5725dc7a01fff953c7f2a3ac4', '0x2ee3cd8', '0x163ad0'),
    (527, '0x157e60', 'b4a88afd6391ba7c21aebd817baa2617a1f232301ac27a046338872975c598f0', '0x2ee3c20', '0x156ff0'),
    (528, '0x156fb0', '8e272f99d0de49b0757593e0f2e92802958ed166a7e0ff5e18010e64e0651728', '0x2ee3c18', '0x1561c0'),
    (531, '0x1442c0', 'c26d0735db5c5e961e5f87a40fedc67288d03fb3b718ee59fc3904e3dee613f5', '0x2ee2cf8', '0x144290'),
    (533, '0x143300', 'ef1a0037bb9e7c347db968b7a01fcedb441fbde2a6ba3d7c0bc1088dbeab13d6', '0x2ee2a50', '0x143110'),
    (542, '0xfb3d0', '6c3faa0e74883e54abf295e133acf03065db7ed8bb20ad166e4efcc2b9a108dc', '0x2edef60', '0xfaf80'),
    (546, '0xa25f0', 'a147298c876e1be8f55ff956e3685cdc8676fbf566ab39ddd8051f9fc6d70110', '0x2ed37c0', '0xa1da0'),
    (547, '0x9a970', '16f45798a45dd60798103610659a6d3bada8403054c0af03510505469457e78b', '0x2ed37b0', '0x994c0'),
    (549, '0x96650', 'e0f5c805f97a95383b25f5fb8e545dc20be78cf3db4d2f5d5fb00a25ee86c91e', '0x2ed3670', '0x95620'),
    (551, '0x8d740', 'aa273c8c8d57b4c7b40026b7997225d9682d6b7e971c022dab955a7dbcd5f4f3', '0x2ed33f0', '0x8d620'),
    (564, '0x31220', 'c1cc9ee99113f63c11a0cc363e0f2fa132b1a806e8459b254181b335766c5d7e', '0x2dfd678', '0x2fd20'),
    (565, '0x2fcf0', 'ac26ab7ed9241a63dc23bdbac61eb4d83a70fe13a3f45def74846e20efa3f679', '0x2dfd4b8', '0x26290'),
]


def require(condition, message):
    if not condition:
        raise SystemExit(f'FAIL {message}')


def load_analyzer():
    spec = importlib.util.spec_from_file_location(
        'slot_struct', ROOT / 'tools/lift/analyze_slot_struct.py')
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main():
    elf = (ROOT / 'local/re/eboot.elf').read_bytes()
    digest = hashlib.sha256(elf).hexdigest()
    require(digest == ELF_SHA256, f'unexpected ELF sha256 {digest}')
    analyzer = load_analyzer()
    _, image, name_by_nid, nid_at = analyzer.load_image()

    for index, rva_hex, body_sha, global_hex, dtor_hex in ENTRIES:
        rva = int(rva_hex, 16)
        report = analyzer.build_report(rva, image, name_by_nid, nid_at)
        tag = f'index {index} / {rva_hex}'
        require(report['span_bytes'] == 33, f'{tag} span {report["span_bytes"]}')
        require(report['body_sha256'] == body_sha, f'{tag} body digest changed')
        require(report['blocks_terminated_by_ret'] == 0, f'{tag} unexpectedly returns')
        require(report['direct_calls'] == [], f'{tag} has a direct call')
        require(report['indirect_transfers'] == [], f'{tag} has an indirect transfer')
        require(len(report['branch_edges']) == 1, f'{tag} has {len(report["branch_edges"])} edges')
        edge = report['branch_edges'][0]
        require(edge.get('tail_call') and edge['target'] == '0xf8f390'
                and edge['thunk']['name'] == '__cxa_atexit',
                f'{tag} does not tail-call __cxa_atexit')

        # Exact body shape: lea rsi / lea rdi / lea rdx / mov qword [rsi],0 / jmp rel32.
        body = image.read(rva, 33)
        require(body[0:3] == b'\x48\x8d\x35', f'{tag} first instruction is not lea rsi,[rip]')
        require(body[7:10] == b'\x48\x8d\x3d', f'{tag} second instruction is not lea rdi,[rip]')
        require(body[14:17] == b'\x48\x8d\x15', f'{tag} third instruction is not lea rdx,[rip]')
        require(body[21:28] == b'\x48\xc7\x06\x00\x00\x00\x00',
                f'{tag} does not zero the slot with a qword store')
        require(body[28] == 0xe9, f'{tag} does not end with a rel32 jmp')

        refs = {r['target'] for r in report['rip_relative_refs']}
        require(global_hex in refs, f'{tag} global {global_hex} missing')
        require(dtor_hex in refs, f'{tag} destructor {dtor_hex} missing')
        require(DSO in refs, f'{tag} shared dso {DSO} missing')
        cell = int(global_hex, 16)
        require(BSS_START <= cell < IMAGE_END, f'{tag} {global_hex} is not a .bss cell')
        require(int(dtor_hex, 16) < BSS_START, f'{tag} {dtor_hex} is not a code address')

    print(f'OK: {len(ENTRIES)} static-cleanup thunks share one 33-byte idiom: zero one .bss cell, '
          f'register its destructor with __cxa_atexit(dso={DSO}), no calls, no stack frame')
    for index, rva_hex, _, global_hex, dtor_hex in ENTRIES:
        print(f'  slot {index:>3} {rva_hex:>9}  clears {global_hex}  dtor {dtor_hex}')


if __name__ == '__main__':
    main()
