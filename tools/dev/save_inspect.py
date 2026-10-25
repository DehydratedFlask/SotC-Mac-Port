#!/usr/bin/env python3
"""Acceptance inspector for a user-owned SotC PS4 progress save (SAV-04 helper).

This tool is READ-ONLY. It never writes to the save directory and never copies game data into the
repository. Point it at a slot directory (one containing `USR-DATA`) or at the `USR-DATA` file:

    python3 -I tools/dev/save_inspect.py "/path/to/SAVEDATA01"
    python3 -I tools/dev/save_inspect.py "/path/to/SAVEDATA01/USR-DATA" --json

It checks the byte-level contract established by the native SaveData slices (E598/E601/E604/E606):

  * outer header (16 B LE): u32 version = 1, u32 FNV-1a-32(body), u32 body_size, u32 descriptor = 4
  * body size 20005 + 4N and <= 0x6400000
  * tagged-chunk stream: per 12 B header {u32 name_hash, u16 magic 0xBEEF, u16 tag, u32 payload}
  * optional native `USR-META` wrapper {SOTCMETA, 1, descriptor, data_hash, 0x530} + param block
  * optional `icon0.png` signature and the native 1 MiB cap

The chunk structure is checked; the meaning of the 15 576-byte GameData blob is NOT decoded here
and is not claimed. A passing run means "this file matches the recovered container contract", not
"the native loader consumes retail saves" — real-save import remains a separate, unproven step.

Exit codes: 0 all checks passed, 1 a check failed, 2 usage/IO.
"""

from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path

HEADER_BYTES = 16
CHUNK_HEADER_BYTES = 12
CHUNK_MAGIC = 0xBEEF
PROGRESS_DESCRIPTOR = 4
MAX_BODY_BYTES = 0x6400000
K_PROGRESS_GAME_DATA_BYTES = 0x3CD8
K_PROGRESS_BASE_BODY = 20005
META_MAGIC = b"SOTCMETA"
META_VERSION = 1
META_HEADER_BYTES = 0x20
META_PARAM_BYTES = 0x530
ICON_MAX_BYTES = 1 << 20
PNG_SIG = b"\x89PNG\r\n\x1a\n"


def fnv1a32(data: bytes) -> int:
    value = 0x811C9DC5
    for byte in data:
        value ^= byte
        value = (value * 0x01000193) & 0xFFFFFFFF
    return value


class Report:
    def __init__(self) -> None:
        self.checks: list[dict[str, object]] = []

    def check(self, name: str, ok: bool, detail: str) -> bool:
        self.checks.append({"name": name, "ok": bool(ok), "detail": detail})
        return ok

    @property
    def ok(self) -> bool:
        return all(c["ok"] for c in self.checks)


def decode_text_slot(block: bytes, offset: int, length: int) -> tuple[str, bool]:
    slot = block[offset:offset + length]
    nul = slot.find(b"\x00")
    terminated = nul >= 0
    raw = slot if not terminated else slot[:nul]
    return raw.decode("utf-8", "replace"), terminated


