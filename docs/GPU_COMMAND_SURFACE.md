# PS4 GPU command surface of this title (static census)

Scope: what the pinned game asks of the PS4 GPU stack, from its import table only. Source:
`local/re/imports.json` for the pinned `eboot.elf` (SHA-256 `d0723afb…61d3`). This is a **static
import census**; it does not observe a running submission, count call sites, or assign meaning to
any PM4 opcode.

Why it matters: the port must replace `libSceGnmDriver` and `libSceVideoOut` with native code that
produces Metal work. The import set fixes the **minimum service boundary**; anything absent here
cannot be required by this title.

## 1. Counts

| Library | Imported symbols |
|---|---|
| `libSceGnmDriver` | **101** |
| `libSceVideoOut` | **24** |

## 2. GnmDriver families

| Family | Count | What it forces our renderer to support |
|---|---|---|
| Draw | 11 | Indexed and auto draws, indirect draws and indirect-count-multi, multi-instanced, index-offset, opaque-auto, plus two default-hardware-state initialisers |
| Dispatch | 4 | Compute: direct, indirect, indirect-on-MEC, default compute state |
| Shader binding | 17 | **Vertex, pixel, export, local, hull, geometry and compute stages**, embedded PS/VS, GS ring sizes, program-base queries and the `Update*` variants (shader patching) |
| Tessellation / VGT | 4 | Off-chip tessellation buffer sizing, factor-ring base address, VGT control |
| Resource registration | 13 | Named resource registration, GDS resources, per-owner registration memory, user data, unregister paths |
| Workload streams | 8 | Begin/end workload, create/destroy stream, submit/flip per workload |
| Event queue / flip | 6 | EQ events, ding-dong markers, wait-flip |
| Markers | 6 | Push/pop/set/colour markers, thread-trace markers (debug/wire only) |
| Submit | 4 | `sceGnmSubmitCommandBuffers`, `…SubmitAndFlipCommandBuffers`, `…SubmitDone`, `…AreSubmitsAllowed` |
| Queries / debug | 8 | GPU block status, info status, last waited address, TCA units, owner name, protection-fault timestamp |
| Other | 20 | Compute wait-on-address/semaphore, compute queue mapping, mip-stats reporting, capture/trace flags, garlic flush, CU/TCA logical-to-physical mapping, wave-limit multipliers |

Consequences that are already decidable from this table:

- The title uses **geometry and hull shaders**. Metal has neither, so the renderer needs the
  compute-based geometry/tessellation emulation that the prior-art Metal renderer also implements.
- The title uses **indirect draws and indirect dispatch**, so the command processor must read
  draw arguments from guest memory, not only from the command stream.
- The title uses **shader `Update*` calls**, i.e. shader code can be patched after registration;
  a pipeline cache keyed only on the initial program address is insufficient.
- The title uses **workload streams**, so submission is more than one flat queue of buffers.
- Timestamp/query calls exist, but nothing here proves they are on the frame path.

## 3. VideoOut imports

`AddFlipEvent`, `AdjustColor`, `AdjustColor_`, `Close`, `ColorSettingsSetGamma`,
`ColorSettingsSetGamma_`, `ConfigureOutputMode_`, `DeleteFlipEvent`, `GetDeviceCapabilityInfo`,
`GetDeviceCapabilityInfo_`, `GetEventData`, `GetEventId`, `GetResolutionStatus`, `IsFlipPending`,
`ModeSetAny_`, `Open`, `RegisterBuffers`, `SetBufferAttribute`, `SetFlipRate`,
`SetWindowModeMargins`, `SubmitChangeBufferAttribute`, `SubmitFlip`, `UnregisterBuffers`,
`WaitVblank`.

Minimum present path for us: open a display, register N colour buffers, submit flips, wait for the
flip event, close. Gamma/adjust/margins calls exist but their use on the frame path is unproven.

## 4. Native boundary contract (what our engine must expose)

The lifted game code may call these as ordinary native functions; none may reach a Vulkan or
MoltenVK runtime.

| Contract | Shape |
|---|---|
| Command submission | `submit(words, word_count, ...)` for each command buffer in submission order; the decoder in `native/platform/pm4_decoder.*` frames it today |
| Flip | submit-and-flip pairs a submission with a display buffer and an EQ event; the host must signal completion in submission order |
| Shader registration | register/update per stage with program bytes + modifier; the renderer translates offline and keys the pipeline on content, not on address alone |
| Resource registration | named resources with base address, size and user data; unregister must drop host objects |
| Workload streams | an ordered submission scope; begin/end must not reorder submissions across streams |
| Event queue | create/add/delete event, ding-dong and wait-flip signalling consumed by guest threads |
| Markers | no-op unless tracing is enabled; must never change rendering |
| Queries | may return zero/unknown without breaking the frame path, but must be fail-visible when a lifted caller depends on the value |

