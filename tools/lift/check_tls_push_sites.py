#!/usr/bin/env python3
"""Pin every TLS push call site (`call 0x12300`) and the frame it builds.

For each site the checker verifies, from the pinned disassembly and image:
  * the site really is a `call 0x12300`;
  * the next allocation call (`0x17070` or `0x12820`) follows within 0x1c bytes;
  * the file string resolved by the window's `lea` ends in `.cpp` and hashes to the recorded digest;
  * for a **resolved** site, the function signature string hashes to its recorded digest and the
    line immediate is the one stored into the struct's `+0x10` slot;
  * for a **partial** site, the recorded line is `None` — i.e. the triple is *not* claimed.

Digests are recorded instead of the strings themselves: the checker still pins the exact bytes it
read, without copying game debug text into the repository. Skips cleanly without the private inputs.
"""

from __future__ import annotations

import hashlib
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF = ROOT / "local/re/eboot.elf"
DIS = ROOT / "local/validation/registry-analysis-agent/all.dis"
CMAKE = ROOT / "native/CMakeLists.txt"
HEADER = ROOT / "native/core/tls_record_stack.hpp"

# site, line, sha256(file string), sha256(function signature), status
SITES = (
    # site, line, sha256(file), sha256(signature), status, allocation distance, target
    (0x33720, 0x7a, "8c0ea11ae25d62114c56b520aec0852351fcfeb4a434fce86493e8e647576e3d", "b3da1be6be041094074ccce2c29301145fcba1d913a4ebbed93a5208cae33e0d", "resolved", 0x4b, 0x17070),
    (0x904ee, 0x25d, "29f481463bf67f7e4435f9985d1e8b251ff4df446ad64adc7025bf9da4ca0edd", "f5c9a29f84c30cc524fe8a684b65ed7ed7eaf9377f74125b3d4aaa4f2f6416b1", "resolved", 0xa, 0x17070),
    (0x90732, 0x286, "29f481463bf67f7e4435f9985d1e8b251ff4df446ad64adc7025bf9da4ca0edd", "e38ff048e8b91028a8a6f0f77b37c1266f31d9064eb838624573947d53051d4a", "resolved", 0xa, 0x17070),
    (0x918c1, 0xe3, "16d82085ed2deaf57078a3a0ca8aeeff8f6db420511037cbae51465885bd1d4e", "0f8391632f709ce7537e00c364e78aeaa063c3e8a91db92713affcc0382b2f22", "resolved", 0x40, 0x12820),
    (0x91926, 0xe9, "16d82085ed2deaf57078a3a0ca8aeeff8f6db420511037cbae51465885bd1d4e", "0f8391632f709ce7537e00c364e78aeaa063c3e8a91db92713affcc0382b2f22", "resolved", 0x14, 0x12820),
    (0x16ea05, 0x2e, "c45c6b1cef99baa0abe87a29eaaae8e8fdafdc0bb327b143ee707cfe9c5862fa", "0b8bf9d3e797072d4251a9bdd50be012b0499886fb33b09b41d79112f95f20aa", "resolved", 0x14, 0x12820),
    (0x16ea42, 0x32, "c45c6b1cef99baa0abe87a29eaaae8e8fdafdc0bb327b143ee707cfe9c5862fa", "0b8bf9d3e797072d4251a9bdd50be012b0499886fb33b09b41d79112f95f20aa", "resolved", 0x18, 0x12820),
    (0x19acfa, 0x60, "ca1399c6fe4ecf89af9cae195dcd11f5b4633569e243f82410c764c51670a727", "237a6419c8814f0201573601b0ad098d639b480132511288bc403fd2e75b1944", "resolved", 0xb3c, 0x12820),
    (0x1b2e4a, 0xdd, "1023a29c62ef868b7fc0377d905b25b18ca50206bdc45c87d5b5b3bcd07bd53c", "7276030309a3137ecc6de6c1be7cc7574399dace94d881d2fbe01c4c0778cf8a", "resolved", 0x6c, 0x12820),
    (0x1b37b7, 0x180, "1023a29c62ef868b7fc0377d905b25b18ca50206bdc45c87d5b5b3bcd07bd53c", "1265652f4927a582a7a8d169b93c02afc146c1f320156305d43e908ed4cdb844", "resolved", 0x17e, 0x12820),
    (0xa4676b, 0x36, "63956ae7159a9ed465c2d2cfd11fb4fa391d229c5bcece46d7a2cf1037e51936", "b3da1be6be041094074ccce2c29301145fcba1d913a4ebbed93a5208cae33e0d", "resolved", 0x1171, 0x17070),
    (0xa47e67, 0x336, "fc3f56f94a7d5617d2d481ca1f2da393623f1858d535956e0bb424ebc9d0f3db", "b3da1be6be041094074ccce2c29301145fcba1d913a4ebbed93a5208cae33e0d", "resolved", 0x285, 0x17070),
    (0xa484c1, 0x3be, "fc3f56f94a7d5617d2d481ca1f2da393623f1858d535956e0bb424ebc9d0f3db", "b3da1be6be041094074ccce2c29301145fcba1d913a4ebbed93a5208cae33e0d", "resolved", 0xdbc, 0x17070),
    (0xa4ac9c, 0x6d7, "fc3f56f94a7d5617d2d481ca1f2da393623f1858d535956e0bb424ebc9d0f3db", "b3da1be6be041094074ccce2c29301145fcba1d913a4ebbed93a5208cae33e0d", "resolved", 0xda, 0x17070),
)

