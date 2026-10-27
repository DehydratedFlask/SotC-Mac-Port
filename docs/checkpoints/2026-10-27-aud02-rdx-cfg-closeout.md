# AUD-02 cached-CFG argument-origin task closeout

Local date/time: 2026-10-27 00:15 CDT (America/Chicago). Agent: `/root`.

Task: `AUD-02-NGS2-RACKQUERY-RDX-CFG-01`, parent `AUD-02-NGS2-RACKQUERY-BUFFERSIZE-01`.

## Live source and input verification

Root ran:

```sh
shasum -a 256 scripts/aud02_rackquery_cfg.py scripts/test_aud02_rackquery_cfg.py local/validation/aud02-root-replay/rackquery-args.json local/re/evidence/aud02-ngs2-rackquery-buffersize-01/function-0094ebc2/function-94ebc2.stdout.json local/re/eboot.elf local/validation/rdx-cfg-harden-20261025/new/strict.json local/validation/rdx-cfg-harden-20261025/new/strict2.json local/validation/rdx-cfg-harden-20261025/rv-strict.json local/validation/rdx-cfg-harden-20261025/new/rbxrsp.json > local/validation/orch-20261027/aud02-rdx-closeout/live-artifact-hashes.txt
```

It exited 0. The private output confirms that the accepted E692 source and test hashes remain
current; the baseline, cached REA output and ELF also match Task 3's pins. The three retained strict
outputs have the same hash. Root byte-compared those outputs and the two independent review outputs
with this command (exit 0):

```sh
python3 -I -c 'import hashlib,pathlib; paths=[pathlib.Path("local/validation/rdx-cfg-harden-20261025/new/strict.json"),pathlib.Path("local/validation/rdx-cfg-harden-20261025/new/strict2.json"),pathlib.Path("local/validation/rdx-cfg-harden-20261025/rv-strict.json"),pathlib.Path("local/validation/review-rdx-cfg-20261025/pres1.stdout"),pathlib.Path("local/validation/review-rdx-cfg-20261025/pres2.stdout")]; data=[p.read_bytes() for p in paths]; assert data[0]==data[1]==data[2]; assert data[3]==data[4]; print("root strict outputs byte-identical:",hashlib.sha256(data[0]).hexdigest()); print("independent review outputs byte-identical:",hashlib.sha256(data[3]).hexdigest())' > local/validation/orch-20261027/aud02-rdx-closeout/equivalence-check.txt
```

The private root logs are `local/validation/orch-20261027/aud02-rdx-closeout/live-artifact-hashes.txt`
and `equivalence-check.txt`. The existing test record
`local/validation/rdx-cfg-harden-20261025/unittest-new.log` reports 30/30 tests; tests were not
rerun for this documentation closeout.

## Accepted boundary

E692's independent review is **ACCEPT WITH NOTES**. Its M1 partial-indirect-successor handling and
M2 mid-block control-transfer gaps were fixed in the exact source/test revisions verified above.
The strict analysis covers 15 callsites, 109 blocks and 1,255 cached instructions, with all 15
callsite bodies byte-checked against the pinned ELF. Two strict-analysis outputs and both reviewer
outputs were rechecked byte-identical. The task history records two failed replay hypotheses and a
successful third replay.

Eleven strict argument-origin results remain unknown by construction of the `0x9507a2` noreturn
poison; they do not describe the actual origins at those sites. Assumed RBX/RSP preservation yields
conditional candidates with unknowns, not proved pointers or ABI. No codec, ABI, reachability,
playback, or runtime conclusion follows. Parent `AUD-02` remains ACTIVE.

Status: **STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED** for this bounded offline task. See E514/E692
for the original analysis, test and review evidence; E728 records the current live-tree verification.
