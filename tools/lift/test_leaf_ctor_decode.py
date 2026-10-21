#!/usr/bin/env python3
"""Synthetic fail-closed regression for the strict leaf-constructor decoder."""
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import leaf_ctor_decode  # noqa: E402


class SyntheticNopImage:
    def read(self, address, size):
        if (address, size) != (0x4000, 16):
            raise AssertionError(f"unexpected image read {address:#x}+{size}")
        return b"\x90" * 16


class LeafCtorDecodeTests(unittest.TestCase):
    def test_unsupported_nop_fails_closed(self):
        with self.assertRaises(leaf_ctor_decode.DecodeError) as raised:
            leaf_ctor_decode.decode(SyntheticNopImage(), 0x4000)

        self.assertTrue(
            str(raised.exception).startswith("unrecognised encoding at 0x4000:"),
            str(raised.exception),
        )


if __name__ == "__main__":
    unittest.main()
