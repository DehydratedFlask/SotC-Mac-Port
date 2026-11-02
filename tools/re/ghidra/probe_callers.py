# Report every caller of the given RVAs (call/jump references), with the containing function.
#
#   local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
#       --project-dir local/ghidra/persistent-20261020 --project-name SotC \
#       --script tools/re/ghidra/probe_callers.py --out /dev/null 0x89d3b0
#
# Read-only. Prints CALLER lines and a CALLERCOUNT per target.

MAX_CALLERS = 400


def _hex(addr):
    return "0x%x" % addr.getOffset()


def main():
    program = currentProgram
    if program is None:
        print("CALLERS error=no-current-program")
        return
    fm = program.getFunctionManager()
    listing = program.getListing()
    base = program.getImageBase()
    refman = program.getReferenceManager()

    for text in (getScriptArgs() or []):
        text = text.strip()
        if not text:
            continue
        try:
            va = int(text, 16) if text.lower().startswith("0x") else int(text)
        except ValueError:
            print("CALLERS ignored=%s" % text)
            continue
        target = base.add(va)
        count = 0
        for ref in refman.getReferencesTo(target):
            from_addr = ref.getFromAddress()
            containing = fm.getFunctionContaining(from_addr)
            insn = listing.getInstructionAt(from_addr)
            count += 1
            if count > MAX_CALLERS:
                break
            print("CALLER target=0x%x from=%s reftype=%s entry=%s fn=%s insn=%s"
                  % (va, _hex(from_addr), ref.getReferenceType(),
                     _hex(containing.getEntryPoint()) if containing else "none",
                     containing.getName() if containing else "none",
                     insn.toString() if insn else "(no instruction)"))
        print("CALLERCOUNT target=0x%x %d" % (va, count))


main()
