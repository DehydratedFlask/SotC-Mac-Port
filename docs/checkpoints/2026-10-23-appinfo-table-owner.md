# Checkpoint — the `0x15ff060..0x15ff0a0` window is a static pointer slice, not an owned table

- Local: 2026-10-23 08:0x (America/Chicago)
- Agent: static-RE worker; integrator replay of the byte facts
- Task: `CPU-01-APPINFO-TABLE-OWNER-01`
- Gate: **G1 PARTIAL — ownership question resolved; no runtime claim**
- Evidence: E562

## Question

HANDOFF next-work item 1: identify the owner of the function-pointer table
`0x15ff060..0x15ff0a0` and how `0x25a4d0` is reached through slot `0x15ff080`.

## Answer

The window is **eight consecutive `R_X86_64_RELATIVE` slots** in the read-only relocated segment
(`PT_SCE_RELRO`, vaddr `0x1594000`, file `0x1598000`), file `0x1603060..0x160309f`, sha256
`a4620329…bb7af5`:

```
{0x259b30, 0x2662b0, 0x362cb0, 0x3d6930, 0x25a4d0, 0x260e90, 0x2eee8b0, 0x24c15a0}
```

Slots 6 and 7 are **data**, not callables: `0x2eee8b0` is BSS used as a default pointer and
`0x24c15a0` is a `.data` object holding `"skel/obj_bird_A.skb"` metadata. That alone rules out a
vtable reading.

The base is never materialised anywhere in the image: 0 `lea`, 0 immediate, 0 RELA addend, and the
only qwords equal to a slot address are the eight RELA `r_offset` fields. Eleven instructions read
the window one slot at a time (nine `vmovq`, one `cmove`, one `cmp`); none writes it. It is a slice
of a larger `.data.rel.ro` pointer run (`0x15fec38..0x15ff1c0`, 177 relocated words), i.e. a
file-scope static array, not an object.

`0x2569d0` is a **consumer**, not the owner. It reads slots 0, 1, 2 and 4 and constructs the 0x60 B
registry container at `*(0x2ee68c8)`; slot 4 (`0x25a4d0`) becomes the list-3 callable
`{receiver = app object, fn = 0x25a4d0, adjust = 0}`. The dispatcher `0x1bdf30` later calls it as
`0x25a4d0(app, registry)` from the GameLoop path (`0x25c080` → `0x25d8b0` → `0x25db9e`). The
earlier "reached through slot `0x15ff080`" claim holds only for *storage*; invocation is through the
registry list, not the table.

## Consequence

HANDOFF item 1 is closed. The registry owner is constructed in the `0x2569d0` block
`0x256f98..0x257021` (allocate 0x60 → `0x1bdb10` → replace `*(0x2ee68c8)` → insert slot-4 delegate),
which is the block the native `RegistryOwner` models.

## Verification

- Private evidence: `local/re/evidence/appinfo-table-owner-15ff060/` (`report.md`, `summary.json`,
  `refs.json`, `sites.json`, `table.bin`, `reproduce.sh`).
- ELF sha256 `d0723afb…861d3`; PT_LOAD vaddr == project RVA (confirmed by the 0xf590 byte match in
  the parallel 0xf590 census).

## Unresolved

Exact array start/end and defining TU; whether slots 6/7 belong to the same array; the ownership
constructors around vtable slots `0x159c5b8` / `0x15d5bc0` / `0x15c8d18`; run-order proof that
`0x2569d0` precedes the GameLoop dispatch. **G1 PARTIAL.**
