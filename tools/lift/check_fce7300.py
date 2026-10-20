#!/usr/bin/env python3
"""Tie the 0xce7300 native fixture to its ELF bytes and manifest entries."""

import hashlib
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d


RVA = 0xCE7300
ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
EXPECTED_SOURCE = 0x12F8BB0
EXPECTED_DESTINATION = 0x3351CF0
EXPECTED_BITS = (0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0)
SURVEY_INDEX = 149
COPY_MANIFEST_INDEX = 91


def require(condition, message):
    if not condition:
        raise SystemExit(f'FAIL {message}')


def main():
    elf = (ROOT / 'local/re/eboot.elf').read_bytes()
    digest = hashlib.sha256(elf).hexdigest()
    require(digest == ELF_SHA256, f'unexpected ELF SHA256 {digest}')

    _, headers = d.program_headers(elf)
    image = d.Image(elf, headers)
    body = image.read(RVA, 17)
    template = bytearray.fromhex('c5 f8 10 05 00 00 00 00 c5 f8 11 05 00 00 00 00 c3')
    masked = bytearray(body)
    masked[4:8] = b'\0' * 4
    masked[12:16] = b'\0' * 4
    require(bytes(masked) == bytes(template), f'unexpected constructor encoding {body.hex()}')

    source = RVA + 8 + struct.unpack_from('<i', body, 4)[0]
    destination = RVA + 16 + struct.unpack_from('<i', body, 12)[0]
    require(source == EXPECTED_SOURCE, f'RIP-relative source is {source:#x}')
    require(destination == EXPECTED_DESTINATION, f'RIP-relative destination is {destination:#x}')
    bits = struct.unpack('<4I', image.read(source, 16))
    require(bits == EXPECTED_BITS, f'source words are {[hex(value) for value in bits]}')

    survey = json.loads((ROOT / 'local/re/work/ctor-survey.json').read_text())
    survey_row = survey[SURVEY_INDEX]
    require(int(survey_row['ctor'], 16) == RVA,
            f'survey position {SURVEY_INDEX} does not identify {RVA:#x}')

    copy_manifest = json.loads((ROOT / 'local/re/ctor-vec-copies.json').read_text())['entries']
    copy_row = copy_manifest[COPY_MANIFEST_INDEX]
    require(int(copy_row['ctor'], 16) == RVA,
            f'vector-copy manifest position {COPY_MANIFEST_INDEX} does not identify {RVA:#x}')
    require(int(copy_row['source'], 16) == source and int(copy_row['dest'], 16) == destination,
            'vector-copy manifest source/destination do not match the decoded ELF bytes')
    require(tuple(copy_row['bits']) == bits, 'vector-copy manifest lanes do not match the ELF')

    sibling_cells = {
        (0x12F8D90, 0x3351D00),  # 0xce74a0
        (0x12F9100, 0x3351D10),  # 0xce8440
        (0x12F89A0, 0x3351CE0),  # 0xce6fb0
        (0x12F95C0, 0x3351D30),  # 0xceb070
    }
    require((source, destination) not in sibling_cells, 'source/destination pair aliases a sibling')

    executable = ROOT / 'local/build/native/test_vec_ce7300'
    result = subprocess.run([str(executable), '--dump'], check=True, capture_output=True, text=True)
    native = json.loads(result.stdout)
    require(native.get('bits') == list(bits), 'native lanes differ from the source data')
    require(native.get('source') == source, 'native source address differs')
    require(native.get('destination') == destination, 'native destination address differs')
    require(native.get('rva') == RVA, 'native RVA differs')
    require(native.get('survey_index') == SURVEY_INDEX, 'native survey position differs')
    require(native.get('copy_manifest_index') == COPY_MANIFEST_INDEX,
            'native vector-copy manifest position differs')

    private_dump = ROOT / 'local/re/work/cce7300.bin'
    if private_dump.is_file():
        require(private_dump.read_bytes() == body, 'private constructor dump differs from ELF bytes')

    print(
        f'OK: 0xce7300 source={source:#x} destination={destination:#x} '
        f'bits=~0,~0,~0,0; survey position {SURVEY_INDEX}; '
        f'vector-copy manifest position {COPY_MANIFEST_INDEX}; native dump matches'
    )


if __name__ == '__main__':
    main()