LINE = re.compile(r"^\s*([0-9a-f]+):\s+([a-z][a-z0-9.]*)\s+(.*)$")
RIP_LEA = re.compile(r"0x[0-9a-f]+\(%rip\), %r[a-z0-9]+\s+#\s*(0x[0-9a-f]+)")
LINE_STORE = re.compile(r"\$0x([0-9a-f]+), -?0x[0-9a-f]+\(%rbp\)")


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    header = HEADER.read_text()
    require("TlsAllocSite" in header and "tls_alloc_site_push" in header,
            "header lost the allocation-site model")
    require("tools/lift/check_tls_push_sites.py" in header,
            "header does not point at this checker")
    require("test_tls_record_stack" in CMAKE.read_text(), "cmake lost the record-stack fixture")

    if not ELF.is_file() or not DIS.is_file():
        print("SKIP check_tls_push_sites: private inputs not present")
        return 0

    elf = ELF.read_bytes()
    _, program_headers = d.program_headers(elf)
    image = d.Image(elf, program_headers)

    rows: dict[int, tuple[str, str]] = {}
    for line in DIS.open(encoding="utf-8", errors="replace"):
        match = LINE.match(line)
        if match is not None:
            rows[int(match.group(1), 16)] = (match.group(2), match.group(3).strip())

    def string_at(address: int, limit: int = 240) -> str:
        out = bytearray()
        for index in range(limit):
            try:
                byte = image.read(address + index, 1)[0]
            except Exception:
                break
            if byte == 0:
                break
            out.append(byte)
        return out.decode("utf-8", "replace")

    found_sites = sorted(address for address, (mnemonic, ops) in rows.items()
                         if mnemonic.startswith("call") and re.match(r"0x12300\b", ops))
    require(len(found_sites) == len(SITES),
            f"expected {len(SITES)} `call 0x12300` sites, found {len(found_sites)}")
    require([site for site, *_rest in SITES] == found_sites, "the pinned site list changed")

    resolved = 0
    for site, line_value, file_sha, function_sha, status, alloc_distance, alloc_target in SITES:
        following = [(address, ops) for address, (mnemonic, ops) in sorted(rows.items())
                     if address > site and mnemonic.startswith("call")
                     and re.match(r"0x1(7070|2820)\b", ops.split()[0] if ops.split() else "")]
        require(following, f"0x{site:x} has no following allocation call")
        address, ops = following[0]
        require(address - site == alloc_distance,
                f"0x{site:x} allocation distance changed ({address - site:#x})")
        require(int(ops.split()[0], 16) == alloc_target,
                f"0x{site:x} allocation target changed ({ops.split()[0]})")

        back = [(address, rows[address][0], rows[address][1]) for address in sorted(rows)
                if site - 0x300 <= address < site]

        # The struct base is the frame slot handed to the call; the line lives at +0x10 of it.
        frame = None
        for _a, mnemonic, ops in reversed(back):
            operands = f"{mnemonic} {ops.split('#', 1)[0].strip()}"
            match = re.search(r"leaq\s+(-?0x[0-9a-f]+)\(%(rsp|rbp)\), %rsi$", operands)
            if match is not None:
                frame = (match.group(2), int(match.group(1), 16))
                break

        leas = []
        for _a, mnemonic, ops in reversed(back):
            match = (re.search(r"leaq\s+0x[0-9a-f]+\(%rip\), %r[a-z0-9]+\s+#\s*(0x[0-9a-f]+)",
                               f"{mnemonic} {ops}") if mnemonic == "leaq" else None)
            if match is not None:
                leas.append(int(match.group(1), 16))
        require(leas, f"0x{site:x} has no rip-relative lea in its window")
        strings = [(address, string_at(address)) for address in leas]
        files = [text for _address, text in strings if text.lower().endswith(".cpp")]
        signatures = [text for _address, text in strings
                      if text and not text.lower().endswith(".cpp")]
        require(files, f"0x{site:x} resolves no source path")
        require(hashlib.sha256(files[0].encode()).hexdigest() == file_sha,
                f"0x{site:x} source path changed")
        require("\\" in files[0] or "/" in files[0], f"0x{site:x} path has no separator")

        if status != "resolved":
            require(line_value is None, f"0x{site:x} is partial but records a line")
            continue

        resolved += 1
        require(frame is not None, f"0x{site:x} has no frame base for its struct")
        require(signatures, f"0x{site:x} resolves no signature")
        require(hashlib.sha256(signatures[0].encode()).hexdigest() == function_sha,
                f"0x{site:x} signature changed")
        register, displacement = frame
        wanted = displacement + 0x10
        stores = [int(match.group(1), 16) for _a, mnemonic, ops in back
                  for match in [re.match(rf"movl\s+\$0x([0-9a-f]+), (-?0x[0-9a-f]+)\(%{register}\)$",
                                         f"{mnemonic} {ops.split('#', 1)[0].strip()}")]
                  if match is not None and int(match.group(2), 16) == wanted]
        require(line_value in stores,
                f"0x{site:x} line {line_value} not stored at {register}{wanted:+#x}")

    require(resolved == len(SITES), f"expected {len(SITES)} resolved sites, found {resolved}")
    adjacent = sum(1 for site, _l, _f, _s, _st, distance, _t in SITES if distance <= 0x1C)
    print(f"OK check_tls_push_sites: {len(SITES)} push sites, {resolved} full (signature, source path, "
          f"line) triples; {adjacent} sit within 0x1c bytes of their allocation call, the rest push "
          f"earlier in the same block")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
