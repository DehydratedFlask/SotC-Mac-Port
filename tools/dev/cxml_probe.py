#!/usr/bin/env python3
"""Read-only structure probe for a length-prefixed `.cxml` member (`<varint length><XML text>`).

Scope. This tool answers exactly one question about one member: what is its byte-level envelope,
and if the envelope wraps XML text, what are the element/attribute **name** counts? It reads the
member into memory, never writes payload bytes, never emits character data or attribute values, and
refuses (`ProbeError`) instead of guessing when an identity or structure check fails.

Envelope (observed on the audited E824 member, `AUD-01-PAKN-CRMP-CXML-STRUCTURE-PROBE-01`):

  * bytes `[0, k)` hold a minimal unsigned base-128 ("LEB128") varint whose value is exactly the
    number of payload bytes that follow, `k = 3` for that member;
  * the payload is UTF-8 XML 1.0 text with no byte-order mark, beginning with an XML declaration
    and ending at the last byte of the member.

A plain 24-bit little- or big-endian field does **not** reproduce the observed bytes, so the varint
reading is the specific match, not a generic "3-byte integer". A member whose payload starts with
`<` (no prefix) is also accepted and reported with `header_bytes == 0`; anything else is rejected.

This tool is NOT a schema, a codec, an asset extractor, or a runtime claim. It does not resolve
DTDs, does not fetch external entities, does not validate against a schema, and does not interpret
any attribute value. Structural counts are metadata about names, not content.

Usage:
    python3 -I tools/dev/cxml_probe.py --self-test
    python3 -I tools/dev/cxml_probe.py --archive ARCHIVE.psarc --member PATH [--expect-size N]
                                       [--expect-sha256 HEX] [--json]
    python3 -I tools/dev/cxml_probe.py --file MEMBER.cxml [--expect-size N] [--expect-sha256 HEX]

Exit codes: 0 all checks passed, 1 a check failed (or the member was rejected), 2 usage/IO.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import sys
import unicodedata
from collections import Counter
from pathlib import Path
from typing import Iterable
from xml.parsers import expat

MAX_VARINT_BYTES = 5          # more than 5 bytes cannot describe a member this tool accepts
MAX_VARINT_VALUE = 1 << 32    # defensive: declared payload length must fit comfortably


class ProbeError(ValueError):
    """Fail-closed rejection. Carries a stable machine-readable reason code."""

    def __init__(self, code: str, detail: str) -> None:
        super().__init__(f"{code}: {detail}")
        self.code = code
        self.detail = detail


def decode_varint(data: bytes, pos: int = 0, limit: int = MAX_VARINT_BYTES) -> tuple[int, int]:
    """Decode a minimal unsigned base-128 varint at `pos`. Returns (value, byte_length).

    Rejects truncation, more than `limit` bytes, an out-of-range value, and non-minimal encodings
    (a continuation byte followed by a zero terminal group), so a caller cannot pass off an
    arbitrary 3-byte integer as a length.
    """
    value = 0
    for index in range(limit):
        offset = pos + index
        if offset >= len(data):
            raise ProbeError("VARINT_TRUNCATED", f"no terminal byte within {len(data) - pos} byte(s)")
        byte = data[offset]
        value |= (byte & 0x7F) << (7 * index)
        if value > MAX_VARINT_VALUE:
            raise ProbeError("VARINT_TOO_LARGE", f"value exceeds {MAX_VARINT_VALUE}")
        if not byte & 0x80:
            length = index + 1
            if length > 1 and byte == 0:
                raise ProbeError("VARINT_NON_MINIMAL",
                                 f"{length}-byte encoding ends in a zero group")
            return value, length
    raise ProbeError("VARINT_OVERLONG", f"more than {limit} continuation bytes")


def detect_bom(payload: bytes) -> str:
    """Return the byte-order-mark kind at offset 0. Does not strip anything."""
    for kind, prefix in (("utf-8", b"\xef\xbb\xbf"),
                         ("utf-32le", b"\xff\xfe\x00\x00"),
                         ("utf-32be", b"\x00\x00\xfe\xff"),
                         ("utf-16le", b"\xff\xfe"),
                         ("utf-16be", b"\xfe\xff")):
        if payload.startswith(prefix):
            return kind
    return "none"


def split_envelope(data: bytes) -> dict:
    """Split `data` into (header, payload) using the length-prefixed envelope. Fails closed."""
    if not data:
        raise ProbeError("EMPTY_MEMBER", "member has no bytes")
    head = data[:4]
    if head[:1] in (b"<",) or head.startswith(b"\xef\xbb\xbf"):
        return {"header_bytes": 0, "declared_payload_length": len(data),
                "payload_offset": 0, "payload": data, "header_hex": ""}
    declared, width = decode_varint(data, 0)
    payload = data[width:]
    if declared != len(payload):
        raise ProbeError("LENGTH_MISMATCH",
                         f"declared {declared} payload byte(s), member has {len(payload)}")
    if width > 0 and not payload:
        raise ProbeError("EMPTY_PAYLOAD", "declared payload is empty")
    return {"header_bytes": width, "declared_payload_length": declared,
            "payload_offset": width, "payload": payload, "header_hex": data[:width].hex()}


def classify_bytes(payload: bytes) -> dict:
    """Describe the payload's leading byte class without interpreting it as content."""
    bom = detect_bom(payload)
    body = payload
    if bom == "utf-8":
        body = payload[3:]
    # Leading ASCII whitespace is legal before an XML declaration or root element.
    index = 0
    while index < len(body) and body[index] in b" \t\r\n":
        index += 1
    first = body[index] if index < len(body) else None
    description = {
        "byte_order_mark": bom,
        "leading_whitespace_bytes": index,
        "first_significant_byte_hex": f"{first:02x}" if first is not None else None,
        "starts_with_xml_markup": first == 0x3C,     # '<'
        "nul_byte_count": payload.count(b"\x00"),
    }
    return description


