# Bounded call-graph walk (callees and/or callers) with a region filter.
#
#   local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
#       --project-dir local/ghidra/persistent-20261020 --project-name SotC \
#       --script tools/re/ghidra/probe_callgraph.py --out /dev/null \
#       --dir=callees --depth=4 --seed=0xb0cc50 --seed=0xb0d3e0 --watch=0x890000-0x8b0000
#
# Answers "is library X reachable from loader Y" without decompiling every node.
# Read-only.

MAX_NODES = 60000


def _hex(addr):
    return "0x%x" % addr.getOffset()


def _parse(args):
    opts = {"dir": "callees", "depth": "4", "seed": [], "watch": []}
    for text in args:
        if text.startswith("dir="):
            opts["dir"] = text.split("=", 1)[1]
        elif text.startswith("depth="):
            opts["depth"] = text.split("=", 1)[1]
        elif text.startswith("seed="):
            opts["seed"].append(text.split("=", 1)[1])
        elif text.startswith("watch="):
            opts["watch"].append(text.split("=", 1)[1])
    return opts


def _in_watch(offset, ranges):
    for lo, hi in ranges:
        if lo <= offset < hi:
            return True
    return False


def main():
    program = currentProgram
    if program is None:
        print("GRAPH error=no-current-program")
        return
    opts = _parse(getScriptArgs() or [])
    depth_limit = int(opts["depth"])
    watch = []
    for spec in opts["watch"]:
        lo, hi = spec.split("-")
        watch.append((int(lo, 16), int(hi, 16)))

    fm = program.getFunctionManager()
    listing = program.getListing()
    base = program.getImageBase()
    refman = program.getReferenceManager()

    def callees(func):
        out = set()
        body = func.getBody()
        it = body.getAddresses(True)
        while it.hasNext():
            addr = it.next()
            insn = listing.getInstructionAt(addr)
            if insn is None:
                continue
            if not insn.getMnemonicString().upper().startswith("CALL"):
                continue
            for i in range(insn.getNumOperands()):
                for ref in insn.getOperandReferences(i):
                    target = fm.getFunctionAt(ref.getToAddress())
                    if target is not None:
                        out.add(target.getEntryPoint().getOffset())
        return out

    def callers(func):
        out = set()
        for ref in refman.getReferencesTo(func.getEntryPoint()):
            containing = fm.getFunctionContaining(ref.getFromAddress())
            if containing is not None:
                out.add(containing.getEntryPoint().getOffset())
        return out

    step = callees if opts["dir"] == "callees" else callers
    seeds = [int(s, 16) for s in opts["seed"]]
    seen = set(seeds)
    frontier = list(seeds)
    print("GRAPH dir=%s depth=%d seeds=%d watch=%s"
          % (opts["dir"], depth_limit, len(seeds), opts["watch"]))
    for depth in range(1, depth_limit + 1):
        nxt = []
        for entry in frontier:
            func = fm.getFunctionAt(base.add(entry))
            if func is None:
                continue
            for target in sorted(step(func)):
                if target in seen:
                    continue
                seen.add(target)
                nxt.append(target)
                func2 = fm.getFunctionAt(base.add(target))
                size = func2.getBody().getNumAddresses() if func2 else -1
                flag = " WATCH" if _in_watch(target, watch) else ""
                print("DEPTH%d 0x%x size=%d from=0x%x%s"
                      % (depth, target, size, entry, flag))
                if len(seen) > MAX_NODES:
                    print("GRAPH truncated nodes=%d" % len(seen))
                    return
        frontier = nxt
        print("GRAPH depth=%d new=%d cumulative=%d" % (depth, len(nxt), len(seen)))


main()
