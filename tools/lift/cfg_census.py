#!/usr/bin/env python3
"""cfg_census.py -- static direct-call CFG census over a textual disassembly.

Reads an unwind-table (FDE) range dump plus an `llvm-objdump -D` style text
disassembly of the same module and reports:

  * FDE range sanity: count, byte total, overlaps, duplicate keys.
  * Address coverage of the executable range derived from the ELF PT_LOAD
    program headers (or from --text-range, or fall back to the FDE span).
  * A single streaming pass over the disassembly that decodes direct call
    (and optionally direct jump) sites and classifies every edge target as
    internal (inside an FDE range), external (outside every range but still a
    plausible in-image address) or unresolved.
  * The transitive direct-call closure of a caller-supplied seed set, plus the
    size of the "unresolved frontier": closure members that contain at least
    one outgoing indirect call or indirect jump.

Nothing is executed.  No indirect target is guessed, resolved or invented: the
indirect sites are counted and attributed to their containing function only.

Stdlib only.  Deterministic: identical inputs produce byte-identical --json.

Exit codes
  0  success
  2  input unreadable / malformed (missing file, bad JSON, bad ELF, bad CLI)
  4  FDE ranges overlap (breaks one-function-per-address attribution)
"""

from __future__ import annotations

import argparse
import bisect
import hashlib
import json
import os
import re
import struct
import sys
import time
from pathlib import Path

SCHEMA = "cfg_census/1"

REPO_ROOT = Path(__file__).resolve().parents[2]
DEFAULT_FDE = REPO_ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"
DEFAULT_DIS = REPO_ROOT / "local/validation/registry-analysis-agent/all.dis"
DEFAULT_ELF = REPO_ROOT / "local/re/eboot.elf"

# PT_LOAD with PF_X
PT_LOAD = 1
PF_X = 0x1

# "addr:" prefix of an objdump disassembly line.
_LINE = re.compile(r"^\s*([0-9a-fA-F]+):\s+(.*\S)\s*$")
# Optional instruction prefix ("rep", "lock", "addr32", ...), mnemonic, operands.
_INSN = re.compile(r"^(?:([a-z][a-z0-9._]*)\s+)?([a-z][a-z0-9._]*)\s+(\S.*)$")

CALL_MNEMONICS = ("callq", "lcallq", "call", "calll")
JMP_MNEMONICS = ("jmp", "jmpq", "jmpw", "jmpl")

INT_MAX = 1 << 63


def die(msg: str, code: int = 2) -> "NoReturn":  # noqa: F821 - typing only
    sys.stderr.write("cfg_census: error: %s\n" % msg)
    raise SystemExit(code)


def hx(value: int) -> str:
    return "0x%x" % value


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with open(path, "rb") as handle:
        for block in iter(lambda: handle.read(1 << 20), b""):
            digest.update(block)
    return digest.hexdigest()


# --------------------------------------------------------------------------
# inputs
# --------------------------------------------------------------------------


class Ranges:
    """Sorted, non-overlapping address ranges with O(log n) lookup."""

    def __init__(self, items):
        # items: iterable of (start, length)
        self.items = sorted(items)
        self.starts = [start for start, _ in self.items]
        self.span = sum(length for _, length in self.items)

    def __len__(self) -> int:
        return len(self.items)

    def find(self, addr: int):
        """Return (start, length) of the range containing addr, else None."""
        index = bisect.bisect_right(self.starts, addr) - 1
        if index < 0:
            return None
        start, length = self.items[index]
        if addr < start + length:
            return (start, length)
        return None

    def overlaps(self):
        found = []
        for (start, length), (next_start, _) in zip(self.items, self.items[1:]):
            if next_start < start + length:
                found.append((start, start + length, next_start))
        return found

    def gaps(self, extra_end: int):
        gaps = []
        cursor = 0
        for start, length in self.items:
            if start > cursor:
                gaps.append((cursor, start - cursor))
            cursor = max(cursor, start + length)
        if cursor < extra_end:
            gaps.append((cursor, extra_end - cursor))
        return gaps