def _is_xml_name_char_safe(name: str) -> bool:
    """Reject names that would smuggle arbitrary text into a report as a 'name'."""
    if not name or len(name) > 256:
        return False
    if name != unicodedata.normalize("NFC", name):
        return False
    return all(ch.isprintable() for ch in name) and " " not in name


def profile_xml(payload: bytes) -> dict:
    """Parse XML text with the standard library's non-validating parser. Names and counts only.

    Configuration: Expat (non-validating — it never consults a DTD for validation), parameter
    entity parsing disabled, and an external-entity handler that refuses and returns 0 so no file
    or network resource can be opened. Character data has no handler, so text nodes are never
    buffered by this tool; attribute handlers keep key names only.
    """
    elements: Counter[str] = Counter()
    attributes: Counter[str] = Counter()
    state = {"root": None, "depth": 0, "max_depth": 0, "doctype": False, "doctype_name": None,
             "entity_decls": 0, "external_entity_refs": 0, "attribute_occurrences": 0,
             "comment_count": 0, "processing_instruction_count": 0}

    def on_start(name: str, attrs: dict[str, str]) -> None:
        if state["root"] is None:
            state["root"] = name
        elements[name] += 1
        state["attribute_occurrences"] += len(attrs)
        attributes.update(attrs.keys())
        state["depth"] += 1
        state["max_depth"] = max(state["max_depth"], state["depth"])

    def on_end(_name: str) -> None:
        state["depth"] -= 1

    def on_doctype(name: str, _sysid: str | None, _pubid: str | None, _has_internal: int) -> None:
        state["doctype"] = True
        state["doctype_name"] = name

    def on_entity_decl(*_args: object) -> None:
        state["entity_decls"] += 1

    def on_external_entity_ref(*_args: object) -> int:
        # Refuse: no file, no network. Returning 0 makes Expat report a skipped external entity.
        state["external_entity_refs"] += 1
        return 0

    def on_comment(_data: str) -> None:
        state["comment_count"] += 1

    def on_pi(_target: str, _data: str) -> None:
        state["processing_instruction_count"] += 1

    parser = expat.ParserCreate()
    parser.StartElementHandler = on_start
    parser.EndElementHandler = on_end
    parser.StartDoctypeDeclHandler = on_doctype
    parser.EntityDeclHandler = on_entity_decl
    parser.ExternalEntityRefHandler = on_external_entity_ref
    parser.CommentHandler = on_comment
    parser.ProcessingInstructionHandler = on_pi
    parser.SetParamEntityParsing(expat.XML_PARAM_ENTITY_PARSING_NEVER)

    try:
        parser.Parse(payload, True)
    except expat.ExpatError as exc:
        raise ProbeError("EXPAT_REJECTED",
                         f"{expat.ErrorString(exc.code)} at line {exc.lineno}, "
                         f"column {exc.offset}") from None

    for name in list(elements) + list(attributes):
        if not _is_xml_name_char_safe(name):
            raise ProbeError("NAME_NOT_REPORTABLE", "element/attribute name is not printable")

    return {
        "status": "EXPAT_ACCEPTED_NON_VALIDATING",
        "root_element_name": state["root"],
        "doctype_declared": state["doctype"],
        "doctype_name": state["doctype_name"],
        "entity_declaration_count": state["entity_decls"],
        "external_entity_reference_count": state["external_entity_refs"],
        "comment_count": state["comment_count"],
        "processing_instruction_count": state["processing_instruction_count"],
        "element_count": sum(elements.values()),
        "distinct_element_name_count": len(elements),
        "element_name_counts": dict(sorted(elements.items())),
        "max_depth": state["max_depth"],
        "attribute_occurrences": state["attribute_occurrences"],
        "distinct_attribute_name_count": len(attributes),
        "attribute_name_counts": dict(sorted(attributes.items())),
        "character_data_recorded": False,
        "attribute_values_recorded": False,
    }


