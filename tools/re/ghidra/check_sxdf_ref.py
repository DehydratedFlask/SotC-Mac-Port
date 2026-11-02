# Read the two addresses that bound the SXDF code-to-data reference, and nothing else.
#
# Runs as a Ghidra post-script under PyGhidra (see tools/re/ghidra/run_dump.py; analyzeHeadless
# cannot start a `.py` script in this install). Usage:
#
#   local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
#       --project-dir local/ghidra/persistent-20261020 --project-name SotC \
#       --script tools/re/ghidra/check_sxdf_ref.py \
#       --out local/validation/aud02-sxdf-data-reference-ghidra-20261101/sxdf-ref.txt
#
# Scope is deliberately two addresses:
#   0x9b5b10  the LEA that E1007 read as reaching the SXDF data base
#   0x12ba74a the data base it reaches
# There are no xrefs, no decompilation, no function bodies, no memory sweeps, and no writes. Every
# line is `SXDF key=value` so tools/lift/check_sxdf_data_reference.py can parse the result without
# re-running Ghidra.
#
# A rip-relative operand is reported twice: once as Ghidra models it (operand representation,
# operand objects, references from the instruction) and once as the raw disp32 in the instruction
# bytes computes it. If those disagree the output shows both; nothing is reconciled here.

import hashlib
import os

INSTRUCTION_RVA = 0x9B5B10
DATA_RVA = 0x12BA74A
DATA_WINDOW = 48

OUTPUT = []


def emit(key, value):
    line = "SXDF %s=%s" % (key, value)
    OUTPUT.append(line)
    print(line)


def hexof(data):
    return "".join("%02x" % (b & 0xFF) for b in data)


def asciiof(data):
    parts = []
    for b in data:
        value = b & 0xFF
        if value == 0:
            parts.append("\\0")
        elif 0x20 <= value <= 0x7E:
            parts.append(chr(value))
        else:
            parts.append("\\x%02x" % value)
    return "".join(parts)


