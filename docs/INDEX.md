# Project documentation index

Workspace: `/Volumes/Extra/SoC Mac Native`. Start with [HANDOFF](../HANDOFF.md),
[AGENTS](../AGENTS.md), and [ROADMAP](../ROADMAP.md). The live files are implementation truth.

## Authoritative navigation

| Document | Purpose / owner |
|---|---|
| [TASKS](TASKS.md) | Dependency-ordered task register and detailed work packets; integrator |
| [EVIDENCE](EVIDENCE.md) | Verified artifact/source/tool facts and unresolved questions; integrator |
| [POINTERS](POINTERS.md) | Address classes, tagged 31-bit fields, evidence labels (CPU-02 partial); integrator |
| [ASSETS](ASSETS.md) | PSARC archive findings and audio-asset leads (DATA-02/AUD-01 partial); integrator |
| [GPU01_PREP](GPU01_PREP.md) | Offline `.csdr`/`.cmat` member-name inventory from manifests (GPU-01 prep); GPU worker |
| [CTOR_CLASSES](CTOR_CLASSES.md) | 573-entry constructor-table classification by reachable-body idiom plus the slot 10–19/24–47 family contract; CPU-01 |
| [INPUTTABLE_551140](INPUTTABLE_551140.md) | Numeric-only deepen of Init-hash 275-entry table (`0x551140`); CPU-01 |
| [INPUTTABLE_551140_NAMES_FIXTURE](INPUTTABLE_551140_NAMES_FIXTURE.md) | Private 275-name fixture recipe for registry fill `0x52a220`; CPU-01 |
| [EXTRACTION](EXTRACTION.md) | Repeatable native extractor, inputs, SELF reconstruction; artifact worker |
| [REVERSE_ENGINEERING](REVERSE_ENGINEERING.md) | Ghidra procedure, symbol manifests, boot/audio/save recovery; RE worker |
| [REFERENCE_AUDIT](REFERENCE_AUDIT.md) | Pinned SoTC/Bloodborne/shadPS4 source anchors; source-audit worker |
| [PLATFORM](PLATFORM.md) | Native architecture and subsystem requirements; native-core owner |
| [VALIDATION](VALIDATION.md) | Measurable gates, failures, real-game proof; validation owner |
| [G1 PARTIAL pack (2026-10-22)](checkpoints/2026-10-22-g1-partial-evidence-pack.md) | Phase C evidence; G1 PARTIAL not PASSED; E404 |
| [App-main Init registry slice (2026-10-22)](checkpoints/2026-10-22-appmain-init-registry-slice.md) | Measured Init-hash edge via tip AllocatorFront; PR5 merge gap; E405; G1 PARTIAL |
| [Init trivial remainder leaves (2026-10-22)](checkpoints/2026-10-22-init-531d60-trivial-leaves.md) | Init remainder beyond fragment; E406; G1 PARTIAL |
| [Base Initialize 0x255130 (2026-10-22)](checkpoints/2026-10-22-base-initialize-255130.md) | Orchestrator + 0x357f0 leaf; E407; G1 PARTIAL |
| [Prologue 0x255180 early (2026-10-22)](checkpoints/2026-10-22-prologue-255180-early.md) | Early fragment + 0x335a0; E408; G1 PARTIAL |
| [Prologue 0x255180 tail (2026-10-22)](checkpoints/2026-10-22-prologue-255180-tail.md) | 0x98a90 / app+0x3b58/+0x3b40; E409; G1 PARTIAL |
| [Ctor 0x3be050 header (2026-10-22)](checkpoints/2026-10-22-ctor-98a90-3be050.md) | Nested 0x48 + hooks; E410; G1 PARTIAL |
| [Ctor 0x3be050 buf grow (2026-10-22)](checkpoints/2026-10-22-ctor-98a90-3be050-bufgrow.md) | Empty-path 0x80 vec grow; E411; G1 PARTIAL |
| [Ctor 0x3be050 listwalk (2026-10-22)](checkpoints/2026-10-22-ctor-98a90-3be050-listwalk.md) | Null-head cells 0x2ef63f8/0x2ef6400; E412; G1 PARTIAL |
| [TSC leaf 0x323d0 (2026-10-22)](checkpoints/2026-10-22-store-tsc-323d0.md) | Host monotonic tsc; E413; G1 PARTIAL |
| [Ensure 0x3c2f70 (2026-10-22)](checkpoints/2026-10-22-ensure-3c2f70.md) | Empty-path 0xa800 ensure; E414; G1 PARTIAL |
| [Sub-init 0x3be930 (2026-10-22)](checkpoints/2026-10-22-init-sub-3be930.md) | 0x98-stride banks; E415; G1 PARTIAL |
| [Ctor 0x1ab550 0x40 (2026-10-22)](checkpoints/2026-10-22-ctor-40-1ab550.md) | Prologue-tail 0x40 ctor + base; E416; G1 PARTIAL |
| [Prologue gates 0x2552d4 (2026-10-22)](checkpoints/2026-10-22-prologue-gates-2552d4.md) | Debug gates + empty ba40; E417; G1 PARTIAL |
| [Vslot 0x534130 (2026-10-22)](checkpoints/2026-10-22-vslot-30-534130.md) | Base Init +0x30 bool gate; E418; G1 PARTIAL |
| [Mid 0x255340 (2026-10-22)](checkpoints/2026-10-22-mid-255340.md) | Base Init debug-menu mid; E419; G1 PARTIAL |
| [Singleton 0xf08 (2026-10-22)](checkpoints/2026-10-22-singleton-f08-531d84.md) | Init 0xf08 singleton install; E420; G1 PARTIAL |
| [Vslot 0x533100 header (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-header.md) | Bounded Base Init vslot+0x40 header; E421; G1 PARTIAL |
| [Vslot 0x533100 post (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-post.md) | Post-header + 0x1fc80 show_areaInfo; E422; G1 PARTIAL |
| [Vslot 0x533100 miss (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-miss.md) | Miss path r_fps after show_areaInfo; E423; G1 PARTIAL |
| [Vslot 0x533100 join (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-join.md) | Join+ Position at self+0x3b60; E424; G1 PARTIAL |
| [Vslot 0x533100 gfx (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-gfx.md) | Post-join gfx-cell 0x2ee2d00 prelude; E425; G1 PARTIAL |
| [Vslot 0x533100 shadow (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-shadow.md) | Shadowmodels load-attempt; E426; G1 PARTIAL |
| [Vslot 0x533100 shadow attach (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-shadow-attach.md) | Shadowmodels attach fail-path + install; E427; G1 PARTIAL |
| [Vslot 0x533100 shadowskels (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-shadowskels.md) | Shadowskels load-attempt; E428; G1 PARTIAL |
| [Vslot 0x533100 skels attach (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-skels-attach.md) | Shadowskels attach fail-path + install f08+0x10; E429; G1 PARTIAL |
| [Vslot 0x533100 shadowanims (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-shadowanims.md) | Shadowanims load-attempt; E430; G1 PARTIAL |
| [Vslot 0x533100 anims attach (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-anims-attach.md) | Shadowanims attach fail-path + install f08+0x20; E431; G1 PARTIAL |
| [Vslot 0x533100 shadowbanks (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-shadowbanks.md) | Shadowbanks load-attempt; E432; G1 PARTIAL |
| [Vslot 0x533100 banks attach (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-banks-attach.md) | Shadowbanks attach fail-path + install f08+0x40; E433; G1 PARTIAL |
| [Vslot 0x533100 shadowstreams (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-shadowstreams.md) | Shadowstreams load-attempt; E434; G1 PARTIAL |
| [Vslot 0x533100 streams attach (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-streams-attach.md) | Shadowstreams attach fail-path + install f08+0x50; E435; G1 PARTIAL |
| [Vslot 0x533100 shadowrandomevents (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-shadowrandomevents.md) | Shadowrandomevents load-attempt; E436; G1 PARTIAL |
| [Vslot 0x533100 randomevents attach (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-randomevents-attach.md) | Shadowrandomevents attach fail-path + install f08+0x60; E437; G1 PARTIAL |
| [Vslot 0x533100 shadowbossmusic (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-shadowbossmusic.md) | Shadowbossmusic load-attempt; E438; G1 PARTIAL |
| [Vslot 0x533100 bossmusic attach (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-bossmusic-attach.md) | Shadowbossmusic attach fail-path + install f08+0x70; E439; G1 PARTIAL |
| [Vslot 0x533100 cp3Characters (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-cp3characters.md) | cp3Characters.calt load-attempt; E440; G1 PARTIAL |
| [Vslot 0x533100 cp3Characters attach (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-cp3characters-attach.md) | cp3Characters attach fail-path + install self+0x3cb8; E441; G1 PARTIAL |
| [Vslot 0x533100 cp3shadowprecache (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-cp3shadowprecache.md) | cp3shadowprecache.calt load-attempt; E442; G1 PARTIAL |
| [Vslot 0x533100 cp3shadowprecache attach (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-cp3shadowprecache-attach.md) | cp3shadowprecache attach fail-path + install self+0x3cc8; E443; G1 PARTIAL |
| [Vslot 0x533100 tail 0x140d90 (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-tail-140d90.md) | Tail 0x140d90(self+0x3cc8,9)+f590 call-site; E444; G1 PARTIAL |
| [Vslot 0x533100 stackchk (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-stackchk.md) | Stack-check success epilogue through ret; E445; G1 PARTIAL |
| [Init post-base 0x499f80 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-499f80.md) | Init continue 0x499f80(self+0x3c70) call-site; E446; G1 PARTIAL |
| [Init continue 0x56dcb0 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-56dcb0.md) | Init continue 0x56dcb0 call-site; E447; G1 PARTIAL |
| [Init continue 0x5c64c0 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-5c64c0.md) | Init continue 0x5c64c0 call-site; E448; G1 PARTIAL |
| [Init continue 0x1290 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-1290.md) | Init continue 0x1290 call-site; E449; G1 PARTIAL |
| [Init continue 0x56db70 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-56db70.md) | Init continue 0x56db70 call-site (ret-nop); E450; G1 PARTIAL |
| [Init continue 0x5b3d00 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-5b3d00.md) | Init continue 0x5b3d00 call-site (ret-nop); E451; G1 PARTIAL |
| [Init continue 0x5b3cf0 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-5b3cf0.md) | Init continue 0x5b3cf0 call-site (ret-nop); E452; G1 PARTIAL |
| [Init continue AVX BSS (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-avx-bss.md) | Init continue AVX BSS copy (0x1b1dd40→dsts); E453; G1 PARTIAL |
| [Init continue 0x7f7220 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-7f7220.md) | Init continue 0x7f7220 call-site (staffroll helper); E454; G1 PARTIAL |
| [Init continue 0x56fad0 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-56fad0.md) | Init continue 0x56fad0 call-site (ret-nop); E455; G1 PARTIAL |
| [Init continue 0xf90310 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f90310.md) | Init continue 0xf90310 call-site (splash-hide PLT); E456; G1 PARTIAL |
| [Init continue ymm BSS (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-ymm-bss.md) | Init continue ymm BSS seed (0x1014a80→BSS); E457; G1 PARTIAL |
| [Init continue ymm inter-cell (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-ymm-intercell.md) | Init continue ymm inter-cell BSS copy; E458; G1 PARTIAL |
| [Init continue xmm seed (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-xmm-seed.md) | Init continue xmm BSS seed (0x1014be0→BSS); E459; G1 PARTIAL |
| [Init continue ymm reload (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-ymm-reload.md) | Init continue ymm reload (0x160430c→0x160433c); E460; G1 PARTIAL |
| [Init continue 0x26800/0xf590 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-26800-f590.md) | Init continue 0x26800/0xf590 call-sites; E461; G1 PARTIAL |
| [Init continue Shadow Boss Jump (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-sbj.md) | Init continue Shadow Boss Jump call-sites; E462; G1 PARTIAL |
| [Init continue Shadow Boss Outro Jump (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-sboj.md) | Init continue Shadow Boss Outro Jump call-sites; E463; G1 PARTIAL |
| [Init continue 0x52eb50 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-52eb50.md) | Init continue 0x52eb50 call-site; E464; G1 PARTIAL |
| [Init continue lea rbx / vmovq (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-lea-vmovq.md) | Init continue lea rbx / vmovq seed; E465; G1 PARTIAL |
| [Init continue 0x532ca0 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-532ca0.md) | Init continue 0x532ca0 call-site; E466; G1 PARTIAL |
| [Init continue virtual-call +0x18 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-vcall18.md) | Init continue virtual-call [rax+0x18]; E467; G1 PARTIAL |
| [Init continue second 0x532ca0 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-532ca0-u2.md) | Init continue second 0x532ca0 call-site; E468; G1 PARTIAL |
| [Init continue second virtual-call +0x18 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-vcall18-u2.md) | Init continue second virtual-call [rax+0x18]; E469; G1 PARTIAL |
| [Init continue third 0x532ca0 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-532ca0-u3.md) | Init continue third 0x532ca0 call-site; E470; G1 PARTIAL |
| [Init continue third virtual-call +0x18 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-vcall18-u3.md) | Init continue third virtual-call [rax+0x18]; E471; G1 PARTIAL |
| [Init continue fourth 0x532ca0 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-532ca0-u4.md) | Init continue fourth 0x532ca0 call-site; E472; G1 PARTIAL |
| [Init continue fourth virtual-call +0x18 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-vcall18-u4.md) | Init continue fourth virtual-call [rax+0x18]; E473; G1 PARTIAL |
| [Init continue fifth 0x532ca0 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-532ca0-u5.md) | Init continue fifth 0x532ca0 call-site; E474; G1 PARTIAL |
| [Init continue fifth virtual-call +0x18 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-vcall18-u5.md) | Init continue fifth virtual-call [rax+0x18]; E475; G1 PARTIAL |
| [Init continue first 0x532ed0 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-532ed0.md) | Init continue first 0x532ed0 call-site; E476; G1 PARTIAL |
| [Init continue first 0x532ed0-series vcall18 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-vcall18-532ed0.md) | Init continue first 0x532ed0-series virtual-call [rax+0x18]; E477; G1 PARTIAL |
| [Init continue second 0x532ed0 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-532ed0-u2.md) | Init continue second 0x532ed0 call-site; E478; G1 PARTIAL |
| [Init continue second 0x532ed0-series vcall18 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-vcall18-532ed0-u2.md) | Init continue second 0x532ed0-series virtual-call [rax+0x18]; E479; G1 PARTIAL |
| [Init continue first 0x17070 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-17070.md) | Init continue first 0x17070 call-site; E480; G1 PARTIAL |
| [Init continue first 0x544fe0 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-544fe0.md) | Init continue first 0x544fe0 call-site; E481; G1 PARTIAL |
| [Init continue cell-install 0x2eff450 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-cell-2eff450.md) | Init continue cell-install 0x2eff450; E482; G1 PARTIAL |
| [Init continue second 0x17070 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-17070-u2.md) | Init continue second 0x17070 call-site; E483; G1 PARTIAL |
| [Init continue first 0x53fd10 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-53fd10.md) | Init continue first 0x53fd10 call-site; E484; G1 PARTIAL |
| [Init continue cell-install 0x2eff448 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-cell-2eff448.md) | Init continue cell-install 0x2eff448; E485; G1 PARTIAL |
| [Init continue vcall [rax+0x38] (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-vcall38.md) | Init continue vcall [rax+0x38]; E486; G1 PARTIAL |
| [Init continue lea r13 pin 0x32f4fe0 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-lea-32f4fe0.md) | Init continue lea r13 pin 0x32f4fe0; E487; G1 PARTIAL |
| [Init continue first 0xa2caf0 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-a2caf0.md) | Init continue first 0xa2caf0 call-site; E488; G1 PARTIAL |
| [Init continue first 0xa2ed00 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-a2ed00.md) | Init continue first 0xa2ed00 call-site; E489; G1 PARTIAL |
| [Init continue post-a2ed00 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00.md) | Init continue post-a2ed00 0xf590 call-site; E490; G1 PARTIAL |
| [Init continue second post-a2ed00 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u2.md) | Init continue second post-a2ed00 0xf590 call-site; E491; G1 PARTIAL |
| [Init continue second 0xa2ed00 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-a2ed00-u2.md) | Init continue second 0xa2ed00 call-site; E492; G1 PARTIAL |
| [Init continue first post-a2ed00-u2 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u3.md) | Init continue first post-a2ed00-u2 0xf590 call-site; E493; G1 PARTIAL |
| [Init continue second post-a2ed00-u2 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u4.md) | Init continue second post-a2ed00-u2 0xf590 call-site; E494; G1 PARTIAL |
| [Init continue third 0xa2ed00 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-a2ed00-u3.md) | Init continue third 0xa2ed00 call-site; E495; G1 PARTIAL |
| [Init continue first post-a2ed00-u3 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u5.md) | Init continue first post-a2ed00-u3 0xf590 call-site; E496; G1 PARTIAL |
| [Init continue second post-a2ed00-u3 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u6.md) | Init continue second post-a2ed00-u3 0xf590 call-site; E497; G1 PARTIAL |
| [Init continue fourth 0xa2ed00 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-a2ed00-u4.md) | Init continue fourth 0xa2ed00 call-site; E498; G1 PARTIAL |
| [Init continue first post-a2ed00-u4 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u7.md) | Init continue first post-a2ed00-u4 0xf590 call-site; E499; G1 PARTIAL |
| [Init continue second post-a2ed00-u4 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u8.md) | Init continue second post-a2ed00-u4 0xf590 call-site; E500; G1 PARTIAL |
| [Init continue fifth 0xa2ed00 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-a2ed00-u5.md) | Init continue fifth 0xa2ed00 call-site; E501; G1 PARTIAL |
| [Init continue first post-a2ed00-u5 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u9.md) | Init continue first post-a2ed00-u5 0xf590 call-site; E502; G1 PARTIAL |
| [Init continue second post-a2ed00-u5 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u10.md) | Init continue second post-a2ed00-u5 0xf590 call-site; E503; G1 PARTIAL |
| [Init continue sixth 0xa2ed00 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-a2ed00-u6.md) | Init continue sixth 0xa2ed00 call-site; E504; G1 PARTIAL |
| [Init continue first post-a2ed00-u6 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u11.md) | Init continue first post-a2ed00-u6 0xf590 call-site; E505; G1 PARTIAL |
| [Init continue second post-a2ed00-u6 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u12.md) | Init continue second post-a2ed00-u6 0xf590 call-site; E506; G1 PARTIAL |
| [Init continue seventh 0xa2ed00 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-a2ed00-u7.md) | Init continue seventh 0xa2ed00 call-site; E507; G1 PARTIAL |
| [Init continue first post-a2ed00-u7 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u13.md) | Init continue first post-a2ed00-u7 0xf590 call-site; E508; G1 PARTIAL |
| [FOLDERS](FOLDERS.md) | Generated public/source folder inventory; `scripts/project_index.py` |
| [INSTALL_LOG](../INSTALL_LOG.md) | Existing and newly installed tools, downloads and removals |
| [Checkpoint template](checkpoints/TEMPLATE.md) | Required record at each checkpoint |
| [Escalation template](escalations/TEMPLATE.md) | Three-try handback to principal |
| [Function schema](schemas/function.example.json) | Address-to-native-symbol recovery record |

## Current parallel work

- [Parallel foundations plan](checkpoints/2026-10-22-codex-parallel-foundations-plan.md)
- [Prepared contributor cleanup](checkpoints/2026-10-22-codex-contributor-cleanup-prepared.md)

## Folder contracts

| Folder | What belongs here | Index / protection |
|---|---|---|
| Root | Agent rules, current handoff, roadmap, README, dependency log | This index; root initially has no `.git` |
| `docs/` | Reviewable plans/evidence summaries, task status, schemas, checkpoint/escalation records | This file + generated FOLDERS |
| `scripts/` | Small stdlib project/analysis helpers; never production game code | FOLDERS + source docstrings; tests |
| `native/` | Recovered ARM64 C++ (core/), tests/, README with build/test command; no original bytes or guest state | FOLDERS + native/README; Graft scope added |
| `launcher/` | SwiftPM SwiftUI launcher shell (LauncherCore + app + tests); builds go to local/build/launcher | launcher/README; Graft scope added |
| `tools/` | Reproducible tool build adapters and analysis recipes | FOLDERS + INSTALL_LOG |
| `reference/ShadowOfTheColossus-PC/` | SoTC PC source, unchanged pinned checkout | Graft; REFERENCE_AUDIT |
| `reference/bloodborne_pc/` | Bloodborne performance/architecture reference, unchanged | Graft; REFERENCE_AUDIT |
| `reference/shadPS4/` | Service/GPU semantics reference, unchanged; its AGENTS applies there | Graft; REFERENCE_AUDIT |
| `reference/shadPS4Plus-pkg/` | Standalone extractor source, unchanged | Graft + EXTRACTION |
| `Shadow of the Colossus/` | Original base/update/DLC PKGs, metadata, images; READ ONLY | Private inventory; excluded from publication |
| `local/` | Extracted copyrighted data, reconstructed ELF, raw RE evidence, Ghidra state, builds | `local/indexes/folders.json`; excluded |
| `logs/` | Raw install/build/extraction/verification logs | INSTALL_LOG + evidence links; excluded |
| `graft/` | Regenerable source graph; not canonical documentation | `graft check`; excluded |

Every existing subfolder is listed in generated indexes. Build/cache internals and `.git`
administration and Finder metadata are deliberately summarized, not treated as source documentation. Original
game/private analysis folders have a local-only inventory. Do not hand-author thousands of
duplicate source summaries; Graft is the source-level navigation layer.

## Source identities

| Source | Commit | Role |
|---|---|---|
| [AjncSystem/ShadowOfTheColossus-PC](https://github.com/AjncSystem/ShadowOfTheColossus-PC/tree/70bb43859cd7ff181a6d409540b757866184d60d) | `70bb43859cd7ff181a6d409540b757866184d60d` | Main behavior reference |
| [Supermedo/bloodborne_pc](https://github.com/Supermedo/bloodborne_pc/tree/de2e38af055f09f819946951075c6bb119e62ae6) | `de2e38af055f09f819946951075c6bb119e62ae6` | Optimization methodology; inspect subtree licenses |
| [shadps4-emu/shadPS4](https://github.com/shadps4-emu/shadPS4/tree/0ebea15c24253b60f514fc37a348dabc8d0bcb90) | `0ebea15c24253b60f514fc37a348dabc8d0bcb90` | PS4 interface/format reference |
| [AzaharPlus/shadPS4Plus](https://github.com/AzaharPlus/shadPS4Plus/tree/771a8d58aff540448c7cbace7c40a66a5daf010d) | `771a8d58aff540448c7cbace7c40a66a5daf010d` | PKG_EXTRACTOR_1_1 extraction source |

Verify with `git -C reference/<name> rev-parse HEAD` and `git -C reference/<name> status --short`.
Do not fetch or update a pin silently. Uninitialized submodules are not installed dependencies.
Pinned source URLs support auditability; live upstream pages are separately dated evidence.

## Source / binary indexing workflow

1. Source question: `graft ask "specific question" --source --in reference/ShadowOfTheColossus-PC/`.
2. Need all uses: `graft grep "symbol" --in reference/ShadowOfTheColossus-PC/`.
3. Before changes: `graft callers Symbol --depth all`; respect all connected platform files.
4. Ghidra finding: append its original RVA, file offset, artifact SHA, evidence ID, native
   symbol/path, confidence and unresolved edges to the local function manifest.
5. New/moved folders or handoff: `python3 scripts/project_index.py`; then `graft check`.
6. If Graft is stale: rebuild and report status. Current persisted scope recipe is
   `graft build --follow-nested-repos --only-dir scripts --only-dir tools --only-dir native --only-dir launcher --only-dir reference`.
   When first creating native/launcher/tests, append their scopes while retaining all current
   ones. `graft check` only checks configured scopes; prove new files queryable with skeleton/grep.
   Never index extracted binaries, original game archives or generated pseudocode as source.

Future implementation directories (`native/`, `launcher/`, `tests/`) are proposed, not created
as fake completed components. Their first task must add a folder contract and update the indexes.

## Skills/tools to use for each workstream

| Workstream | Available skill / tool | Boundary |
|---|---|---|
| Binary recovery | `reverse-engineer-anything`, REA Ghidra provider / headless Ghidra | One shared MCP binary-session owner; export evidence; static is not runtime proof |
| Source exploration | `graft` CLI / graph tools | Query before source search; map callers before changes; private binary index stays separate |
| Native core/tool utilities | `ponytail`, Apple Clang/CMake/Ninja | Reuse stdlib/native facilities, keep scripts bounded; no guest-state shortcuts |
| Swift launcher | `macos-design-guidelines`, `swiftui-pro:swiftui-pro`, Xcode/Swift tools | Read skills at implementation time; native macOS lifecycle, not an iOS simulator test |
| Native UI validation | `computer-use` by main agent, native REA observations, Xcode/Instruments | Actual native app/Finder behavior; browser-only testing cannot certify Mac UI |
| Independent bounded work | Collaboration subagents / existing agent workflows | Separate ownership, one HANDOFF integrator, preserved failed-try counts |
| Shared project knowledge | `@wiki`, config-resolved Projects topic | Read index first; durable promotion only when explicitly requested |

Other tools may be installed only when a concrete task needs them. Record version/source/path,
command, architecture, purpose and removal boundary in INSTALL_LOG, then verify the live result.
Do not install a Sony SDK, an emulator or a second analysis engine just to avoid the native task.
| [Init 0x1290 body (2026-10-22)](checkpoints/2026-10-22-init-1290-body-analysis.md) | 195 B full body: guard read-only, `void`, one no-op vcall, two descriptor registrations; E509; G1 PARTIAL |
| [Init continue batch A (2026-10-22)](checkpoints/2026-10-22-init-531d60-continue-batch-a.md) | 22 units / 95 B `0x532720..0x53277f`, 5 calls + 3 lea + `je` pinned; E512; G1 PARTIAL |
| [Repo-health round (2026-10-22)](checkpoints/2026-10-22-repo-health-round.md) | Full suite 326/326 after stale ctor-test fix; publish-audit final-component gap fixed; E510/E511 |
| [Init 0x1290 body (2026-10-22)](checkpoints/2026-10-22-init-1290-body-analysis.md) | 195 B full body: guard read-only, `void`, one no-op vcall, two descriptor registrations; E516; G1 PARTIAL |
| [Init continue batch A (2026-10-22)](checkpoints/2026-10-22-init-531d60-continue-batch-a.md) | 22 units / 95 B `0x532720..0x53277f`, 5 calls + 3 lea + `je` pinned; E519; G1 PARTIAL |
| [Repo-health round (2026-10-22)](checkpoints/2026-10-22-repo-health-round.md) | Full suite green after stale ctor-test fix; publish-audit final-component gap fixed; E517/E518 |