def probe_member(data: bytes, *, expect_size: int | None = None,
                 expect_sha256: str | None = None) -> dict:
    """Full bounded probe of one member held in memory. Raises ProbeError on any mismatch."""
    digest = hashlib.sha256(data).hexdigest()
    if expect_size is not None and len(data) != expect_size:
        raise ProbeError("SIZE_MISMATCH", f"expected {expect_size}, got {len(data)}")
    if expect_sha256 is not None and digest != expect_sha256.lower():
        raise ProbeError("SHA256_MISMATCH", f"expected {expect_sha256}, got {digest}")

    envelope = split_envelope(data)
    payload = envelope["payload"]
    byte_class = classify_bytes(payload)
    if not byte_class["starts_with_xml_markup"]:
        raise ProbeError("NOT_XML_TEXT",
                         f"payload does not begin with '<' (first byte "
                         f"{byte_class['first_significant_byte_hex']}); no XML structure claimed")
    if byte_class["nul_byte_count"]:
        raise ProbeError("NUL_IN_XML_PAYLOAD",
                         f"{byte_class['nul_byte_count']} NUL byte(s) inside the payload")
    try:
        payload.decode("utf-8")
    except UnicodeDecodeError as exc:
        raise ProbeError("NOT_UTF8", f"payload is not valid UTF-8 (byte {exc.start})") from None

    structure = profile_xml(payload)
    if not structure["element_count"] or structure["root_element_name"] is None:
        raise ProbeError("NO_ROOT_ELEMENT", "payload declared no element; no structure claimed")
    return {
        "member_bytes": len(data),
        "member_sha256": digest,
        "envelope": {
            "header_bytes": envelope["header_bytes"],
            "header_hex": envelope["header_hex"],
            "header_encoding": ("minimal unsigned base-128 varint (payload length)"
                                if envelope["header_bytes"] else "none"),
            "declared_payload_length": envelope["declared_payload_length"],
            "actual_payload_length": len(payload),
            "length_matches": True,
        },
        "payload_byte_class": byte_class,
        "payload_encoding": "utf-8" if byte_class["byte_order_mark"] == "none" else
                            byte_class["byte_order_mark"],
        "payload_utf8_valid": True,
        "structure": structure,
    }


class Report:
    def __init__(self) -> None:
        self.checks: list[dict[str, object]] = []

    def check(self, name: str, ok: bool, detail: str) -> bool:
        self.checks.append({"name": name, "ok": ok, "detail": detail})
        return ok

    @property
    def ok(self) -> bool:
        return all(bool(c["ok"]) for c in self.checks)


def _synthetic_varint(value: int) -> bytes:
    out = bytearray()
    while True:
        byte = value & 0x7F
        value >>= 7
        out.append(byte | (0x80 if value else 0))
        if not value:
            return bytes(out)


