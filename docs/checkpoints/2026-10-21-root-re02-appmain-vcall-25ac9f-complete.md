# RE-02 app-main +0x28 callsite — bounded static path verified

- Analysis time / timezone: 2026-10-21 22:35 CDT; root review/integration: 2026-10-21 22:37 CDT
  (America/Chicago).
- Agent and integrator: worker `/root/re02_vcall_25ac9f`; shared-document integrator `/root`
  (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / gate / status: `RE-02-APPMAIN-VCALL-25AC9F-01`, bounded complete as
  `STATIC_PATH_BYTE_VERIFIED`. The runtime call edge remains unresolved. No G1 claim.
- Files changed: worker added `tools/lift/verify_appmain_vcall_25ac9f.py` and private evidence under
  `local/re/evidence/re02-appmain-vcall-25ac9f-01/`; root updated `HANDOFF.md`, `docs/TASKS.md`,
  `docs/EVIDENCE.md`, `docs/FOLDERS.md`, and this checkpoint. Existing user-owned
  `native/api/CMakeLists.txt` and `native/api/host/` remain untouched and excluded.
- Prerequisite evidence / input: parent report and verifier bind app-main `0x25a8d0..0x25accc`;
  pinned `local/re/eboot.elf` size 65,214,308, SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
- Hypothesis: the constructor's `+0x28` table word might be the callsite target. The analysis
  verifies that word and the call operand, while requiring proof that the object's current first
  word still contains the constructor vptr before classifying a runtime target.
- Worker probe command: `python3 -I
  local/re/evidence/re02-appmain-vcall-25ac9f-01/attempt-01/probe_vtable_slot.py`; exit 0;
  stdout SHA-256 `6c87d8a8b9748ac5e9af72fd05c3c2ea7b2bc0c8418ddf4309441e2a9b8b973f`, stderr empty.
- Worker verifier command: `python3 -I tools/lift/verify_appmain_vcall_25ac9f.py`; exit 0;
  stdout SHA-256 `f27559dd088cfdffe126db29e21d780ab3201dd6a31bbf7d8041e1f6f9a10e62`, stderr empty.
  Verifier source SHA-256 `c0454ec60e63604952f66fda100b99eee31934e5bf92b891b35bdae6e7c3e262`; the report
  source SHA-256 is `90df6e3b1bf0ffc9f5bf49d2be168e26ef730c3a615abb70adc849f7004d0da3`.
- Root independently ran `python3 -I tools/lift/verify_appmain_vcall_25ac9f.py`; exit 0 and stdout
  byte-matches the worker output. Root stdout path/hash:
  `local/checkpoints/round78-re02-25ac9f-root.stdout`, SHA-256
  `f27559dd088cfdffe126db29e21d780ab3201dd6a31bbf7d8041e1f6f9a10e62`; stderr path/hash
  `local/checkpoints/round78-re02-25ac9f-root.stderr`, SHA-256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`; exit file
  `local/checkpoints/round78-re02-25ac9f-root.exit` contains `0`.
- The verifier's imported `scripts/ps4_dynlink.py` helper SHA-256 is
  `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9`.
- Static observations: app-main copies incoming RDI into R15 at `0x25a8f2`; before the target call,
  it loads `[R15]` into RAX at `0x25ac99`, passes the current R15 value in RDI at `0x25ac9c`, and
  calls `[RAX+0x28]` at `0x25ac9f`. The constructor computes vtable base `0x15c8cc8`, stores the
  address point `0x15c8cd8`, and its `+0x18` word points to app-main `0x25a8d0`. A unique file-backed
  PHDR mapping at the same address point's `+0x28` slot contains little-endian `0x5365b0`; that
  address has a unique executable PT_LOAD mapping and begins with byte `0x55`.
- Observation versus inference: call bytes, register sequence and static file-table value are
  byte-bound observations. `0x5365b0` is only a conditional candidate if the runtime object still
  has constructor address point `0x15c8cd8`; prior indirect/direct calls pass R15, and their effects
  on the object's first word are not established. The dynamic edge therefore remains `UNRESOLVED`.
  No function semantics, class identity, ABI contract, reachability, runtime behavior, or game
  behavior is inferred. No REA/Ghidra query was used by this worker.
- Attempt count: one bounded analysis, 0 failed hypotheses. A first shell capture wrapper used
  zsh's reserved `status` variable; rerun with `rc` captured the same output and exit 0. This was a
  wrapper correction, not an analysis failure.
- Remaining risk: resolving the dynamic target requires proving the object's first word after the
  preceding calls. Do not carry the conditional table word into a native lift or runtime claim.
- Index/graph checks: `python3 scripts/project_index.py` and then
  `python3 scripts/project_index.py --check` both exited 0; current count is 19,273 indexed folders.
  The first `trail build --check` exited 1 and reported the newly added
  `tools/lift/verify_appmain_vcall_25aa06.py` and its symbols stale. `trail build` then exited 0;
  the following `trail build --check` exited 0 with 3,597 nodes, 4,911 edges and 945 cards. The
  Graft cache remains ignored. Logs: stale check `local/checkpoints/round78-re02-25ac9f-complete-index-graph.log`
  SHA-256 `100d8b2aacaf78067470b69749412997e1f3719777a83bee9f519111cb6f289f`; rebuild/recheck
  `local/checkpoints/round78-re02-graph-build.log` SHA-256
  `c02c9197b3a7c78542015a16068491d7de85b6996c79eff4c279b13f17e2c12e`.
- Publication preflight: `git diff --cached --check` passed. `python3 -I
  scripts/audit_publish_paths.py` accepted 8 staged paths; `python3 -I
  scripts/audit_publish_paths.py --all` accepted 1,664 paths. Logs:
  `local/checkpoints/round78-re02-complete-staged-audit.log` SHA-256
  `bd9c35eacde393e8471b178bf72f93b286cfd038b19b580c333b2a8decfc50e3` and
  `local/checkpoints/round78-re02-complete-full-audit.log` SHA-256
  `1371b3c20d1f58dea0147dfe614384c7e97bf5fc8930fcf9e7098e1c135e1033`. Only the task's
  allowlisted verifier and shared documentation are staged; the user-owned native CMake/host files
  and the distinct in-progress `0x25aa06` verifier remain unstaged.
- Next exact action: no work remains on this bounded static task. Any dynamic-vptr investigation is
  a new task with a separate claim and evidence boundary; root controls all further REA use.
- Task status: `STATIC_PATH_BYTE_VERIFIED`; G0 remains PARTIAL and G1–G6 remain NOT RUN.

## Completion publication checkpoint

- Time / timezone: 2026-10-21 22:40 CDT (America/Chicago); integrator `/root`.
- Files published: the bounded verifier, E237, task status, and the claim/completion/REA-timeout
  handoff records. The separate, in-progress `0x25aa06` verifier and user-owned native API files
  were not staged.
- Exact preflight: `git diff --cached --check` passed;
  `python3 -I scripts/audit_publish_paths.py` accepted 8 staged paths;
  `python3 -I scripts/audit_publish_paths.py --all` accepted 1,664 paths.
- Commit/push: `git commit -m "trace unresolved app-main +0x28 callsite"` created
  `ed15ff09811a51f5942dbd549404ddaf2c7af43e`; `git push origin main` succeeded.
  Private post-push log `local/checkpoints/round78-re02-25ac9f-complete-postpush.log` SHA-256
  `b14a8432a42335a7eae9464f39285a0aa7f1f19d026d410178d5b0784284436b`.
- Fetch verification: `git fetch origin`; `git rev-parse HEAD`; `git rev-parse origin/main`;
  `git rev-list --left-right --count HEAD...origin/main`. Result: matching tip
  `ed15ff09811a51f5942dbd549404ddaf2c7af43e`, divergence `0 0`.
- Audit logs: staged `local/checkpoints/round78-re02-complete-staged-audit.log` SHA-256
  `bd9c35eacde393e8471b178bf72f93b286cfd038b19b580c333b2a8decfc50e3`; full
  `local/checkpoints/round78-re02-complete-full-audit.log` SHA-256
  `1371b3c20d1f58dea0147dfe614384c7e97bf5fc8930fcf9e7098e1c135e1033`.
- Observation versus inference: the commit is on the public remote and fetch-verified. This
  publication does not resolve the runtime target or advance a game acceptance gate.
- Remaining work: integrate and independently replay the separate `0x25aa06` bounded result; leave
  the runtime edge unresolved unless the object/vptr is byte-proven.
