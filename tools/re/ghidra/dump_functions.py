# Dump Ghidra decompiler evidence for a list of guest RVAs.
#
# Runs as a Ghidra post-script under PyGhidra. Usage:
#
#   analyzeHeadless <proj_dir> <proj_name> -process -noanalysis -readOnly \
#       -scriptPath tools/re/ghidra \
#       -postScript dump_functions.py <output.txt> 0x2569d0 0x1bdf30 ...
#
# For every RVA it records the entry, body size, signature, callee list and the decompiled C.
# The output is a stable, line-oriented dossier so tools/lift/check_ghidra_pipeline.py can parse it
# without re-running Ghidra.
#
# This script only reads the program. It does not rename, markup, or save anything.
#
# Addresses passed in are RVAs relative to the program image base (for this ELF the image base is
# 0x0, so RVA == address; the script adds the base anyway so it stays correct if that changes).

import hashlib
import os
import time

DECOMPILE_TIMEOUT_SECONDS = 600


def _sha256_of(path):
    digest = hashlib.sha256()
    with open(path, "rb") as handle:
        for chunk in iter(lambda: handle.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _executable_sha256(program):
    path = str(program.getExecutablePath() or "")
    if "://" in path:
        path = path.split("://", 1)[1]
    if path and os.path.isfile(path):
        return _sha256_of(path)
    return "unknown"


def _open_decompiler(program):
    from ghidra.app.decompiler import DecompInterface, DecompileOptions

    interface = DecompInterface()
    interface.setOptions(DecompileOptions())
    interface.toggleCCode(True)
    interface.toggleSyntaxTree(True)
    try:
        started = interface.openProgram(program, monitor, DecompileOptions())
    except TypeError:
        started = interface.openProgram(program)
    if not started:
        raise RuntimeError("DecompInterface.openProgram failed: %s" % interface.getLastMessage())
    return interface


def _callees(function):
    """Called functions as (entry, name) pairs, sorted by address."""
    rows = {}
    try:
        called = function.getCalledFunctions(monitor)
    except Exception:
        called = []
    for callee in called:
        rows[callee.getEntryPoint().getOffset()] = callee.getName()
    return sorted(rows.items())


def _call_sites(function):
    """Explicit CALL/JUMP targets seen in the body, including indirect ones Ghidra could not name."""
    sites = []
    body = function.getBody()
    listing = currentProgram.getListing()
    functions = currentProgram.getFunctionManager()
    # Iterate the body's addresses directly: Instruction.getNext() returns an Instruction, not an
    # Address, and mixing the two is what JPype rejects ("no matching overloads for contains").
    for address in body.getAddresses(True):
        instruction = listing.getInstructionAt(address)
        if instruction is None:
            continue
        flow_type = instruction.getFlowType()
        if not (flow_type.isCall() or flow_type.isJump()):
            continue
        targets = instruction.getFlows()
        if len(targets) == 0:
            sites.append((address.getOffset(), "indirect", instruction.toString()))
        for destination in targets:
            resolved = functions.getFunctionAt(destination)
            name = resolved.getName() if resolved is not None else ""
            sites.append((address.getOffset(), "0x%x" % destination.getOffset(), name))
    return sites


def main():
    args = list(getScriptArgs() or [])
    if len(args) < 2:
        printerr("usage: dump_functions.py <output.txt> <rva> [<rva> ...]")
        raise SystemExit(2)

    output_path = args[0]
    requested = []
    for text in args[1:]:
        text = text.strip()
        if not text:
            continue
        requested.append(int(text, 16) if text.lower().startswith("0x") else int(text, 10))

    program = currentProgram
    image_base = program.getImageBase().getOffset()
    decompiler = _open_decompiler(program)

    lines = []
    lines.append("# Ghidra decompiler dossier")
    lines.append("# program: %s" % program.getName())
    lines.append("# language: %s" % program.getLanguageID().getIdAsString())
    lines.append("# compiler: %s" % program.getCompilerSpec().getCompilerSpecID().getIdAsString())
    lines.append("# image_base: 0x%x" % image_base)
    lines.append("# executable_path: %s" % program.getExecutablePath())
    lines.append("# executable_md5: %s" % program.getExecutableMD5())
    lines.append("# executable_sha256: %s" % _executable_sha256(program))
    lines.append("# function_count: %d" % program.getFunctionManager().getFunctionCount())
    lines.append("# requested_rvas: %s" % " ".join("0x%x" % rva for rva in requested))
    lines.append("# decompile_timeout_seconds: %d" % DECOMPILE_TIMEOUT_SECONDS)

    for rva in requested:
        address = program.getImageBase().add(rva)
        function = program.getFunctionManager().getFunctionAt(address)
        lines.append("=== 0x%x ===" % rva)
        lines.append("ENTRY=0x%x" % rva)
        lines.append("ADDRESS=0x%x" % address.getOffset())
        if function is None:
            # Record the miss explicitly instead of emitting an empty section.
            lines.append("PRESENT=no")
            lines.append("SIZE=0")
            lines.append("DECOMPILE_STATUS=no-function-at-address")
            continue

        body = function.getBody()
        lines.append("PRESENT=yes")
        lines.append("NAME=%s" % function.getName())
        lines.append("SIZE=%d" % body.getNumAddresses())
        lines.append("BLOCKS=%d" % body.getNumAddressRanges())
        lines.append("INSTRUCTIONS=%d" % function.getBody().getNumAddresses())
        lines.append("SIGNATURE=%s" % function.getPrototypeString(False, False))

        callees = _callees(function)
        lines.append("CALLEE_COUNT=%d" % len(callees))
        for entry, name in callees:
            lines.append("CALLEE=0x%x %s" % (entry, name))

        sites = _call_sites(function)
        lines.append("CALL_SITE_COUNT=%d" % len(sites))
        for site, target, text in sites:
            lines.append("CALL_SITE=0x%x -> %s %s" % (site, target, text))

        started = time.time()
        result = decompiler.decompileFunction(function, DECOMPILE_TIMEOUT_SECONDS, monitor)
        elapsed = time.time() - started
        lines.append("DECOMPILE_SECONDS=%.2f" % elapsed)
        if result is None or not result.decompileCompleted():
            message = "unknown"
            if result is not None:
                message = str(result.getErrorMessage())
            lines.append("DECOMPILE_STATUS=failed")
            lines.append("DECOMPILE_ERROR=%s" % message.replace("\n", " "))
            continue

        decompiled = result.getDecompiledFunction()
        text = decompiled.getC() if decompiled is not None else ""
        lines.append("DECOMPILE_STATUS=ok")
        lines.append("C_LINES=%d" % len(text.splitlines()))
        lines.append("--- decompiled C ---")
        lines.extend(text.rstrip("\n").split("\n"))
        lines.append("--- end decompiled C ---")

    decompiler.dispose()

    directory = os.path.dirname(os.path.abspath(output_path))
    if directory and not os.path.isdir(directory):
        os.makedirs(directory, exist_ok=True)
    with open(output_path, "w", encoding="utf-8") as handle:
        handle.write("\n".join(lines) + "\n")
    print("WROTE %s (%d functions)" % (output_path, len(requested)))


main()
