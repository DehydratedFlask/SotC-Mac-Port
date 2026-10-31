# `lookup_1fc80` case-fold model correction — E1049 claim

- **Local time / agent:** 2026-10-31 01:29 CDT, `/root`
- **Task:** `CPU-01-LOOKUP-1FC80-CASEFOLD-MODEL-01`
- **Status:** ACTIVE; failed implementation hypotheses: 0.
- **Scope:** change only the host lookup helper's default comparator and related hook/comment naming; add case-only lookup coverage and make the existing parent byte-checker diagnostic comparator-neutral. Preserve NID-name inference and make no runtime/game claim.
- **Pinned evidence:** ELF SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; program-header 0 maps VMA `0x1fcde` to file offset 146654, bytes `e8 1d f8 f6 00`, rel32 target `0xf8f500`. Same-ELF imports manifest SHA-256 `78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744` labels NID `AV6ipCNa4Rw` at `0x1600070` as `strcasecmp`, with `name-by-nid-hash-only` status.
- **Observed sources:** E521 contract notes and E1047 comparator audit; current host model returns `std::strcmp` when the override hook is null. The Continue-A fixture injects a comparator wrapper explicitly.
- **Inference:** conventional case-insensitive behavior follows from the NID-derived imported function name; runtime binding is not established.
- **Graft:** `graft callers lookup_1fc80 --depth all` warned that duplicate names suppress some edges; exhaustive `graft grep 'strcmp_f8f500'` found the header field, implementation use, and Continue-A assignment across three files. Live files and hashes were checked after.
- **Target confirmed:** `ctest --test-dir local/build/orch-r22 -N -R 'init_continue_ab_native|vslot40_post'` lists `init_continue_ab_native` (CTest #237). There is no helper-only checker; use `python3 tools/lift/check_f533100_vslot40_post.py` to pin the helper and parent callsite.
- `python3 scripts/project_index.py` — exit 0, 44,293 folders indexed after creating this task's private log directory.
- **Next exact action:** implement the null-hook default as case-insensitive, rename `strcmp_f8f500` to `compare_f8f500`, add a table fixture whose stored name differs only by case, make the checker diagnostic comparator-neutral, then run focused build/CTest/checker and refresh source graphs.