def inspect_meta(meta_path: Path, data_hash: int, report: Report) -> dict[str, object] | None:
    try:
        raw = meta_path.read_bytes()
    except OSError as exc:
        report.check("meta.readable", False, f"cannot read {meta_path}: {exc}")
        return None
    report.check("meta.readable", True, f"{len(raw)} bytes")
    if not report.check("meta.size", len(raw) == META_HEADER_BYTES + META_PARAM_BYTES,
                        f"{len(raw)} bytes (want {META_HEADER_BYTES + META_PARAM_BYTES})"):
        return None
    if not report.check("meta.magic", raw[:8] == META_MAGIC, raw[:8].hex(" ")):
        return None
    version, descriptor = struct.unpack_from("<II", raw, 8)
    stored_hash, param_bytes = struct.unpack_from("<QQ", raw, 0x10)
    report.check("meta.version", version == META_VERSION, f"{version}")
    report.check("meta.descriptor", descriptor == PROGRESS_DESCRIPTOR, f"{descriptor}")
    report.check("meta.param_bytes", param_bytes == META_PARAM_BYTES, f"{param_bytes:#x}")
    report.check("meta.data_hash", stored_hash == data_hash,
                 f"stored {stored_hash:#010x}, USR-DATA header {data_hash:#010x}")
    block = raw[META_HEADER_BYTES:]
    title, title_nul = decode_text_slot(block, 0x000, 0x80)
    subtitle, subtitle_nul = decode_text_slot(block, 0x080, 0x80)
    detail, detail_nul = decode_text_slot(block, 0x100, 0x400)
    report.check("meta.texts_nul", title_nul and subtitle_nul and detail_nul, "NUL-terminated slots")
    user_param = struct.unpack_from("<I", block, 0x500)[0]
    reserved_nonzero = any(block[0x504:0x530])
    report.check("meta.reserved_zero", not reserved_nonzero, "0x2c reserved tail")
    return {
        "title": title,
        "subtitle": subtitle,
        "detail": detail,
        "user_param": user_param,
        "descriptor": descriptor,
        "data_hash": stored_hash,
    }


def inspect(data_path: Path, report: Report) -> dict[str, object]:
    summary: dict[str, object] = {"path": str(data_path)}
    try:
        raw = data_path.read_bytes()
    except OSError as exc:
        report.check("file.readable", False, f"cannot read {data_path}: {exc}")
        return summary
    report.check("file.readable", True, f"{len(raw)} bytes")
    summary["file_bytes"] = len(raw)
    if not report.check("file.min_size", len(raw) >= HEADER_BYTES + CHUNK_HEADER_BYTES,
                        f"{len(raw)} bytes"):
        return summary

    version, body_hash, body_size, descriptor = struct.unpack_from("<IIII", raw, 0)
    summary.update({"version": version, "body_hash": body_hash, "body_size": body_size,
                    "descriptor": descriptor})
    report.check("header.version", version == 1, f"{version}")
    report.check("header.descriptor", descriptor == PROGRESS_DESCRIPTOR, f"{descriptor}")
    body = raw[HEADER_BYTES:]
    report.check("header.body_size", body_size == len(body),
                 f"declared {body_size}, available {len(body)}")
    report.check("header.max_body", body_size <= MAX_BODY_BYTES, f"{body_size} <= {MAX_BODY_BYTES:#x}")
    report.check("header.fnv", fnv1a32(body) == body_hash,
                 f"computed {fnv1a32(body):#010x}, stored {body_hash:#010x}")
    remainder = (body_size - K_PROGRESS_BASE_BODY) % 4 if body_size >= K_PROGRESS_BASE_BODY else -1
    report.check("header.body_formula", body_size >= K_PROGRESS_BASE_BODY and remainder == 0,
                 f"{body_size} = {K_PROGRESS_BASE_BODY} + 4N (N={(body_size - K_PROGRESS_BASE_BODY) // 4 if body_size >= K_PROGRESS_BASE_BODY else '?'})")

    # Strict chunk tiling; the original's import mode stops at the first non-0xBEEF record, which
    # would hide corruption, so acceptance uses the strict reading.
    chunks: list[dict[str, object]] = []
    tags: dict[int, int] = {}
    names: dict[int, int] = {}
    offset = 0
    tiling_ok = True
    while offset < len(body):
        if offset + CHUNK_HEADER_BYTES > len(body):
            tiling_ok = report.check("chunks.truncated_header", False, f"at {offset}")
            break
        name_hash, magic, tag, payload = struct.unpack_from("<IHHI", body, offset)
        if magic != CHUNK_MAGIC:
            tiling_ok = report.check("chunks.magic", False,
                                     f"at {offset}: {magic:#06x}, want {CHUNK_MAGIC:#06x}")
            break
        end = offset + CHUNK_HEADER_BYTES + payload
        if end > len(body):
            tiling_ok = report.check("chunks.overrun", False,
                                     f"at {offset}: payload {payload} needs {end}, body {len(body)}")
            break
        chunks.append({"offset": offset, "name_hash": name_hash, "tag": tag, "payload": payload})
        tags[tag] = tags.get(tag, 0) + 1
        names[name_hash] = names.get(name_hash, 0) + 1
        offset = end
    if tiling_ok:
        report.check("chunks.tiling", offset == len(body), f"{len(chunks)} chunks, {offset} bytes")
    summary["chunks"] = len(chunks)
    summary["tags"] = {str(k): v for k, v in sorted(tags.items())}
    summary["duplicate_names"] = sorted(k for k, v in names.items() if v > 1)
    game_data = [c for c in chunks if c["payload"] == 8 + K_PROGRESS_GAME_DATA_BYTES]
    summary["game_data_chunks"] = len(game_data)
    report.check("chunks.game_data", len(game_data) >= 1,
                 f"{len(game_data)} chunk(s) with payload {8 + K_PROGRESS_GAME_DATA_BYTES}")

    meta_path = data_path.parent / "USR-META"
    if meta_path.is_file():
        summary["meta"] = inspect_meta(meta_path, body_hash, report)
    else:
        summary["meta"] = None
    icon_path = data_path.parent / "icon0.png"
    if icon_path.is_file():
        icon = icon_path.read_bytes()
        report.check("icon.size", len(icon) <= ICON_MAX_BYTES, f"{len(icon)} bytes")
        report.check("icon.png", icon.startswith(PNG_SIG), icon[:8].hex(" "))
        summary["icon_bytes"] = len(icon)
    else:
        summary["icon_bytes"] = None
    return summary


