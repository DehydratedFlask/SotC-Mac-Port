# AUD-01 audio member identity claim

- Date/time: 2026-10-19 22:48 CDT (America/Chicago)
- Agent: `/root` (integrator); implementation worker: `/root/aud01_probe`
- Task ID: AUD-01, bounded audio-inventory identity checkpoint
- Files changed in this claim: `HANDOFF.md`, `docs/TASKS.md`, this checkpoint; audio source and private outputs remain untouched.
- Exact Graft command: `graft ask "AUD-01: locate code that inspects PSARC member bytes, audio header classification for .sxd/.hd/.bd, and any existing sound bank/archive readers" --source` — exit 0, approximately 7,842 estimated tokens saved.
- Exact read-only probe command (exit 0 in approximately 0.4 s; no persistent log/output file):
  ```sh
  python3 -I - <<'PY'
  import sys, json, hashlib
  from pathlib import Path
  sys.path.insert(0,'scripts')
  from psarc import Psarc
  h=json.load(open('local/manifests/assets/audio-headers.json'))
  archives=sorted({Path(r['archive']) for r in h['members']},key=str)
  print('manifest member_count',h['member_count'],'archives',len(archives))
  for p in archives:
      a=Psarc(p)
      try:
          names=a.paths(); rows=[]
          for i,n in enumerate(names,1):
              if n.lower().endswith(('.sxd','.sxd1','.sxd2','.hd','.bd')):
                  b=a.read(i)
                  rows.append((n,len(b),hashlib.sha256(b).hexdigest(),a.entries[i]['offset'],a.entries[i]['block'],i))
          print(p.name,'archive_bytes',p.stat().st_size,'members',len(rows),'audio_bytes',sum(x[1] for x in rows))
          for row in rows[:2]: print(' ',row)
      finally:a.close()
  PY
  ```
  It read 86 members totaling 3,726,760 bytes across five archives.
- Existing evidence paths: `docs/ASSETS.md`, `scripts/classify_audio_headers.py`, and `local/manifests/assets/audio-headers.json`.
- Observations: the current private manifest has header/entropy labels but no full member digest or archive-entry locator. The probe found member payloads small enough for a bounded full read.
- Inference: storing each payload SHA-256 together with its entry ID, byte offset, and first block will make these classifications reproducible without identifying their codec or semantics.
- Planned verification: `python3 -I -m unittest scripts/test_audio_headers.py`; regenerate with `python3 -I scripts/classify_audio_headers.py --out local/manifests/assets/audio-headers.json`; independently compare each recorded digest with a fresh `Psarc.read()` result.
- Remaining risks: member identities do not establish codec, NGS2 voice/rack behavior, music/dialogue coverage, or playable audio. G1 NOT PASSED; G2-G6 NOT RUN.
- Next exact action: implement only the private inventory digest/locator fields plus synthetic tests, then verify 86 rows and update docs/evidence.
- Task status: ACTIVE CLAIM; failed tries: 0.
