#!/usr/bin/env python3
"""Measure the lowering-shape ladder over the private corpus, offline.

For each per-lane-eligible program in a deterministic sample this reports one of:

  MANIFEST-ONLY  no shape blocker left, so a filled manifest is the only missing input
  LOWERED        the program produced MSL through `gcn_to_msl.generate_stage`, and (with
                 `--compile`) that MSL compiled with `xcrun -sdk macosx metal`
  BLOCKED        one or more shape blockers remain; the histogram is printed

The bindings come from `msl_abi_manifest.synthetic_fill`, which is a *placeholder*: it fills every
hole so the form can close. A program with no holes therefore lowers with a manifest that has **no
bindings at all**, and only that case makes no binding claim. Everything else is a shape proof, not
a retail ABI proof, and the report says so per row.

Nothing here executes a shader, writes no payload bytes and stores no listing: the MSL is private
derivative data under `local/`, and only digests, sizes, slot numbers and the decoder's own strings
are recorded.

  python3 -I tools/dev/msl_lower_ladder.py --sample 512 --out local/validation/msl-shape-ladder
  python3 -I tools/dev/msl_lower_ladder.py --sample 512 --out DIR --compile
  python3 -I tools/dev/msl_lower_ladder.py --self-test
"""
from __future__ import annotations

import argparse
from collections import Counter
import hashlib
import importlib.util
import json
from pathlib import Path
import shutil
import subprocess
import sys

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
VERSION = 'msl-lower-ladder-v1'


def _module(name: str, filename: str):
    spec = importlib.util.spec_from_file_location(name, HERE / filename)
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


abi = _module('msl_abi_manifest_for_ladder', 'msl_abi_manifest.py')
m = _module('gcn_to_msl_for_ladder', 'gcn_to_msl.py')


class LadderError(ValueError):
    pass


def lower_one(code: bytes, stage: str) -> dict:
    """Analyse, fill, project and lower one program. Returns a row, never raises for a block."""
    analysis = abi.analyze(code, stage)
    if not analysis['per_lane_eligible']:
        return {'status': None}
    row = {
        'stage': stage,
        'code_bytes': analysis['bytes'],
        'code_sha256': analysis['sha256'],
        'instructions': analysis['instructions'],
        'binding_holes': analysis['counts']['binding_holes'],
        'shape_kinds': sorted({blocker['kind'] for blocker in analysis['shape']}),
        'shape_count': len(analysis['shape']),
        'manifest_only': analysis['manifest_only'],
    }
    if not analysis['manifest_only']:
        row['status'] = 'BLOCKED'
        return row
    manifest = abi.synthetic_fill(analysis)
    problems = abi.validate(manifest, analysis)
    if problems:
        row['status'] = 'MANIFEST-ONLY'
        row['validate_problems'] = [problem['code'] for problem in problems]
        return row
    row['filled_bindings'] = sum(
        len(manifest.get(section) or {})
        for section in ('scalar_bases', 'buffer_descriptors', 'exports', 'initial_scalars',
                        'initial_vectors'))
    try:
        source, report = m.generate_stage(code, abi.to_lowerer_stage_abi(manifest), stage)
    except m.LoweringError as exc:
        row['status'] = 'MANIFEST-ONLY'
        row['lowering_error'] = str(exc)
        return row
    row.update({'status': 'LOWERED', 'msl_sha256': hashlib.sha256(source.encode()).hexdigest(),
                'msl_bytes': len(source.encode()), 'msl_source': source,
                'exports': report['exports'], 'null_exports': report['null_exports'],
                'valid_mask_exports': report['valid_mask_exports']})
    return row


def compile_msl(source: str, workdir: Path, name: str) -> dict:
    """Compile one MSL file with the Metal compiler; returns the command and its result."""
    metal = shutil.which('xcrun')
    if metal is None:
        raise LadderError('xcrun is not on PATH, so no MSL compile can be attempted')
    path = workdir / f'{name}.metal'
    path.write_text(source)
    argv = ['xcrun', '-sdk', 'macosx', 'metal', '-c', str(path), '-o', str(path.with_suffix('.air'))]
    done = subprocess.run(argv, capture_output=True, text=True)
    return {'argv': [str(piece) for piece in argv], 'returncode': done.returncode,
            'stderr': done.stderr.strip()[-400:]}


def resolve_out(out: Path) -> Path:
    resolved = out.resolve()
    local = (ROOT / 'local').resolve()
    if resolved.is_relative_to(ROOT.resolve()) and not resolved.is_relative_to(local):
        raise LadderError('private ladder output inside this repo must stay under local/')
    return resolved


