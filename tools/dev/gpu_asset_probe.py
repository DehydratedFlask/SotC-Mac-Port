#!/usr/bin/env python3
"""Read-only offline probe for the Metal first-frame path (docs/METAL_FRAME_PATH.md).

Three checks, all offline and read-only; nothing is extracted or written, and only counts, offsets
and digests are printed (never game bytes):

  pairing  PSARC path <-> entry pairing. The 16-byte TOC value of every entry is MD5(path); the
           entries are sorted by that value while the manifest (entry 0) is sorted alphabetically,
           so manifest line k is NOT entry k+1. This reports both pairings.
  csdr     Raw (non-0x8C-prefixed) `.csdr` members: header u32 == number of `OrbShdr` blobs, one
           `s_mov_b32 vcc_hi, imm` token (0xBEEB03FF) whose `token + (imm + 1) * 8` is the
           `OrbShdr` BinaryInfo, and `BinaryInfo.length` bytes of code that end in `s_endpgm`.
  cmsh     The simple raw `.cmsh` layout seen in the engine-support meshes (descriptor table of
           POS0/NRM0/TAN0/TEX0 records, an 8-byte 0xFF sentinel, then streams, then an index tail).

    python3 -I tools/dev/gpu_asset_probe.py --self-test
    python3 -I tools/dev/gpu_asset_probe.py pairing local/extracted/CUSA08809/pak0.psarc
    python3 -I tools/dev/gpu_asset_probe.py csdr local/extracted/CUSA08809/pak0.psarc
    python3 -I tools/dev/gpu_asset_probe.py cmsh local/extracted/CUSA08809/pak0.psarc \\
        --path coredata/enginesupport/models/_cmn/cone.cmsh

Scope limits: this does not decode the 0x8C-prefixed (Oodle-like) blocks, textures, materials or
GCN code, and a passing run is container evidence only. It is not a graphics path and proves no
G3 behaviour. Exit codes: 0 all checks passed, 1 a check failed, 2 usage/IO.
"""

from __future__ import annotations

import argparse
import hashlib
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "scripts"))
from psarc import Psarc  # noqa: E402  (repo reader; used read-only)

ORB = b"OrbShdr"
TOKEN = 0xBEEB03FF  # s_mov_b32 vcc_hi, imm32 (the first dword of every observed shader program)
S_ENDPGM = 0xBF810000
OODLE_LIKE_PREFIXES = (b"\x8c\x0a", b"\x8c\x06")


# --- PSARC pairing -------------------------------------------------------------------------


def md5_index(arch: Psarc) -> dict[str, int]:
    """path -> entry index using MD5(path); raises on a missing or duplicate TOC value."""
    toc = {}
    for i, entry in enumerate(arch.entries):
        if i and entry["md5"] in toc:
            raise ValueError(f"duplicate TOC value {entry['md5']}")
        toc[entry["md5"]] = i
    out = {}
    for path in arch.paths():
        key = hashlib.md5(path.encode()).hexdigest()
        if key not in toc:
            raise ValueError(f"no TOC entry for MD5({path!r})")
        out[path] = toc[key]
    return out


def pairing_report(arch: Psarc) -> dict:
    paths = arch.paths()
    index = md5_index(arch)
    positional = sum(1 for k, p in enumerate(paths) if index[p] == k + 1)
    values = [e["md5"] for e in arch.entries[1:]]
    return {
        "members": len(paths),
        "md5_hits": len(index),
        "positional_pairing_agrees": positional,
        "entries_sorted_by_md5": values == sorted(values),
        "manifest_sorted": paths == sorted(paths),
    }


# --- raw .csdr container -------------------------------------------------------------------


class ContainerError(ValueError):
    pass


def parse_raw_csdr(data: bytes) -> list[dict]:
    """Validate the observed raw container and return one dict per OrbShdr blob."""
    if len(data) < 16:
        raise ContainerError("too short")
    declared = struct.unpack_from("<I", data, 0)[0]
    offsets, pos = [], 0
    while (hit := data.find(ORB, pos)) >= 0:
        offsets.append(hit)
        pos = hit + 1
    if declared != len(offsets) or not offsets:
        raise ContainerError(f"header count {declared} != {len(offsets)} OrbShdr blobs")
    blobs = []
    for info in offsets:
        if info + 28 > len(data):
            raise ContainerError("BinaryInfo runs past the end")
        word = struct.unpack_from("<I", data, info + 8)[0]
        shader_hash, crc = struct.unpack_from("<QI", data, info + 16)
        length = word >> 8
        tokens, pos = [], 0
        while (tok := data.find(struct.pack("<I", TOKEN), pos, info)) >= 0:
            imm = struct.unpack_from("<I", data, tok + 4)[0]
            if tok + (imm + 1) * 8 == info:
                tokens.append(tok)
            pos = tok + 1
        if len(tokens) != 1:
            raise ContainerError(f"{len(tokens)} token candidates for BinaryInfo at {info}")
        code = tokens[0]
        if length < 8 or code + length > info or length % 4:
            raise ContainerError("BinaryInfo.length does not fit before the BinaryInfo")
        if struct.unpack_from("<I", data, code + length - 4)[0] != S_ENDPGM:
            raise ContainerError("code does not end in s_endpgm")
        blobs.append({
            "info_offset": info, "code_offset": code, "version": data[info + 7],
            "type": (word >> 2) & 15, "source_type": (word >> 6) & 3, "length": length,
            "shader_hash": shader_hash, "crc32": crc,
        })
    return blobs


