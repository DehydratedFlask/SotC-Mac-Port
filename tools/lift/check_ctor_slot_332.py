#!/usr/bin/env python3
"""ELF byte-oracle of startup constructor slot 332 (0x805440, 694 B): two (bool var, 0x26db0 item) pairs, Shadow IK / CP10-Anim.
See tools/lift/slotfam_check.py for what is pinned (table word, body hash/extent/frame, ordered store/call
trace, strings, callee hashes, class tables, native constants/tests/cmake, mutation self-test).
Private: needs local/re/eboot.elf."""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import slotfam_check as lib  # noqa: E402

if __name__ == "__main__":
    raise SystemExit(lib.main(332, "two (bool var, 0x26db0 item) pairs, Shadow IK / CP10-Anim"))
