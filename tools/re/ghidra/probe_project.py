# Probe a Ghidra project program and print the facts the pipeline depends on.
#
# Runs as a Ghidra post-script (PyGhidra). Usage:
#   analyzeHeadless <proj_dir> <proj_name> -process <file> -noanalysis -readOnly \
#       -scriptPath tools/re/ghidra -postScript probe_project.py 0x2569d0 0x1bdf30
#
# Prints one "KEY=VALUE" line per fact so a checker can parse it without guessing.
# Extra RVAs on the command line are reported as "PROBE 0x<rva> present=<bool> kind=<...>".
#
# This script only reads the program. It never decompiles, disassembles for saving, or writes.

import hashlib
import os
import struct


def _emit(key, value):
    print("PROBE %s=%s" % (key, value))


def _executable_sha256(program):
    """SHA-256 of the original imported file, when Ghidra kept the source path."""
    try:
        fsrl = program.getExecutablePath()
    except Exception:
        return "unknown"
    if not fsrl:
        return "unknown"
    path = str(fsrl)
    # Ghidra may prefix the path with a scheme such as "file:/"; keep the tail.
    if "://" in path:
        path = path.split("://", 1)[1]
    if not os.path.isfile(path):
        return "unreadable:" + path
    digest = hashlib.sha256()
    with open(path, "rb") as handle:
        for chunk in iter(lambda: handle.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def main():
    program = currentProgram
    if program is None:
        print("PROBE error=no-current-program")
        return

    _emit("name", program.getName())
    _emit("language", program.getLanguageID().getIdAsString())
    _emit("compiler", program.getCompilerSpec().getCompilerSpecID().getIdAsString())
    _emit("image_base", "0x%x" % program.getImageBase().getOffset())
    _emit("executable_path", program.getExecutablePath())
    _emit("executable_md5", program.getExecutableMD5())
    _emit("executable_sha256", _executable_sha256(program))
    functions = program.getFunctionManager().getFunctionCount()
    _emit("function_count", functions)
    _emit("symbol_count", program.getSymbolTable().getNumSymbols())

    memory = program.getMemory()
    _emit("memory_blocks", memory.getNumAddressRanges())

    args = list(getScriptArgs() or [])
    for text in args:
        text = text.strip()
        if not text:
            continue
        try:
            rva = int(text, 16) if text.lower().startswith("0x") else int(text)
        except ValueError:
            # The shared driver prepends its output path; anything non-numeric is not an RVA.
            print("PROBE ignored=%s" % text)
            continue
        address = program.getImageBase().add(rva)
        function = program.getFunctionManager().getFunctionAt(address)
        listing = program.getListing()
        instruction = listing.getInstructionAt(address)
        kind = "none"
        if function is not None:
            kind = "function"
        elif instruction is not None:
            kind = "instruction"
        elif listing.getDataAt(address) is not None:
            kind = "data"
        size = -1
        body_blocks = 0
        if function is not None:
            size = function.getBody().getNumAddresses()
            body_blocks = function.getBody().getNumAddressRanges()
        print("PROBE 0x%x present=%s kind=%s size=%d blocks=%d"
              % (rva, "yes" if function is not None else "no", kind, size, body_blocks))


main()
