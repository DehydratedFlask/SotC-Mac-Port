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
  frames packets without interpreting them.
- Which of the 101 imports are on the boot/first-frame path is unknown until a real submission is
  captured or the lifted init chain reaches the renderer.
- The 125 symbols are the union across the whole title; per-scene usage may be far smaller.