def csdr_scan(arch: Psarc, index: dict[str, int]) -> dict:
    stats = {"members": 0, "oodle_like": 0, "raw": 0, "raw_ok": 0, "raw_fail": 0, "failures": [],
             "type_sets": {}}
    for path, i in index.items():
        if not path.endswith(".csdr"):
            continue
        stats["members"] += 1
        head = arch.read(i, 2)
        if head in OODLE_LIKE_PREFIXES:
            stats["oodle_like"] += 1
            continue
        stats["raw"] += 1
        try:
            blobs = parse_raw_csdr(arch.read(i))
        except ContainerError as exc:
            stats["raw_fail"] += 1
            stats["failures"].append((path, str(exc)))
            continue
        stats["raw_ok"] += 1
        key = tuple(b["type"] for b in blobs)
        stats["type_sets"][key] = stats["type_sets"].get(key, 0) + 1
    return stats


# --- simple raw .cmsh ----------------------------------------------------------------------

CMSH_TAGS = {b"0SOP": "POS0", b"0MRN": "NRM0", b"0NAT": "TAN0", b"0XET": "TEX0"}


def parse_simple_cmsh(data: bytes) -> dict:
    """Decode the engine-support mesh layout: descriptors, 8x0xFF sentinel, streams, index tail."""
    first = data.find(b"0SOP")
    if first < 0:
        raise ContainerError("no POS0 descriptor")
    descs, pos = [], first
    while data[pos:pos + 4] in CMSH_TAGS:
        fmt, flags, offset, _, size, _ = struct.unpack_from("<HHIIII", data, pos + 4)
        descs.append({"tag": CMSH_TAGS[data[pos:pos + 4]], "format": fmt, "flags": flags,
                      "offset": offset, "size": size})
        pos += 24
    if not descs or descs[0]["tag"] != "POS0":
        raise ContainerError("descriptor table does not start with POS0")
    if data[pos:pos + 8] != b"\xff" * 8:
        raise ContainerError("missing 0xFF sentinel after the descriptor table")
    data_start = pos + 8
    base = data_start - descs[0]["offset"]
    if descs[0]["size"] % 12:
        raise ContainerError("POS0 size is not a multiple of 12")
    count = descs[0]["size"] // 12
    for d in descs[1:]:
        if d["size"] != count * 4:
            raise ContainerError(f"{d['tag']} size {d['size']} != 4 * {count}")
    if base + descs[0]["offset"] + count * 12 > len(data):
        raise ContainerError("POS0 stream runs past the end")
    positions = [struct.unpack_from("<3f", data, base + descs[0]["offset"] + 12 * i)
                 for i in range(count)]
    tail = base + max(d["offset"] + d["size"] for d in descs)
    marker, n_idx = struct.unpack_from("<II", data, tail)
    if tail + 8 + 2 * n_idx > len(data):
        raise ContainerError("index tail runs past the end")
    indices = list(struct.unpack_from(f"<{n_idx}H", data, tail + 8))
    if indices and max(indices) >= count:
        raise ContainerError("index out of range")
    if n_idx % 3:
        raise ContainerError("index count is not a multiple of 3")
    return {"vertices": count, "indices": n_idx, "positions": positions, "index_list": indices,
            "streams": [d["tag"] for d in descs], "index_marker": marker}


# --- self test (synthetic bytes only) ------------------------------------------------------


def _synthetic_csdr(blobs: list[tuple[int, int]], declared: int | None = None) -> bytes:
    """blobs = [(type, code_dwords)]; each blob = token,imm,nops...,s_endpgm,pad,BinaryInfo."""
    out = bytearray(struct.pack("<I", len(blobs) if declared is None else declared) + b"\0" * 60)
    for type_, dwords in blobs:
        body = [TOKEN, 0] + [0xBF800000] * (dwords - 3) + [S_ENDPGM]
        code_len = len(body) * 4
        pad = 8  # extra bytes between s_endpgm and the BinaryInfo, like the observed files
        imm = (code_len + pad) // 8 - 1
        body[1] = imm
        out += struct.pack(f"<{len(body)}I", *body) + b"\0" * pad
        word = (1 << 0) | (type_ << 2) | (1 << 6) | (code_len << 8)
        out += ORB + b"\x07" + struct.pack("<I", word) + b"\x02\x01\x0f\x05"
        out += struct.pack("<QI", 0x1122334455667788, 0xDEADBEEF)
    return bytes(out)