def executable_sha256(program):
    """SHA-256 of the file Ghidra recorded as the program's source, when it is still readable."""
    try:
        path = str(program.getExecutablePath() or "")
    except Exception:
        return "unknown"
    if "://" in path:
        path = path.split("://", 1)[1]
    if not path or not os.path.isfile(path):
        return "unreadable:" + path
    digest = hashlib.sha256()
    with open(path, "rb") as handle:
        for chunk in iter(lambda: handle.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def read_bytes(memory, address, size):
    """Byte-at-a-time: this install's MemoryMapDB has no getBytes(Address, int) overload."""
    data = []
    for index in range(size):
        try:
            data.append(memory.getByte(address.add(index)) & 0xFF)
        except Exception as exc:
            raise RuntimeError("read failed at 0x%x: %r" % (address.add(index).getOffset(), exc))
    return data


def block_of(memory, address):
    try:
        block = memory.getBlock(address)
    except Exception:
        return None
    return block


def kind_at(listing, address):
    if listing.getInstructionAt(address) is not None:
        return "instruction"
    if listing.getDefinedDataAt(address) is not None:
        return "data"
    if listing.getDataContaining(address) is not None:
        return "inside-data"
    return "undefined"


def describe_data(listing, address, key):
    exact = listing.getDefinedDataAt(address)
    containing = listing.getDataContaining(address)
    emit(key + "_kind", kind_at(listing, address))
    emit(key + "_exact", "none" if exact is None else str(exact))
    if exact is not None:
        emit(key + "_exact_start", "0x%x" % exact.getAddress().getOffset())
        emit(key + "_exact_length", exact.getLength())
        emit(key + "_exact_type", exact.getDataType().getName())
    emit(key + "_containing", "none" if containing is None else str(containing))
    if containing is not None:
        emit(key + "_containing_start", "0x%x" % containing.getAddress().getOffset())
        emit(key + "_containing_length", containing.getLength())
        emit(key + "_containing_type", containing.getDataType().getName())


def describe_reference_from(instruction):
    refs = list(instruction.getReferencesFrom())
    emit("instruction_reference_count", len(refs))
    target = None
    for index, ref in enumerate(refs):
        to = ref.getToAddress()
        emit("instruction_reference_%d" % index,
             "0x%x type=%s" % (to.getOffset(), ref.getReferenceType()))
        if target is None:
            target = to.getOffset()
    return target


def describe_operands(instruction):
    """Operand text plus the objects Ghidra decomposed them into; scalar addresses are collected."""
    scalars = []
    count = instruction.getNumOperands()
    emit("operand_count", count)
    for index in range(count):
        emit("operand_%d" % index, instruction.getDefaultOperandRepresentation(index))
        try:
            objects = list(instruction.getOpObjects(index))
        except Exception as exc:
            emit("operand_%d_objects" % index, "error:" + repr(exc))
            continue
        for position, obj in enumerate(objects):
            class_name = obj.getClass().getSimpleName()
            text = str(obj)
            emit("operand_%d_object_%d" % (index, position), "%s:%s" % (class_name, text))
            if class_name == "Scalar":
                try:
                    value = obj.getUnsignedValue()
                except Exception:
                    continue
                # A scalar that lands inside the program is the resolved rip-relative target;
                # an immediate length such as 4 is not, so memory membership decides.
                if block_of(currentProgram.getMemory(), currentProgram.getAddressFactory()
                            .getDefaultAddressSpace().getAddress(value)) is not None:
                    scalars.append(value)
    return scalars


def main():
    program = currentProgram
    if program is None:
        emit("error", "no-current-program")
        return

    # --- identity: everything the task has to pin before reading an address -------------------
    emit("program_name", program.getName())
    emit("language", program.getLanguageID().getIdAsString())
    emit("compiler", program.getCompilerSpec().getCompilerSpecID().getIdAsString())
    emit("image_base", "0x%x" % program.getImageBase().getOffset())
    emit("executable_path", program.getExecutablePath())
    emit("executable_md5", program.getExecutableMD5())
    emit("executable_sha256", executable_sha256(program))
    emit("function_count", program.getFunctionManager().getFunctionCount())

    memory = program.getMemory()
    listing = program.getListing()
    base = program.getImageBase()
    address_space = program.getAddressFactory().getDefaultAddressSpace()

    # --- address 1: the instruction -----------------------------------------------------------
    insn_address = base.add(INSTRUCTION_RVA)
    instruction = listing.getInstructionAt(insn_address)
    emit("instruction_rva", "0x%x" % INSTRUCTION_RVA)
    emit("instruction_address", "0x%x" % insn_address.getOffset())
    if instruction is None:
        emit("instruction_present", "no")
        emit("instruction_kind", kind_at(listing, insn_address))
        describe_data(listing, insn_address, "instruction_data")
        return

    emit("instruction_present", "yes")
    emit("instruction_text", instruction.toString())
    emit("instruction_mnemonic", instruction.getMnemonicString())
    emit("instruction_length", instruction.getLength())
    instruction_bytes = read_bytes(memory, insn_address, instruction.getLength())
    emit("instruction_bytes", hexof(instruction_bytes))
    emit("instruction_fall_through", "0x%x" % instruction.getFallThrough().getOffset()
         if instruction.getFallThrough() is not None else "none")
    instruction_block = block_of(memory, insn_address)
    emit("instruction_block", "none" if instruction_block is None
         else "%s perms=%s%s%s" % (instruction_block.getName(),
                                   "R" if instruction_block.isRead() else "-",
                                   "W" if instruction_block.isWrite() else "-",
                                   "X" if instruction_block.isExecute() else "-"))
    function = program.getFunctionManager().getFunctionContaining(insn_address)
    emit("instruction_function", "none" if function is None
         else "%s@0x%x" % (function.getName(), function.getEntryPoint().getOffset()))

    scalars = describe_operands(instruction)
    reference_target = describe_reference_from(instruction)

    # Ghidra's answer: a reference it added, else a resolved operand scalar.
    ghidra_target = None
    ghidra_source = "none"
    if reference_target is not None:
        ghidra_target = reference_target
        ghidra_source = "reference"
    elif scalars:
        ghidra_target = scalars[0]
        ghidra_source = "operand-scalar"
    emit("rip_target", "0x%x" % ghidra_target if ghidra_target is not None else "none")
    emit("rip_target_source", ghidra_source)

    # The bytes' own answer: sign-extended disp32 at bytes[3:7] for `48 8d 3x disp32`.
    byte_target = None
    if instruction_bytes[:2] == [0x48, 0x8D] and (instruction_bytes[2] & 0xC7) == 0x05:
        disp = (instruction_bytes[3] & 0xFF) | ((instruction_bytes[4] & 0xFF) << 8) \
            | ((instruction_bytes[5] & 0xFF) << 16) | ((instruction_bytes[6] & 0xFF) << 24)
        if disp >= 0x80000000:
            disp -= 0x100000000
        byte_target = insn_address.getOffset() + instruction.getLength() + disp
        emit("byte_disp32", "0x%x" % (disp & 0xFFFFFFFF))
        emit("byte_target", "0x%x" % byte_target)
    else:
        emit("byte_disp32", "not-rip-relative-lea")

    # --- address 2: the data ------------------------------------------------------------------
    data_rva = DATA_RVA
    if byte_target is not None and ghidra_target is not None and byte_target != ghidra_target:
        emit("target_disagreement", "ghidra=0x%x bytes=0x%x" % (ghidra_target, byte_target))
    data_address = base.add(data_rva)
    emit("data_rva", "0x%x" % data_rva)
    emit("data_address", "0x%x" % data_address.getOffset())
    describe_data(listing, data_address, "data")

    data_block = block_of(memory, data_address)
    if data_block is None:
        emit("data_block", "none")
    else:
        emit("data_block", "%s perms=%s%s%s start=0x%x end=0x%x"
             % (data_block.getName(),
                "R" if data_block.isRead() else "-",
                "W" if data_block.isWrite() else "-",
                "X" if data_block.isExecute() else "-",
                data_block.getStart().getOffset(), data_block.getEnd().getOffset()))

    try:
        window = read_bytes(memory, data_address, DATA_WINDOW)
    except Exception as exc:
        emit("data_bytes_error", repr(exc))
        return
    emit("data_bytes_length", len(window))
    emit("data_bytes", hexof(window))
    emit("data_ascii", asciiof(window))

    # Bounded verdict lines: only the two addresses above feed them.
    emit("verdict_instruction_matches_e1007", "yes" if instruction.getLength() == 7 and
         list(instruction_bytes) == [0x48, 0x8D, 0x35, 0x33, 0x4C, 0x90, 0x00] else "no")
    emit("verdict_target_matches_e1007",
         "yes" if byte_target == DATA_RVA else "no")
    emit("verdict_ghidra_and_bytes_agree",
         "yes" if ghidra_target is not None and byte_target is not None and ghidra_target == byte_target
         else ("not-modelled" if ghidra_target is None else "no"))


try:
    main()
finally:
    args = [text for text in (getScriptArgs() or []) if text.strip()]
    for text in args:
        path = text.strip()
        if path.lower().startswith("0x"):
            continue
        directory = os.path.dirname(path)
        if directory and not os.path.isdir(directory):
            os.makedirs(directory)
        with open(path, "w") as handle:
            handle.write("\n".join(OUTPUT) + "\n")
        print("SXDF wrote=%s" % path)
        break
