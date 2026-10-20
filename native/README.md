# native/ — recovered game code and platform services (CPU-01 onward)

Contract: ordinary ARM64 C++ only. No guest registers, flags struct, dispatch loop, embedded
original bytes or executable pointers. Original addresses appear only in comments and in the
private `local/re/functions.jsonl` records (ELF sha256 d0723afb...c25861d3).

| Path | Contents |
|---|---|
| `core/math_constants.{hpp,cpp}` | Native form of original RVA 0xf6cad0 (first global constructor): guarded float/mask constants |
| `core/scratch_reset.{hpp,cpp}` | Native form of original RVA 0x125990 (ctor index 538): memset 8 KiB + seq_cst zero store; first recovered function with a service call |
| `core/trig_constants.{hpp,cpp}`, `core/libm_policy.hpp` | Native form of original RVA 0xe10490 (ctor index 48): vector copy + `_FSin` result; libm policy L1 (double eval, round once, <=1 ULP fixtures, no bit-exactness claim) |
| `core/normal_threshold.hpp` | Fragment of original RVA 0xe0e140: unfused 3-float dot product > threshold (consumer of the cos 3deg constant); built with `-ffp-contract=off` |
| `core/addr.{hpp,cpp}` | CPU-02: `Tagged31` (bit31 = heap-relative), `HeapWindow`, checked `decode`, `tag_if_in_heap`, `Gpu40`; see docs/POINTERS.md |
| `core/allocator_region.{hpp,cpp}` | CPU-02: contract for RVA `0x19ae0` — request args, `R[+0x58]/[+0x60]` layout, alignment helpers; not a full allocator lift |
| `core/host_map.{hpp,cpp}` | SRV-01 mem: Darwin anon mmap behind `0x19ae0` preferred_base==0 path; ties `check_host_map_calls.py` (ReserveVirtual/MapFlexible sites) |
| `core/free_list_carve.{hpp,cpp}` | SRV-01 mem: pure `0x1a800` carve-fit math (lead/align/page budget); not full free-list/bitmap |
| `core/region_split.{hpp,cpp}` | SRV-01 mem: OBS split surgery `0x1a8b6..0x1a8fa` after node pop; `0x19660` replenish OPEN |
| `core/vec_neg036.{hpp,cpp}` | Native form of original RVA `0xf4b510` (ctor index 7): guarded splat of `0xbeb851ea` (~−0.36f)×4 |
| `core/xyz_mask_copy.{hpp,cpp}` | Native form of original RVA `0xf30460` (ctor index 9): unguarded `{~0,~0,~0,0}` copy to its own BSS cell |
| `core/clear_flag32.{hpp,cpp}` | Native form of original RVA `0x196c90` (ctor index 520): seq_cst clear of a u32 cell via `xchg` |
| `core/startup_ctors.{hpp,cpp}` | Replacement for the CRT ctor runner: runs recovered slots in original order and blocks visibly (no stub success) at an unrecovered one |
| `tests/test_math_constants.cpp` | Independent IEEE oracles + guard-semantics checks; `--dump` mode feeds the binary tie check |

Build and test (ARM64, ASan+UBSan):

    cmake -S native -B local/build/native -G Ninja && cmake --build local/build/native && ctest --test-dir local/build/native --output-on-failure

Binary-vs-native ties (private, need local/re/eboot.elf): `python3 -I tools/lift/check_f6cad0.py` and `check_f125990.py`, `check_fe10490.py`, `check_fe0e140.py`, `check_f66dec0.py`, `check_heap_window.py`, `check_f19ae0.py`, `check_ff4b510.py`, `check_ff30460.py`, `check_f196c90.py`.
Threading check (not in ctest): `clang++ -std=c++20 -fsanitize=thread -Inative native/core/scratch_reset.cpp native/tests/test_scratch_reset.cpp`.
Status: FIXTURE only (Drive registry: 6 of 573 ctors; allocator region contract typed; peer may add more). Not wired to any engine startup; G1 is not passed.
