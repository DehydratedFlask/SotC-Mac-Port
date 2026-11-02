# Call-graph census over an address window: which functions *outside* the window call *into* it.
#
#   local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
#       --project-dir local/ghidra/persistent-20261020 --project-name SotC \
#       --script tools/re/ghidra/sweep_region_calls.py --out /dev/null 0x890000 0x8c0000
#
# Used to find the statically linked Oodle block's public entry points: the decompressor has no
# import and no ordinary caller if the game only reaches it through a function pointer, so the
# inbound-call census has to be read together with the pointer-table census.
#
# Read-only.

MAX_ROWS = 400


def _hex(addr):
    return "0x%x" % addr.getOffset()


def main():
    program = currentProgram
    if program is None:
        print("REGION error=no-current-program")
        return
    args = [a for a in (getScriptArgs() or []) if a.strip().lower().startswith("0x")]
    if len(args) < 2:
        print("REGION error=need-lo-hi")
        return
    lo = int(args[0], 16)
    hi = int(args[1], 16)

    fm = program.getFunctionManager()
    listing = program.getListing()
    base = program.getImageBase()

    inbound = {}
    total_calls = 0
    for insn in listing.getInstructions(True):
        mnem = insn.getMnemonicString().upper()
        if not mnem.startswith("CALL"):
            continue
        total_calls += 1
        for i in range(insn.getNumOperands()):
            try:
                refs = insn.getOperandReferences(i)
            except Exception:
                refs = []
            for ref in refs:
                target = ref.getToAddress().getOffset()
                if not (lo <= target < hi):
                    continue
                from_addr = insn.getAddress()
                caller = fm.getFunctionContaining(from_addr)
                caller_entry = caller.getEntryPoint().getOffset() if caller else None
                entry = inbound.setdefault(target, {})
                key = caller_entry if caller_entry is not None else -1
                entry[key] = entry.get(key, 0) + 1

    print("REGION lo=0x%x hi=0x%x calls_total=%d distinct_targets=%d"
          % (lo, hi, total_calls, len(inbound)))
    rows = 0
    for target in sorted(inbound):
        callers = inbound[target]
        outside = {k: v for k, v in callers.items() if k != -1 and not (lo <= k < hi)}
        orphan = callers.get(-1, 0)
        func = fm.getFunctionAt(base.add(target))
        print("INBOUND 0x%x size=%s outside_callers=%d inside_callers=%d orphan_sites=%d sites=%d"
              % (target, func.getBody().getNumAddresses() if func else "none",
                 len(outside), len(callers) - len(outside) - (1 if orphan else 0), orphan,
                 sum(callers.values())))
        for k in sorted(outside)[:8]:
            print("   from 0x%x x%d" % (k, outside[k]))
        rows += 1
        if rows >= MAX_ROWS:
            break


main()