def self_test() -> int:
    """Synthetic fixtures only. Proves the envelope rule, the counts, and every rejection path."""
    report = Report()

    plain = b'<root a="SECRET"><child/><child b="v"/></root>'
    result = probe_member(plain)
    report.check("plain_header_zero", result["envelope"]["header_bytes"] == 0,
                 f"header_bytes {result['envelope']['header_bytes']}")
    report.check("plain_element_count", result["structure"]["element_count"] == 3,
                 f"{result['structure']['element_count']} elements, expected 3")
    report.check("plain_names", result["structure"]["element_name_counts"] == {"root": 1, "child": 2},
                 f"{result['structure']['element_name_counts']}")
    report.check("plain_attributes", result["structure"]["attribute_name_counts"] == {"a": 1, "b": 1},
                 f"{result['structure']['attribute_name_counts']}")
    report.check("plain_depth", result["structure"]["max_depth"] == 2,
                 f"max_depth {result['structure']['max_depth']}, expected 2")
    report.check("plain_root", result["structure"]["root_element_name"] == "root",
                 f"root {result['structure']['root_element_name']!r}")

    # The critical rule: a minimal base-128 length prefix is accepted and equals the payload size.
    prefixed = _synthetic_varint(len(plain)) + plain
    shaped = probe_member(prefixed)
    report.check("prefixed_header_width", shaped["envelope"]["header_bytes"] == len(_synthetic_varint(len(plain))),
                 f"header {shaped['envelope']['header_hex']} = {shaped['envelope']['header_bytes']} bytes")
    report.check("prefixed_length", shaped["envelope"]["declared_payload_length"] == len(plain),
                 f"declared {shaped['envelope']['declared_payload_length']} == {len(plain)}")
    report.check("prefixed_counts_match", shaped["structure"] == result["structure"],
                 "prefixed structure equals plain structure")
    for width, value in ((1, 0x7F), (2, 0x80), (2, 0x3FFF), (3, 0x4000), (3, 0x1FFFFF)):
        encoded = _synthetic_varint(value)
        decoded, used = decode_varint(encoded + b"\x00")
        report.check(f"varint_{value:#x}", decoded == value and used == width and len(encoded) == width,
                     f"{encoded.hex()} -> {decoded} in {used} byte(s)")

    # Rejections: every one must fail closed with a stable reason code.
    def rejects(label: str, data: bytes, code: str, **kwargs: object) -> None:
        try:
            probe_member(data, **kwargs)  # type: ignore[arg-type]
        except ProbeError as exc:
            report.check(f"reject_{label}", exc.code == code, f"{exc.code} ({exc.detail})")
        else:
            report.check(f"reject_{label}", False, f"accepted, expected {code}")

    rejects("length_mismatch", _synthetic_varint(len(plain) + 1) + plain, "LENGTH_MISMATCH")
    rejects("truncated_varint", b"\x83\xdb", "VARINT_TRUNCATED")
    rejects("non_minimal_varint", b"\x81\x80\x00" + plain, "VARINT_NON_MINIMAL")
    rejects("overlong_varint", b"\x81\x81\x81\x81\x81\x01" + plain, "VARINT_OVERLONG")
    rejects("empty", b"", "EMPTY_MEMBER")
    rejects("not_xml", _synthetic_varint(4) + b"\x00\x01\x02\x03", "NOT_XML_TEXT")
    rejects("nul_payload", b"<root>\x00</root>", "NUL_IN_XML_PAYLOAD")
    rejects("bad_utf8", b"<root>\xff\xfe</root>", "NOT_UTF8")
    rejects("malformed_xml", b"<root><unclosed></root>", "EXPAT_REJECTED")
    rejects("size_mismatch", plain, "SIZE_MISMATCH", expect_size=len(plain) + 1)
    rejects("sha_mismatch", plain, "SHA256_MISMATCH", expect_sha256="0" * 64)

    # A 3-byte prefix is exactly what the audited member shows; prove a non-varint 3-byte reading
    # cannot masquerade as the length (plain 24-bit LE/BE of 355715 is not 83 db 15).
    payload_355715 = b"<r/>" + b" " * (355_715 - 4)
    header = _synthetic_varint(len(payload_355715))
    report.check("audited_header_shape", header.hex() == "83db15" and len(header) == 3,
                 f"varint(355715) = {header.hex()}")
    report.check("audited_header_is_not_plain_int",
                 int.from_bytes(header, "little") != len(payload_355715)
                 and int.from_bytes(header, "big") != len(payload_355715),
                 "24-bit LE/BE interpretations do not equal the payload length")
    big = probe_member(header + payload_355715)
    report.check("audited_payload_accepted",
                 big["envelope"]["declared_payload_length"] == len(payload_355715),
                 f"{big['envelope']['declared_payload_length']} bytes")

    # UTF-8 BOM is reported, not silently stripped into the name space.
    bom_case = probe_member(b"\xef\xbb\xbf" + plain)
    report.check("bom_reported", bom_case["payload_byte_class"]["byte_order_mark"] == "utf-8",
                 f"bom {bom_case['payload_byte_class']['byte_order_mark']}")

    # A declared external entity must be refused by the handler, never resolved.
    refused = (b'<!DOCTYPE root [<!ENTITY ext SYSTEM "http://example.invalid/x">]>'
               b'<root>&ext;</root>')
    try:
        probe_member(refused)
    except ProbeError as exc:
        report.check("external_entity_refused", exc.code == "EXPAT_REJECTED",
                     f"{exc.code} (handler returned 0; nothing resolved)")
    else:
        report.check("external_entity_refused", False, "accepted a refused external entity")

    # An external DTD subset must never even be requested (parameter entity parsing is off).
    subset = b'<!DOCTYPE root SYSTEM "http://example.invalid/x.dtd"><root/>'
    subset_result = probe_member(subset)
    report.check("external_dtd_not_requested",
                 subset_result["structure"]["doctype_declared"] is True
                 and subset_result["structure"]["external_entity_reference_count"] == 0,
                 f"doctype {subset_result['structure']['doctype_name']!r} reported, "
                 f"external refs {subset_result['structure']['external_entity_reference_count']}")

    # No attribute value or character data may appear anywhere in a report.
    secret = "SECRET"
    rendered = json.dumps(probe_member(plain))
    report.check("no_attribute_values_in_report", secret not in rendered,
                 "synthetic attribute value absent from JSON report")
    rendered_prefixed = json.dumps(shaped)
    report.check("no_values_in_prefixed_report", secret not in rendered_prefixed,
                 "synthetic attribute value absent from prefixed report")

    ok = report.ok
    for check in report.checks:
        print(f"{'PASS' if check['ok'] else 'FAIL'} {check['name']}: {check['detail']}")
    print(f"self-test {'PASS' if ok else 'FAIL'} ({len(report.checks)} checks)")
    return 0 if ok else 1