def load_fde_ranges(path: Path):
    """Load {"0xSTART": length} with duplicate-key detection."""
    try:
        raw = path.read_text(encoding="utf-8")
    except OSError as exc:
        die("cannot read FDE ranges %s: %s" % (path, exc))

    duplicates = []

    def hook(pairs):
        seen = {}
        for key, value in pairs:
            if key in seen:
                duplicates.append(key)
            seen[key] = value
        return seen

    try:
        data = json.loads(raw, object_pairs_hook=hook)
    except json.JSONDecodeError as exc:
        die("malformed FDE ranges JSON %s: %s" % (path, exc))

    if not isinstance(data, dict):
        die("FDE ranges %s is not a JSON object" % path)

    items = []
    for key, value in data.items():
        try:
            start = int(key, 0)
        except (TypeError, ValueError):
            die("FDE ranges %s: bad start address %r" % (path, key))
        if not isinstance(value, int) or isinstance(value, bool):
            die("FDE ranges %s: length for %s is not an integer" % (path, key))
        if start < 0 or value < 0:
            die("FDE ranges %s: negative range %s -> %s" % (path, key, value))
        items.append((start, value))
    if not items:
        die("FDE ranges %s is empty" % path)
    return Ranges(items), duplicates


def elf_exec_ranges(path: Path):
    """Return (exec_ranges, image_end) from ELF64 program headers."""
    try:
        with open(path, "rb") as handle:
            header = handle.read(64)
            if len(header) < 64 or header[:4] != b"\x7fELF":
                die("%s is not an ELF file" % path)
            if header[4] != 2:
                die("%s is not ELF64 (EI_CLASS=%d)" % (path, header[4]))
            if header[5] != 1:
                die("%s is not little-endian (EI_DATA=%d)" % (path, header[5]))
            phoff = struct.unpack_from("<Q", header, 32)[0]
            phentsize = struct.unpack_from("<H", header, 54)[0]
            phnum = struct.unpack_from("<H", header, 56)[0]
            if phentsize < 56 or phnum == 0:
                die("%s has no usable program headers" % path)
            handle.seek(phoff)
            table = handle.read(phentsize * phnum)
    except OSError as exc:
        die("cannot read ELF %s: %s" % (path, exc))

    if len(table) != phentsize * phnum:
        die("%s program header table is truncated" % path)

    exec_ranges = []
    image_end = 0
    for index in range(phnum):
        off = index * phentsize
        p_type, p_flags = struct.unpack_from("<II", table, off)
        # p_offset @8, p_vaddr @16, p_paddr @24, p_filesz @32, p_memsz @40
        _, p_vaddr, _, _, p_memsz = struct.unpack_from("<QQQQQ", table, off + 8)
        if p_type != PT_LOAD:
            continue
        end = p_vaddr + p_memsz
        image_end = max(image_end, end)
        if p_flags & PF_X:
            exec_ranges.append((p_vaddr, p_memsz))
    if not exec_ranges:
        die("%s has no executable PT_LOAD segment" % path)
    return Ranges(exec_ranges), image_end


def parse_text_range(text: str):
    try:
        start_text, end_text = text.split("-", 1)
        start, end = int(start_text, 0), int(end_text, 0)
    except ValueError:
        die("--text-range must look like 0xSTART-0xEND, got %r" % text)
    if end <= start:
        die("--text-range end must exceed start (%s)" % text)
    return Ranges([(start, end - start)])


# --------------------------------------------------------------------------
# disassembly pass
# --------------------------------------------------------------------------


