#!/usr/bin/env python3
"""Verify static bytes and mappings for RE-02 app-main callsite 0x25aa06.

This intentionally reports an unresolved edge when the receiver's reaching
pointer-table value is not unique from the pinned file evidence.
"""
from __future__ import annotations

import hashlib
import json
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
EVIDENCE = ROOT / "local/re/evidence/re02-appmain-vcall-25aa06"
ELF_PATH = ROOT / "local/re/eboot.elf"
EXPECTED_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
TARGET = 0x25AA06


def fail(message: str) -> None:
    raise ValueError(message)


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def parse_elf(data: bytes) -> dict:
    if len(data) < 64 or data[:4] != b"\x7fELF" or data[4:6] != b"\x02\x01":
        fail("input is not a little-endian ELF64 file")
    e_type, machine = struct.unpack_from("<HH", data, 16)
    entry, phoff = struct.unpack_from("<QQ", data, 24)
    phentsize, phnum = struct.unpack_from("<HH", data, 54)
    if phentsize != 56 or phoff + phentsize * phnum > len(data):
        fail("invalid ELF64 program-header table")
    headers = []
    for i in range(phnum):
        off = phoff + i * phentsize
        p_type, flags, p_offset, vaddr, _, filesz, memsz, _ = struct.unpack_from("<IIQQQQQQ", data, off)
        if p_offset + filesz > len(data):
            fail(f"program header {i} exceeds the file")
        headers.append({"index": i, "type": p_type, "flags": flags, "offset": p_offset,
                        "vaddr": vaddr, "filesz": filesz, "memsz": memsz})
    return {"type": e_type, "machine": machine, "entry": entry, "phoff": phoff,
            "phentsize": phentsize, "phnum": phnum, "headers": headers}


def mapping(elf: dict, address: int, size: int = 1) -> dict:
    candidates = []
    for ph in elf["headers"]:
        if ph["vaddr"] <= address and address + size <= ph["vaddr"] + ph["filesz"]:
            candidates.append((ph["offset"] + address - ph["vaddr"], ph))
    if any(ph["type"] == 1 for _, ph in candidates):
        candidates = [(offset, ph) for offset, ph in candidates if ph["type"] == 1]
    if not candidates:
        fail(f"no file mapping for {address:#x}+{size:#x}")
    offsets = {offset for offset, _ in candidates}
    if len(offsets) != 1:
        fail(f"ambiguous file mapping for {address:#x}+{size:#x}")
    offset = next(iter(offsets))
    chosen = next(ph for item_offset, ph in candidates if item_offset == offset)
    alternatives = []
    for ph in elf["headers"]:
        if ph["vaddr"] <= address and address + size <= ph["vaddr"] + ph["filesz"]:
            alternatives.append({"program_header": ph["index"], "type": ph["type"],
                                 "file_offset": ph["offset"] + address - ph["vaddr"]})
    return {"file_offset": offset, "program_header": chosen["index"],
            "type": chosen["type"], "flags": chosen["flags"], "other_file_views": alternatives}


def parse_objdump(name: str) -> dict[int, dict]:
    path = EVIDENCE / name
    if not path.is_file():
        fail(f"missing LLVM disassembly artifact: {path}")
    parsed: dict[int, dict] = {}
    for line in path.read_text(errors="strict").splitlines():
        if "\t" not in line:
            continue
        left, text = line.split("\t", 1)
        fields = left.split()
        if len(fields) < 2 or not fields[0].endswith(":"):
            continue
        tokens = fields[1:]
        if any(not re.fullmatch(r"[0-9a-fA-F]{2}", token) for token in tokens):
            continue
        address = int(fields[0][:-1], 16)
        if address in parsed:
            fail(f"duplicate disassembly address {address:#x} in {name}")
        parsed[address] = {"bytes": bytes.fromhex("".join(tokens)), "text": text.strip()}
    if not parsed:
        fail(f"no instruction records parsed from {name}")
    return parsed


def check_instruction(data: bytes, elf: dict, disassembly: dict[int, dict],
                      address: int, expected_hex: str, label: str) -> dict:
    expected = bytes.fromhex(expected_hex)
    where = mapping(elf, address, len(expected))
    raw = data[where["file_offset"]:where["file_offset"] + len(expected)]
    if raw != expected:
        fail(f"raw file bytes mismatch for {label} at {address:#x}: {raw.hex()}")
    ins = disassembly.get(address)
    if ins is None or ins["bytes"] != expected:
        fail(f"LLVM disassembly mismatch for {label} at {address:#x}")
    return {"label": label, "address": f"0x{address:x}", "bytes": raw.hex(),
            "length": len(raw), "instruction": ins["text"], "mapping": where}


def read_word(data: bytes, elf: dict, address: int, label: str) -> dict:
    where = mapping(elf, address, 8)
    raw = data[where["file_offset"]:where["file_offset"] + 8]
    return {"label": label, "address": f"0x{address:x}", "bytes": raw.hex(),
            "value": f"0x{int.from_bytes(raw, 'little'):x}", "mapping": where}


