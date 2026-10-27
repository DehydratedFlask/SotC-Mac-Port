#!/usr/bin/env python3
"""ELF byte-oracle of startup constructor slot 327 (0x836330, 455 B): bool var colossusCanSeePlayer + 0x26ea0 'Colossus'/'Can See Player' toggle.
See tools/lift/slotfam_check.py for what is pinned (table word, body hash/extent/frame, ordered store/call
trace, strings, callee hashes, class tables, native constants/tests/cmake, mutation self-test).
Private: needs local/re/eboot.elf."""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import slotfam_check as lib  # noqa: E402

if __name__ == "__main__":
    raise SystemExit(lib.main(327, "bool var colossusCanSeePlayer + 0x26ea0 'Colossus'/'Can See Player' toggle"))
