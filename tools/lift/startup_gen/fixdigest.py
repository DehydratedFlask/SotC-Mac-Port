#!/usr/bin/env python3
"""Re-derive the registered-index-set digest pinned in check_ctor_slots.py after new slots were wired (needs SOTC_NATIVE_BUILD)."""
import os, pathlib, re, subprocess
root = pathlib.Path(__file__).resolve().parents[3]
os.chdir(root)
r = subprocess.run(['python3', '-I', 'tools/lift/check_ctor_slots.py'], capture_output=True, text=True)
m = re.search(r'index-set sha256 changed: ([0-9a-f]{64})', r.stderr + r.stdout)
if m:
    p = root / 'tools/lift/check_ctor_slots.py'
    s = p.read_text()
    s = re.sub(r'(registered_digest == ")[0-9a-f]{64}', r'\g<1>' + m.group(1), s, count=1)
    p.write_text(s)
    r = subprocess.run(['python3', '-I', str(p)], capture_output=True, text=True)
print(r.returncode, (r.stdout + r.stderr)[-300:])