## 5. Limits

- Static census only. No runtime submission was observed; no call-site counts, no frame-path proof.
- PM4 opcode and register semantics are **not** assigned here. `native/platform/pm4_decoder.*`
  frames packets without interpreting them. Section 6 grounds the *opcode values this title's own
  code writes*; it does not ground what the GPU does with them.
- Which of the 101 imports are on the boot/first-frame path is unknown until a real submission is
  captured or the lifted init chain reaches the renderer.
- The 125 symbols are the union across the whole title; per-scene usage may be far smaller.

## 6. Grounded command-packet opcodes (the title's own writers)

Section 1 counts what the title *imports*. This section records what the pinned `eboot.elf` itself
**writes**: the executable contains its own PM4 packet emitters, so part of the command stream is
built by game code and part by `libSceGnmDriver.sprx`. Native counterpart:
`native/platform/gnm_opcode_table.{hpp,cpp}`; drift check: `tools/lift/check_gnm_opcode_table.py`.
Everything below is `STATIC_ANALYSIS_VERIFIED` against `local/re/eboot.elf`
(SHA-256 `d0723afb…61d3`) — no stream was captured, nothing here is a rendering claim.

### 6.1 Reaching submission

- All 103 `libSceGnmDriver` imports have a 16-byte `jmp *GOT` thunk at `0xf903e0..0xf90a40`.
  51 of them are called directly (`e8 rel32`). The other 52 are wrapped in a second layer of
  16-byte `jmp thunk` stubs at `0x87f3b0..0x883070` (the title's internal driver table); 41 calls
  enter that layer.
- `sceGnmSubmitCommandBuffers` is called **once**: from RVA `0x8635fc` inside the function at
  `0x863310`, through the stub at `0x882ab0`. It is handed a count plus three stack arrays
  (sizes, addresses, counts) filled from a buffer object.
- The siblings are called from their own functions: `sceGnmSubmitCommandBuffersForWorkload` at
  `0x8639c7` (function `0x8636e0`), and `sceGnmSubmitAndFlipCommandBuffers` /
  `…ForWorkload` at `0x863eec` / `0x863ea7` (function `0x863af0`).
- `sceGnmSubmitDone` is called from `0xd0928`; `sceGnmDingDong` from four sites in `0x85d3xx`.

### 6.2 The emitters

293 functions in `0x865fa0..0x8852a0` write type-3 headers. They all use one growable-buffer object:

| field | role (observable) |
|---|---|
| `this+0x08` | buffer end |
| `this+0x10` | write cursor; advanced by `1 + payload dwords` per packet |
| `this+0x18` | callback invoked when `(end - cursor)` is short; `false` aborts the write |
| `this+0x20` | argument passed to that callback |
| `this+0x28` | flag word; bit 0 reaches a draw call and, at `0x86b7dd`, header bit 0 |

Representative writer (opcode `0x69`, register write): `0x86935a` stores the header, `0x869360`
stores `payload[0] = argument - 0xA000`, `0x869363` stores `payload[1] = value`, and `0x869366`
advances the cursor by `0xc` (three dwords). The `-0xA000` / `-0x2000` / `-0x4000` biases are what
the *callers* subtract from their own index argument; they are index-space offsets, not register
names.

### 6.3 The header rule, grounded

`header = 0xC0000000 | (opcode << 8) | ((payload_dwords - 1) << 16)`

Every stored header has bits `[7:1] == 0`. Bit 0 is **not** reserved: `0x86b7dd` computes
`0xC0048C00 | (this->flags & 1)` and still writes five payload dwords and advances six dwords, so
the packet is real. `payload_dwords == count + 1` is confirmed wherever the same basic block also
advances the cursor: `0x865fd8` (count 0 → 1 payload / 2 dwords), `0x86606a` (3 → 4 / 5),
`0x869b1f` (4 → 5 / 6), `0x86a776` (5 → 6 / 7), `0x86b7dd` (4 → 5 / 6). Sites that build the count
at runtime do `shll $0x10, %reg` then `orl $0xC0.., %reg` (e.g. `0x86aa5e`, `0x866113`).

### 6.4 The grounded opcode table

27 opcode values, 711 writer instructions. Payload bounds are what the writers produce; `unbounded`
means a writer patches the count from a runtime value, so only the floor is pinned.

| opcode | payload dwords | size kind | family | writer sites | anchor RVA |
|---|---|---|---|---|---|
| `0x10` | 1..63 | unbounded | inline_data | 318 | `0x8662c1` |
| `0x13` | 1 | fixed | state_word | 1 | `0x86bfe8` |
| `0x20` | 2 | fixed | state_word | 1 | `0x86e7c5` |
| `0x22` | 4 | fixed | addressed | 3 | `0x8663af` |
| `0x26` | 2 | fixed | state_word | 1 | `0x86bf8f` |
| `0x2A` | 1 | fixed | state_word | 1 | `0x86bf1c` |
| `0x2F` | 1 | fixed | state_word | 1 | `0x86c038` |
| `0x33` | 3 | fixed | addressed | 2 | `0x866648` |
| `0x34` | 5 | fixed | state_block | 2 | `0x86efbf` |
| `0x37` | 1..4 | unbounded | inline_data | 6 | `0x86644d` |
| `0x39` | 2 | fixed | state_word | 3 | `0x86fb11` |
| `0x3C` | 6 | fixed | descriptor | 7 | `0x86d040` |
| `0x3F` | 3 | fixed | state_block | 2 | `0x86fa48` |
| `0x46` | 1..3 | closed | state_block | 6 | `0x86cf16` |
| `0x47` | 5 | fixed | state_block | 3 | `0x86ebcd` |
| `0x50` | 6 | fixed | state_block | 24 | `0x86a776` |
| `0x58` | 6 | fixed | descriptor | 4 | `0x86cf9a` |
| `0x68` | 1..2 | unbounded | register_write | 4 | `0x86943a` |
| `0x69` | 1..15 | unbounded | register_write | 94 | `0x86935a` |
| `0x76` | 1..2 | unbounded | state_word | 9 | `0x8695f7` |
| `0x79` | 1..4 | unbounded | register_write | 211 | `0x86940c` |
| `0x81` | 1 | unbounded | inline_data | 2 | `0x866113` |
| `0x83` | 4 | fixed | state_block | 1 | `0x86606a` |
| `0x88` | 1 | fixed | state_word | 1 | `0x865fd8` |
| `0x8C` | 1..5 | unbounded | inline_data | 2 | `0x86b7dd` |
| `0x8D` | 3 | unbounded | state_block | 1 | `0x86b898` |
| `0x8E` | 3 | fixed | state_block | 1 | `0x86a8ca` |

`family` is structural, not semantic: `register_write` = `payload[0]` is an index the caller biased
(`0x68` by `0x2000`, `0x79` by `0x4000`, `0x69` by `0xA000`); `inline_data` = `payload[0]` is a
constant selector and `payload[1..]` is copied through `memcpy`; `addressed` = a 48-bit
address-shaped field plus a value; `descriptor` = a fixed block whose last word is a constant
selector (`0xa` at both `0x3c` and `0x58`).

### 6.5 What remains unknown

- **What any of it means to the GPU.** No opcode is named after a draw, dispatch, register block or
  synchronisation primitive. The table states shapes and values, nothing else.
- **Opcode `0x00`.** `0xC0000000` appears 274+ times in this image as an ordinary constant
  (masks, flags, stack initialisers) and is never *stored* inside the emitter region, so it is not
  in the table and a stream containing it is rejected.
- **229 of the 256 possible type-3 opcode values.** Only 27 are emitted by game code.
- **The driver's half of the stream.** Only 8 of the 711 writer sites sit in a function that calls a
  `libSceGnmDriver` import at all (`sceGnmDrawIndex` at `0x86a9d0`/`0x86aab0`,
  `sceGnmInsertThreadTraceMarker` ×2, `sceGnmLogicalTcaUnitToPhysical` ×4). Draws are therefore
  requested from the driver, not written by game code. *Inference, not proof:* at `0x86a9d0` the
  driver is handed a pointer inside the same buffer (`cursor + 0x30`) and the game then writes its
  own opcode `0x10` packet at the cursor, so the driver's packets appear to land in the stream the
  game submits. Those bytes are produced inside `libSceGnmDriver.sprx`, which this project may not
  ship and does not have. A stream capture is the only way to extend the table to those opcodes;
  until then the decoder must stay fail-visible on them.
- **Index-space names.** The `0x2000` / `0x4000` / `0xA000` biases and the constant indices seen in
  `0x69` payloads (`0x3`, `0x4`, `0x105`, `0x10C`, `0x1BA`, …) are recorded as numbers only.
- **Bit 0 of a header** is a runtime flag at exactly one writer site; its GPU meaning is unknown.

