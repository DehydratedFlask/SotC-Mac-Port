#!/usr/bin/env python3
"""Pin the SXDF code-to-data reference and the program identity that produced it.

The bounded question this checker answers: at `0x9b5b10` the pinned image holds a 7-byte
rip-relative `LEA RSI,[0x12ba74a]`, and the 48 bytes at `0x12ba74a` begin `FORM\\0VAGp\\0SXDF\\0`.
Two providers must agree on exactly that, and both are checked here:

1. **The pinned ELF is the authority.** The bytes are read through `scripts/ps4_dynlink.py`
   (`Image.read`) after the file hashes to the pinned SHA-256, so a provider that disagrees with the
   file is caught rather than believed.
2. **Ghidra is the second provider.** `tools/re/ghidra/check_sxdf_ref.py` reads the same two
   addresses from the persistent project and prints `SXDF key=value` lines; a *present* dump that
   disagrees with the ELF fails here. Its program identity (name, language, image base, source
   SHA-256, function count) is pinned too, because the E1012 provider never exposed an image base.
3. **The saved disassembly** `local/validation/registry-analysis-agent/all.dis` is the third,
   independent render of the instruction line when it is present.

Nothing here reads an asset, decodes audio, or claims a reader/codec/playback use. The pins are
address correspondence and bytes only.

Skips cleanly (exit 0, `SKIP ...`) when every private input is absent, exactly like the other
private-input checkers. A *present* input that is wrong is a failure, never a skip.

`--elf` and `--dump` exist only so the failure paths can be demonstrated against a mutated copy;
the defaults are the pinned private paths.
"""

from __future__ import annotations

import argparse
import hashlib
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ELF = ROOT / "local/re/eboot.elf"
PINNED_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"

RUNNER = ROOT / "tools/re/ghidra/run_dump.py"
SCRIPT = ROOT / "tools/re/ghidra/check_sxdf_ref.py"
PIPELINE_DOC = ROOT / "docs/GHIDRA_PIPELINE.md"
CHECKPOINT = ROOT / "docs/checkpoints/2026-11-01-aud02-sxdf-data-reference-result.md"
DUMP = ROOT / "local/validation/aud02-sxdf-data-reference-ghidra-20261101/sxdf-ref.txt"
SAVED_DIS = ROOT / "local/validation/registry-analysis-agent/all.dis"

INSTRUCTION_RVA = 0x9B5B10
INSTRUCTION_BYTES = "488d35334c9000"
INSTRUCTION_LENGTH = 7
LEA_PREFIX = bytes((0x48, 0x8D, 0x35))  # REX.W + LEA + mod=00 r/m=101 -> RIP-relative, RSI
DISP32 = 0x904C33
TARGET_RVA = 0x12BA74A
DATA_BYTES = ("464f524d0056414770005358444600666d742000736d706c00666163740064617461"
              "0063756520006164746c00494e46")
DATA_LENGTH = 48

# Program identity the dump must confirm. The image is pinned by SHA-256, so the function count and
# language are deterministic for it.
GHIDRA_NAME = "eboot.elf"
GHIDRA_LANGUAGE = "x86:LE:64:default"
GHIDRA_IMAGE_BASE = "0x0"
GHIDRA_FUNCTION_COUNT = 38845

EXPECTED_DUMP = {
    "program_name": GHIDRA_NAME,
    "language": GHIDRA_LANGUAGE,
    "image_base": GHIDRA_IMAGE_BASE,
    "executable_sha256": PINNED_SHA256,
    "function_count": str(GHIDRA_FUNCTION_COUNT),
    "instruction_rva": "0x%x" % INSTRUCTION_RVA,
    "instruction_present": "yes",
    "instruction_length": str(INSTRUCTION_LENGTH),
    "instruction_bytes": INSTRUCTION_BYTES,
    "rip_target": "0x%x" % TARGET_RVA,
    "byte_target": "0x%x" % TARGET_RVA,
    "verdict_instruction_matches_e1007": "yes",
    "verdict_target_matches_e1007": "yes",
    "verdict_ghidra_and_bytes_agree": "yes",
    "data_rva": "0x%x" % TARGET_RVA,
    "data_bytes": DATA_BYTES,
    "data_bytes_length": str(DATA_LENGTH),
    # Ghidra models the reference target as an *undefined* byte, not as a typed data item or a
    # string. Recording that keeps the honest scope: the evidentiary content is the byte run.
    "data_kind": "inside-data",
    "data_exact": "none",
    "data_containing_start": "0x%x" % TARGET_RVA,
    "data_containing_length": "1",
    "data_containing_type": "undefined",
}