class EdgeStats:
    def __init__(self):
        self.lines = 0
        self.call_sites = 0
        self.direct_calls = 0
        self.indirect_calls = 0
        self.direct_jumps = 0
        self.indirect_jumps = 0
        self.unparsed_call_lines = 0
        self.unparsed_jump_lines = 0
        self.calls_from_non_fde_source = 0
        self.internal = 0
        self.internal_from_fde_source = 0
        self.internal_from_other_source = 0
        self.external_in_image = 0
        self.ext_below_first_fde = 0
        self.ext_in_text_gap = 0
        self.ext_after_last_fde = 0
        self.external_out_of_image = 0
        self.unresolved = 0
        self.midrange_targets = 0
        self.out_of_image_from_fde_source = 0
        self.out_of_image_from_other_source = 0
        self.internal_edges = set()
        self.internal_callees = set()
        self.callees_external = set()
        self.callees_out_of_image = set()
        # function start -> {"icall": n, "ijmp": n, "calls": n}
        self.functions = {}


def scan_disassembly(path: Path, fdes: Ranges, text: Ranges, stats: EdgeStats,
                     follow_direct_jumps: bool):
    """One streaming pass.  Fills `stats`; returns the set of FDE starts."""
    fde_starts = set()
    for start, _ in fdes.items:
        fde_starts.add(start)
    first_fde_start = fdes.items[0][0]
    last_fde_end = fdes.items[-1][0] + fdes.items[-1][1]

    try:
        handle = open(path, "r", encoding="utf-8", errors="replace")
    except OSError as exc:
        die("cannot read disassembly %s: %s" % (path, exc))

    find = fdes.find
    functions = stats.functions
    internal_edges = stats.internal_edges
    line_no = 0

    with handle:
        for line in handle:
            line_no += 1
            # Cheap prefilter: only instruction lines that can carry an edge.
            if "call" not in line and "jmp" not in line:
                continue
            match = _LINE.match(line)
            if match is None:
                # "		..." continuation / blank padding.
                continue
            source = int(match.group(1), 16)
            insn = _INSN.match(match.group(2))
            if insn is None:
                continue
            mnemonic = insn.group(2)
            operands = insn.group(3)

            is_call = mnemonic in CALL_MNEMONICS
            is_jump = mnemonic in JMP_MNEMONICS
            if not is_call and not is_jump:
                continue

            owner = find(source)
            owner_start = None if owner is None else owner[0]
            record = None
            if owner_start is not None:
                record = functions.get(owner_start)
                if record is None:
                    record = {"icall": 0, "ijmp": 0, "calls": 0}
                    functions[owner_start] = record

            if is_call:
                stats.call_sites += 1
                if owner_start is None:
                    stats.calls_from_non_fde_source += 1

            target = None
            indirect = operands.startswith("*")
            if indirect:
                if is_call:
                    stats.indirect_calls += 1
                    if record is not None:
                        record["icall"] += 1
                else:
                    stats.indirect_jumps += 1
                    if record is not None:
                        record["ijmp"] += 1
                continue

            head = operands.split()[0]
            try:
                if head.startswith("0x") or head.startswith("-0x"):
                    target = int(head, 16)
                elif head[:1].isdigit():
                    target = int(head, 10)
            except ValueError:
                target = None

            if target is None:
                if is_call:
                    stats.unparsed_call_lines += 1
                    stats.unresolved += 1
                else:
                    stats.unparsed_jump_lines += 1
                continue

            if is_jump:
                stats.direct_jumps += 1
                if not follow_direct_jumps:
                    continue
                # Only tail calls into a known function entry are followed;
                # intra-function branches are not part of a call graph.
                if target not in fde_starts:
                    continue

            if is_call:
                stats.direct_calls += 1
                if record is not None:
                    record["calls"] += 1

            containing = find(target)
            if containing is not None:
                callee = containing[0]
                if target != callee:
                    stats.midrange_targets += 1
                stats.internal += 1
                if owner_start is None:
                    stats.internal_from_other_source += 1
                else:
                    stats.internal_from_fde_source += 1
                internal_edges.add((owner_start, callee))
                stats.internal_callees.add(callee)
            elif text.find(target) is not None:
                stats.external_in_image += 1
                if target < first_fde_start:
                    stats.ext_below_first_fde += 1
                elif target >= last_fde_end:
                    stats.ext_after_last_fde += 1
                else:
                    stats.ext_in_text_gap += 1
                stats.callees_external.add(target)
            else:
                stats.external_out_of_image += 1
                stats.callees_out_of_image.add(target)
                if owner_start is None:
                    stats.out_of_image_from_other_source += 1
                else:
                    stats.out_of_image_from_fde_source += 1

    stats.lines = line_no
    return fde_starts


