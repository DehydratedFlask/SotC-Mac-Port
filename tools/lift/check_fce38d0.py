#!/usr/bin/env python3
"""Tie native/core/vec_ce38d0 to original RVA 0xce38d0 (private; needs the ELF)."""
import hashlib
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

A = 0xce38d0
ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
BODY_SHA256 = '0d0620cf67759226e5cbfd64d5cdc8286f7a7b6c20d65cab134b2f5841eeada0'
BODY_HEX = 'c5f81005f8466100c5f81105c0e36602c3'  # vmovups xmm0,[rip+d]; vmovups [rip+d],xmm0; ret
SURVEY_INDEX = 154
COPY_MANIFEST_INDEX = 96

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
assert (src, dst) == (0x12f7fd0, 0x3351ca0), (hex(src), hex(dst))
for pair in ((0x12f8d90, 0x3351d00), (0x12f9100, 0x3351d10), (0x12f9370, 0x3351d20),
             (0x12f95c0, 0x3351d30), (0x12f8790, 0x3351cd0), (0x12f89a0, 0x3351ce0),
             (0x12f8330, 0x3351cb0), (0x12f8560, 0x3351cc0)):
    assert (src, dst) != pair, 'aliases a sibling'

survey = json.loads((ROOT / 'local/re/work/ctor-survey.json').read_text())
assert int(survey[SURVEY_INDEX]['ctor'], 16) == A
row = json.loads((ROOT / 'local/re/ctor-vec-copies.json').read_text())['entries'][COPY_MANIFEST_INDEX]
assert int(row['ctor'], 16) == A
assert int(row['source'], 16) == src and int(row['dest'], 16) == dst and row['bits'] == bits

exe = ROOT / 'local/build/native/test_vec_ce38d0'
native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
assert native['bits'] == bits
assert native['source'] == src and native['value'] == dst
assert native['rva'] == A and native['index'] == SURVEY_INDEX
dump = ROOT / 'local/re/work/cce38d0.bin'
if dump.is_file():
    assert dump.read_bytes() == bytes(body)
print(f'OK vec_ce38d0 binary tie: body_sha256={BODY_SHA256} src={src:#x} dst={dst:#x}')