SAVED_DIS_LINE = re.compile(r"^\s*9b5b10:\s")


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit("FAIL " + msg)


def sha256_of(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def check_public_surface() -> None:
    for path in (RUNNER, SCRIPT, PIPELINE_DOC, CHECKPOINT):
        require(path.is_file(), "missing %s" % path.relative_to(ROOT))

    doc = PIPELINE_DOC.read_text()
    for needle in ("tools/re/ghidra/run_dump.py", PINNED_SHA256):
        require(needle in doc, "docs/GHIDRA_PIPELINE.md no longer documents %r" % needle)

    checkpoint = CHECKPOINT.read_text()
    require(str(SCRIPT.relative_to(ROOT)) in checkpoint,
            "the checkpoint no longer names tools/re/ghidra/check_sxdf_ref.py")
    for needle in ("0x9b5b10", "0x12ba74a"):
        require(needle in checkpoint, "the checkpoint no longer records %s" % needle)

    text = SCRIPT.read_text()
    # The script must stay bounded to the two addresses, and must not grow an xref or decompile path.
    for needle in ("0x9B5B10", "0x12BA74A", "SXDF %s=%s"):
        require(needle in text, "check_sxdf_ref.py no longer names %r" % needle)
    for forbidden in ("DecompInterface", "getReferencesTo", "getCodeUnitIterator", "getBody()"):
        require(forbidden not in text,
                "check_sxdf_ref.py grew %r; the task is bounded to two addresses" % forbidden)

    require("0x9b5b10" in CHECKPOINT.read_text() and "0x12ba74a" in CHECKPOINT.read_text(),
            "the checkpoint no longer records both addresses")


def check_elf(elf: Path):
    """Return (facts, instruction_bytes, target) after proving the file is the pinned image."""
    require(elf.is_file(), "%s is missing" % elf)
    require(sha256_of(elf) == PINNED_SHA256,
            "%s is not the pinned image (%s)" % (elf, sha256_of(elf)))

    sys.path.insert(0, str(ROOT / "scripts"))
    import ps4_dynlink  # noqa: E402  (after the hash check, so the import is the authority's reader)

    raw = elf.read_bytes()
    _entry, ph = ps4_dynlink.program_headers(raw)
    image = ps4_dynlink.Image(raw, ph)

    instruction = image.read(INSTRUCTION_RVA, INSTRUCTION_LENGTH)
    require(instruction.hex() == INSTRUCTION_BYTES,
            "0x%x bytes are %s, pinned %s" % (INSTRUCTION_RVA, instruction.hex(), INSTRUCTION_BYTES))
    require(instruction[:3] == LEA_PREFIX,
            "0x%x is not a RIP-relative LEA into RSI (%s)" % (INSTRUCTION_RVA, instruction[:3].hex()))

    disp = int.from_bytes(instruction[3:7], "little", signed=True)
    require(disp == DISP32, "0x%x disp32 is 0x%x, pinned 0x%x"
            % (INSTRUCTION_RVA, disp & 0xFFFFFFFF, DISP32))

    target = INSTRUCTION_RVA + INSTRUCTION_LENGTH + disp
    require(target == TARGET_RVA,
            "0x%x computes 0x%x, pinned 0x%x" % (INSTRUCTION_RVA, target, TARGET_RVA))
    require(image.mapped(target, DATA_LENGTH), "0x%x is not file-backed in a loadable segment" % target)

    data = image.read(target, DATA_LENGTH)
    require(data.hex() == DATA_BYTES,
            "0x%x bytes are %s, pinned %s" % (target, data.hex(), DATA_BYTES))
    require(data[:4] == b"FORM" and data[10:14] == b"SXDF",
            "0x%x no longer holds FORM at +0 and SXDF at +10" % target)

    return ["elf sha256 pinned", "instruction bytes pinned", "disp32/target pinned",
            "data 48 bytes pinned"]


def parse_dump(path: Path):
    rows = {}
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        if not line.startswith("SXDF "):
            continue
        body = line[len("SXDF "):]
        if "=" not in body:
            continue
        key, value = body.split("=", 1)
        rows.setdefault(key, []).append(value)
    return rows


def check_dump(path: Path):
    require(path.is_file(), "%s is missing" % path)
    rows = parse_dump(path)
    require(rows, "%s has no SXDF key=value rows" % path.name)

    for key, expected in EXPECTED_DUMP.items():
        require(key in rows, "dump is missing %s" % key)
        recorded = rows[key][-1]
        require(recorded == expected, "dump %s=%r, pinned %r" % (key, recorded, expected))

    text = rows["instruction_text"][-1]
    require("LEA" in text.upper() and "0x%x" % TARGET_RVA in text,
            "dump instruction_text %r does not name the LEA target 0x%x" % (text, TARGET_RVA))
    require("perms=R-X" in rows["instruction_block"][-1],
            "0x%x is no longer in an executable block" % INSTRUCTION_RVA)
    require("perms=R-X" in rows["data_block"][-1],
            "0x%x is no longer in the executable read-only block" % TARGET_RVA)

    data_ascii = rows["data_ascii"][-1]
    require("SXDF" in data_ascii, "dump data_ascii no longer shows SXDF: %r" % data_ascii)

    return ["Ghidra identity pinned (base %s)" % rows["image_base"][-1],
            "Ghidra instruction/reference pinned", "Ghidra data bytes pinned"]


def check_saved_disassembly(path: Path):
    require(path.is_file(), "%s is missing" % path)
    hits = []
    with path.open("r", encoding="utf-8", errors="replace") as handle:
        for line in handle:
            if SAVED_DIS_LINE.match(line):
                hits.append(line.rstrip("\n"))
    require(len(hits) == 1, "%s has %d rows for 0x%x, expected 1"
            % (path.name, len(hits), INSTRUCTION_RVA))
    line = hits[0]
    require("leaq" in line and "0x904c33(%rip)" in line and "0x%x" % TARGET_RVA in line,
            "saved disassembly line disagrees with the pinned reference: %r" % line)
    return ["saved disassembly line pinned"]


def main(argv) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--elf", default=str(ELF), help="override the image (mutation testing only)")
    parser.add_argument("--dump", default=str(DUMP), help="override the Ghidra dump (mutation testing only)")
    parser.add_argument("--saved-dis", default=str(SAVED_DIS), help="override the saved disassembly")
    args = parser.parse_args(argv)

    check_public_surface()

    elf = Path(args.elf)
    dump = Path(args.dump)
    saved_dis = Path(args.saved_dis)

    facts = []
    absent = []
    if elf.is_file():
        try:
            facts.extend(check_elf(elf))
        except SystemExit:
            raise
    else:
        absent.append("elf absent")
    if dump.is_file():
        facts.extend(check_dump(dump))
    else:
        absent.append("ghidra dump absent")
    if saved_dis.is_file():
        facts.extend(check_saved_disassembly(saved_dis))
    else:
        absent.append("saved disassembly absent")

    if len(absent) == 3:
        print("SKIP check_sxdf_data_reference: pinned ELF / Ghidra dump / saved disassembly absent "
              "(address and identity pins checked)")
        return 0
    if not facts:
        raise SystemExit("FAIL no private input could be verified")
    print("OK check_sxdf_data_reference: " + ", ".join(facts))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
