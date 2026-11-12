#!/usr/bin/env python3
"""Update the registration-count pins after startup slots were wired (batch-11 helper).

usage: bump_pins.py <slot> [<slot> ...]   (the slots must already be rows of native/core/startup_ctors.cpp)
Edits: test_startup_ctor_coverage.cpp (count / cliff / result checks), test_startup_ctors.cpp (expect[]),
check_console_var_ctors.py (COVERAGE_TOKENS), check_fctor_slots_296_297_300.py (row count) and check_ctor_slots.py
(DEBUG_PAGE_BATCH_11, unregistered count, cliff; the digest is re-derived by running the checker afterwards).
Reads only repository text; nothing is executed from the original game.
"""
import pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parents[3]
new = sorted({int(a) for a in sys.argv[1:]})


def rd(p):
    return (ROOT / p).read_text()


def wr(p, s):
    (ROOT / p).write_text(s)


cpp = rd('native/core/startup_ctors.cpp')
rows = sorted({int(m.group(1)) for m in re.finditer(r'\{0x[0-9a-f]+, (\d+), \[\]', cpp)})
for n in new:
    assert n in rows, f'slot {n} is not registered'
table = {int(i): int(r, 16) for i, r in re.findall(r'^\s*\{\s*(\d+)\s*,\s*(0x[0-9a-fA-F]+)\s*\},', rd('native/core/ctor_original_table.cpp'), re.M)}
N = len(rows)
unreg = [i for i in sorted(table) if i not in rows]
U = len(unreg)
cliff = unreg[0]
print('registered', N, 'unregistered', U, 'cliff', cliff, hex(table[cliff]))

p = 'native/tests/test_startup_ctor_coverage.cpp'
s = rd(p)
s = re.sub(r'CHECK\(slots\.size\(\) == \d+\);', f'CHECK(slots.size() == {N});', s)
s = re.sub(r'first_unrecovered_constructor_index\(\) == \d+', f'first_unrecovered_constructor_index() == {cliff}', s)
s = re.sub(r'first_unrecovered_constructor_rva\(\) == 0x[0-9a-f]+', f'first_unrecovered_constructor_rva() == {table[cliff]:#x}', s)
s = re.sub(r'slots\.size\(\) \+ \d+ == kOriginalCtorCount', f'slots.size() + {U} == kOriginalCtorCount', s)
for n in new:
    if f'slot{n}_result.ok' not in s:
        anchor = re.search(r'    CHECK\(st\.slot349_result\.ok[^\n]*\n', s)
        s = s[:anchor.end()] + f'    CHECK(st.slot{n}_result.ok && st.slot{n}_result.rows_done == kSlot{n}RowCount);   // batch 14\n' + s[anchor.end():]
wr(p, s)

p = 'native/tests/test_startup_ctors.cpp'
s = rd(p)
i0 = s.index('const std::uint32_t expect[] = {')
i1 = s.index('};', i0)
blk = s[i0:i1]
nums = [int(x) for x in re.findall(r'\b\d+\b', blk[blk.index('{'):])]
for n in new:
    if n in nums:
        continue
    lower = max(x for x in nums if x < n)
    blk = re.sub(rf'\b{lower},', f'{lower}, {n},', blk, count=1)
    nums.append(n)
assert sorted(nums) == [int(x) for x in re.findall(r'\b\d+\b', blk[blk.index('{'):])]
s = s[:i0] + blk + s[i1:]
wr(p, s)

p = 'tools/lift/check_console_var_ctors.py'
s = rd(p)
s = re.sub(r'slots\.size\(\) == \d+', f'slots.size() == {N}', s)
s = re.sub(r'first_unrecovered_constructor_index\(\) == \d+', f'first_unrecovered_constructor_index() == {cliff}', s)
s = re.sub(r'first_unrecovered_constructor_rva\(\) == 0x[0-9a-f]+', f'first_unrecovered_constructor_rva() == {table[cliff]:#x}', s)
s = re.sub(r'slots\.size\(\) \+ \d+ == kOriginalCtorCount', f'slots.size() + {U} == kOriginalCtorCount', s)
wr(p, s)

p = 'tools/lift/check_fctor_slots_296_297_300.py'
s = rd(p)
s = re.sub(r'len\(rows\) == \d+, f"expected \d+ registered rows', f'len(rows) == {N}, f"expected {N} registered rows', s)
wr(p, s)

p = 'tools/lift/check_ctor_slots.py'
s = rd(p)
m = re.search(r'^DEBUG_PAGE_BATCH_14 = \{([^}]*)\}', s, re.M)
if m:
    cur = {int(x) for x in re.findall(r'\d+', m.group(1))} | set(new)
    s = s.replace(m.group(0), 'DEBUG_PAGE_BATCH_14 = {' + ', '.join(map(str, sorted(cur))) + '}')
else:
    anchor = re.search(r'^DEBUG_PAGE_BATCH_13 = \{[^}]*\}\n', s, re.M)
    s = s[:anchor.end()] + ('# Batch 14 (CPU-01-STARTUP-BATCH-14): same generator (tools/lift/startup_gen), plus the tree / node-reference row kinds\n'
                            'DEBUG_PAGE_BATCH_14 = {' + ', '.join(map(str, new)) + '}\n') + s[anchor.end():]
    s = s.replace('| DEBUG_PAGE_BATCH_12 | DEBUG_PAGE_BATCH_13)', '| DEBUG_PAGE_BATCH_12 | DEBUG_PAGE_BATCH_13 | DEBUG_PAGE_BATCH_14)')
s = re.sub(r'assert len\(unregistered\) == \d+, f"expected \d+ unregistered slots', f'assert len(unregistered) == {U}, f"expected {U} unregistered slots', s)
s = re.sub(r'assert unregistered\[0\] == \d+ and original_rvas\[\d+\] == 0x[0-9a-f]+', f'assert unregistered[0] == {cliff} and original_rvas[{cliff}] == {table[cliff]:#x}', s)
s = re.sub(r'# The remaining \d+ original slots', f'# The remaining {U} original slots', s)
wr(p, s)