def self_test() -> int:
    report = Report()
    # Two chunks tile exactly to 20005 bytes: a GameData-sized blob plus an opaque remainder.
    payload = bytes(K_PROGRESS_GAME_DATA_BYTES + 8)  # opaque: structure only, no field claim
    chunk = struct.pack("<IHHI", 0x12345678, CHUNK_MAGIC, 13, len(payload)) + payload
    remainder = K_PROGRESS_BASE_BODY - len(chunk) - CHUNK_HEADER_BYTES
    body = chunk + struct.pack("<IHHI", 0x9ABCDEF0, CHUNK_MAGIC, 13, remainder) + bytes(remainder)
    assert len(body) == K_PROGRESS_BASE_BODY, len(body)
    image = struct.pack("<IIII", 1, fnv1a32(body), len(body), PROGRESS_DESCRIPTOR) + body
    import tempfile
    with tempfile.TemporaryDirectory() as tmp:
        path = Path(tmp) / "USR-DATA"
        path.write_bytes(image)
        summary = inspect(path, report)
    if not report.ok:
        for c in report.checks:
            print(f"{'PASS' if c['ok'] else 'FAIL'} {c['name']}: {c['detail']}")
        return 1
    print(f"self-test PASS: {summary['chunks']} chunks, body {summary['body_size']} bytes")
    return 0


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("path", nargs="?", help="slot directory containing USR-DATA, or the USR-DATA file")
    parser.add_argument("--json", action="store_true", help="print the summary as JSON")
    parser.add_argument("--self-test", action="store_true", help="run the synthetic container mechanics test")
    args = parser.parse_args(argv)
    if args.self_test:
        return self_test()
    if not args.path:
        parser.error("a slot directory or USR-DATA path is required")
    target = Path(args.path).expanduser()
    if target.is_dir():
        data_path = target / "USR-DATA"
    elif target.is_file():
        data_path = target
    else:
        print(f"FAIL path: {target} does not exist", file=sys.stderr)
        return 2
    report = Report()
    summary = inspect(data_path, report)
    if args.json:
        print(json.dumps({"ok": report.ok, "summary": summary, "checks": report.checks}, indent=2))
    else:
        for c in report.checks:
            print(f"{'PASS' if c['ok'] else 'FAIL'} {c['name']}: {c['detail']}")
        print(f"VERDICT: {'PASS' if report.ok else 'FAIL'} ({len(report.checks)} checks)")
    return 0 if report.ok else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