def _load_from_archive(archive_path: Path, member: str) -> bytes:
    sys.dont_write_bytecode = True
    sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "scripts"))
    import psarc  # noqa: E402

    archive = psarc.Psarc(archive_path)
    try:
        return archive.read(archive.entry_index(member))
    finally:
        archive.close()


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--archive", type=Path, help="PSARC archive holding the member")
    parser.add_argument("--member", help="exact member path inside --archive")
    parser.add_argument("--file", type=Path, help="read one already-decoded member from disk")
    parser.add_argument("--expect-size", type=int, default=None)
    parser.add_argument("--expect-sha256", default=None)
    parser.add_argument("--json", action="store_true", help="print the probe as JSON")
    parser.add_argument("--self-test", action="store_true", help="run the synthetic fixture test")
    args = parser.parse_args(argv)

    if args.self_test:
        return self_test()
    if args.file and (args.archive or args.member):
        parser.error("--file cannot be combined with --archive/--member")
    if not args.file and not (args.archive and args.member):
        parser.error("use --file PATH or --archive ARCHIVE --member PATH (or --self-test)")
    if args.expect_size is not None and args.expect_size < 0:
        parser.error("--expect-size must not be negative")

    try:
        data = (args.file.expanduser().read_bytes() if args.file
                else _load_from_archive(args.archive.expanduser(), args.member))
    except (OSError, ValueError) as exc:      # psarc.PsarcError subclasses ValueError
        print(f"FAIL io: {exc}", file=sys.stderr)
        return 2

    try:
        result = probe_member(data, expect_size=args.expect_size,
                              expect_sha256=args.expect_sha256)
    except ProbeError as exc:
        print(json.dumps({"ok": False, "rejected": exc.code, "detail": exc.detail}, indent=2)
              if args.json else f"FAIL {exc.code}: {exc.detail}")
        return 1
    finally:
        data = b""

    if args.json:
        print(json.dumps({"ok": True, "probe": result}, indent=2, sort_keys=True))
    else:
        envelope = result["envelope"]
        structure = result["structure"]
        print(f"member {result['member_bytes']} B sha256 {result['member_sha256']}")
        print(f"envelope header {envelope['header_hex'] or '(none)'} "
              f"= {envelope['header_bytes']} B, declares {envelope['declared_payload_length']} B "
              f"(actual {envelope['actual_payload_length']})")
        print(f"payload utf-8 {result['payload_utf8_valid']}, BOM "
              f"{result['payload_byte_class']['byte_order_mark']}, NULs "
              f"{result['payload_byte_class']['nul_byte_count']}")
        print(f"root <{structure['root_element_name']}> elements {structure['element_count']} "
              f"({structure['distinct_element_name_count']} distinct), max depth "
              f"{structure['max_depth']}, attributes "
              f"{structure['attribute_occurrences']} "
              f"({structure['distinct_attribute_name_count']} distinct)")
        print(f"doctype {structure['doctype_declared']}, entities "
              f"{structure['entity_declaration_count']}, external refs "
              f"{structure['external_entity_reference_count']}")
        print("element names:", json.dumps(structure["element_name_counts"], sort_keys=True))
        print("attribute names:", json.dumps(structure["attribute_name_counts"], sort_keys=True))
        print("no character data or attribute values recorded; structure only")
        print(f"VERDICT: {structure['status']}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