def run(archives: list[Path], sample: int, out: Path, compile_msl_files: bool,
        overwrite: bool) -> tuple[dict, Path]:
    cov = abi._load_coverage()
    target = resolve_out(out)
    missing = [str(path) for path in archives if not path.is_file()]
    if missing:
        raise LadderError('private archive(s) missing, no ladder produced:\n  '
                          + '\n  '.join(missing))
    population, identities = [], []
    for path in archives:
        path = path.resolve()
        archive = cov.Psarc(path)
        try:
            members = archive.members()
            csdr = [(name, index) for name, index in members if name.endswith('.csdr')]
        finally:
            archive.close()
        relative = (str(path.relative_to(ROOT)) if path.is_relative_to(ROOT) else str(path))
        identities.append({'name': path.name, 'path': relative, 'members': len(members),
                           'csdr_members': len(csdr)})
        population.extend((name, index, path) for name, index in csdr)
    population.sort(key=lambda item: (item[0], item[2].name))
    indices = cov.sample_indices(len(population), sample)
    handles: dict[Path, object] = {}
    counters = Counter()
    rows: list[dict] = []
    try:
        for position in indices:
            name, index, path = population[position]
            archive = handles.setdefault(path, cov.Psarc(path))
            data = archive.read(index)
            try:
                blobs = cov.parse_raw_csdr(data)
            except cov.ContainerError:
                counters['container-rejected'] += 1
                continue
            for blob in blobs:
                code = data[blob['code_offset']:blob['code_offset'] + blob['length']]
                spans, _split = cov.split_stages(code)
                label = cov.STAGE_BY_TYPE.get(blob['type'])
                for span in spans:
                    if label not in abi.STAGES:
                        counters['other-stage'] += 1
                        continue
                    try:
                        row = lower_one(span, label)
                    except abi.AbiError:
                        counters['not-linear'] += 1
                        continue
                    if row.get('status') is None:
                        counters['per-lane-blocked'] += 1
                        continue
                    row.update({'path': name, 'member_bytes': len(data)})
                    counters[row['status'].lower().replace('-', '_')] += 1
                    rows.append(row)
    finally:
        for archive in handles.values():
            archive.close()
    # per-lane eligibility was already enforced by analyze(); count it explicitly
    rows = [row for row in rows if row.get('status')]
    rows.sort(key=lambda row: (row['status'] != 'LOWERED', -row['code_bytes'], row['path']))
    compilations = 0
    if compile_msl_files:
        if shutil.which('xcrun') is None:
            raise LadderError('--compile requires xcrun; the Metal toolchain is not installed')
        workdir = target / 'msl'
        if workdir.exists() and not overwrite:
            raise LadderError(f'{workdir} already exists; pass --overwrite')
        workdir.mkdir(parents=True, exist_ok=True)
        for position, row in enumerate(rows):
            if row['status'] != 'LOWERED':
                continue
            name = f'{position:03d}_{row["stage"]}_{row["code_sha256"][:12]}'
            row['compile'] = compile_msl(row.pop('msl_source'), workdir, name)
        compilations = sum(1 for row in rows if row.get('compile', {}).get('returncode') == 0)
    else:
        for row in rows:
            row.pop('msl_source', None)
    shape = Counter()
    for row in rows:
        for kind in row['shape_kinds']:
            shape[kind] += 1
    document = {
        'schema': VERSION,
        'sample': {'requested': sample, 'selected': len(indices), 'population': len(population),
                   'indices': indices, 'archives': identities},
        'counts': dict(sorted(counters.items())),
        'programs': len(rows),
        'lowered': sum(1 for row in rows if row['status'] == 'LOWERED'),
        'manifest_only_but_not_lowered': sum(1 for row in rows
                                            if row['status'] == 'MANIFEST-ONLY'),
        'blocked': sum(1 for row in rows if row['status'] == 'BLOCKED'),
        'shape_kind_histogram': dict(sorted(shape.items())),
        'compiled': compilations,
        'rows': rows,
        'tool': {'path': str(Path(__file__).resolve().relative_to(ROOT)),
                 'sha256': hashlib.sha256(Path(__file__).resolve().read_bytes()).hexdigest(),
                 'decoder': m.gcn.VERSION if hasattr(m.gcn, 'VERSION') else None,
                 'python': sys.version.split()[0]},
        'limits': ['bindings are placeholders from synthetic_fill except where a program has no '
                   'holes',
                   'stage is the INF filename nibble, not verified semantics',
                   'no pipeline state object, no Metal pipeline, no runtime path',
                   'a compiled MSL file proves syntax and types, not that the shader is correct',
                   'G3 NOT RUN'],
    }
    target.mkdir(parents=True, exist_ok=True)
    path_out = target / 'ladder.json'
    if path_out.exists() and not overwrite:
        raise LadderError(f'{path_out} already exists; pass --overwrite')
    path_out.write_text(json.dumps(document, indent=2, sort_keys=True) + '\n')
    return document, path_out


