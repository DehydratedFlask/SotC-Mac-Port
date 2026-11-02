# Disassemble an address range on demand (in-memory only) and list what it produced.
#
#   local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
#       --project-dir local/ghidra/persistent-20261020 --project-name SotC \
#       --script tools/re/ghidra/probe_disasm_range.py --out /dev/null 0x1aa351 0x1aa42f
#
# Why: `getReferencesTo` and `getFunctionContaining` answer questions about Ghidra's CURRENT
# analysis, and auto-analysis can leave ranges undecoded (E1065). A missing call edge is then a
# coverage hole, not absent code. This script makes the hole visible: it disassembles the range in
# memory, prints the instructions, and reports which function now contains them.
#
# Nothing is saved: run_dump.py closes the project without committing, so the persistent project
# keeps its original analysis.
#
# Read-only with respect to the project; the only mutation is to the in-memory program copy.

MAX_INSN = 400


def _hex(addr):
    return "0x%x" % addr.getOffset()


def main():
    program = currentProgram
    if program is None:
        print("DISASM error=no-current-program")
        return
    args = [a for a in (getScriptArgs() or []) if a.strip().lower().startswith("0x")]
    if len(args) < 2:
        print("DISASM error=need-lo-hi")
        return
    lo = int(args[0], 16)
    hi = int(args[1], 16)
    base = program.getImageBase()
    fm = program.getFunctionManager()
    listing = program.getListing()

    before = 0
    addr = base.add(lo)
    while addr.getOffset() < hi:
        if listing.getInstructionAt(addr) is not None:
            before += 1
            addr = addr.add(listing.getInstructionAt(addr).getLength())
        else:
            addr = addr.add(1)
    print("DISASM range=0x%x..0x%x instructions_before=%d" % (lo, hi, before))

    print("DISASM disassemble=%s" % disassemble(base.add(lo)))

    count = 0
    addr = base.add(lo)
    while addr.getOffset() < hi and count < MAX_INSN:
        insn = listing.getInstructionAt(addr)
        if insn is None:
            if not disassemble(addr):
                print("DISASM undecodable_at=0x%x" % addr.getOffset())
                addr = addr.add(1)
                continue
            insn = listing.getInstructionAt(addr)
            if insn is None:
                print("DISASM hole_at=0x%x" % addr.getOffset())
                addr = addr.add(1)
                continue
        containing = fm.getFunctionContaining(addr)
        print("INSN 0x%x %-40s entry=%s fn=%s"
              % (addr.getOffset(), insn.toString(),
                 _hex(containing.getEntryPoint()) if containing else "none",
                 containing.getName() if containing else "none"))
        count += 1
        addr = insn.getAddress().add(insn.getLength())
    print("DISASM instructions_emitted=%d" % count)


main()
