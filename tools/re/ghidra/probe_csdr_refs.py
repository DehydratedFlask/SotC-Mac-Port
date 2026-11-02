# Symbol + reference probe for the .csdr loader hunt.
#
#   local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
#       --project-dir local/ghidra/persistent-20261020 --project-name SotC \
#       --script tools/re/ghidra/probe_csdr_refs.py --out local/validation/csdr-loader/probe.txt \
#       0x12cfc1c 0x12cfbe0
#
# Read-only: never disassembles for saving, never decompiles, never writes the program.
# Prints:
#   SYM <pattern> <count>            number of symbols matching a case-insensitive substring
#   SYMHIT <name> <addr> <kind>      for the first N matches
#   REF <target_va> <from_va> <type> <inside_entry> <inside_name> <instruction text>
#   FUNC <entry> size= blocks= name= for every containing function of a reference

import re

MAX_PER_PATTERN = 40
PATTERNS = ["oodle", "OodleLZ", "csdr", "shaderTypeToLoad", "ShaderList",
            "OodleLZ_Decompress", "rrlzh", "huffman"]


def _hex(addr):
    return "0x%x" % addr.getOffset()


def main():
    program = currentProgram
    if program is None:
        print("PROBE error=no-current-program")
        return

    fm = program.getFunctionManager()
    listing = program.getListing()
    st = program.getSymbolTable()
    base = program.getImageBase()
    print("PROBE image_base=%s functions=%d symbols=%d"
          % (_hex(base), fm.getFunctionCount(), st.getNumSymbols()))

    for pattern in PATTERNS:
        needle = pattern.lower()
        hits = []
        it = st.getAllSymbols(True)
        while it.hasNext():
            sym = it.next()
            if needle in sym.getName().lower():
                hits.append(sym)
                if len(hits) > 4000:
                    break
        print("SYM %s %d" % (pattern, len(hits)))
        for sym in hits[:MAX_PER_PATTERN]:
            print("SYMHIT %s %s kind=%s" % (sym.getName(), _hex(sym.getAddress()),
                                            sym.getSymbolType()))

    seen_funcs = {}
    for text in (getScriptArgs() or []):
        text = text.strip()
        if not text:
            continue
        try:
            va = int(text, 16) if text.lower().startswith("0x") else int(text)
        except ValueError:
            print("REF ignored=%s" % text)
            continue
        target = base.add(va)
        refs = program.getReferenceManager().getReferencesTo(target)
        n = 0
        for ref in refs:
            n += 1
            frm = ref.getFromAddress()
            insn = listing.getInstructionAt(frm)
            containing = fm.getFunctionContaining(frm)
            cname = containing.getName() if containing is not None else "none"
            centry = _hex(containing.getEntryPoint()) if containing is not None else "none"
            print("REF 0x%x from=%s type=%s fn=%s entry=%s insn=%s"
                  % (va, _hex(frm), ref.getReferenceType(), cname, centry,
                     insn.toString() if insn is not None else "(no instruction)"))
            if containing is not None:
                seen_funcs[containing.getEntryPoint().getOffset()] = containing
        print("REFS 0x%x count=%d" % (va, n))

    for entry, func in sorted(seen_funcs.items()):
        print("FUNC 0x%x size=%d blocks=%d name=%s"
              % (entry, func.getBody().getNumAddresses(),
                 func.getBody().getNumAddressRanges(), func.getName()))


main()