def _synthetic_cmsh(vertices=4, indices=(0, 1, 2, 2, 1, 3)) -> bytes:
    n = vertices
    streams = [(b"0SOP", 2, 12 * n), (b"0MRN", 16, 4 * n), (b"0NAT", 16, 4 * n), (b"0XET", 6, 4 * n)]
    head = b"\0" * 0x4E
    table, offset = b"", 0x70
    for tag, fmt, size in streams:
        table += tag + struct.pack("<HHIIII", fmt, 2, offset, 0, size, 0)
        offset += (size + 15) // 16 * 16
    pos = [(float(i), float(i + 1), float(i + 2)) for i in range(n)]
    body = bytearray()
    for tag, fmt, size in streams:
        body += (b"".join(struct.pack("<3f", *p) for p in pos) if tag == b"0SOP"
                 else b"\0" * size)
        body += b"\0" * (-len(body) % 16)
    tail = struct.pack("<II", 1, len(indices)) + struct.pack(f"<{len(indices)}H", *indices)
    return head + table + b"\xff" * 8 + bytes(body[:offset - 0x70]) + tail + b"HSEM"


def self_test() -> int:
    fails = []

    def check(name, cond):
        if not cond:
            fails.append(name)

    good = parse_raw_csdr(_synthetic_csdr([(1, 20), (0, 12)]))
    check("two blobs", [b["type"] for b in good] == [1, 0])
    check("lengths", [b["length"] for b in good] == [80, 48])
    check("hash", good[0]["shader_hash"] == 0x1122334455667788)
    for name, blob in (
        ("count mismatch", _synthetic_csdr([(1, 20)], declared=2)),
        ("truncated", _synthetic_csdr([(1, 20)])[:-6]),
    ):
        try:
            parse_raw_csdr(blob)
            fails.append(f"{name} accepted")
        except ContainerError:
            pass
    bad = bytearray(_synthetic_csdr([(1, 20)]))
    bad[bad.find(struct.pack("<I", S_ENDPGM))] ^= 0xFF
    try:
        parse_raw_csdr(bytes(bad))
        fails.append("corrupt s_endpgm accepted")
    except ContainerError:
        pass
    mesh = parse_simple_cmsh(_synthetic_cmsh())
    check("cmsh counts", (mesh["vertices"], mesh["indices"]) == (4, 6))
    check("cmsh positions", mesh["positions"][3] == (3.0, 4.0, 5.0))
    try:
        parse_simple_cmsh(_synthetic_cmsh(indices=(0, 1, 9)))
        fails.append("out-of-range index accepted")
    except ContainerError:
        pass
    index_demo = {p: i for i, p in enumerate(["b", "a"], start=1)}
    check("pairing demo", index_demo["a"] == 2)
    for name in fails:
        print(f"FAIL {name}")
    print("self-test:", "FAILED" if fails else "ok")
    return 1 if fails else 0


# --- CLI -----------------------------------------------------------------------------------


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawTextHelpFormatter)
    ap.add_argument("--self-test", action="store_true", help="run the synthetic-byte tests")
    sub = ap.add_subparsers(dest="cmd")
    for name in ("pairing", "csdr", "cmsh"):
        p = sub.add_parser(name)
        p.add_argument("archive", type=Path)
        if name != "pairing":
            p.add_argument("--path", help="inspect one member instead of scanning all")
    args = ap.parse_args()
    if args.self_test:
        return self_test()
    if not args.cmd:
        ap.print_usage(sys.stderr)
        return 2
    try:
        arch = Psarc(args.archive)
    except (OSError, ValueError) as exc:
        print(f"cannot open {args.archive}: {exc}", file=sys.stderr)
        return 2
    try:
        if args.cmd == "pairing":
            report = pairing_report(arch)
            print(report)
            return 0 if report["md5_hits"] == report["members"] else 1
        index = md5_index(arch)
        if args.cmd == "csdr":
            if args.path:
                data = arch.read(index[args.path])
                print(f"{args.path}: {len(data)} B sha256 {hashlib.sha256(data).hexdigest()}")
                for blob in parse_raw_csdr(data):
                    print(" ", blob)
                return 0
            stats = csdr_scan(arch, index)
            print({k: v for k, v in stats.items() if k != "failures"})
            for path, why in stats["failures"][:10]:
                print("FAIL", path, why)
            return 0 if stats["raw_fail"] == 0 else 1
        data = arch.read(index[args.path or ""])
        mesh = parse_simple_cmsh(data)
        print(f"{args.path}: {len(data)} B sha256 {hashlib.sha256(data).hexdigest()}")
        print({k: v for k, v in mesh.items() if k not in ("positions", "index_list")})
        lo = [min(p[i] for p in mesh["positions"]) for i in range(3)]
        hi = [max(p[i] for p in mesh["positions"]) for i in range(3)]
        print("bbox", lo, hi)
        return 0
    except (ContainerError, KeyError, ValueError) as exc:
        print(f"FAIL {exc}", file=sys.stderr)
        return 1
    finally:
        arch.close()


if __name__ == "__main__":
    sys.exit(main())