def checked_word(data: bytes, elf: dict, address: int, expected: int, label: str) -> dict:
    item = read_word(data, elf, address, label)
    if int(item["value"], 16) != expected:
        fail(f"{label} at {address:#x} is {item['value']}, expected {expected:#x}")
    return item


def main() -> dict:
    source = ELF_PATH.read_bytes()
    digest = sha256(source)
    if digest != EXPECTED_SHA256:
        fail(f"pinned ELF digest mismatch: {digest}")
    elf = parse_elf(source)
    if elf["type"] != 0xFE10 or elf["machine"] != 62 or elf["phnum"] != 11:
        fail("unexpected ELF identity or program-header count")

    call_dasm = parse_objdump("callsite.objdump.txt")
    main_dasm = parse_objdump("main-ctor.objdump.txt")
    target_a_dasm = parse_objdump("candidate-targets.objdump.txt")
    target_b_dasm = parse_objdump("candidate-targets-2.objdump.txt")

    instructions = []
    # Function-entry RDI -> R15 and callsite's immediate pointer-table load/use.
    for address, raw, label, dasm in [
        (0x25A8F2, "4989ff", "copy entry RDI to R15", call_dasm),
        (0x25A9FD, "498b07", "load qword [R15] into RAX", call_dasm),
        (0x25AA00, "4c89ff", "copy R15 to RDI at call", call_dasm),
        (0x25AA03, "4c89f6", "copy R14 to RSI at call", call_dasm),
        (0x25AA06, "ff5020", "indirect call [RAX+0x20]", call_dasm),
        (0x536742, "4c8da5c0c0ffff", "main forms stack object address", main_dasm),
        (0x53675E, "4c89e7", "main passes object to constructor", main_dasm),
        (0x536775, "e826030000", "main calls constructor", main_dasm),
        (0x536AB8, "4889fb", "constructor copies object pointer to RBX", main_dasm),
        (0x536AC7, "488d05fa210901", "constructor LEA vtable base", main_dasm),
        (0x536AF2, "4883c010", "constructor advances addresspoint by 0x10", main_dasm),
        (0x536AFA, "488903", "constructor stores pointer at object offset zero", main_dasm),
        (0x536839, "488b85c0c0ffff", "main reloads object first qword", main_dasm),
        (0x536840, "488dbdc0c0ffff", "main sets RDI to same stack object", main_dasm),
        (0x53684E, "ff5018", "main dispatches through slot +0x18", main_dasm),
        (0x531D60, "55", "first target byte at app-main table slot +0x20", target_b_dasm),
        (0x255130, "55", "first target byte at two other pointer windows", target_a_dasm),
    ]:
        instructions.append(check_instruction(source, elf, dasm, address, raw, label))

    call_bytes = bytes.fromhex("ff5020")
    if (call_bytes[0] != 0xFF or ((call_bytes[1] >> 3) & 7) != 2
            or (call_bytes[1] >> 6) != 1 or (call_bytes[1] & 7) != 0):
        fail("callsite encoding is not FF /2 through [RAX+disp8]")
    call_disp = struct.unpack_from("<b", call_bytes, 2)[0]
    if call_disp != 0x20:
        fail("callsite displacement is not +0x20")

    # Decode the constructor's RIP-relative LEA from the independently checked bytes.
    lea_at = 0x536AC7
    lea_raw = bytes.fromhex("488d05fa210901")
    vtable_base = lea_at + len(lea_raw) + struct.unpack_from("<i", lea_raw, 3)[0]
    addresspoint = vtable_base + 0x10
    if (vtable_base, addresspoint) != (0x15C8CC8, 0x15C8CD8):
        fail("constructor-derived addresspoint does not match expected byte target")

    appmain_entry_word = checked_word(source, elf, addresspoint + 0x18, 0x25A8D0,
                                      "constructor-linked +0x18 entry word")
    appmain_slot_word = read_word(source, elf, addresspoint + 0x20,
                                  "constructor-linked +0x20 call slot")
    appmain_candidate = int(appmain_slot_word["value"], 16)
    if appmain_candidate != 0x531D60:
        fail("constructor-linked +0x20 slot differs from expected observed word")

    # Two additional file-backed pointer windows also contain the function entry
    # at +0x18, but their following +0x20 words differ from the app-main table.
    alternate_windows = []
    for entry_word_address in (0x159C590, 0x15D5B98):
        entry_word = checked_word(source, elf, entry_word_address, 0x25A8D0,
                                  "additional +0x18 function-pointer word")
        slot_word = read_word(source, elf, entry_word_address + 8,
                              "following +0x20 pointer word")
        target = int(slot_word["value"], 16)
        if target != 0x255130:
            fail(f"alternate following word at {entry_word_address + 8:#x} changed")
        alternate_windows.append({"candidate_addresspoint": f"0x{entry_word_address - 0x18:x}",
                                  "function_pointer": entry_word, "following_slot": slot_word})

    # All observed static candidate targets must be file-backed executable bytes.
    appmain_target_map = mapping(elf, appmain_candidate, 1)
    other_target_map = mapping(elf, 0x255130, 1)
    if appmain_target_map["type"] != 1 or other_target_map["type"] != 1:
        fail("one or more candidate target entries are not PT_LOAD-backed")

    rea_bundle_path = EVIDENCE / "rea-session-evidence.json"
    rea_bundle = json.loads(rea_bundle_path.read_text())
    if not all(rea_bundle.get(key) == [] for key in
               ("records", "unknowns", "providers", "environments", "artifacts", "captures", "scenarios")):
        fail("REA export bundle is not the recorded empty export")
    rea_bundle_sha = sha256(rea_bundle_path.read_bytes())
    if rea_bundle_sha != "000bef2a45fbd4b17adc4e2ab0acd65b076ef5c25f0d8479244c8088730de9cd":
        fail(f"REA export bundle digest changed: {rea_bundle_sha}")
    timeout_path = EVIDENCE / "rea-timeout-record.json"
    timeout = json.loads(timeout_path.read_text())
    if (timeout.get("attempt") != 1
            or timeout.get("retained_output_summary", {}).get("error_class") != "provider_timeout"
            or timeout.get("retained_output_summary", {}).get("target_sha256") != digest
            or timeout.get("retained_output_summary", {}).get("run_id") != "1d30f13b-1bf7-4897-a088-b59b8e01e6d4"
            or timeout.get("retained_output_summary", {}).get("analysis_profile_digest")
               != "769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa"
            or timeout.get("session_cleanup", {}).get("session_closed") is not True
            or timeout.get("session_cleanup", {}).get("evidence_records") != 0):
        fail("REA timeout/cleanup record does not match the retained summary")

    return {
        "schema_version": 1,
        "task_id": "RE-02-APPMAIN-VCALL-25AA06-01",
        "result": "PASS_UNRESOLVED_EDGE",
        "source": {"path": str(ELF_PATH), "size": len(source), "sha256": digest,
                   "elf_type": hex(elf["type"]), "machine": "EM_X86_64",
                   "entry": hex(elf["entry"]), "program_header_count": elf["phnum"]},
        "analysis": {"mode": "offline static byte and mapping verification",
                     "rea_attempt_1": timeout,
                     "rea_evidence_bundle": {"path": str(rea_bundle_path), "sha256": rea_bundle_sha,
                                             "bytes": rea_bundle_path.stat().st_size,
                                             "records": 0, "unknowns": 0,
                                             "closed": True}},
        "callsite": {"address": "0x25aa06", "instruction_bytes": "ff5020",
                      "decoded_operand": {"base_register": "RAX", "displacement": 32},
                      "reaching_register_flow": [
                          {"address": "0x25a8f2", "observation": "R15 receives the value in RDI"},
                          {"address": "0x25a9fd", "observation": "RAX loads the qword at [R15]"},
                          {"address": "0x25aa00", "observation": "RDI receives R15 immediately before the call"},
                          {"address": "0x25aa06", "observation": "call reads pointer at [RAX+0x20]"}],
                      "coverage_limit": "Intervening direct-call effects on R15 and receiver memory are not closed by this local slice; no ABI or runtime claim is made."},
        "known_appmain_context": {"object_local": "[RBP-0x3f40]",
                                  "constructor_vptr_addresspoint": hex(addresspoint),
                                  "constructor_vtable_base": hex(vtable_base),
                                  "main_slot_0x18_word": appmain_entry_word,
                                  "slot_0x20_word": appmain_slot_word,
                                  "slot_0x20_candidate_target": hex(appmain_candidate),
                                  "candidate_target_mapping": appmain_target_map,
                                  "candidate_target_entry_bytes": "55",
                                  "interpretation": "For the constructor-linked app-main table, the bytes place 0x531d60 at addresspoint+0x20. This is a path-specific static candidate, not a global callsite resolution."},
        "alternate_pointer_windows": alternate_windows,
        "alternate_candidate_target": {"address": "0x255130", "mapping": other_target_map,
                                        "entry_bytes": "55"},
        "instruction_checks": instructions,
        "edge_status": "unresolved",
        "unresolved_reason": "The callsite reads the receiver's current first qword as a table base, but the pinned bytes do not establish a unique reaching table for every invocation. The constructor-linked table yields 0x531d60 at +0x20; two other file-backed +0x18 words point to 0x25a8d0 while their following +0x20 words yield 0x255130.",
        "exact_missing_evidence": [
            "A complete, byte-backed provenance for the receiver and its current [receiver+0] value at 0x25aa06 across all call paths.",
            "Evidence that excludes or resolves the two additional pointer windows at 0x159c590 and 0x15d5b98 as possible receiver-table contexts for this invocation.",
            "Interprocedural proof that the function's receiver register and first object word retain the constructor-linked values through the intervening calls; no runtime trace or ABI contract is asserted."
        ],
        "limits": ["Static offline observations only; no runtime reachability or behavior was tested.",
                   "No class identity, method identity, ABI, semantic contract, or native lift is claimed.",
                   "Only callsite 0x25aa06 was analyzed; callsite 0x25ac9f is outside this task."]
    }


if __name__ == "__main__":
    try:
        result = main()
        print(json.dumps(result, indent=2, sort_keys=True))
    except Exception as exc:  # Keep verifier failure explicit for shell capture.
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
