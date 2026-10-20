# CPU-01d `0xcdc760` vector-copy leaf claim

- Date/time: 2026-10-19 22:54 CDT (America/Chicago)
- Agent: `/root` (integrator); read-only probe: `/root/cpu01d_probe`
- Task ID: CPU-01d, one unguarded vector-copy constructor leaf
- Files changed in this claim: `HANDOFF.md`, `docs/TASKS.md`, this checkpoint; no native code was changed.
- Exact Graft commands and results:
  - `graft ask "CPU-01d: find how existing unguarded 16-byte vector-copy fixtures pin original ELF bytes, derive source and destination addresses, and map fixture entries to the private constructor survey and copy manifest" --source --in native/` — exit 0; approximately 3,420 tokens saved.
  - `graft grep "init_vec_ce7300"` — exit 0; 5 hits across startup registration, API, implementation, and fixture.
  - `graft skeleton tools/lift/check_fce7300.py` — exit 0; checker API spans identified.
- Exact candidate-evidence command (exit 0):
  ```sh
  python3 -I - <<'PY'
  import hashlib,json,struct,sys
  from pathlib import Path
  root=Path('.').resolve(); sys.path.insert(0,str(root/'scripts'))
  import ps4_dynlink as d
  rva=0xcdc760
  elf=(root/'local/re/eboot.elf').read_bytes()
  survey=json.loads((root/'local/re/work/ctor-survey.json').read_text())
  copies=json.loads((root/'local/re/ctor-vec-copies.json').read_text())['entries']
  si=next(i for i,r in enumerate(survey) if int(r['ctor'],16)==rva)
  ci=next(i for i,r in enumerate(copies) if int(r['ctor'],16)==rva)
  _,ph=d.program_headers(elf); image=d.Image(elf,ph)
  body=image.read(rva,17)
  src_disp=struct.unpack_from('<i',body,4)[0]
  dst_disp=struct.unpack_from('<i',body,12)[0]
  src=rva+8+src_disp; dst=rva+16+dst_disp
  words=struct.unpack('<4I',image.read(src,16))
  print('elf_sha256='+hashlib.sha256(elf).hexdigest())
  print(f'ctor_survey_index={si} row={json.dumps(survey[si],sort_keys=True)}')
  print(f'copy_manifest_index={ci} row={json.dumps(copies[ci],sort_keys=True)}')
  print(f'body17={body.hex(" ")}')
  print('template=c5 f8 10 05 <src disp32le> c5 f8 11 05 <dst disp32le> c3')
  print(f'source_disp_signed={src_disp} source_disp_u32=0x{struct.unpack_from("<I",body,4)[0]:08x}')
  print(f'destination_disp_signed={dst_disp} destination_disp_u32=0x{struct.unpack_from("<I",body,12)[0]:08x}')
  print(f'source=0x{src:x} destination=0x{dst:x} words={[hex(x) for x in words]}')
  print('non_target_bytes_at_disp_masked='+bytes(body[:4]+b'\x00'*4+body[8:12]+b'\x00'*4+body[16:]).hex(' '))
  PY
  ```
- Exact output facts: ELF SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; survey position 157; copy-manifest position 99; 17-byte body `c5 f8 10 05 d8 ac 61 00 c5 f8 11 05 00 55 67 02 c3`; source `0x12f7440`, destination `0x3351c70`, words `ffffffff ffffffff ffffffff 00000000`.
- Other read-only checks: `rg -n -F '0xcdc760' HANDOFF.md docs/TASKS.md docs/EVIDENCE.md native tools/lift || true` produced no hits before this claim; `git log --all --oneline --grep='cdc760' -i` and `git log --all --oneline -S'0xcdc760' -- native/core native/tests tools/lift docs HANDOFF.md` produced no output; uniqueness checks reported pair multiplicity 1 and no source/destination cell alias.
- Observation: the constructor survey describes one 17-byte load/store/ret with no direct or indirect calls; source and destination cells are unique in the copy manifest. Inference: this can be represented by a conventional native 16-byte copy fixture. No startup registration or runtime reachability is claimed.
- Proposed implementation files: `native/core/vec_cdc760.{hpp,cpp}`, `native/tests/test_vec_cdc760.cpp`, `tools/lift/check_fcdc760.py`, `native/CMakeLists.txt`, `native/README.md`.
- Planned verification: build target `test_vec_cdc760`; focused CTest `^vec_cdc760$`; run `python3 -I tools/lift/check_fcdc760.py`.
- Remaining risks: leaf fixture only; no engine chain, startup reachability, or G1 evidence. Keep original ELF private and read-only. The escalated `0xd71f60` remains untouched.
- Next exact action: implement only the native leaf, its fixture, checker, and build/docs mapping; verify before closing the task.
- Task status: ACTIVE CLAIM; failed tries: 0.
