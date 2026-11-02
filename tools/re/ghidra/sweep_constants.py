# Exhaustive scalar-operand sweep of the whole loaded image, plus a compare-immediate census.
#
#   local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
#       --project-dir local/ghidra/persistent-20261020 --project-name SotC \
#       --script tools/re/ghidra/sweep_constants.py --out local/validation/csdr-loader/sweep.txt
#
# Why: the earlier negative result used `objdump` text patterns (`cmpb $0x8c`, `cmpl $0x8c0a`, ...).
# That only sees immediates that objdump chose to print in one form, only over bytes it
# disassembled, and never sees a compare whose immediate arrives through a load or an indexed
# table. This script asks Ghidra for every instruction it has, collects every scalar operand,
# and matches a superset of the constants the prefix could appear as.
#
# Read-only: no disassembly for saving, no decompilation, no program write.

MAX_HITS_PER_PREDICATE = 60

# Every constant the compressed prefix (0x8c 0x0a / 0x8c 0x06 / 0xcc 0x0a / 0xcc 0x06) could be
# compared as, in any of the encodings a compiler could plausibly use.
PREDICATES = {
    # single byte
    "0x8c": 0x8C, "0xcc": 0xCC, "0x8C": 0x8C,
    # big-endian 16-bit pair, i.e. the literal appears in source as 0x8C0A
    "0x8c0a": 0x8C0A, "0x8c06": 0x8C06, "0xcc0a": 0xCC0A, "0xcc06": 0xCC06,
    # little-endian 16-bit load of the same two bytes
    "0x0a8c": 0x0A8C, "0x068c": 0x068C, "0x0acc": 0x0ACC, "0x06cc": 0x06CC,
    # 32-bit constants with the pair in either half
    "0x8c0a0000": 0x8C0A0000, "0x8c060000": 0x8C060000,
    "0xcc0a0000": 0xCC0A0000, "0xcc060000": 0xCC060000,
    "0x00008c0a": 0x8C0A, "0x00008c06": 0x8C06,
    # the raw container magic candidates around the .csdr resource type
    "RDSC": 0x52445343, "rdsc": 0x72647363,
    "CSDR": 0x43534452, "csdr": 0x63736472,
    "ORB": 0x42524F, "ORBS": 0x5342524F, "OrbS": 0x5362724F, "OrbShdr32": 0x5362724F,
}
COUNT_ONLY = {"0x100": 0x100, "0x14": 0x14, "0x16": 0x16}


def _hex(addr):
    return "0x%x" % addr.getOffset()


def _scalars(insn):
    """Every scalar operand of an instruction, from both the operand and the object views."""
    out = []
    for i in range(insn.getNumOperands()):
        try:
            scalar = insn.getScalar(i)
        except Exception:
            scalar = None
        if scalar is not None:
            out.append(int(scalar.getUnsignedValue()) & 0xFFFFFFFFFFFFFFFF)
        try:
            objects = insn.getOpObjects(i)
        except Exception:
            objects = []
        for obj in objects:
            if hasattr(obj, "getUnsignedValue"):
                try:
                    out.append(int(obj.getUnsignedValue()) & 0xFFFFFFFFFFFFFFFF)
                except Exception:
                    pass
    return out


def main():
    program = currentProgram
    if program is None:
        print("SWEEP error=no-current-program")
        return

    fm = program.getFunctionManager()
    listing = program.getListing()
    memory = program.getMemory()

    blocks = []
    for block in memory.getBlocks():
        blocks.append("%s init=%s exec=%s %s..%s" % (
            block.getName(), block.isInitialized(), block.isExecute(),
            _hex(block.getStart()), _hex(block.getEnd())))
    print("SWEEP image_base=%s" % _hex(program.getImageBase()))
    print("SWEEP memory_blocks=%d" % len(blocks))
    for line in blocks:
        print("BLOCK " + line)
    print("SWEEP functions=%d" % fm.getFunctionCount())

    total = 0
    in_function = 0
    not_in_function = 0
    byte_compares = {}          # every CMP/JMP-family immediate in 0x80..0xFF, histogram
    hits = {name: [] for name in PREDICATES}
    count_only = {name: 0 for name in COUNT_ONLY}

    insn_iter = listing.getInstructions(True)
    for insn in insn_iter:
        total += 1
        addr = insn.getAddress()
        containing = fm.getFunctionContaining(addr)
        if containing is None:
            not_in_function += 1
        else:
            in_function += 1
        values = _scalars(insn)
        if not values:
            continue
        for name, want in PREDICATES.items():
            if want in values:
                bucket = hits[name]
                if len(bucket) < MAX_HITS_PER_PREDICATE:
                    if containing is None:
                        bucket.append((_hex(addr), "none", "none", insn.toString()))
                    else:
                        bucket.append((_hex(addr), _hex(containing.getEntryPoint()),
                                       containing.getName(), insn.toString()))
        for name, want in COUNT_ONLY.items():
            if want in values:
                count_only[name] += 1
        mnemonic = insn.getMnemonicString().upper()
        if mnemonic in ("CMP", "CMPXCHG"):
            for v in values:
                if 0x80 <= v <= 0xFF:
                    key = "0x%02x" % v
                    byte_compares[key] = byte_compares.get(key, 0) + 1

    print("SWEEP instructions_total=%d in_function=%d not_in_function=%d"
          % (total, in_function, not_in_function))

    for name in sorted(PREDICATES):
        bucket = hits[name]
        print("HITCOUNT %s %d" % (name, len(bucket)))
        for addr, entry, func, text in bucket:
            print("HIT %s addr=%s entry=%s fn=%s insn=%s" % (name, addr, entry, func, text))
    for name in sorted(COUNT_ONLY):
        print("COUNTONLY %s %d" % (name, count_only[name]))

    print("BYTECMP distinct=%d" % len(byte_compares))
    for key in sorted(byte_compares):
        print("BYTECMP %s %d" % (key, byte_compares[key]))


main()
