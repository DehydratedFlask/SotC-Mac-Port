#!/usr/bin/env python3
"""Hash supplied PKGs and inventory extracted outputs; no extraction or game execution."""
import hashlib
import json
from pathlib import Path
import runpy
import struct

ROOT = Path(__file__).resolve().parents[1]


def digest(path):
    before = path.stat()
    result = hashlib.sha256()
    with path.open("rb") as stream:
        while block := stream.read(4 * 1024 * 1024):
            result.update(block)
    after = path.stat()
    if (before.st_size, before.st_mtime_ns) != (after.st_size, after.st_mtime_ns):
        raise ValueError(f"input changed while hashing: {path}")
    return result.hexdigest()


def inspect():
    inputs = []
    for path in sorted((ROOT / "Shadow of the Colossus").rglob("*.pkg")):
        with path.open("rb") as stream:
            header = stream.read(128)
        if len(header) < 128 or header[:4] != b"\x7fCNT":
            raise ValueError(f"not an admitted PS4 PKG: {path}")
        item = {"path": path.relative_to(ROOT).as_posix(), "bytes": path.stat().st_size,
                "content_id": header[64:100].rstrip(b"\0").decode("ascii"), "sha256": digest(path)}
        inputs.append(item)
        print(item["path"], item["sha256"], flush=True)
    reader = ROOT / "reference/ShadowOfTheColossus-PC/scripts/prepare.py"
    read_sfo = runpy.run_path(str(reader), run_name="sotc_reader")["sfo"]
    extracted = []
    for path in sorted((ROOT / "local/extracted").rglob("*")):
        if not path.is_file():
            continue
        item = {"path": path.relative_to(ROOT).as_posix(), "bytes": path.stat().st_size}
        if path.name == "param.sfo":
            values = read_sfo(path.read_bytes())
            item["identity"] = {key: values.get(key) for key in
                                ["TITLE", "TITLE_ID", "CONTENT_ID", "APP_VER", "VERSION", "CATEGORY"]}
        if path.suffix.lower() == ".psarc":
            with path.open("rb") as stream:
                header = stream.read(32)
            if len(header) != 32 or header[:4] != b"PSAR":
                raise ValueError(f"invalid PSARC header: {path}")
            magic, version, codec, toc_bytes, entry_bytes, entries, block_bytes, flags = struct.unpack(
                ">4sI4sIIIII", header)
            item["psarc"] = {"version": hex(version), "codec": codec.decode("ascii"),
                             "toc_bytes": toc_bytes, "entry_bytes": entry_bytes,
                             "entries": entries, "block_bytes": block_bytes, "flags": flags}
        if path.name == "eboot.bin" or path.suffix.lower() in {".prx", ".sprx", ".at9"}:
            item["sha256"] = digest(path)
        extracted.append(item)
    result = {"schema_version": 1, "packages": inputs, "extracted_files": extracted,
              "limitations": ["Inventory and container headers only, not complete asset/schema validation.",
                              "No executable was run and no native game milestone is established."]}
    output = ROOT / "local/manifests/packages.json"
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(result, indent=2) + "\n")
    print(f"{len(inputs)} packages, {len(extracted)} extracted files; {output}")
    return result


if __name__ == "__main__":
    inspect()