# --------------------------------------------------------------------------
# closure
# --------------------------------------------------------------------------


def build_adjacency(edges):
    adjacency = {}
    for caller, callee in edges:
        if caller is None:
            continue
        adjacency.setdefault(caller, []).append(callee)
    for callees in adjacency.values():
        callees.sort()
    return adjacency


def closure(seeds, adjacency):
    reached = set()
    stack = list(seeds)
    while stack:
        node = stack.pop()
        if node in reached:
            continue
        reached.add(node)
        for callee in adjacency.get(node, ()):
            if callee not in reached:
                stack.append(callee)
    return reached


# --------------------------------------------------------------------------
# main
# --------------------------------------------------------------------------


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(
        prog="cfg_census.py",
        description="Static direct-call CFG census over an objdump text disassembly "
                    "(nothing is executed; indirect targets are counted, never resolved).",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="Exit codes: 0 ok, 2 unreadable/malformed input, 4 FDE ranges overlap.",
    )
    parser.add_argument("--fde", default=str(DEFAULT_FDE),
                        help="FDE range JSON {0xSTART: length} (default: %(default)s)")
    parser.add_argument("--dis", default=str(DEFAULT_DIS),
                        help="objdump -D text disassembly (default: %(default)s)")
    parser.add_argument("--elf", default=str(DEFAULT_ELF),
                        help="ELF used for executable PT_LOAD coverage (default: %(default)s)")
    parser.add_argument("--no-elf", action="store_true",
                        help="skip the ELF and derive the text range from the FDE span")
    parser.add_argument("--text-range", default=None, metavar="0xSTART-0xEND",
                        help="override the executable address range")
    parser.add_argument("--seed", action="append", default=[], metavar="0xADDR",
                        help="closure seed function address (repeatable)")
    parser.add_argument("--include-direct-jumps", action="store_true",
                        help="also follow direct jmp edges whose target is an FDE start "
                             "(tail calls); off by default")
    parser.add_argument("--json", default=None, metavar="PATH",
                        help="write the deterministic JSON summary here")
    parser.add_argument("--no-hash", action="store_true",
                        help="skip sha256 of the inputs (faster, less provenance)")
    parser.add_argument("--top-gaps", type=int, default=5,
                        help="report this many largest coverage gaps (default: %(default)s)")
    args = parser.parse_args(argv)

    started = time.time()

    fde_path = Path(args.fde)
    dis_path = Path(args.dis)
    if not fde_path.is_file():
        die("FDE ranges not found: %s" % fde_path)
    if not dis_path.is_file():
        die("disassembly not found: %s" % dis_path)

    fdes, duplicate_keys = load_fde_ranges(fde_path)
    overlaps = fdes.overlaps()

    if args.text_range is not None:
        text = parse_text_range(args.text_range)
        text_source = "cli"
    elif args.no_elf:
        text = Ranges([(fdes.items[0][0], fdes.items[-1][0] + fdes.items[-1][1]
                        - fdes.items[0][0])])
        text_source = "fde-span (assumed; coverage is circular)"
    else:
        elf_path = Path(args.elf)
        if not elf_path.is_file():
            die("ELF not found: %s (use --no-elf or --text-range)" % elf_path)
        text, image_end_hint = elf_exec_ranges(elf_path)
        text_source = "elf-ptload-x"

    text_start = text.items[0][0]
    text_end = max(start + length for start, length in text.items)

    if args.no_elf and args.text_range is None:
        image_end = text_end
        image_end_source = "unknown (derived from FDE span)"
    elif args.text_range is not None:
        image_end = text_end
        image_end_source = "unknown (--text-range)"
    else:
        image_end = max(image_end_hint, text_end)
        image_end_source = "elf-ptload-max"

    if overlaps:
        sys.stderr.write("cfg_census: error: %d overlapping FDE ranges; "
                         "one-function-per-address attribution is unsound\n" % len(overlaps))
        for start, end, next_start in overlaps[:10]:
            sys.stderr.write("  %s+%s overlaps %s\n"
                             % (hx(start), hx(end - start), hx(next_start)))
        return 4

    stats = EdgeStats()
    follow_jumps = args.include_direct_jumps
    print("cfg_census: scanning %s" % dis_path)
    sys.stdout.flush()
    fde_starts = scan_disassembly(dis_path, fdes, text, stats, follow_jumps)

    if stats.direct_calls == 0:
        die("no direct call sites decoded in %s -- wrong file or wrong format" % dis_path)

    seeds = []
    for raw in args.seed:
        try:
            seed = int(raw, 0)
        except ValueError:
            die("bad --seed %r (expected 0xADDR)" % raw)
        containing = fdes.find(seed)
        if containing is None:
            die("seed %s is not inside any FDE range" % hx(seed))
        if containing[0] != seed:
            sys.stderr.write("cfg_census: note: seed %s is inside function %s, snapping\n"
                             % (hx(seed), hx(containing[0])))
        seeds.append(containing[0])
    seeds = sorted(set(seeds))

    adjacency = build_adjacency(stats.internal_edges)
    reached = closure(seeds, adjacency) if seeds else set()

    frontier = sorted(fn for fn in reached
                      if fn in stats.functions
                      and (stats.functions[fn]["icall"] or stats.functions[fn]["ijmp"]))
    closure_edges = sum(len(adjacency.get(fn, ())) for fn in reached)
    internal_targets = sorted({callee for fn in reached for callee in adjacency.get(fn, ())})

    gaps = fdes.gaps(text_end)
    gaps.sort(key=lambda gap: (-gap[1], gap[0]))
    top_gaps = gaps[: max(0, args.top_gaps)]
    covered = sum(length for start, length in fdes.items
                  if start < text_end and start + length > text_start)

    hashes = {}
    if not args.no_hash:
        for label, path in (("fde", fde_path), ("dis", dis_path)):
            hashes[label] = sha256_file(path)
        if not args.no_elf and args.text_range is None:
            hashes["elf"] = sha256_file(Path(args.elf))

    summary = {
        "schema": SCHEMA,
        "inputs": {
            "fde": {"path": str(fde_path), "sha256": hashes.get("fde")},
            "dis": {"path": str(dis_path), "sha256": hashes.get("dis")},
            "elf": (None if args.no_elf or args.text_range is not None
                    else {"path": str(Path(args.elf)), "sha256": hashes.get("elf")}),
        },
        "fde": {
            "count": len(fdes),
            "total_bytes": fdes.span,
            "min_start": hx(fdes.items[0][0]),
            "max_end": hx(fdes.items[-1][0] + fdes.items[-1][1]),
            "overlap_count": len(overlaps),
            "duplicate_keys": sorted(duplicate_keys),
            "zero_length_count": sum(1 for _, length in fdes.items if length == 0),
            "adjacent_pairs": sum(1 for (s, l), (n, _) in zip(fdes.items, fdes.items[1:])
                                  if s + l == n),
        },
        "text": {
            "source": text_source,
            "ranges": [[hx(start), hx(start + length)] for start, length in text.items],
            "total_bytes": text.span,
            "covered_bytes": covered,
            "uncovered_bytes": text.span - covered,
            "coverage_pct": (round(100.0 * covered / text.span, 4) if text.span else 0.0),
            "gap_count": len(gaps),
            "largest_gaps": [{"start": hx(start), "length": length} for start, length in top_gaps],
            "image_end": hx(image_end),
            "image_end_source": image_end_source,
        },
        "disassembly": {
            "lines": stats.lines,
            "call_sites": stats.call_sites,
            "direct_calls": stats.direct_calls,
            "indirect_calls": stats.indirect_calls,
            "direct_jumps": stats.direct_jumps,
            "indirect_jumps": stats.indirect_jumps,
            "unparsed_call_lines": stats.unparsed_call_lines,
            "unparsed_jump_lines": stats.unparsed_jump_lines,
            "calls_from_non_fde_source": stats.calls_from_non_fde_source,
            "source_functions": len(stats.functions),
        },
        "edges": {
            "internal": stats.internal,
            "internal_from_fde_source": stats.internal_from_fde_source,
            "internal_from_other_source": stats.internal_from_other_source,
            "internal_unique": len(stats.internal_edges),
            "internal_callees_unique": len(stats.internal_callees),
            "external_in_image": stats.external_in_image,
            "external_in_image_below_first_fde": stats.ext_below_first_fde,
            "external_in_image_text_gap": stats.ext_in_text_gap,
            "external_in_image_after_last_fde": stats.ext_after_last_fde,
            "external_in_image_callees_unique": len(stats.callees_external),
            "external_out_of_image": stats.external_out_of_image,
            "external_out_of_image_unique": len(stats.callees_out_of_image),
            "unresolved": stats.unresolved,
            "midrange_targets": stats.midrange_targets,
            "out_of_image_from_fde_source": stats.out_of_image_from_fde_source,
            "out_of_image_from_other_source": stats.out_of_image_from_other_source,
        },
        "closure": {
            "requested": bool(seeds),
            "follow_direct_jumps": follow_jumps,
            "seeds": [hx(seed) for seed in seeds],
            "reached": len(reached),
            "reached_list": [hx(fn) for fn in sorted(reached)],
            "edges_followed": closure_edges,
            "distinct_callees": len(internal_targets),
            "indirect_frontier_size": len(frontier),
            "indirect_frontier": [
                {
                    "fn": hx(fn),
                    "indirect_calls": stats.functions[fn]["icall"],
                    "indirect_jumps": stats.functions[fn]["ijmp"],
                }
                for fn in frontier
            ],
            "per_seed": {
                hx(seed): len(closure([seed], adjacency)) for seed in seeds
            },
        },
    }

    elapsed = time.time() - started

    # ---- human report -----------------------------------------------------
    out = sys.stdout
    out.write("\n== inputs ==\n")
    out.write("  fde  %s%s\n" % (fde_path, "" if not hashes.get("fde") else "  sha256=%s" % hashes["fde"]))
    out.write("  dis  %s%s\n" % (dis_path, "" if not hashes.get("dis") else "  sha256=%s" % hashes["dis"]))
    if hashes.get("elf"):
        out.write("  elf  %s  sha256=%s\n" % (args.elf, hashes["elf"]))

    out.write("\n== FDE ranges ==\n")
    out.write("  count          %d\n" % len(fdes))
    out.write("  total bytes    %d\n" % fdes.span)
    out.write("  extent         %s .. %s\n"
              % (hx(fdes.items[0][0]), hx(fdes.items[-1][0] + fdes.items[-1][1])))
    out.write("  overlaps       %d\n" % len(overlaps))
    out.write("  duplicate keys %d\n" % len(duplicate_keys))
    out.write("  zero length    %d\n" % summary["fde"]["zero_length_count"])

    out.write("\n== executable coverage (%s) ==\n" % text_source)
    for start, length in text.items:
        out.write("  text range     %s .. %s (%d bytes)\n" % (hx(start), hx(start + length), length))
    out.write("  covered        %d / %d bytes (%.2f%%)\n"
              % (covered, text.span, summary["text"]["coverage_pct"]))
    out.write("  gaps           %d\n" % len(gaps))
    for start, length in top_gaps:
        out.write("    gap %s len %d\n" % (hx(start), length))
    out.write("  image end      %s\n" % hx(image_end))

    out.write("\n== disassembly pass ==\n")
    out.write("  lines scanned        %d\n" % stats.lines)
    out.write("  call sites           %d\n" % stats.call_sites)
    out.write("    direct             %d\n" % stats.direct_calls)
    out.write("    indirect           %d\n" % stats.indirect_calls)
    out.write("  jumps                %d (direct %d, indirect %d)\n"
              % (stats.direct_jumps + stats.indirect_jumps,
                 stats.direct_jumps, stats.indirect_jumps))
    out.write("  unparsed call lines  %d\n" % stats.unparsed_call_lines)
    out.write("  calls from non-FDE    %d\n" % stats.calls_from_non_fde_source)
    out.write("  source functions     %d\n" % len(stats.functions))

    out.write("\n== direct edges ==\n")
    out.write("  internal (in FDE)    %d  (from FDE source %d, from elsewhere %d)\n"
              % (stats.internal, stats.internal_from_fde_source,
                 stats.internal_from_other_source))
    out.write("                       unique pairs %d, unique callees %d\n"
              % (len(stats.internal_edges), len(stats.internal_callees)))
    out.write("  external in-image    %d  (unique callees %d)\n"
              % (stats.external_in_image, len(stats.callees_external)))
    out.write("                       below first FDE %d, inside FDE gaps %d, after last FDE %d\n"
              % (stats.ext_below_first_fde, stats.ext_in_text_gap,
                 stats.ext_after_last_fde))
    out.write("  external out-image   %d  (unique callees %d)\n"
              % (stats.external_out_of_image, len(stats.callees_out_of_image)))
    out.write("  unresolved           %d\n" % stats.unresolved)
    out.write("  mid-range targets    %d\n" % stats.midrange_targets)
    out.write("  out-of-image from FDE source / other source: %d / %d\n"
              % (stats.out_of_image_from_fde_source, stats.out_of_image_from_other_source))

    out.write("\n== closure ==\n")
    for seed in seeds:
        out.write("  seed %s alone       %d functions\n"
                  % (hx(seed), summary["closure"]["per_seed"][hx(seed)]))
    if not seeds:
        out.write("  no --seed given; closure not computed\n")
    else:
        out.write("  seeds               %s\n" % ", ".join(hx(seed) for seed in seeds))
        out.write("  reached             %d\n" % len(reached))
        out.write("  edges followed      %d\n" % closure_edges)
        out.write("  indirect frontier   %d functions (%.1f%% of closure)\n"
                  % (len(frontier), 100.0 * len(frontier) / max(1, len(reached))))
        for fn in frontier[:10]:
            record = stats.functions[fn]
            out.write("    %s  indirect calls %d, indirect jumps %d\n"
                      % (hx(fn), record["icall"], record["ijmp"]))
        if len(frontier) > 10:
            out.write("    ... %d more\n" % (len(frontier) - 10))

    out.write("\n  elapsed %.2fs\n" % elapsed)
    out.write("  STATIC_ANALYSIS_VERIFIED only: no code was executed.\n")

    if args.json:
        json_path = Path(args.json)
        if json_path.parent and str(json_path.parent) not in ("", "."):
            json_path.parent.mkdir(parents=True, exist_ok=True)
        payload = json.dumps(summary, indent=2, sort_keys=True) + "\n"
        json_path.write_text(payload, encoding="utf-8")
        sys.stderr.write("cfg_census: wrote %s\n" % json_path)

    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except BrokenPipeError:
        os._exit(0)
    except KeyboardInterrupt:
        die("interrupted", 130)
