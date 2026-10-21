"""Small stdlib checks for private artifact reconstruction and deterministic folder indexing."""
import hashlib
from pathlib import Path
import struct
import sys
import tempfile
import unittest

# `python3 -I` removes the script directory from sys.path; keep the sibling imports explicit.
sys.path.insert(0, str(Path(__file__).resolve().parent))

from project_index import generate
from unwrap_self import unwrap


def fixture_elf():
    ident = b"\x7fELF\x02\x01\x01" + bytes(9)
    header = struct.pack("<16sHHIQQQIHHHHHH", ident, 2, 62, 1, 0x1000, 64, 0, 0, 64, 56, 1, 0, 0, 0)
    program = struct.pack("<IIQQQQQQ", 1, 5, 0, 0x1000, 0x1000, 128, 128, 0x1000)
    return header + program + b"fixture!"


class ToolChecks(unittest.TestCase):
    def test_plaintext_self_roundtrip_and_identity(self):
        elf = fixture_elf()
        data = bytearray(512 + len(elf))
        data[:4] = b"O\x15=\x1d"
        struct.pack_into("<H", data, 24, 1)
        struct.pack_into("<QQQQ", data, 32, 0x800, 512, len(elf), len(elf))
        data[64:184] = elf[:120]
        data[512:] = elf
        with tempfile.TemporaryDirectory() as folder:
            source, output = Path(folder) / "fixture.self", Path(folder) / "out.elf"
            source.write_bytes(data)
            record = unwrap(source, output)
            self.assertEqual(output.read_bytes(), elf)
            self.assertEqual(record["elf_sha256"], hashlib.sha256(elf).hexdigest())
            self.assertEqual(record["missing_segments"], [])
            struct.pack_into("<Q", data, 32, 0x802)
            source.write_bytes(data)
            with self.assertRaisesRegex(ValueError, "encrypted/compressed"):
                unwrap(source, output)
            self.assertEqual(output.read_bytes(), elf)

    def test_truncated_program_header_table_does_not_create_outputs(self):
        elf = bytearray(fixture_elf())
        struct.pack_into("<H", elf, 56, 2)
        with tempfile.TemporaryDirectory() as folder:
            source, output = Path(folder) / "input.elf", Path(folder) / "out.elf"
            identity = Path(folder) / "out.identity.json"
            source.write_bytes(elf)
            with self.assertRaisesRegex(ValueError, "out of bounds"):
                unwrap(source, output)
            self.assertFalse(output.exists())
            self.assertFalse(identity.exists())

    def test_refuse_overwrite_of_other_artifact(self):
        with tempfile.TemporaryDirectory() as folder:
            source, output = Path(folder) / "input.elf", Path(folder) / "out.elf"
            source.write_bytes(fixture_elf())
            output.write_bytes(b"preserve me")
            with self.assertRaisesRegex(ValueError, "refusing to replace"):
                unwrap(source, output)
            self.assertEqual(output.read_bytes(), b"preserve me")

    def test_identity_sidecar_collision_does_not_overwrite_source(self):
        elf = fixture_elf()
        data = bytearray(512 + len(elf))
        data[:4] = b"O\x15=\x1d"
        struct.pack_into("<H", data, 24, 1)
        struct.pack_into("<QQQQ", data, 32, 0x800, 512, len(elf), len(elf))
        data[64:184] = elf[:120]
        data[512:] = elf

        with tempfile.TemporaryDirectory() as folder:
            source = Path(folder) / "input.identity.json"
            destination = Path(folder) / "input.elf"
            source.write_bytes(data)
            original_source = source.read_bytes()

            try:
                unwrap(source, destination)
            except ValueError as error:
                self.assertIn("identity sidecar would overwrite source", str(error))
            else:
                self.fail(
                    "expected identity-sidecar collision rejection; "
                    f"source_overwritten={source.read_bytes() != original_source}, "
                    f"elf_output_created={destination.exists()}"
                )

            self.assertEqual(source.read_bytes(), original_source)
            self.assertFalse(destination.exists())

    def test_index_is_stable_and_detects_a_new_folder(self):
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            generate(root)
            generate(root, check=True)
            (root / ".DS_Store").write_bytes(b"Finder metadata is not project content")
            generate(root, check=True)
            (root / "new-module").mkdir()
            with self.assertRaisesRegex(ValueError, "Stale indexes"):
                generate(root, check=True)
            generate(root)
            generate(root, check=True)
            self.assertIn("new-module", (root / "docs/FOLDERS.md").read_text())

    def test_index_ignores_nested_admin_directories(self):
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            nested = root / "nested"
            nested.mkdir()
            generate(root)

            git_dir = nested / ".git"
            git_dir.mkdir()
            (git_dir / "marker").write_text("repository metadata")
            cache_dir = nested / "__pycache__"
            cache_dir.mkdir()
            (cache_dir / "marker.pyc").write_bytes(b"python cache")

            generate(root, check=True)


if __name__ == "__main__":
    unittest.main()
