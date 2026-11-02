# Byte-level `call`/`jmp rel32` census into an address window, independent of Ghidra's references.
#
#   local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
#       --project-dir local/ghidra/persistent-20261020 --project-name SotC \
#       --script tools/re/ghidra/sweep_rel32_calls.py --out /dev/null 0x890000 0x8c0000
#
# Why this exists: a Ghidra reference query answers "did the disassembler record a reference",
# not "does a call exist". When auto-analysis leaves a hole, `getReferencesTo` returns nothing
# and `getFunctionContaining` can still report the address as inside a function body (E1065).
# This script decodes the rel32 itself and then classifies each site against Ghidra's coverage:
#
#   CLEAN        the source is a disassembled instruction and Ghidra recorded the reference
#   NO_REF       the source is a disassembled instruction but no reference was recorded
#   BODY_HOLE    the source is inside a function body but is not a disassembled instruction
#   OUTSIDE      the source is not inside any function body (data, or code Ghidra never claimed)
#
# Read-only.

MAX_ROWS = 500
MAX_DISTINCT_PRINT = 200


def _hex(value):
    return "0x%x" % value


def main():
    program = currentProgram
    if program is None:
        print("REL32 error=no-current-program")
        return
    args = [a for a in (getScriptArgs() or []) if a.strip().lower().startswith("0x")]
    if len(args) < 2:
        print("REL32 error=need-lo-hi")
        return
    lo = int(args[0], 16)
    hi = int(args[1], 16)

    fm = program.getFunctionManager()
    listing = program.getListing()
    memory = program.getMemory()
    refman = program.getReferenceManager()

    total_bytes = 0
    sites = 0
    kept = 0
    classes = {"CLEAN": 0, "NO_REF": 0, "BODY_HOLE": 0, "OUTSIDE": 0}
    per_target = {}
    rows = []
    import jpype
    for block in memory.getBlocks():
        if not block.isInitialized():
            continue
        start = block.getStart().getOffset()
        size = int(block.getSize())
        total_bytes += size
        raw = jpype.JArray(jpype.JByte)(size)
        memory.getBytes(block.getStart(), raw)
        buf = bytes(b & 0xFF for b in raw)
        for i in range(size - 5):
            if buf[i] not in (0xE8, 0xE9):
                continue
            sites += 1
            rel = int.from_bytes(buf[i + 1:i + 5], "little", signed=True)
            src = start + i
            target = src + 5 + rel
            if not (lo <= target < hi):
                continue
            kept += 1
            insn = listing.getInstructionAt(program.getImageBase().add(src))
            containing = fm.getFunctionContaining(program.getImageBase().add(src))
            has_ref = False
            if insn is not None:
                for operand in range(insn.getNumOperands()):
                    for ref in insn.getOperandReferences(operand):
                        if ref.getToAddress().getOffset() == target:
                            has_ref = True
            if insn is not None and has_ref:
                klass = "CLEAN"
            elif insn is not None:
                klass = "NO_REF"
            elif containing is not None:
                klass = "BODY_HOLE"
            else:
                klass = "OUTSIDE"
            classes[klass] += 1
            per_target.setdefault(target, []).append((src, klass,
                                                      buf[i] == 0xE8))
            if len(rows) < MAX_ROWS:
                rows.append((target, src, klass, buf[i] == 0xE8))

    print("REL32 exec_bytes=%d candidate_rel32_sites=%d in_window=%d" %
          (total_bytes, sites, kept))
    print("REL32 classes " + " ".join("%s=%d" % (k, v) for k, v in sorted(classes.items())))
    print("REL32 distinct_targets=%d" % len(per_target))
    for target in sorted(per_target):
        entries = per_target[target]
        kinds = {}
        for _src, klass, is_call in entries:
            kinds[klass] = kinds.get(klass, 0) + 1
        print("TARGET 0x%x sites=%d %s" %
              (target, len(entries), " ".join("%s=%d" % kv for kv in sorted(kinds.items()))))
    for target, src, klass, is_call in rows:
        print("SITE target=0x%x from=0x%x %s %s"
              % (target, src, "call" if is_call else "jmp", klass))


main()
