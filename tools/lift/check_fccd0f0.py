#!/usr/bin/env python3
"""Tie native/core/vec_ccd0f0 to original RVA 0xccd0f0 (private; needs the ELF)."""
import hashlib
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0xccd0f0
ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
BODY_SHA256 = '0b327c698acafb7e34cfe9944e4a38381bbe3d7859f98814c7f0d9d99aef9682'
BODY_HEX = 'c5f8100568946200c5f81105304b6802c3'  # vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret
SURVEY_INDEX = 161
COPY_MANIFEST_INDEX = 103

elf = (ROOT / 'local/re/eboot.elf').read_bytes()
assert hashlib.sha256(elf).hexdigest() == ELF_SHA256, 'unexpected ELF'
_, ph = d.program_headers(elf)
img = d.Image(elf, ph)
body = img.read(A, 17)
assert body.hex() == BODY_HEX, body.hex()
assert hashlib.sha256(body).hexdigest() == BODY_SHA256
template = bytes.fromhex('c5f81005' '00000000' 'c5f81105' '00000000' 'c3')
masked = bytearray(body)
masked[4:8] = b'\0\0\0\0'
masked[12:16] = b'\0\0\0\0'
assert bytes(masked) == template, (bytes(masked).hex(), template.hex())
src = A + 8 + struct.unpack_from('<i', body, 4)[0]
dst = A + 16 + struct.unpack_from('<i', body, 12)[0]
bits = list(struct.unpack('<4I', img.read(src, 16)))
assert bits == [0xffffffff, 0xffffffff, 0xffffffff, 0]
assert (src, dst) == (0x12f6560, 0x3351c30), (hex(src), hex(dst))
allm = json.loads((ROOT / 'local/re/ctor-vec-copies.json').read_text())['entries']
for k, o in enumerate(allm):
    if k != COPY_MANIFEST_INDEX:
        assert (int(o['source'], 16), int(o['dest'], 16)) != (src, dst) and int(o['dest'], 16) != dst, f'aliases manifest entry {k}'

survey = json.loads((ROOT / 'local/re/work/ctor-survey.json').read_text())
assert int(survey[SURVEY_INDEX]['ctor'], 16) == A
row = json.loads((ROOT / 'local/re/ctor-vec-copies.json').read_text())['entries'][COPY_MANIFEST_INDEX]
assert int(row['ctor'], 16) == A
assert int(row['source'], 16) == src and int(row['dest'], 16) == dst and row['bits'] == bits

exe = ROOT / 'local/build/native/test_vec_ccd0f0'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['bits'] == bits
assert native['source'] == src and native['value'] == dst
assert native['rva'] == A and native['index'] == SURVEY_INDEX
dump = ROOT / 'local/re/work/cccd0f0.bin'
if dump.is_file():
    assert dump.read_bytes() == bytes(body)
print(f'OK vec_ccd0f0 binary tie: body_sha256={BODY_SHA256} src={src:#x} dst={dst:#x}')
