# E1035 — Init hook inventory (CPU-01-INIT-531D60-HOOK-INVENTORY-01)

Date: 2026-10-30 CDT
Source: Haiku delegate (read-only); reconciled with E1034 (CritSec + `0x499f80` body now native).

## Scope

- **36** unmodelled Init `0x531d60` hook callees from `native/core/init_531d60_*.hpp` hook fields.
- **`appmain_init_*.hpp`:** no separate hook fields.
- **Slot 309** `0xa505e0`: not in Init headers; still unregistered (characterization ACTIVE).

Body sizes are from docs/EIDs where noted; `?` = not stated in scanned docs.

## Leverage ranking (post-E1034)

1. **Config / ConVar lookup** — `0x1e4c0`, `0x1fc80`, `0x91210` (no OS contract; gates branches at `0x5328e0` / `0x5329ae`).
2. **CritSec** — **FIXTURE_VERIFIED E1034** (`critsec_3b310`, `init_499f80_body`). Inventory item closed for mutex chain; PS4 ABI remains inference-only.
3. **pthread create** — `0xa52530` → `scePthreadCreate` (E1030); slot-309 ordering still open.
4. **Save-data cluster** — `0x544fe0`, `0x54b250`, `0xa54920` (large; `0xa54920` ~1840 B).
5. **Pool insert** — `0x21cd0` vs native `insert_1beeb0` (E532 lead).

## `0x499f80` status

- Call-site: E446 (`init_531d60_post_base_499f80`).
- Body: **native** `run_init_499f80_body` (E1034); **not yet** invoked from Init slice driver (hook-only in `appmain_init_ctor_98a90_slice`).

## Doc conflicts (recorded, not resolved)

- `0x1fc80`: 0x90 B (E524) vs 143 B (alternate line).
- `0x56fad0`: staffroll ~108 B (E453) vs 1 B `ret` (E455) — inventory used E455.

## Gate

No product gate movement. G1 PARTIAL; `engine_ready=0`.
