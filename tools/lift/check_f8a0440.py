#!/usr/bin/env python3
"""Tie native/core/leaf_8a0440 to original RVA 0x8a0440 (private; needs local/re/eboot.elf).

The body is decoded from the pinned ELF with tools/lift/leaf_ctor_decode.py, which matches each
instruction by exact encoding and raises on anything unknown. The body SHA-256, length, ordered
store list and region image are pinned, and the native --dump bytes (zero-initialised and
0xAA-poisoned, so coverage is checked too) are compared with the decoded store image.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import leaf_check_common as c  # noqa: E402

SPEC = dict(
    name='leaf_8a0440',
    rva=0x8a0440,
    ctor_index=321,
    body_length=49,
    instructions=7,
    body_sha256='b3380bd7a9212c73d27f48b6f9a5363ded04fb9bddf8fc8cdc0d3e6e97e33987',
    region_base=0x3294f40,
    region='efbeaddeefbeaddeefbeaddeefbeaddeb3856f20b3856f20b3856f20b3856f2025b5685725b5685725b5685725b56857',
    stores=[dict(dest=0x3294f40, width=16, bytes='efbeaddeefbeaddeefbeaddeefbeadde'),
        dict(dest=0x3294f50, width=16, bytes='b3856f20b3856f20b3856f20b3856f20'),
        dict(dest=0x3294f60, width=16, bytes='25b5685725b5685725b5685725b56857')],
)

if __name__ == '__main__':
    c.run(SPEC)
