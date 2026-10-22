# Project index recovery checkpoint

- Local date/time: 2026-10-22 07:30 CDT (America/Chicago). Integrator: /root (Codex desktop task 01a15eab-88d5-7a83-a23b-cc55dc002eb6).
- Task context: publication preparation for CPU-01-TRIG-CONSTANTS-PRESEEDED-STATE-OVERWRITE-FIXTURE-01. No source file was changed by this checkpoint.
- First commands: python3 scripts/project_index.py exited 0 and reported 61,815 folders indexed; python3 scripts/project_index.py --check exited 1 with “Stale indexes: local/indexes/folders.json”. The first generation log hash is eb9f8195459b5ab959efdb7ab7bd4a5e4d78298c9953857cd6281bc8cad45b08; the failed-check log hash is 31dad00ffd5208458ee89a329fea70b0fa13494e525ddd8a7e837c7614a2266b.
- Root cause: live scripts/project_index.py inventory() counts immediate files for every directory. Shell redirection created final-project-index-check.log after generation, changing the inventory before --check ran. This cause was confirmed from the live inventory implementation and the passing recovery sequence with all log files pre-created.
- Recovery: touch all required validation log paths first; then run python3 scripts/project_index.py and python3 scripts/project_index.py --check serially. Both exited 0 at 61,815 folders. Recovery generation/check log SHA256 eb9f8195459b5ab959efdb7ab7bd4a5e4d78298c9953857cd6281bc8cad45b08 / 168a1084e22908566be8b8fd902d661fe0dd5b39a765667c646916903e6ee714.
- trail build --check exited 0; wiring graph is in sync at 3,620 nodes / 1,240 files and its meaning tier remains unbuilt. Log SHA256 1ddb50d0af182ac35be897653619d0273d4df5a1f0d5a4db750a2dc9bf5b5bcb.
- git diff --check exited 0; output is empty (SHA256 e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855).
- Observation versus inference: the initial failure was caused by an audit log file being added between generation and check; pre-creating all logs removed the inventory race. No code or test acceptance result changed.
- Next exact action: stage only the trig test and allowlisted documentation, audit, commit, and push. Do not stage existing user-owned API/Graft paths.
- Logs: local/validation/cpu01-trig-preseed-20261022/.
