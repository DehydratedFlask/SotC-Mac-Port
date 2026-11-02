# Report which function *contains* an address, as opposed to which function *starts* there.
#
# Runs as a GhidraScript (PyGhidra) through tools/re/ghidra/run_dump.py or analyzeHeadless:
#
#   local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
#       --project-dir local/ghidra/persistent-20261020 --project-name SotC \
#       --script tools/re/ghidra/probe_containing.py --out /dev/null 0xa4bddb
#
# Why this exists: `FunctionManager.getFunctionAt` (used by probe_project.py) answers "is this a
# function ENTRY", so an address in the middle of a function reports `present=no`. That reads like
# "not code", which is wrong. This script separates the two questions and also prints the body
# ranges, so a hole in a function body is visible instead of being mistaken for a separate function.
#
# Read-only: it never disassembles for saving, never decompiles and never writes the program.

MAX_RANGES = 32


def _hex(addr):
    return "0x%x" % addr.getOffset()


def main():
    program = currentProgram
    if program is None:
        print("CONTAIN error=no-current-program")
        return

    fm = program.getFunctionManager()
    listing = program.getListing()
    base = program.getImageBase()

    print("CONTAIN image_base=%s" % _hex(base))
    print("CONTAIN function_count=%d" % fm.getFunctionCount())

    for text in (getScriptArgs() or []):
        text = text.strip()
        if not text:
            continue
        try:
            rva = int(text, 16) if text.lower().startswith("0x") else int(text)
        except ValueError:
            print("CONTAIN ignored=%s" % text)
            continue
        addr = base.add(rva)

        entry = fm.getFunctionAt(addr)
        containing = fm.getFunctionContaining(addr)
        instruction = listing.getInstructionAt(addr)

        print("CONTAIN 0x%x is_entry=%s" % (rva, "yes" if entry is not None else "no"))
        print("CONTAIN 0x%x is_instruction=%s" % (rva, "yes" if instruction is not None else "no"))
        if instruction is not None:
            print("CONTAIN 0x%x instruction=%s" % (rva, instruction.toString()))
            print("CONTAIN 0x%x instruction_length=%d" % (rva, instruction.getLength()))

        if containing is None:
            print("CONTAIN 0x%x containing_any=none" % rva)
            continue
        body = containing.getBody()
        print("CONTAIN 0x%x containing_entry=%s" % (rva, _hex(containing.getEntryPoint())))
        print("CONTAIN 0x%x containing_name=%s" % (rva, containing.getName()))
        print("CONTAIN 0x%x containing_size=%d" % (rva, body.getNumAddresses()))
        print("CONTAIN 0x%x containing_blocks=%d" % (rva, body.getNumAddressRanges()))
        print("CONTAIN 0x%x in_body=%s" % (rva, "yes" if body.contains(addr) else "no"))
        if body.getNumAddressRanges() <= MAX_RANGES:
            for index, rng in enumerate(body.getAddressRanges()):
                print("CONTAIN 0x%x range_%d=%s..%s"
                      % (rva, index, _hex(rng.getMinAddress()), _hex(rng.getMaxAddress())))


main()