def render(document: dict) -> list[str]:
    lines = [f"programs            : {document['programs']}",
             f"LOWERED             : {document['lowered']}"
             + (f" ({document['compiled']} compiled)" if 'compiled' in document else ''),
             f"manifest-only       : {document['manifest_only_but_not_lowered']}",
             f"blocked             : {document['blocked']}"]
    for kind, count in document['shape_kind_histogram'].items():
        lines.append(f"  blocked by {kind:<28} {count}")
    if document['counts']:
        lines.append('counters            : '
                     + ', '.join(f'{k} {v}' for k, v in document['counts'].items()))
    for row in document['rows'][:document.get('top', 10)]:
        detail = (f"holes={row['binding_holes']} shape={row['shape_kinds']}"
                  if row['status'] == 'BLOCKED' else
                  f"holes={row['binding_holes']} bindings={row.get('filled_bindings')}")
        lines.append(f"  {row['status']:<13} {row['stage']} {row['code_bytes']:>4}B "
                     f"{row['instructions']:>3}i {detail}")
    return lines


def _self_test() -> int:
    """Synthetic only: the ladder must classify a hole-free program as LOWERED."""
    checks = 0

    def check(name: str, condition: bool) -> None:
        nonlocal checks
        if not condition:
            raise AssertionError(f'self-test failed: {name}')
        checks += 1

    gcn = abi.gcn

    def encode(form: str, op: int, **values) -> bytes:
        _form, _mask, tag, shift, opmask, count = next(s for s in gcn.FORMATS if s[0] == form)
        word = tag | ((op & opmask) << shift)
        for name, low, width in gcn.FIELDS[form]:
            if name in values:
                word |= (values[name] & ((1 << width) - 1)) << low
        return word.to_bytes(count * 4, 'little')

    end = encode('SOPP', 1)
    null_ps = encode('EXP', 0, en=0, target=9, done=1, vm=1) + end
    row = lower_one(null_ps, 'ps')
    check('a hole-free null-export program lowers', row['status'] == 'LOWERED')
    check('it needs no binding', row['filled_bindings'] == 0)
    check('the sink has no export write', row['exports'] == {})
    check('the valid mask is recorded', row['valid_mask_exports'] == [9])

    blocked = encode('VOP1', 53, src0=256, vdst=4) + encode(
        'EXP', 0, en=15, target=0, done=1, vsrc0=4, vsrc1=4, vsrc2=4, vsrc3=4) + end
    row = lower_one(blocked, 'ps')
    check('an unlowered row blocks', row['status'] == 'BLOCKED')
    check('the blocker is named', row['shape_kinds'] == ['unsupported-row'])

    print(f'msl lower ladder self-test: {checks} checks passed (synthetic spans only)')
    return 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--self-test', action='store_true')
    parser.add_argument('--archives', nargs='*', type=Path, default=None)
    parser.add_argument('--sample', type=int, default=512)
    parser.add_argument('--out', type=Path,
                        default=ROOT / 'local/validation/msl-shape-ladder/latest')
    parser.add_argument('--compile', action='store_true',
                        help='compile every generated MSL with xcrun -sdk macosx metal')
    parser.add_argument('--overwrite', action='store_true')
    parser.add_argument('--top', type=int, default=10)
    args = parser.parse_args(argv)
    try:
        if args.self_test:
            return _self_test()
        archives = args.archives or [ROOT / name for name in (
            'local/extracted/CUSA08809/pak0.psarc',
            'local/extracted/CUSA08809/pak1.psarc',
            'local/extracted/CUSA08809/pakN.psarc')]
        document, path = run(archives, args.sample, args.out, args.compile, args.overwrite)
        document['top'] = args.top
        print('\n'.join(render(document)))
        print(f'wrote: {path}')
        return 0
    except (OSError, ValueError, KeyError, TypeError) as exc:
        print(f'FAILED: {exc}', file=sys.stderr)
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
