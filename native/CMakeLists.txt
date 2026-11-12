cmake_minimum_required(VERSION 3.20)
project(sotc_native LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES arm64 CACHE STRING "native Apple Silicon only" FORCE)
add_compile_options(-Wall -Wextra -Werror -ffp-contract=off)   # recovered float code must not be fused
option(SOTC_SANITIZE "ASan+UBSan for tests" ON)
if(SOTC_SANITIZE)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()
add_library(sotc_core STATIC
  core/math_constants.cpp
  core/guest_entry_table.cpp
  core/guarded_dd91b0.cpp
  core/guarded_dd7e30.cpp
  core/guarded_dd4210.cpp
  core/guarded_d39b00.cpp
  core/guarded_d366d0.cpp
  core/guarded_d33b50.cpp
  core/avx_d8fa40.cpp
  core/avx_d86ed0.cpp
  core/avx_d5a280.cpp
  core/leaf_cab080.cpp
  core/leaf_ca18d0.cpp
  core/leaf_c94ae0.cpp
  core/leaf_b692a0.cpp
  core/leaf_c811b0.cpp
  core/leaf_8a0440.cpp
  core/leaf_c05980.cpp
  core/leaf_c6f520.cpp
  core/leaf_c6e9d0.cpp
  core/leaf_a816d0.cpp
  core/leaf_3ca7c0.cpp
  core/leaf_c0d380.cpp
  core/leaf_c675f0.cpp
  core/leaf_c16b30.cpp
  core/leaf_8f3d90.cpp
  core/leaf_163a40.cpp
  core/leaf_9b75a0.cpp
  core/leaf_575d40.cpp
  core/leaf_99350.cpp
  core/leaf_83d800.cpp
  core/leaf_7ce8d0.cpp
  core/leaf_e71990.cpp
  core/slot23_names_fixture.cpp
  core/leaf_df94c0.cpp
  core/leaf_de0160.cpp
  core/fnv_cstr_161fb0.cpp
  core/fnv_path_cstr_162020.cpp
  core/hashed_name_ctors.cpp
  core/guarded_static_helpers.cpp
  core/atomic_add_ref_14ccb0.cpp
  core/atomic_xadd_b30cf0.cpp
  core/store_value_clear_flag_158340.cpp
  core/fnv_span_161f80.cpp
  core/hashed_string_14c3a0.cpp
  core/shared_str_release_f590.cpp
  core/registry_object_14c9a0.cpp
  core/registry_object_2efd160.cpp
  core/registry_sorted_insert.cpp
  core/inputtable_names_fixture.cpp
  core/registry_fill_52a220.cpp
  core/registry_init_531d60.cpp
  core/appmain_init_registry_slice.cpp
  core/init_531d60_trivial_leaves.cpp
  core/init_1290.cpp
  core/init_post_bodies.cpp
  core/init_7f6f90_body.cpp
  core/init_96f20_body.cpp
  core/init_560b60_body.cpp
  core/init_5b9f90_slices.cpp
  core/init_a5d990_body.cpp
  core/init_a5cd90_body.cpp
  core/init_946880_body.cpp
  core/init_5d5410_body.cpp
  core/init_531d60_entry_gaps.cpp
  core/init_window_runtime.cpp
  core/stat_query_54b250.cpp
  core/savedata_stats_a54920.cpp
  core/savedata_family.cpp
  core/insert_1beeb0.cpp
  core/insert_1bde30.cpp
  core/shared_release_f590.cpp
  core/alloc_wrappers_17070.cpp
  core/insert_1bdb50.cpp
  core/app_info_registry.cpp
  core/app_info_install.cpp
  core/registry_owner.cpp
  core/container_dtor_1bede0.cpp
  core/registry_slot.cpp
  core/app_info_collect.cpp
  core/appinfo_listb.cpp
  core/app_info_snapshot.cpp
  core/report_tree.cpp
  core/insert_1bdc80.cpp
  core/owner_properties_2569d0.cpp
  core/owner_prop_bag.cpp
  core/owner_members.cpp
  core/owner_simple_ctors.cpp
  core/owner_ctor_b111e0_slot48.cpp
  core/owner_ctor_b11210_slot58.cpp
  core/owner_ctor_a5c850.cpp
  core/owner_dtor_a5cb30.cpp
  core/shared_str_ctor_f1a0.cpp
  core/shared_str_resize_f3c0.cpp
  core/resource_id_14c2f0.cpp
  core/resource_pool.cpp
  core/app_info_run.cpp
  core/snapshot_bug_data.cpp
  core/screenshot_contract.cpp
  core/frame_root.cpp
  core/platform_tag_147e20.cpp
  core/camera_location_25a4d0.cpp
  core/list_dtor_1bdeb0.cpp
  core/insert_21cd0.cpp
  core/container_1bdb10.cpp
  core/owner_dtor_1bdae0.cpp
  core/appmain_init_remainder_slice.cpp
  core/xchg_store_u32_357f0.cpp
  core/base_initialize_255130.cpp
  core/appmain_init_base_initialize_slice.cpp
  core/init_flag_object_335a0.cpp
  core/prologue_255180.cpp
  core/store_tsc_323d0.cpp
  core/ensure_capacity_3c2f70.cpp
  core/init_sub_3be930.cpp
  core/ctor_40_1ab550.cpp
  core/prologue_gates_2552d4.cpp
  core/vslot_30_534130.cpp
  core/mid_255340.cpp
  core/singleton_f08_531d84.cpp
  core/vslot_40_533100_header.cpp
  core/lookup_1fc80.cpp
  core/vslot_40_533100_post.cpp
  core/vslot_40_533100_miss.cpp
  core/vslot_40_533100_join.cpp
  core/vslot_40_533100_gfx_2ee2d00.cpp
  core/vslot_40_533100_shadowmodels.cpp
  core/vslot_40_533100_attach_common.cpp
  core/vslot_40_533100_shadow_attach.cpp
  core/vslot_40_533100_shadowskels.cpp
  core/vslot_40_533100_skels_attach.cpp
  core/vslot_40_533100_shadowanims.cpp
  core/vslot_40_533100_anims_attach.cpp
  core/vslot_40_533100_shadowbanks.cpp
  core/vslot_40_533100_banks_attach.cpp
  core/vslot_40_533100_shadowstreams.cpp
  core/vslot_40_533100_streams_attach.cpp
  core/vslot_40_533100_shadowrandomevents.cpp
  core/vslot_40_533100_randomevents_attach.cpp
  core/vslot_40_533100_shadowbossmusic.cpp
  core/vslot_40_533100_bossmusic_attach.cpp
  core/vslot_40_533100_cp3characters.cpp
  core/vslot_40_533100_cp3characters_attach.cpp
  core/vslot_40_533100_cp3shadowprecache.cpp
  core/vslot_40_533100_cp3shadowprecache_attach.cpp
  core/vslot_40_533100_tail_140d90.cpp
  core/vslot_40_533100_stackchk.cpp
  core/init_531d60_post_base_499f80.cpp
  core/init_531d60_post_base_56dcb0.cpp
  core/init_531d60_post_base_5c64c0.cpp
  core/init_531d60_post_base_1290.cpp
  core/init_531d60_post_base_56db70.cpp
  core/init_531d60_post_base_5b3d00.cpp
  core/init_531d60_post_base_5b3cf0.cpp
  core/init_531d60_post_base_avx_bss.cpp
  core/init_531d60_post_base_7f7220.cpp
  core/init_531d60_post_base_56fad0.cpp
  core/init_531d60_post_base_f90310.cpp
  core/init_531d60_post_base_ymm_bss.cpp
  core/init_531d60_post_base_ymm_intercell.cpp
  core/init_531d60_post_base_xmm_seed.cpp
  core/init_531d60_post_base_ymm_reload.cpp
  core/init_531d60_post_base_26800_f590.cpp
  core/init_531d60_post_base_sbj.cpp
  core/init_531d60_post_base_sboj.cpp
  core/init_531d60_post_base_52eb50.cpp
  core/init_531d60_post_base_lea_vmovq.cpp
  core/init_531d60_post_base_532ca0.cpp
  core/init_531d60_post_base_vcall18.cpp
  core/init_531d60_post_base_532ca0_u2.cpp
  core/init_531d60_post_base_vcall18_u2.cpp
  core/init_531d60_post_base_532ca0_u3.cpp
  core/init_531d60_post_base_vcall18_u3.cpp
  core/init_531d60_post_base_532ca0_u4.cpp
  core/init_531d60_post_base_vcall18_u4.cpp
  core/init_531d60_post_base_532ca0_u5.cpp
  core/init_531d60_post_base_vcall18_u5.cpp
  core/init_531d60_post_base_532ed0.cpp
  core/init_531d60_post_base_vcall18_532ed0.cpp
  core/init_531d60_post_base_532ed0_u2.cpp
  core/init_531d60_post_base_vcall18_532ed0_u2.cpp
  core/init_531d60_post_base_17070.cpp
  core/init_531d60_post_base_544fe0.cpp
  core/init_531d60_post_base_cell_2eff450.cpp
  core/init_531d60_post_base_17070_u2.cpp
  core/init_531d60_post_base_53fd10.cpp
  core/init_531d60_post_base_cell_2eff448.cpp
  core/init_531d60_post_base_vcall38.cpp
  core/init_531d60_post_base_lea_32f4fe0.cpp
  core/init_531d60_post_base_a2caf0.cpp
  core/init_531d60_post_base_a2ed00.cpp
  core/init_531d60_post_base_a2ed00_u2.cpp
  core/init_531d60_post_base_a2ed00_u3.cpp
  core/init_531d60_post_base_f590_a2ed00.cpp
  core/init_531d60_post_base_f590_a2ed00_u2.cpp
  core/init_531d60_post_base_f590_a2ed00_u3.cpp
  core/init_531d60_post_base_f590_a2ed00_u4.cpp
  core/init_531d60_post_base_f590_a2ed00_u5.cpp
  core/init_531d60_post_base_f590_a2ed00_u6.cpp
  core/init_531d60_post_base_a2ed00_u4.cpp
  core/init_531d60_post_base_f590_a2ed00_u7.cpp
  core/init_531d60_post_base_f590_a2ed00_u8.cpp
  core/init_531d60_post_base_a2ed00_u5.cpp
  core/init_531d60_post_base_f590_a2ed00_u9.cpp
  core/init_531d60_post_base_f590_a2ed00_u10.cpp
  core/init_531d60_post_base_a2ed00_u6.cpp
  core/init_531d60_post_base_f590_a2ed00_u11.cpp
  core/init_531d60_post_base_f590_a2ed00_u12.cpp
  core/init_531d60_post_base_a2ed00_u7.cpp
  core/init_531d60_post_base_f590_a2ed00_u13.cpp
  core/init_531d60_post_base_f590_a2ed00_u14.cpp
  core/init_531d60_post_base_a2ea00_u1.cpp
  core/init_531d60_post_base_a2e5d0_u1.cpp
  core/init_531d60_post_base_a543b0_u1.cpp
  core/init_531d60_post_base_1fc80_u1.cpp
  core/init_531d60_post_base_1e4c0_u1.cpp
  core/init_531d60_post_base_4c4950_u1.cpp
  core/init_531d60_post_base_a52b70_u1.cpp
  core/init_531d60_post_base_a599e0_u1.cpp
  core/init_531d60_post_base_a431e0_u1.cpp
  core/init_531d60_post_base_a473b0_u1.cpp
  core/init_531d60_post_base_a54920_u1.cpp
  core/init_531d60_post_base_a54920_532a62_u1.cpp
  core/init_531d60_post_base_a54920_532ae2_u1.cpp
  core/init_531d60_post_base_vcall18_532a17.cpp
  core/init_531d60_post_base_vcall18_532a72_u1.cpp
  core/init_531d60_post_base_vcall18_532af2_u1.cpp
  core/init_531d60_post_base_lea_rax_2eff450_532af5_u1.cpp
  core/init_531d60_post_base_xor_esi_532afc_u1.cpp
  core/init_531d60_post_base_mov_edx_2_532afe_u1.cpp
  core/init_531d60_post_base_mov_rdi_rax_mem_532b03_u1.cpp
  core/init_531d60_post_base_mov_rax_rdi_mem_532b06_u1.cpp
  core/init_531d60_post_base_vcall08_532b09_u1.cpp
  core/init_531d60_post_base_mov_edi_98_532b0c_u1.cpp
  core/init_531d60_post_base_call_17070_532b11_u1.cpp
  core/init_531d60_post_base_mov_r14_rax_532b16_u1.cpp
  core/init_531d60_post_base_call_54dc10_532b19_u1.cpp
  core/init_531d60_post_base_mov_rbx_rax_532b1e_u1.cpp
  core/init_531d60_post_base_call_54dc20_532b21_u1.cpp
  core/init_531d60_post_base_mov_r15_rax_532b26_u1.cpp
  core/init_531d60_post_base_mov_rsi_rbx_532b29_u1.cpp
  core/init_531d60_post_base_mov_edx_eax_532b2c_u1.cpp
  core/init_531d60_post_base_call_52a220_532b2e_u1.cpp
  core/init_531d60_post_base_lea_rbx_2efd160_532b33_u1.cpp
  core/init_531d60_post_base_mov_r13_15fe518_532b3a_u1.cpp
  core/init_531d60_post_base_mov_r12_rbx_532b41_u1.cpp
  core/init_531d60_post_base_cmp_r12_r14_532b44_u1.cpp
  core/init_531d60_post_base_je_532bfb_532b47_u1.cpp
  core/init_531d60_post_base_test_r12_r12_532b4d_u1.cpp
  core/init_531d60_post_base_je_532bf8_532b50_u1.cpp
  core/init_531d60_post_base_mov_r14_mem_rsp10_532b56_u1.cpp
  core/init_531d60_post_base_mov_r13_mem_rsp40_532b5b_u1.cpp
  core/init_531d60_post_base_mov_r14_mem_rsp48_532b60_u1.cpp
  core/init_531d60_post_base_cmp_r13_r14_532b65_u1.cpp
  core/init_531d60_post_base_je_532bba_532b68_u1.cpp
  core/init_531d60_post_base_nopw_532b6a_u1.cpp
  core/init_531d60_post_base_mov_rbx_r13_10_532b70_u1.cpp
  core/init_531d60_post_base_mov_r15_r13_18_532b74_u1.cpp
  core/init_531d60_post_base_cmp_r15_r11_532b78_u1.cpp
  core/init_531d60_post_base_je_532b98_532b7b_u1.cpp
  core/init_531d60_post_base_nop_532b7d_u1.cpp
  core/init_531d60_post_base_mov_rax_rbx_48_532b80_u1.cpp
  core/init_531d60_post_base_lea_rdi_rbx_28_532b84_u1.cpp
  core/init_531d60_post_base_vcall18_532b88_u1.cpp
  core/init_531d60_post_base_add_rbx_58_532b8b_u1.cpp
  core/init_531d60_post_base_cmp_r15_rbx_532b8f_u1.cpp
  core/init_531d60_post_base_jne_532b80_532b92_u1.cpp
  core/init_531d60_post_base_mov_rbx_r13_10_532b94_u1.cpp
  core/init_531d60_post_base_test_rbx_rbx_532b98_u1.cpp
  core/init_531d60_post_base_je_532ba5_532b9b_u1.cpp
  core/init_531d60_post_base_mov_rdi_rbx_532b9d_u1.cpp
  core/init_531d60_post_base_call_170c0_532ba0_u1.cpp
  core/init_531d60_post_base_add_r13_30_532ba5_u1.cpp
  core/init_531d60_post_base_cmp_r13_r14_532ba9_u1.cpp
  core/init_531d60_post_base_jne_532b70_532bac_u1.cpp
  core/init_531d60_post_base_mov_r13_mem_r12_40_532bae_u1.cpp
  core/init_531d60_post_base_lea_rbx_2efd160_532bb3_u1.cpp
  core/init_531d60_post_base_mov_r14_mem_rsp10_532bba_u1.cpp
  core/init_531d60_post_base_cmp_r13_r14_532bbf_u1.cpp
  core/init_531d60_post_base_je_532bcc_532bc2_u1.cpp
  core/init_531d60_post_base_mov_r13_r15_532bc4_u1.cpp
  core/init_531d60_post_base_call_170c0_532bc7_u1.cpp
  core/init_531d60_post_base_mov_rdi_mem_r12_20_532bcc_u1.cpp
  core/init_531d60_post_base_test_rdi_rdi_532bd1_u1.cpp
  core/init_531d60_post_base_je_532bdb_532bd4_u1.cpp
  core/init_531d60_post_base_call_170c0_532bd6_u1.cpp
  core/init_531d60_post_base_mov_rdi_mem_r12_532bdb_u1.cpp
  core/init_531d60_post_base_mov_r13_15fe518_532bdf_u1.cpp
  core/init_531d60_post_base_je_532bf0_532be6_u1.cpp
  core/init_531d60_post_base_call_170c0_532beb_u1.cpp
  core/init_531d60_post_base_mov_rdi_r12_532bf0_u1.cpp
  core/init_531d60_post_base_call_170b0_532bf3_u1.cpp
  core/init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.cpp
  core/init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.cpp
  core/init_531d60_post_base_mov_r14_mem_rsp20_532c02_u1.cpp
  core/init_531d60_post_base_mov_edx_63647372_532c07_u1.cpp
  core/init_531d60_post_base_mov_rdi_rax_mem_532c0c_u1.cpp
  core/init_531d60_post_base_lea_rsi_r14_3ec0_532c0f_u1.cpp
  core/init_531d60_post_base_mov_rax_rdi_mem_532c16_u1.cpp
  core/init_531d60_post_base_vcall70_532c19_u1.cpp
  core/init_531d60_post_base_registry_body.cpp
  core/init_531d60_post_base_registry_tail.cpp
  core/init_531d60_post_base_vmovq_532a75_u1.cpp
  core/init_531d60_post_base_lea_5388e0_532a7d_u1.cpp
  core/init_531d60_post_base_lea_rsp470_532a84_u1.cpp
  core/init_531d60_post_base_lea_rsi_15c8e18_532a8c_u1.cpp
  core/init_531d60_post_base_movq_rsp4a0_0_532a93_u1.cpp
  core/init_531d60_post_base_mov_rdi_r13_532a9f_u1.cpp
  core/init_531d60_post_base_lea_538870_532aa3_u1.cpp
  core/init_531d60_post_base_lea_rbx_rsp490_532aaa_u1.cpp
  core/init_531d60_post_base_mov_rsp470_rdx_532ab2_u1.cpp
  core/init_531d60_post_base_vmovdqa_rsp478_xmm0_532aba_u1.cpp
  core/init_531d60_post_base_mov_rsp488_rax_532ac3_u1.cpp
  core/init_531d60_post_base_mov_rsp4b0_rsi_532acb_u1.cpp
  core/init_531d60_post_base_mov_rsp4b0_rax_532ae7_u1.cpp
  core/init_531d60_post_base_mov_rdi_rbx_532aef_u1.cpp
  core/init_531d60_post_base_lea_rsi_2efda50_532ad3_u1.cpp
  core/init_531d60_post_base_mov_rsp4b8_rcx_532ada_u1.cpp
  core/init_531d60_post_base_continue_a.cpp
  core/init_531d60_post_base_continue_b.cpp
  core/init_531d60_post_base_continue_c.cpp
  core/init_531d60_post_base_continue_d.cpp
  core/init_531d60_post_base_continue_c_after_merge.cpp
  core/init_531d60_post_base_continue_e.cpp
  core/init_531d60_post_base_alt_5327bd.cpp
  core/ctor_98a90_3be050.cpp
  core/appmain_init_prologue_slice.cpp
  core/appmain_init_prologue_tail_slice.cpp
  core/appmain_init_ctor_98a90_slice.cpp
  core/scratch_reset.cpp
  core/save_checksum.cpp
  core/name_hash.cpp
  core/save_chunk_header.cpp
  core/save_stream.cpp
  core/save_chunk_codec.cpp
  core/save_progress_object.cpp
  core/save_slot_store.cpp
  core/save_meta.cpp
  core/save_transaction.cpp
  core/descriptor_registration.cpp
  core/const_table_ee1130.cpp
  core/input_table_551140.cpp
  core/recursive_mutex.cpp
  core/critsec_3b310.cpp
  core/init_499f80_body.cpp
  core/config_bool_lookup_1e4c0.cpp
  core/lazy_bind_91210.cpp
  core/startup_ctors.cpp
  core/ctor_slot_545_b2ec0.cpp
  core/clothing_preview_param.cpp
  core/lazy_param_def_tree.cpp
  core/clothing_physical_mesh_parameters.cpp
  core/registration_thunks.cpp
  core/trig_constants.cpp
  core/addr.cpp
  core/allocator_region.cpp
  core/host_map.cpp
  core/page_owner_map.cpp
  core/page_allocator.cpp
  core/composite_allocator.cpp
  core/bucket_allocator.cpp
  core/heap_allocator.cpp
  core/allocator_bootstrap.cpp
  core/allocator_front.cpp
  core/free_list_carve.cpp
  core/region_split.cpp
  core/region_list.cpp
  core/arena_high_water.cpp
  core/arena_accounting.cpp
  core/exact_fit_finalize.cpp
  core/arena_page_debit.cpp
  core/vec_neg036.cpp
  core/xyz_mask_copy.cpp
  core/vec_dd41e0.cpp
  core/clear_flag32.cpp
  core/clear_flag64.cpp
  core/vec_b52110.cpp
  core/vec_b743c0.cpp
  core/vec_b747f0.cpp
  core/vec_b76950.cpp
  core/vec_ef85e0.cpp
  core/vec_e0b180.cpp
  core/vec_e085e0.cpp
  core/vec_e05320.cpp
  core/store_zero32.cpp
  core/zero_xmm16.cpp
  core/zero_xmm_qword.cpp
  core/zero_struct12.cpp
  core/init_f4fd40.cpp
  core/init_f4eba0.cpp
  core/init_f4b540.cpp
  core/init_ea52f0.cpp
  core/vec_e026b0.cpp
  core/vec_de6d20.cpp
  core/vec_dfd5f0.cpp
  core/vec_de1ad0.cpp
  core/vec_ddef00.cpp
  core/vec_de7fb0.cpp
  core/vec_ddab00.cpp
  core/vec_dddc90.cpp
  core/vec_dda990.cpp
  core/vec_dc4180.cpp
  core/vec_dc2d50.cpp
  core/vec_db3a70.cpp
  core/vec_db93a0.cpp
  core/vec_dbb280.cpp
  core/vec_db5dd0.cpp
  core/vec_db7e60.cpp
  core/vec_db3960.cpp
  core/vec_d90310.cpp
  core/vec_d94cb0.cpp
  core/vec_d853a0.cpp
  core/vec_d80e60.cpp
  core/vec_dac410.cpp
  core/vec_d7fe90.cpp
  core/vec_d84ee0.cpp
  core/vec_d7e640.cpp
  core/vec_d7fe40.cpp
  core/vec_d7c9d0.cpp
  core/vec_d7c4d0.cpp
  core/vec_d7c380.cpp
  core/vec_d7bc60.cpp
  core/vec_d7be60.cpp
  core/vec_d7b930.cpp
  core/vec_d6e0f0.cpp
  core/vec_d6dbf0.cpp
  core/vec_d6d890.cpp
  core/vec_d6d280.cpp
  core/vec_d6a520.cpp
  core/vec_d62b00.cpp
  core/vec_d62690.cpp
  core/vec_d623c0.cpp
  core/vec_d5fbe0.cpp
  core/vec_d47880.cpp
  core/vec_d47420.cpp
  core/vec_d47270.cpp
  core/vec_d42b60.cpp
  core/vec_d40ea0.cpp
  core/vec_d405a0.cpp
  core/vec_d3b790.cpp
  core/vec_d40010.cpp
  core/vec_d32b70.cpp
  core/vec_d3a6d0.cpp
  core/vec_d2ffe0.cpp
  core/vec_d32720.cpp
  core/vec_d2cb30.cpp
  core/vec_d19bb0.cpp
  core/vec_d2c7e0.cpp
  core/vec_d17c10.cpp
  core/vec_d19af0.cpp
  core/vec_d20590.cpp
  core/vec_d19930.cpp
  core/vec_d13300.cpp
  core/vec_d0bde0.cpp
  core/vec_d0a250.cpp
  core/vec_d09160.cpp
  core/vec_d082e0.cpp
  core/vec_d0bcd0.cpp
  core/vec_d08240.cpp
  core/vec_d06dd0.cpp
  core/vec_d00600.cpp
  core/vec_cfcc60.cpp
  core/vec_cfcbc0.cpp
  core/vec_d04ce0.cpp
  core/vec_cf44a0.cpp
  core/vec_cee8a0.cpp
  core/vec_ceb890.cpp
  core/vec_ceb2c0.cpp
  core/vec_ceb1c0.cpp
  core/vec_ceb070.cpp
  core/vec_ce99c0.cpp
  core/vec_ce74a0.cpp
  core/vec_ce7300.cpp
  core/vec_cdc760.cpp
  core/vec_ce57b0.cpp
  core/vec_ce0890.cpp
  core/vec_ce6fb0.cpp
  core/vec_ce8440.cpp
  core/vec_ce67b0.cpp
  core/vec_ce6130.cpp
  core/vec_ce38d0.cpp
  core/vec_cde740.cpp
  core/vec_cd4d60.cpp
  core/vec_cd0420.cpp
  core/vec_cc8ac0.cpp
  core/vec_ccd0f0.cpp
  core/vec_cceaa0.cpp
  core/vec_cc8330.cpp
  core/vec_cc5a10.cpp
  core/vec_cc07d0.cpp
  core/vec_caeed0.cpp
  core/vec_cb5cb0.cpp
  core/vec_cbb2c0.cpp
  core/vec_cae740.cpp
  core/vec_cae240.cpp
  core/vec_cad450.cpp
  core/vec_ca4dc0.cpp
  core/vec_c9e8e0.cpp
  core/vec_c91b10.cpp
  core/vec_c891e0.cpp
  core/vec_c7de60.cpp
  core/vec_c7c310.cpp
  core/vec_c786c0.cpp
  core/vec_c77070.cpp
  core/vec_c741c0.cpp
  core/vec_c70e20.cpp
  core/vec_c6fdf0.cpp
  core/vec_c6d910.cpp
  core/vec_c6bad0.cpp
  core/vec_c69f70.cpp
  core/vec_c686e0.cpp
  core/vec_c66580.cpp
  core/vec_c65f80.cpp
  core/vec_c65da0.cpp
  core/vec_c64060.cpp
  core/vec_c61670.cpp
  core/vec_c5f6f0.cpp
  core/vec_c5c320.cpp
  core/vec_c5c1a0.cpp
  core/vec_c5c040.cpp
  core/vec_c5a400.cpp
  core/vec_c5a100.cpp
  core/vec_c555c0.cpp
  core/vec_c54d20.cpp
  core/vec_c54830.cpp
  core/vec_c53db0.cpp
  core/vec_c4e5e0.cpp
  core/vec_c4aa80.cpp
  core/vec_c47e00.cpp
  core/vec_c451d0.cpp
  core/vec_c44830.cpp
  core/vec_c42b00.cpp
  core/vec_c3eaa0.cpp
  core/vec_c3cf00.cpp
  core/vec_c37020.cpp
  core/vec_c36a80.cpp
  core/vec_c23650.cpp
  core/vec_c22300.cpp
  core/vec_c20710.cpp
  core/vec_c1a9a0.cpp
  core/vec_c191a0.cpp
  core/vec_c17a70.cpp
  core/vec_c152c0.cpp
  core/vec_c14f60.cpp
  core/vec_c0d720.cpp
  core/vec_c0bea0.cpp
  core/vec_bd7a80.cpp
  core/vec_be4ff0.cpp
  core/vec_be50a0.cpp
  core/vec_be5270.cpp
  core/vec_be5410.cpp
  core/vec_c060a0.cpp
  core/vec_c0bbd0.cpp
  core/vec_c0bb00.cpp
  core/vec_c0baa0.cpp
  core/vec_c0aae0.cpp
  core/vec_c09070.cpp
  core/vec_e094e0.cpp
  core/vec_dbfa60.cpp
  core/vec_d85490.cpp
  core/vec_bd7950.cpp
  core/vec_bd60a0.cpp
  core/vec_bd4dd0.cpp
  core/vec_bd32f0.cpp
  core/vec_bd3190.cpp
  core/vec_bd06c0.cpp
  core/vec_bceda0.cpp
  core/vec_bccc00.cpp
  core/vec_bcc960.cpp
  core/vec_bcc4e0.cpp
  core/vec_bcc3f0.cpp
  core/vec_bc8210.cpp
  core/vec_bc7280.cpp
  core/vec_bc5b00.cpp
  core/vec_bc2800.cpp
  core/vec_bc07f0.cpp
  core/vec_bc0670.cpp
  core/vec_bbf580.cpp
  core/vec_bbd5e0.cpp
  core/vec_bbba10.cpp
  core/vec_bb9740.cpp
  core/vec_bb82d0.cpp
  core/vec_bb7990.cpp
  core/vec_bacfb0.cpp
  core/vec_bace80.cpp
  core/vec_bace50.cpp
  core/vec_bacb30.cpp
  core/vec_bab950.cpp
  core/vec_ba3ef0.cpp
  core/vec_b950e0.cpp
  core/vec_b8d170.cpp
  core/vec_b89eb0.cpp
  core/vec_b84580.cpp
  core/vec_b7b270.cpp
  core/vec_b77e90.cpp
  core/vec_b77910.cpp
  core/vec_b77740.cpp
  core/vec_b71f80.cpp
  core/vec_b6e6d0.cpp
  core/vec_b67a70.cpp
  core/vec_b5ecf0.cpp
  core/vec_b58690.cpp
  core/vec_b57360.cpp
  core/vec_b4ef70.cpp
  core/vec_b494d0.cpp
  core/vec_b47100.cpp
  core/vec_b44580.cpp
  core/vec_b33bd0.cpp
  core/capture_a473b0.cpp
  core/owner_ctor_a597d0.cpp
)
target_include_directories(sotc_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_source_files_properties(core/ctor_slot_545_b2ec0.cpp PROPERTIES
  COMPILE_OPTIONS "-ffp-contract=off")
enable_testing()
add_executable(test_math_constants tests/test_math_constants.cpp)
target_link_libraries(test_math_constants sotc_core)
add_test(NAME math_constants COMMAND test_math_constants)
add_executable(test_scratch_reset tests/test_scratch_reset.cpp)
target_link_libraries(test_scratch_reset sotc_core)
add_test(NAME scratch_reset COMMAND test_scratch_reset)
add_executable(test_startup_ctors tests/test_startup_ctors.cpp)
target_link_libraries(test_startup_ctors sotc_core)
add_test(NAME startup_ctors COMMAND test_startup_ctors)
add_executable(test_ctor_slot_545 tests/test_ctor_slot_545.cpp)
target_link_libraries(test_ctor_slot_545 sotc_core)
add_test(NAME ctor_slot_545 COMMAND test_ctor_slot_545)
set_tests_properties(startup_ctors PROPERTIES
  ENVIRONMENT "SOTC_SLOT23_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/slot23_e71990_names.bin")
add_executable(test_slot23_names_fixture tests/test_slot23_names_fixture.cpp)
target_link_libraries(test_slot23_names_fixture sotc_core)
add_test(NAME slot23_names_fixture COMMAND test_slot23_names_fixture)
set_tests_properties(slot23_names_fixture PROPERTIES
  ENVIRONMENT "SOTC_SLOT23_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/slot23_e71990_names.bin")

add_executable(test_clothing_preview_param tests/test_clothing_preview_param.cpp)
target_link_libraries(test_clothing_preview_param sotc_core)
add_test(NAME clothing_preview_param COMMAND test_clothing_preview_param)
add_executable(test_lazy_param_def_tree tests/test_lazy_param_def_tree.cpp)
target_link_libraries(test_lazy_param_def_tree sotc_core)
add_test(NAME lazy_param_def_tree COMMAND test_lazy_param_def_tree)
add_executable(test_clothing_physical_mesh_parameters tests/test_clothing_physical_mesh_parameters.cpp)
target_link_libraries(test_clothing_physical_mesh_parameters sotc_core)
add_test(NAME clothing_physical_mesh_parameters COMMAND test_clothing_physical_mesh_parameters)
add_executable(test_registration_thunks tests/test_registration_thunks.cpp)
target_link_libraries(test_registration_thunks sotc_core)
add_test(NAME registration_thunks COMMAND test_registration_thunks)
add_executable(test_registration_thunks_ahead tests/test_registration_thunks_ahead.cpp)
target_link_libraries(test_registration_thunks_ahead sotc_core)
add_test(NAME registration_thunks_ahead COMMAND test_registration_thunks_ahead)
set_tests_properties(registration_thunks_ahead PROPERTIES
  ENVIRONMENT "SOTC_SLOT23_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/slot23_e71990_names.bin")
add_executable(test_trig_constants tests/test_trig_constants.cpp)
target_link_libraries(test_trig_constants sotc_core)
add_test(NAME trig_constants COMMAND test_trig_constants)
add_executable(test_normal_threshold tests/test_normal_threshold.cpp)
target_link_libraries(test_normal_threshold sotc_core)
add_test(NAME normal_threshold COMMAND test_normal_threshold)
add_executable(test_addr tests/test_addr.cpp)
target_link_libraries(test_addr sotc_core)
add_test(NAME addr COMMAND test_addr)
add_executable(test_allocator_region tests/test_allocator_region.cpp)
target_link_libraries(test_allocator_region sotc_core)
add_test(NAME allocator_region COMMAND test_allocator_region)
add_executable(test_arena_accounting tests/test_arena_accounting.cpp)
target_link_libraries(test_arena_accounting sotc_core)
add_test(NAME arena_accounting COMMAND test_arena_accounting)
add_executable(test_vec_neg036 tests/test_vec_neg036.cpp)
target_link_libraries(test_vec_neg036 sotc_core)
add_test(NAME vec_neg036 COMMAND test_vec_neg036)
add_executable(test_xyz_mask_copy tests/test_xyz_mask_copy.cpp)
target_link_libraries(test_xyz_mask_copy sotc_core)
add_test(NAME xyz_mask_copy COMMAND test_xyz_mask_copy)
add_executable(test_vec_dd41e0 tests/test_vec_dd41e0.cpp)
target_link_libraries(test_vec_dd41e0 sotc_core)
add_test(NAME vec_dd41e0 COMMAND test_vec_dd41e0)
add_executable(test_clear_flag32 tests/test_clear_flag32.cpp)
target_link_libraries(test_clear_flag32 sotc_core)
add_test(NAME clear_flag32 COMMAND test_clear_flag32)
add_executable(test_clear_flag64 tests/test_clear_flag64.cpp)
target_link_libraries(test_clear_flag64 sotc_core)
add_test(NAME clear_flag64 COMMAND test_clear_flag64)
add_executable(test_vec_b52110 tests/test_vec_b52110.cpp)
target_link_libraries(test_vec_b52110 sotc_core)
add_test(NAME vec_b52110 COMMAND test_vec_b52110)
add_executable(test_vec_b743c0 tests/test_vec_b743c0.cpp)
target_link_libraries(test_vec_b743c0 sotc_core)
add_test(NAME vec_b743c0 COMMAND test_vec_b743c0)
add_executable(test_vec_b747f0 tests/test_vec_b747f0.cpp)
target_link_libraries(test_vec_b747f0 sotc_core)
add_test(NAME vec_b747f0 COMMAND test_vec_b747f0)
add_executable(test_vec_b76950 tests/test_vec_b76950.cpp)
target_link_libraries(test_vec_b76950 sotc_core)
add_test(NAME vec_b76950 COMMAND test_vec_b76950)
add_executable(test_vec_ef85e0 tests/test_vec_ef85e0.cpp)
target_link_libraries(test_vec_ef85e0 sotc_core)
add_test(NAME vec_ef85e0 COMMAND test_vec_ef85e0)
add_executable(test_vec_e0b180 tests/test_vec_e0b180.cpp)
target_link_libraries(test_vec_e0b180 sotc_core)
add_test(NAME vec_e0b180 COMMAND test_vec_e0b180)
add_executable(test_vec_e085e0 tests/test_vec_e085e0.cpp)
target_link_libraries(test_vec_e085e0 sotc_core)
add_test(NAME vec_e085e0 COMMAND test_vec_e085e0)
add_executable(test_vec_e05320 tests/test_vec_e05320.cpp)
target_link_libraries(test_vec_e05320 sotc_core)
add_test(NAME vec_e05320 COMMAND test_vec_e05320)
add_executable(test_store_zero32 tests/test_store_zero32.cpp)
target_link_libraries(test_store_zero32 sotc_core)
add_test(NAME store_zero32 COMMAND test_store_zero32)
add_executable(test_zero_xmm16 tests/test_zero_xmm16.cpp)
target_link_libraries(test_zero_xmm16 sotc_core)
add_test(NAME zero_xmm16 COMMAND test_zero_xmm16)
add_executable(test_init_f4fd40 tests/test_init_f4fd40.cpp)
target_link_libraries(test_init_f4fd40 sotc_core)
add_test(NAME init_f4fd40 COMMAND test_init_f4fd40)
add_executable(test_init_f4eba0 tests/test_init_f4eba0.cpp)
target_link_libraries(test_init_f4eba0 sotc_core)
add_test(NAME init_f4eba0 COMMAND test_init_f4eba0)
add_executable(test_init_f4b540 tests/test_init_f4b540.cpp)
target_link_libraries(test_init_f4b540 sotc_core)
add_test(NAME init_f4b540 COMMAND test_init_f4b540)
add_executable(test_init_ea52f0 tests/test_init_ea52f0.cpp)
target_link_libraries(test_init_ea52f0 sotc_core)
add_test(NAME init_ea52f0 COMMAND test_init_ea52f0)
add_executable(test_zero_xmm_qword tests/test_zero_xmm_qword.cpp)
target_link_libraries(test_zero_xmm_qword sotc_core)
add_test(NAME zero_xmm_qword COMMAND test_zero_xmm_qword)
add_executable(test_vec_e026b0 tests/test_vec_e026b0.cpp)
target_link_libraries(test_vec_e026b0 sotc_core)
add_test(NAME vec_e026b0 COMMAND test_vec_e026b0)
add_executable(test_vec_de6d20 tests/test_vec_de6d20.cpp)
target_link_libraries(test_vec_de6d20 sotc_core)
add_test(NAME vec_de6d20 COMMAND test_vec_de6d20)
add_executable(test_vec_dfd5f0 tests/test_vec_dfd5f0.cpp)
target_link_libraries(test_vec_dfd5f0 sotc_core)
add_test(NAME vec_dfd5f0 COMMAND test_vec_dfd5f0)
add_executable(test_vec_de1ad0 tests/test_vec_de1ad0.cpp)
target_link_libraries(test_vec_de1ad0 sotc_core)
add_test(NAME vec_de1ad0 COMMAND test_vec_de1ad0)
add_executable(test_host_map tests/test_host_map.cpp)
target_link_libraries(test_host_map sotc_core)
add_test(NAME host_map COMMAND test_host_map)
add_executable(test_vec_ddef00 tests/test_vec_ddef00.cpp)
target_link_libraries(test_vec_ddef00 sotc_core)
add_test(NAME vec_ddef00 COMMAND test_vec_ddef00)

add_executable(test_vec_de7fb0 tests/test_vec_de7fb0.cpp)
target_link_libraries(test_vec_de7fb0 sotc_core)
add_test(NAME vec_de7fb0 COMMAND test_vec_de7fb0)
add_executable(test_vec_dddc90 tests/test_vec_dddc90.cpp)
target_link_libraries(test_vec_dddc90 sotc_core)
add_test(NAME vec_dddc90 COMMAND test_vec_dddc90)
add_executable(test_vec_ddab00 tests/test_vec_ddab00.cpp)
target_link_libraries(test_vec_ddab00 sotc_core)
add_test(NAME vec_ddab00 COMMAND test_vec_ddab00)
add_executable(test_vec_dda990 tests/test_vec_dda990.cpp)
target_link_libraries(test_vec_dda990 sotc_core)
add_test(NAME vec_dda990 COMMAND test_vec_dda990)
add_executable(test_vec_dc4180 tests/test_vec_dc4180.cpp)
target_link_libraries(test_vec_dc4180 sotc_core)
add_test(NAME vec_dc4180 COMMAND test_vec_dc4180)
add_executable(test_free_list_carve tests/test_free_list_carve.cpp)
target_link_libraries(test_free_list_carve sotc_core)
add_test(NAME free_list_carve COMMAND test_free_list_carve)

add_executable(test_vec_dc2d50 tests/test_vec_dc2d50.cpp)
target_link_libraries(test_vec_dc2d50 sotc_core)
add_test(NAME vec_dc2d50 COMMAND test_vec_dc2d50)
add_executable(test_vec_db3a70 tests/test_vec_db3a70.cpp)
target_link_libraries(test_vec_db3a70 sotc_core)
add_test(NAME vec_db3a70 COMMAND test_vec_db3a70)
add_executable(test_vec_db93a0 tests/test_vec_db93a0.cpp)
target_link_libraries(test_vec_db93a0 sotc_core)
add_test(NAME vec_db93a0 COMMAND test_vec_db93a0)

add_executable(test_vec_dbb280 tests/test_vec_dbb280.cpp)
target_link_libraries(test_vec_dbb280 sotc_core)
add_test(NAME vec_dbb280 COMMAND test_vec_dbb280)
add_executable(test_vec_db5dd0 tests/test_vec_db5dd0.cpp)
target_link_libraries(test_vec_db5dd0 sotc_core)
add_test(NAME vec_db5dd0 COMMAND test_vec_db5dd0)

add_executable(test_vec_db7e60 tests/test_vec_db7e60.cpp)
target_link_libraries(test_vec_db7e60 sotc_core)
add_test(NAME vec_db7e60 COMMAND test_vec_db7e60)

add_executable(test_vec_db3960 tests/test_vec_db3960.cpp)
target_link_libraries(test_vec_db3960 sotc_core)
add_test(NAME vec_db3960 COMMAND test_vec_db3960)
add_executable(test_region_split tests/test_region_split.cpp)
target_link_libraries(test_region_split sotc_core)
add_test(NAME region_split COMMAND test_region_split)

add_executable(test_vec_d90310 tests/test_vec_d90310.cpp)
target_link_libraries(test_vec_d90310 sotc_core)
add_test(NAME vec_d90310 COMMAND test_vec_d90310)
add_executable(test_vec_d94cb0 tests/test_vec_d94cb0.cpp)
target_link_libraries(test_vec_d94cb0 sotc_core)
add_test(NAME vec_d94cb0 COMMAND test_vec_d94cb0)

add_executable(test_vec_d853a0 tests/test_vec_d853a0.cpp)
target_link_libraries(test_vec_d853a0 sotc_core)
add_test(NAME vec_d853a0 COMMAND test_vec_d853a0)
add_executable(test_region_list tests/test_region_list.cpp)
target_link_libraries(test_region_list sotc_core)
add_test(NAME region_list COMMAND test_region_list)

add_executable(test_arena_high_water tests/test_arena_high_water.cpp)
target_link_libraries(test_arena_high_water sotc_core)
add_test(NAME arena_high_water COMMAND test_arena_high_water)

add_executable(test_exact_fit_finalize tests/test_exact_fit_finalize.cpp)
target_link_libraries(test_exact_fit_finalize sotc_core)
add_test(NAME exact_fit_finalize COMMAND test_exact_fit_finalize)

add_executable(test_arena_page_debit tests/test_arena_page_debit.cpp)
target_link_libraries(test_arena_page_debit sotc_core)
add_test(NAME arena_page_debit COMMAND test_arena_page_debit)
add_executable(test_vec_d80e60 tests/test_vec_d80e60.cpp)
target_link_libraries(test_vec_d80e60 sotc_core)
add_test(NAME vec_d80e60 COMMAND test_vec_d80e60)
add_executable(test_vec_dac410 tests/test_vec_dac410.cpp)
target_link_libraries(test_vec_dac410 sotc_core)
add_test(NAME vec_dac410 COMMAND test_vec_dac410)

add_executable(test_vec_d7fe90 tests/test_vec_d7fe90.cpp)
target_link_libraries(test_vec_d7fe90 sotc_core)
add_test(NAME vec_d7fe90 COMMAND test_vec_d7fe90)
add_executable(test_vec_d84ee0 tests/test_vec_d84ee0.cpp)
target_link_libraries(test_vec_d84ee0 sotc_core)
add_test(NAME vec_d84ee0 COMMAND test_vec_d84ee0)

add_executable(test_vec_d7e640 tests/test_vec_d7e640.cpp)
target_link_libraries(test_vec_d7e640 sotc_core)
add_test(NAME vec_d7e640 COMMAND test_vec_d7e640)

add_executable(test_vec_d7fe40 tests/test_vec_d7fe40.cpp)
target_link_libraries(test_vec_d7fe40 sotc_core)
add_test(NAME vec_d7fe40 COMMAND test_vec_d7fe40)

add_executable(test_vec_d7c9d0 tests/test_vec_d7c9d0.cpp)
target_link_libraries(test_vec_d7c9d0 sotc_core)
add_test(NAME vec_d7c9d0 COMMAND test_vec_d7c9d0)

add_executable(test_vec_d7c380 tests/test_vec_d7c380.cpp)
target_link_libraries(test_vec_d7c380 sotc_core)
add_test(NAME vec_d7c380 COMMAND test_vec_d7c380)
add_executable(test_vec_d7c4d0 tests/test_vec_d7c4d0.cpp)
target_link_libraries(test_vec_d7c4d0 sotc_core)
add_test(NAME vec_d7c4d0 COMMAND test_vec_d7c4d0)

add_executable(test_vec_d7bc60 tests/test_vec_d7bc60.cpp)
target_link_libraries(test_vec_d7bc60 sotc_core)
add_test(NAME vec_d7bc60 COMMAND test_vec_d7bc60)

add_executable(test_vec_d7be60 tests/test_vec_d7be60.cpp)
target_link_libraries(test_vec_d7be60 sotc_core)
add_test(NAME vec_d7be60 COMMAND test_vec_d7be60)
add_executable(test_vec_d7b930 tests/test_vec_d7b930.cpp)
target_link_libraries(test_vec_d7b930 sotc_core)
add_test(NAME vec_d7b930 COMMAND test_vec_d7b930)

add_executable(test_vec_d6e0f0 tests/test_vec_d6e0f0.cpp)
target_link_libraries(test_vec_d6e0f0 sotc_core)
add_test(NAME vec_d6e0f0 COMMAND test_vec_d6e0f0)

add_executable(test_vec_d6dbf0 tests/test_vec_d6dbf0.cpp)
target_link_libraries(test_vec_d6dbf0 sotc_core)
add_test(NAME vec_d6dbf0 COMMAND test_vec_d6dbf0)

add_executable(test_vec_d6d890 tests/test_vec_d6d890.cpp)
target_link_libraries(test_vec_d6d890 sotc_core)
add_test(NAME vec_d6d890 COMMAND test_vec_d6d890)

add_executable(test_vec_d6d280 tests/test_vec_d6d280.cpp)
target_link_libraries(test_vec_d6d280 sotc_core)
add_test(NAME vec_d6d280 COMMAND test_vec_d6d280)

add_executable(test_vec_d6a520 tests/test_vec_d6a520.cpp)
target_link_libraries(test_vec_d6a520 sotc_core)
add_test(NAME vec_d6a520 COMMAND test_vec_d6a520)

add_executable(test_vec_d62b00 tests/test_vec_d62b00.cpp)
target_link_libraries(test_vec_d62b00 sotc_core)
add_test(NAME vec_d62b00 COMMAND test_vec_d62b00)

add_executable(test_vec_d62690 tests/test_vec_d62690.cpp)
target_link_libraries(test_vec_d62690 sotc_core)
add_test(NAME vec_d62690 COMMAND test_vec_d62690)

add_executable(test_vec_d5fbe0 tests/test_vec_d5fbe0.cpp)
target_link_libraries(test_vec_d5fbe0 sotc_core)
add_test(NAME vec_d5fbe0 COMMAND test_vec_d5fbe0)

add_executable(test_vec_d47880 tests/test_vec_d47880.cpp)
target_link_libraries(test_vec_d47880 sotc_core)
add_test(NAME vec_d47880 COMMAND test_vec_d47880)

add_executable(test_vec_d47420 tests/test_vec_d47420.cpp)
target_link_libraries(test_vec_d47420 sotc_core)
add_test(NAME vec_d47420 COMMAND test_vec_d47420)

add_executable(test_vec_d47270 tests/test_vec_d47270.cpp)
target_link_libraries(test_vec_d47270 sotc_core)
add_test(NAME vec_d47270 COMMAND test_vec_d47270)

add_executable(test_vec_d42b60 tests/test_vec_d42b60.cpp)
target_link_libraries(test_vec_d42b60 sotc_core)
add_test(NAME vec_d42b60 COMMAND test_vec_d42b60)

add_executable(test_vec_d40ea0 tests/test_vec_d40ea0.cpp)
target_link_libraries(test_vec_d40ea0 sotc_core)
add_test(NAME vec_d40ea0 COMMAND test_vec_d40ea0)

add_executable(test_vec_d405a0 tests/test_vec_d405a0.cpp)
target_link_libraries(test_vec_d405a0 sotc_core)
add_test(NAME vec_d405a0 COMMAND test_vec_d405a0)

add_executable(test_vec_d623c0 tests/test_vec_d623c0.cpp)
target_link_libraries(test_vec_d623c0 sotc_core)
add_test(NAME vec_d623c0 COMMAND test_vec_d623c0)

add_executable(test_vec_d3b790 tests/test_vec_d3b790.cpp)
target_link_libraries(test_vec_d3b790 sotc_core)
add_test(NAME vec_d3b790 COMMAND test_vec_d3b790)

add_executable(test_vec_d40010 tests/test_vec_d40010.cpp)
target_link_libraries(test_vec_d40010 sotc_core)
add_test(NAME vec_d40010 COMMAND test_vec_d40010)

add_executable(test_vec_d32b70 tests/test_vec_d32b70.cpp)
target_link_libraries(test_vec_d32b70 sotc_core)
add_test(NAME vec_d32b70 COMMAND test_vec_d32b70)

add_executable(test_vec_d3a6d0 tests/test_vec_d3a6d0.cpp)
target_link_libraries(test_vec_d3a6d0 sotc_core)
add_test(NAME vec_d3a6d0 COMMAND test_vec_d3a6d0)

add_executable(test_vec_d2ffe0 tests/test_vec_d2ffe0.cpp)
target_link_libraries(test_vec_d2ffe0 sotc_core)
add_test(NAME vec_d2ffe0 COMMAND test_vec_d2ffe0)

add_executable(test_vec_d32720 tests/test_vec_d32720.cpp)
target_link_libraries(test_vec_d32720 sotc_core)
add_test(NAME vec_d32720 COMMAND test_vec_d32720)

add_executable(test_vec_d2cb30 tests/test_vec_d2cb30.cpp)
target_link_libraries(test_vec_d2cb30 sotc_core)
add_test(NAME vec_d2cb30 COMMAND test_vec_d2cb30)

add_executable(test_vec_d19bb0 tests/test_vec_d19bb0.cpp)
target_link_libraries(test_vec_d19bb0 sotc_core)
add_test(NAME vec_d19bb0 COMMAND test_vec_d19bb0)

add_executable(test_vec_d2c7e0 tests/test_vec_d2c7e0.cpp)
target_link_libraries(test_vec_d2c7e0 sotc_core)
add_test(NAME vec_d2c7e0 COMMAND test_vec_d2c7e0)

add_executable(test_vec_d17c10 tests/test_vec_d17c10.cpp)
target_link_libraries(test_vec_d17c10 sotc_core)
add_test(NAME vec_d17c10 COMMAND test_vec_d17c10)

add_executable(test_vec_d19af0 tests/test_vec_d19af0.cpp)
target_link_libraries(test_vec_d19af0 sotc_core)
add_test(NAME vec_d19af0 COMMAND test_vec_d19af0)
add_executable(test_vec_d20590 tests/test_vec_d20590.cpp)
target_link_libraries(test_vec_d20590 sotc_core)
add_test(NAME vec_d20590 COMMAND test_vec_d20590)

add_executable(test_vec_d19930 tests/test_vec_d19930.cpp)
target_link_libraries(test_vec_d19930 sotc_core)
add_test(NAME vec_d19930 COMMAND test_vec_d19930)

add_executable(test_vec_d13300 tests/test_vec_d13300.cpp)
target_link_libraries(test_vec_d13300 sotc_core)
add_test(NAME vec_d13300 COMMAND test_vec_d13300)

add_executable(test_vec_d0bde0 tests/test_vec_d0bde0.cpp)
target_link_libraries(test_vec_d0bde0 sotc_core)
add_test(NAME vec_d0bde0 COMMAND test_vec_d0bde0)

add_executable(test_vec_d0a250 tests/test_vec_d0a250.cpp)
target_link_libraries(test_vec_d0a250 sotc_core)
add_test(NAME vec_d0a250 COMMAND test_vec_d0a250)

add_executable(test_vec_d09160 tests/test_vec_d09160.cpp)
target_link_libraries(test_vec_d09160 sotc_core)
add_test(NAME vec_d09160 COMMAND test_vec_d09160)

add_executable(test_vec_d082e0 tests/test_vec_d082e0.cpp)
target_link_libraries(test_vec_d082e0 sotc_core)
add_test(NAME vec_d082e0 COMMAND test_vec_d082e0)
add_executable(test_vec_d0bcd0 tests/test_vec_d0bcd0.cpp)
target_link_libraries(test_vec_d0bcd0 sotc_core)
add_test(NAME vec_d0bcd0 COMMAND test_vec_d0bcd0)

add_executable(test_vec_d08240 tests/test_vec_d08240.cpp)
target_link_libraries(test_vec_d08240 sotc_core)
add_test(NAME vec_d08240 COMMAND test_vec_d08240)

add_executable(test_vec_d06dd0 tests/test_vec_d06dd0.cpp)
target_link_libraries(test_vec_d06dd0 sotc_core)
add_test(NAME vec_d06dd0 COMMAND test_vec_d06dd0)

add_executable(test_vec_d00600 tests/test_vec_d00600.cpp)
target_link_libraries(test_vec_d00600 sotc_core)
add_test(NAME vec_d00600 COMMAND test_vec_d00600)

add_executable(test_vec_cfcc60 tests/test_vec_cfcc60.cpp)
target_link_libraries(test_vec_cfcc60 sotc_core)
add_test(NAME vec_cfcc60 COMMAND test_vec_cfcc60)

add_executable(test_vec_cfcbc0 tests/test_vec_cfcbc0.cpp)
target_link_libraries(test_vec_cfcbc0 sotc_core)
add_test(NAME vec_cfcbc0 COMMAND test_vec_cfcbc0)

add_executable(test_vec_d04ce0 tests/test_vec_d04ce0.cpp)
target_link_libraries(test_vec_d04ce0 sotc_core)
add_test(NAME vec_d04ce0 COMMAND test_vec_d04ce0)

add_executable(test_vec_cf44a0 tests/test_vec_cf44a0.cpp)
target_link_libraries(test_vec_cf44a0 sotc_core)
add_test(NAME vec_cf44a0 COMMAND test_vec_cf44a0)
add_executable(test_vec_cee8a0 tests/test_vec_cee8a0.cpp)
target_link_libraries(test_vec_cee8a0 sotc_core)
add_test(NAME vec_cee8a0 COMMAND test_vec_cee8a0)

add_executable(test_vec_ceb890 tests/test_vec_ceb890.cpp)
target_link_libraries(test_vec_ceb890 sotc_core)
add_test(NAME vec_ceb890 COMMAND test_vec_ceb890)

add_executable(test_vec_ceb2c0 tests/test_vec_ceb2c0.cpp)
target_link_libraries(test_vec_ceb2c0 sotc_core)
add_test(NAME vec_ceb2c0 COMMAND test_vec_ceb2c0)

add_executable(test_vec_ceb1c0 tests/test_vec_ceb1c0.cpp)
target_link_libraries(test_vec_ceb1c0 sotc_core)
add_test(NAME vec_ceb1c0 COMMAND test_vec_ceb1c0)

add_executable(test_vec_ceb070 tests/test_vec_ceb070.cpp)
target_link_libraries(test_vec_ceb070 sotc_core)
add_test(NAME vec_ceb070 COMMAND test_vec_ceb070)

add_executable(test_vec_ce99c0 tests/test_vec_ce99c0.cpp)
target_link_libraries(test_vec_ce99c0 sotc_core)
add_test(NAME vec_ce99c0 COMMAND test_vec_ce99c0)

add_executable(test_vec_ce74a0 tests/test_vec_ce74a0.cpp)
target_link_libraries(test_vec_ce74a0 sotc_core)
add_test(NAME vec_ce74a0 COMMAND test_vec_ce74a0)

add_executable(test_vec_ce7300 tests/test_vec_ce7300.cpp)
target_link_libraries(test_vec_ce7300 sotc_core)
add_test(NAME vec_ce7300 COMMAND test_vec_ce7300)

add_executable(test_vec_cdc760 tests/test_vec_cdc760.cpp)
target_link_libraries(test_vec_cdc760 sotc_core)
add_test(NAME vec_cdc760 COMMAND test_vec_cdc760)

add_executable(test_vec_ce57b0 tests/test_vec_ce57b0.cpp)
target_link_libraries(test_vec_ce57b0 sotc_core)
add_test(NAME vec_ce57b0 COMMAND test_vec_ce57b0)

add_executable(test_vec_ce0890 tests/test_vec_ce0890.cpp)
target_link_libraries(test_vec_ce0890 sotc_core)
add_test(NAME vec_ce0890 COMMAND test_vec_ce0890)

add_executable(test_vec_ce6fb0 tests/test_vec_ce6fb0.cpp)
target_link_libraries(test_vec_ce6fb0 sotc_core)
add_test(NAME vec_ce6fb0 COMMAND test_vec_ce6fb0)

add_executable(test_vec_ce8440 tests/test_vec_ce8440.cpp)
target_link_libraries(test_vec_ce8440 sotc_core)
add_test(NAME vec_ce8440 COMMAND test_vec_ce8440)

add_executable(test_vec_ce67b0 tests/test_vec_ce67b0.cpp)
target_link_libraries(test_vec_ce67b0 sotc_core)
add_test(NAME vec_ce67b0 COMMAND test_vec_ce67b0)

add_executable(test_vec_ce6130 tests/test_vec_ce6130.cpp)
target_link_libraries(test_vec_ce6130 sotc_core)
add_test(NAME vec_ce6130 COMMAND test_vec_ce6130)

add_executable(test_vec_ce38d0 tests/test_vec_ce38d0.cpp)
target_link_libraries(test_vec_ce38d0 sotc_core)
add_test(NAME vec_ce38d0 COMMAND test_vec_ce38d0)

add_executable(test_vec_cde740 tests/test_vec_cde740.cpp)
target_link_libraries(test_vec_cde740 sotc_core)
add_test(NAME vec_cde740 COMMAND test_vec_cde740)

add_executable(test_vec_cd4d60 tests/test_vec_cd4d60.cpp)
target_link_libraries(test_vec_cd4d60 sotc_core)
add_test(NAME vec_cd4d60 COMMAND test_vec_cd4d60)

add_executable(test_vec_cd0420 tests/test_vec_cd0420.cpp)
target_link_libraries(test_vec_cd0420 sotc_core)
add_test(NAME vec_cd0420 COMMAND test_vec_cd0420)

add_executable(test_vec_cceaa0 tests/test_vec_cceaa0.cpp)
target_link_libraries(test_vec_cceaa0 sotc_core)
add_test(NAME vec_cceaa0 COMMAND test_vec_cceaa0)

add_executable(test_vec_ccd0f0 tests/test_vec_ccd0f0.cpp)
target_link_libraries(test_vec_ccd0f0 sotc_core)
add_test(NAME vec_ccd0f0 COMMAND test_vec_ccd0f0)

add_executable(test_vec_cc8ac0 tests/test_vec_cc8ac0.cpp)
target_link_libraries(test_vec_cc8ac0 sotc_core)
add_test(NAME vec_cc8ac0 COMMAND test_vec_cc8ac0)

add_executable(test_vec_cc8330 tests/test_vec_cc8330.cpp)
target_link_libraries(test_vec_cc8330 sotc_core)
add_test(NAME vec_cc8330 COMMAND test_vec_cc8330)

add_executable(test_vec_cc5a10 tests/test_vec_cc5a10.cpp)
target_link_libraries(test_vec_cc5a10 sotc_core)
add_test(NAME vec_cc5a10 COMMAND test_vec_cc5a10)

add_executable(test_vec_cc07d0 tests/test_vec_cc07d0.cpp)
target_link_libraries(test_vec_cc07d0 sotc_core)
add_test(NAME vec_cc07d0 COMMAND test_vec_cc07d0)

add_executable(test_vec_cbb2c0 tests/test_vec_cbb2c0.cpp)
target_link_libraries(test_vec_cbb2c0 sotc_core)
add_test(NAME vec_cbb2c0 COMMAND test_vec_cbb2c0)

add_executable(test_vec_cb5cb0 tests/test_vec_cb5cb0.cpp)
target_link_libraries(test_vec_cb5cb0 sotc_core)
add_test(NAME vec_cb5cb0 COMMAND test_vec_cb5cb0)

add_executable(test_vec_caeed0 tests/test_vec_caeed0.cpp)
target_link_libraries(test_vec_caeed0 sotc_core)
add_test(NAME vec_caeed0 COMMAND test_vec_caeed0)

add_executable(test_vec_cae740 tests/test_vec_cae740.cpp)
target_link_libraries(test_vec_cae740 sotc_core)
add_test(NAME vec_cae740 COMMAND test_vec_cae740)

add_executable(test_vec_cae240 tests/test_vec_cae240.cpp)
target_link_libraries(test_vec_cae240 sotc_core)
add_test(NAME vec_cae240 COMMAND test_vec_cae240)

add_executable(test_vec_cad450 tests/test_vec_cad450.cpp)
target_link_libraries(test_vec_cad450 sotc_core)
add_test(NAME vec_cad450 COMMAND test_vec_cad450)

add_executable(test_vec_ca4dc0 tests/test_vec_ca4dc0.cpp)
target_link_libraries(test_vec_ca4dc0 sotc_core)
add_test(NAME vec_ca4dc0 COMMAND test_vec_ca4dc0)

add_executable(test_vec_c9e8e0 tests/test_vec_c9e8e0.cpp)
target_link_libraries(test_vec_c9e8e0 sotc_core)
add_test(NAME vec_c9e8e0 COMMAND test_vec_c9e8e0)

add_executable(test_vec_c91b10 tests/test_vec_c91b10.cpp)
target_link_libraries(test_vec_c91b10 sotc_core)
add_test(NAME vec_c91b10 COMMAND test_vec_c91b10)

add_executable(test_vec_c891e0 tests/test_vec_c891e0.cpp)
target_link_libraries(test_vec_c891e0 sotc_core)
add_test(NAME vec_c891e0 COMMAND test_vec_c891e0)

add_executable(test_vec_c7de60 tests/test_vec_c7de60.cpp)
target_link_libraries(test_vec_c7de60 sotc_core)
add_test(NAME vec_c7de60 COMMAND test_vec_c7de60)

add_executable(test_vec_c7c310 tests/test_vec_c7c310.cpp)
target_link_libraries(test_vec_c7c310 sotc_core)
add_test(NAME vec_c7c310 COMMAND test_vec_c7c310)

add_executable(test_vec_c786c0 tests/test_vec_c786c0.cpp)
target_link_libraries(test_vec_c786c0 sotc_core)
add_test(NAME vec_c786c0 COMMAND test_vec_c786c0)

add_executable(test_vec_c77070 tests/test_vec_c77070.cpp)
target_link_libraries(test_vec_c77070 sotc_core)
add_test(NAME vec_c77070 COMMAND test_vec_c77070)

add_executable(test_vec_c741c0 tests/test_vec_c741c0.cpp)
target_link_libraries(test_vec_c741c0 sotc_core)
add_test(NAME vec_c741c0 COMMAND test_vec_c741c0)

add_executable(test_vec_c70e20 tests/test_vec_c70e20.cpp)
target_link_libraries(test_vec_c70e20 sotc_core)
add_test(NAME vec_c70e20 COMMAND test_vec_c70e20)

add_executable(test_vec_c6fdf0 tests/test_vec_c6fdf0.cpp)
target_link_libraries(test_vec_c6fdf0 sotc_core)
add_test(NAME vec_c6fdf0 COMMAND test_vec_c6fdf0)

add_executable(test_vec_c6d910 tests/test_vec_c6d910.cpp)
target_link_libraries(test_vec_c6d910 sotc_core)
add_test(NAME vec_c6d910 COMMAND test_vec_c6d910)

add_executable(test_vec_c6bad0 tests/test_vec_c6bad0.cpp)
target_link_libraries(test_vec_c6bad0 sotc_core)
add_test(NAME vec_c6bad0 COMMAND test_vec_c6bad0)

add_executable(test_vec_c69f70 tests/test_vec_c69f70.cpp)
target_link_libraries(test_vec_c69f70 sotc_core)
add_test(NAME vec_c69f70 COMMAND test_vec_c69f70)

add_executable(test_vec_c686e0 tests/test_vec_c686e0.cpp)
target_link_libraries(test_vec_c686e0 sotc_core)
add_test(NAME vec_c686e0 COMMAND test_vec_c686e0)

add_executable(test_vec_c66580 tests/test_vec_c66580.cpp)
target_link_libraries(test_vec_c66580 sotc_core)
add_test(NAME vec_c66580 COMMAND test_vec_c66580)

add_executable(test_vec_c65f80 tests/test_vec_c65f80.cpp)
target_link_libraries(test_vec_c65f80 sotc_core)
add_test(NAME vec_c65f80 COMMAND test_vec_c65f80)

add_executable(test_vec_c65da0 tests/test_vec_c65da0.cpp)
target_link_libraries(test_vec_c65da0 sotc_core)
add_test(NAME vec_c65da0 COMMAND test_vec_c65da0)

add_executable(test_vec_c64060 tests/test_vec_c64060.cpp)
target_link_libraries(test_vec_c64060 sotc_core)
add_test(NAME vec_c64060 COMMAND test_vec_c64060)

add_executable(test_vec_c61670 tests/test_vec_c61670.cpp)
target_link_libraries(test_vec_c61670 sotc_core)
add_test(NAME vec_c61670 COMMAND test_vec_c61670)

add_executable(test_vec_c5f6f0 tests/test_vec_c5f6f0.cpp)
target_link_libraries(test_vec_c5f6f0 sotc_core)
add_test(NAME vec_c5f6f0 COMMAND test_vec_c5f6f0)

add_executable(test_vec_c5c320 tests/test_vec_c5c320.cpp)
target_link_libraries(test_vec_c5c320 sotc_core)
add_test(NAME vec_c5c320 COMMAND test_vec_c5c320)

add_executable(test_vec_c5c1a0 tests/test_vec_c5c1a0.cpp)
target_link_libraries(test_vec_c5c1a0 sotc_core)
add_test(NAME vec_c5c1a0 COMMAND test_vec_c5c1a0)

add_executable(test_vec_c5c040 tests/test_vec_c5c040.cpp)
target_link_libraries(test_vec_c5c040 sotc_core)
add_test(NAME vec_c5c040 COMMAND test_vec_c5c040)

add_executable(test_vec_c5a400 tests/test_vec_c5a400.cpp)
target_link_libraries(test_vec_c5a400 sotc_core)
add_test(NAME vec_c5a400 COMMAND test_vec_c5a400)

add_executable(test_vec_c5a100 tests/test_vec_c5a100.cpp)
target_link_libraries(test_vec_c5a100 sotc_core)
add_test(NAME vec_c5a100 COMMAND test_vec_c5a100)

add_executable(test_vec_c555c0 tests/test_vec_c555c0.cpp)
target_link_libraries(test_vec_c555c0 sotc_core)
add_test(NAME vec_c555c0 COMMAND test_vec_c555c0)

add_executable(test_vec_c54d20 tests/test_vec_c54d20.cpp)
target_link_libraries(test_vec_c54d20 sotc_core)
add_test(NAME vec_c54d20 COMMAND test_vec_c54d20)

add_executable(test_vec_c54830 tests/test_vec_c54830.cpp)
target_link_libraries(test_vec_c54830 sotc_core)
add_test(NAME vec_c54830 COMMAND test_vec_c54830)

add_executable(test_vec_c53db0 tests/test_vec_c53db0.cpp)
target_link_libraries(test_vec_c53db0 sotc_core)
add_test(NAME vec_c53db0 COMMAND test_vec_c53db0)

add_executable(test_vec_c4e5e0 tests/test_vec_c4e5e0.cpp)
target_link_libraries(test_vec_c4e5e0 sotc_core)
add_test(NAME vec_c4e5e0 COMMAND test_vec_c4e5e0)

add_executable(test_vec_c4aa80 tests/test_vec_c4aa80.cpp)
target_link_libraries(test_vec_c4aa80 sotc_core)
add_test(NAME vec_c4aa80 COMMAND test_vec_c4aa80)

add_executable(test_vec_c47e00 tests/test_vec_c47e00.cpp)
target_link_libraries(test_vec_c47e00 sotc_core)
add_test(NAME vec_c47e00 COMMAND test_vec_c47e00)

add_executable(test_vec_c451d0 tests/test_vec_c451d0.cpp)
target_link_libraries(test_vec_c451d0 sotc_core)
add_test(NAME vec_c451d0 COMMAND test_vec_c451d0)

add_executable(test_vec_c44830 tests/test_vec_c44830.cpp)
target_link_libraries(test_vec_c44830 sotc_core)
add_test(NAME vec_c44830 COMMAND test_vec_c44830)

add_executable(test_vec_c42b00 tests/test_vec_c42b00.cpp)
target_link_libraries(test_vec_c42b00 sotc_core)
add_test(NAME vec_c42b00 COMMAND test_vec_c42b00)

add_executable(test_vec_c3eaa0 tests/test_vec_c3eaa0.cpp)
target_link_libraries(test_vec_c3eaa0 sotc_core)
add_test(NAME vec_c3eaa0 COMMAND test_vec_c3eaa0)

add_executable(test_vec_c3cf00 tests/test_vec_c3cf00.cpp)
target_link_libraries(test_vec_c3cf00 sotc_core)
add_test(NAME vec_c3cf00 COMMAND test_vec_c3cf00)

add_executable(test_vec_c37020 tests/test_vec_c37020.cpp)
target_link_libraries(test_vec_c37020 sotc_core)
add_test(NAME vec_c37020 COMMAND test_vec_c37020)

add_executable(test_vec_c36a80 tests/test_vec_c36a80.cpp)
target_link_libraries(test_vec_c36a80 sotc_core)
add_test(NAME vec_c36a80 COMMAND test_vec_c36a80)

add_executable(test_vec_c23650 tests/test_vec_c23650.cpp)
target_link_libraries(test_vec_c23650 sotc_core)
add_test(NAME vec_c23650 COMMAND test_vec_c23650)

add_executable(test_vec_c22300 tests/test_vec_c22300.cpp)
target_link_libraries(test_vec_c22300 sotc_core)
add_test(NAME vec_c22300 COMMAND test_vec_c22300)

add_executable(test_vec_c20710 tests/test_vec_c20710.cpp)
target_link_libraries(test_vec_c20710 sotc_core)
add_test(NAME vec_c20710 COMMAND test_vec_c20710)

add_executable(test_vec_c1a9a0 tests/test_vec_c1a9a0.cpp)
target_link_libraries(test_vec_c1a9a0 sotc_core)
add_test(NAME vec_c1a9a0 COMMAND test_vec_c1a9a0)

add_executable(test_vec_c191a0 tests/test_vec_c191a0.cpp)
target_link_libraries(test_vec_c191a0 sotc_core)
add_test(NAME vec_c191a0 COMMAND test_vec_c191a0)

add_executable(test_vec_c17a70 tests/test_vec_c17a70.cpp)
target_link_libraries(test_vec_c17a70 sotc_core)
add_test(NAME vec_c17a70 COMMAND test_vec_c17a70)

add_executable(test_vec_c152c0 tests/test_vec_c152c0.cpp)
target_link_libraries(test_vec_c152c0 sotc_core)
add_test(NAME vec_c152c0 COMMAND test_vec_c152c0)

add_executable(test_vec_c14f60 tests/test_vec_c14f60.cpp)
target_link_libraries(test_vec_c14f60 sotc_core)
add_test(NAME vec_c14f60 COMMAND test_vec_c14f60)

add_executable(test_vec_c0d720 tests/test_vec_c0d720.cpp)
target_link_libraries(test_vec_c0d720 sotc_core)
add_test(NAME vec_c0d720 COMMAND test_vec_c0d720)

add_executable(test_vec_c0bea0 tests/test_vec_c0bea0.cpp)
target_link_libraries(test_vec_c0bea0 sotc_core)
add_test(NAME vec_c0bea0 COMMAND test_vec_c0bea0)

add_executable(test_vec_c0bbd0 tests/test_vec_c0bbd0.cpp)
target_link_libraries(test_vec_c0bbd0 sotc_core)
add_test(NAME vec_c0bbd0 COMMAND test_vec_c0bbd0)

add_executable(test_vec_c0bb00 tests/test_vec_c0bb00.cpp)
target_link_libraries(test_vec_c0bb00 sotc_core)
add_test(NAME vec_c0bb00 COMMAND test_vec_c0bb00)

add_executable(test_vec_c0baa0 tests/test_vec_c0baa0.cpp)
target_link_libraries(test_vec_c0baa0 sotc_core)
add_test(NAME vec_c0baa0 COMMAND test_vec_c0baa0)

add_executable(test_vec_c0aae0 tests/test_vec_c0aae0.cpp)
target_link_libraries(test_vec_c0aae0 sotc_core)
add_test(NAME vec_c0aae0 COMMAND test_vec_c0aae0)

add_executable(test_vec_c09070 tests/test_vec_c09070.cpp)
target_link_libraries(test_vec_c09070 sotc_core)
add_test(NAME vec_c09070 COMMAND test_vec_c09070)

add_executable(test_vec_c060a0 tests/test_vec_c060a0.cpp)
target_link_libraries(test_vec_c060a0 sotc_core)
add_test(NAME vec_c060a0 COMMAND test_vec_c060a0)

add_executable(test_vec_bd4dd0 tests/test_vec_bd4dd0.cpp)
target_link_libraries(test_vec_bd4dd0 sotc_core)
add_test(NAME vec_bd4dd0 COMMAND test_vec_bd4dd0)

add_executable(test_vec_bd60a0 tests/test_vec_bd60a0.cpp)
target_link_libraries(test_vec_bd60a0 sotc_core)
add_test(NAME vec_bd60a0 COMMAND test_vec_bd60a0)

add_executable(test_vec_bd7950 tests/test_vec_bd7950.cpp)
target_link_libraries(test_vec_bd7950 sotc_core)
add_test(NAME vec_bd7950 COMMAND test_vec_bd7950)

add_executable(test_vec_bd32f0 tests/test_vec_bd32f0.cpp)
target_link_libraries(test_vec_bd32f0 sotc_core)
add_test(NAME vec_bd32f0 COMMAND test_vec_bd32f0)

add_executable(test_vec_bd3190 tests/test_vec_bd3190.cpp)
target_link_libraries(test_vec_bd3190 sotc_core)
add_test(NAME vec_bd3190 COMMAND test_vec_bd3190)

add_executable(test_vec_bd06c0 tests/test_vec_bd06c0.cpp)
target_link_libraries(test_vec_bd06c0 sotc_core)
add_test(NAME vec_bd06c0 COMMAND test_vec_bd06c0)

add_executable(test_vec_bceda0 tests/test_vec_bceda0.cpp)
target_link_libraries(test_vec_bceda0 sotc_core)
add_test(NAME vec_bceda0 COMMAND test_vec_bceda0)

add_executable(test_vec_bccc00 tests/test_vec_bccc00.cpp)
target_link_libraries(test_vec_bccc00 sotc_core)
add_test(NAME vec_bccc00 COMMAND test_vec_bccc00)

add_executable(test_vec_bcc960 tests/test_vec_bcc960.cpp)
target_link_libraries(test_vec_bcc960 sotc_core)
add_test(NAME vec_bcc960 COMMAND test_vec_bcc960)

add_executable(test_vec_bcc4e0 tests/test_vec_bcc4e0.cpp)
target_link_libraries(test_vec_bcc4e0 sotc_core)
add_test(NAME vec_bcc4e0 COMMAND test_vec_bcc4e0)

add_executable(test_vec_bcc3f0 tests/test_vec_bcc3f0.cpp)
target_link_libraries(test_vec_bcc3f0 sotc_core)
add_test(NAME vec_bcc3f0 COMMAND test_vec_bcc3f0)

add_executable(test_vec_bc8210 tests/test_vec_bc8210.cpp)
target_link_libraries(test_vec_bc8210 sotc_core)
add_test(NAME vec_bc8210 COMMAND test_vec_bc8210)

add_executable(test_vec_bc7280 tests/test_vec_bc7280.cpp)
target_link_libraries(test_vec_bc7280 sotc_core)
add_test(NAME vec_bc7280 COMMAND test_vec_bc7280)

add_executable(test_vec_bc5b00 tests/test_vec_bc5b00.cpp)
target_link_libraries(test_vec_bc5b00 sotc_core)
add_test(NAME vec_bc5b00 COMMAND test_vec_bc5b00)

add_executable(test_vec_bc2800 tests/test_vec_bc2800.cpp)
target_link_libraries(test_vec_bc2800 sotc_core)
add_test(NAME vec_bc2800 COMMAND test_vec_bc2800)

add_executable(test_vec_d85490 tests/test_vec_d85490.cpp)
target_link_libraries(test_vec_d85490 sotc_core)
add_test(NAME vec_d85490 COMMAND test_vec_d85490)

add_executable(test_vec_dbfa60 tests/test_vec_dbfa60.cpp)
target_link_libraries(test_vec_dbfa60 sotc_core)
add_test(NAME vec_dbfa60 COMMAND test_vec_dbfa60)

add_executable(test_vec_e094e0 tests/test_vec_e094e0.cpp)
target_link_libraries(test_vec_e094e0 sotc_core)
add_test(NAME vec_e094e0 COMMAND test_vec_e094e0)

add_executable(test_vec_be5410 tests/test_vec_be5410.cpp)
target_link_libraries(test_vec_be5410 sotc_core)
add_test(NAME vec_be5410 COMMAND test_vec_be5410)

add_executable(test_vec_be5270 tests/test_vec_be5270.cpp)
target_link_libraries(test_vec_be5270 sotc_core)
add_test(NAME vec_be5270 COMMAND test_vec_be5270)

add_executable(test_vec_be50a0 tests/test_vec_be50a0.cpp)
target_link_libraries(test_vec_be50a0 sotc_core)
add_test(NAME vec_be50a0 COMMAND test_vec_be50a0)

add_executable(test_vec_be4ff0 tests/test_vec_be4ff0.cpp)
target_link_libraries(test_vec_be4ff0 sotc_core)
add_test(NAME vec_be4ff0 COMMAND test_vec_be4ff0)

add_executable(test_vec_bd7a80 tests/test_vec_bd7a80.cpp)
target_link_libraries(test_vec_bd7a80 sotc_core)
add_test(NAME vec_bd7a80 COMMAND test_vec_bd7a80)

add_executable(test_vec_bc07f0 tests/test_vec_bc07f0.cpp)
target_link_libraries(test_vec_bc07f0 sotc_core)
add_test(NAME vec_bc07f0 COMMAND test_vec_bc07f0)

add_executable(test_vec_bc0670 tests/test_vec_bc0670.cpp)
target_link_libraries(test_vec_bc0670 sotc_core)
add_test(NAME vec_bc0670 COMMAND test_vec_bc0670)

add_executable(test_vec_bbf580 tests/test_vec_bbf580.cpp)
target_link_libraries(test_vec_bbf580 sotc_core)
add_test(NAME vec_bbf580 COMMAND test_vec_bbf580)

add_executable(test_vec_bbd5e0 tests/test_vec_bbd5e0.cpp)
target_link_libraries(test_vec_bbd5e0 sotc_core)
add_test(NAME vec_bbd5e0 COMMAND test_vec_bbd5e0)

add_executable(test_vec_bbba10 tests/test_vec_bbba10.cpp)
target_link_libraries(test_vec_bbba10 sotc_core)
add_test(NAME vec_bbba10 COMMAND test_vec_bbba10)

add_executable(test_vec_bb9740 tests/test_vec_bb9740.cpp)
target_link_libraries(test_vec_bb9740 sotc_core)
add_test(NAME vec_bb9740 COMMAND test_vec_bb9740)

add_executable(test_save_checksum tests/test_save_checksum.cpp)
target_link_libraries(test_save_checksum sotc_core)
add_test(NAME save_checksum COMMAND test_save_checksum)

add_executable(test_recursive_mutex tests/test_recursive_mutex.cpp)
target_link_libraries(test_recursive_mutex sotc_core)
add_test(NAME recursive_mutex COMMAND test_recursive_mutex)

add_executable(test_critsec_3b310 tests/test_critsec_3b310.cpp)
target_link_libraries(test_critsec_3b310 sotc_core)
add_test(NAME critsec_3b310 COMMAND test_critsec_3b310)

# RE-02-CRITSEC-3B310-3B380-NATIVE-01: registered 0x3b380 identity teardown for slots 414 and 535.
add_executable(test_critsec_slots_teardown tests/test_critsec_slots_teardown.cpp)
target_link_libraries(test_critsec_slots_teardown sotc_core)
add_test(NAME critsec_slots_teardown COMMAND test_critsec_slots_teardown)

add_executable(test_init_499f80_body tests/test_init_499f80_body.cpp)
target_link_libraries(test_init_499f80_body sotc_core)
add_test(NAME init_499f80_body COMMAND test_init_499f80_body)

add_executable(test_config_bool_lookup_1e4c0 tests/test_config_bool_lookup_1e4c0.cpp)
target_link_libraries(test_config_bool_lookup_1e4c0 sotc_core)
add_test(NAME config_bool_lookup_1e4c0 COMMAND test_config_bool_lookup_1e4c0)

add_executable(test_init_continue_ab_native tests/test_init_continue_ab_native.cpp)
target_link_libraries(test_init_continue_ab_native sotc_core)
add_test(NAME init_continue_ab_native COMMAND test_init_continue_ab_native)

add_executable(test_lazy_bind_91210 tests/test_lazy_bind_91210.cpp)
target_link_libraries(test_lazy_bind_91210 sotc_core)
add_test(NAME lazy_bind_91210 COMMAND test_lazy_bind_91210)

add_executable(test_init_continue_c_lazy_bind tests/test_init_continue_c_lazy_bind.cpp)
target_link_libraries(test_init_continue_c_lazy_bind sotc_core)
add_test(NAME init_continue_c_lazy_bind COMMAND test_init_continue_c_lazy_bind)

add_executable(test_init_continue_c_after_merge tests/test_init_continue_c_after_merge.cpp)
target_link_libraries(test_init_continue_c_after_merge sotc_core)
add_test(NAME init_continue_c_after_merge COMMAND test_init_continue_c_after_merge)

add_executable(test_post_base_499f80_native_body tests/test_post_base_499f80_native_body.cpp)
target_link_libraries(test_post_base_499f80_native_body sotc_core)
add_test(NAME post_base_499f80_native_body COMMAND test_post_base_499f80_native_body)

add_executable(test_vec_bb82d0 tests/test_vec_bb82d0.cpp)
target_link_libraries(test_vec_bb82d0 sotc_core)
add_test(NAME vec_bb82d0 COMMAND test_vec_bb82d0)

add_executable(test_vec_bb7990 tests/test_vec_bb7990.cpp)
target_link_libraries(test_vec_bb7990 sotc_core)
add_test(NAME vec_bb7990 COMMAND test_vec_bb7990)

add_executable(test_vec_bacfb0 tests/test_vec_bacfb0.cpp)
target_link_libraries(test_vec_bacfb0 sotc_core)
add_test(NAME vec_bacfb0 COMMAND test_vec_bacfb0)

add_executable(test_vec_bace80 tests/test_vec_bace80.cpp)
target_link_libraries(test_vec_bace80 sotc_core)
add_test(NAME vec_bace80 COMMAND test_vec_bace80)

add_executable(test_vec_bace50 tests/test_vec_bace50.cpp)
target_link_libraries(test_vec_bace50 sotc_core)
add_test(NAME vec_bace50 COMMAND test_vec_bace50)

add_executable(test_vec_bacb30 tests/test_vec_bacb30.cpp)
target_link_libraries(test_vec_bacb30 sotc_core)
add_test(NAME vec_bacb30 COMMAND test_vec_bacb30)

add_executable(test_vec_bab950 tests/test_vec_bab950.cpp)
target_link_libraries(test_vec_bab950 sotc_core)
add_test(NAME vec_bab950 COMMAND test_vec_bab950)

add_executable(test_vec_ba3ef0 tests/test_vec_ba3ef0.cpp)
target_link_libraries(test_vec_ba3ef0 sotc_core)
add_test(NAME vec_ba3ef0 COMMAND test_vec_ba3ef0)

add_executable(test_vec_b950e0 tests/test_vec_b950e0.cpp)
target_link_libraries(test_vec_b950e0 sotc_core)
add_test(NAME vec_b950e0 COMMAND test_vec_b950e0)

add_executable(test_vec_b8d170 tests/test_vec_b8d170.cpp)
target_link_libraries(test_vec_b8d170 sotc_core)
add_test(NAME vec_b8d170 COMMAND test_vec_b8d170)

add_executable(test_vec_b89eb0 tests/test_vec_b89eb0.cpp)
target_link_libraries(test_vec_b89eb0 sotc_core)
add_test(NAME vec_b89eb0 COMMAND test_vec_b89eb0)

add_executable(test_vec_b84580 tests/test_vec_b84580.cpp)
target_link_libraries(test_vec_b84580 sotc_core)
add_test(NAME vec_b84580 COMMAND test_vec_b84580)

add_executable(test_vec_b7b270 tests/test_vec_b7b270.cpp)
target_link_libraries(test_vec_b7b270 sotc_core)
add_test(NAME vec_b7b270 COMMAND test_vec_b7b270)

add_executable(test_vec_b77e90 tests/test_vec_b77e90.cpp)
target_link_libraries(test_vec_b77e90 sotc_core)
add_test(NAME vec_b77e90 COMMAND test_vec_b77e90)

add_executable(test_vec_b77910 tests/test_vec_b77910.cpp)
target_link_libraries(test_vec_b77910 sotc_core)
add_test(NAME vec_b77910 COMMAND test_vec_b77910)

add_executable(test_vec_b77740 tests/test_vec_b77740.cpp)
target_link_libraries(test_vec_b77740 sotc_core)
add_test(NAME vec_b77740 COMMAND test_vec_b77740)

add_executable(test_vec_b71f80 tests/test_vec_b71f80.cpp)
target_link_libraries(test_vec_b71f80 sotc_core)
add_test(NAME vec_b71f80 COMMAND test_vec_b71f80)

add_executable(test_vec_b6e6d0 tests/test_vec_b6e6d0.cpp)
target_link_libraries(test_vec_b6e6d0 sotc_core)
add_test(NAME vec_b6e6d0 COMMAND test_vec_b6e6d0)

add_executable(test_vec_b67a70 tests/test_vec_b67a70.cpp)
target_link_libraries(test_vec_b67a70 sotc_core)
add_test(NAME vec_b67a70 COMMAND test_vec_b67a70)

add_executable(test_vec_b5ecf0 tests/test_vec_b5ecf0.cpp)
target_link_libraries(test_vec_b5ecf0 sotc_core)
add_test(NAME vec_b5ecf0 COMMAND test_vec_b5ecf0)

add_executable(test_vec_b58690 tests/test_vec_b58690.cpp)
target_link_libraries(test_vec_b58690 sotc_core)
add_test(NAME vec_b58690 COMMAND test_vec_b58690)

add_executable(test_vec_b57360 tests/test_vec_b57360.cpp)
target_link_libraries(test_vec_b57360 sotc_core)
add_test(NAME vec_b57360 COMMAND test_vec_b57360)

add_executable(test_vec_b4ef70 tests/test_vec_b4ef70.cpp)
target_link_libraries(test_vec_b4ef70 sotc_core)
add_test(NAME vec_b4ef70 COMMAND test_vec_b4ef70)

add_executable(test_vec_b494d0 tests/test_vec_b494d0.cpp)
target_link_libraries(test_vec_b494d0 sotc_core)
add_test(NAME vec_b494d0 COMMAND test_vec_b494d0)

add_executable(test_vec_b47100 tests/test_vec_b47100.cpp)
target_link_libraries(test_vec_b47100 sotc_core)
add_test(NAME vec_b47100 COMMAND test_vec_b47100)

add_executable(test_vec_b44580 tests/test_vec_b44580.cpp)
target_link_libraries(test_vec_b44580 sotc_core)
add_test(NAME vec_b44580 COMMAND test_vec_b44580)

add_executable(test_vec_b33bd0 tests/test_vec_b33bd0.cpp)
target_link_libraries(test_vec_b33bd0 sotc_core)
add_test(NAME vec_b33bd0 COMMAND test_vec_b33bd0)

add_executable(test_guarded_dd91b0 tests/test_guarded_dd91b0.cpp)
target_link_libraries(test_guarded_dd91b0 sotc_core)
add_test(NAME guarded_dd91b0 COMMAND test_guarded_dd91b0)

add_executable(test_guarded_dd7e30 tests/test_guarded_dd7e30.cpp)
target_link_libraries(test_guarded_dd7e30 sotc_core)
add_test(NAME guarded_dd7e30 COMMAND test_guarded_dd7e30)

add_executable(test_guarded_dd4210 tests/test_guarded_dd4210.cpp)
target_link_libraries(test_guarded_dd4210 sotc_core)
add_test(NAME guarded_dd4210 COMMAND test_guarded_dd4210)

add_executable(test_guarded_d39b00 tests/test_guarded_d39b00.cpp)
target_link_libraries(test_guarded_d39b00 sotc_core)
add_test(NAME guarded_d39b00 COMMAND test_guarded_d39b00)

add_executable(test_guarded_d366d0 tests/test_guarded_d366d0.cpp)
target_link_libraries(test_guarded_d366d0 sotc_core)
add_test(NAME guarded_d366d0 COMMAND test_guarded_d366d0)

add_executable(test_guarded_d33b50 tests/test_guarded_d33b50.cpp)
target_link_libraries(test_guarded_d33b50 sotc_core)
add_test(NAME guarded_d33b50 COMMAND test_guarded_d33b50)

add_executable(test_avx_d8fa40 tests/test_avx_d8fa40.cpp)
target_link_libraries(test_avx_d8fa40 sotc_core)
add_test(NAME avx_d8fa40 COMMAND test_avx_d8fa40)

add_executable(test_avx_d86ed0 tests/test_avx_d86ed0.cpp)
target_link_libraries(test_avx_d86ed0 sotc_core)
add_test(NAME avx_d86ed0 COMMAND test_avx_d86ed0)

add_executable(test_avx_d5a280 tests/test_avx_d5a280.cpp)
target_link_libraries(test_avx_d5a280 sotc_core)
add_test(NAME avx_d5a280 COMMAND test_avx_d5a280)

add_executable(test_leaf_cab080 tests/test_leaf_cab080.cpp)
target_link_libraries(test_leaf_cab080 sotc_core)
add_test(NAME leaf_cab080 COMMAND test_leaf_cab080)

add_executable(test_leaf_ca18d0 tests/test_leaf_ca18d0.cpp)
target_link_libraries(test_leaf_ca18d0 sotc_core)
add_test(NAME leaf_ca18d0 COMMAND test_leaf_ca18d0)

add_executable(test_leaf_c94ae0 tests/test_leaf_c94ae0.cpp)
target_link_libraries(test_leaf_c94ae0 sotc_core)
add_test(NAME leaf_c94ae0 COMMAND test_leaf_c94ae0)

add_executable(test_leaf_b692a0 tests/test_leaf_b692a0.cpp)
target_link_libraries(test_leaf_b692a0 sotc_core)
add_test(NAME leaf_b692a0 COMMAND test_leaf_b692a0)

add_executable(test_leaf_c811b0 tests/test_leaf_c811b0.cpp)
target_link_libraries(test_leaf_c811b0 sotc_core)
add_test(NAME leaf_c811b0 COMMAND test_leaf_c811b0)

add_executable(test_leaf_8a0440 tests/test_leaf_8a0440.cpp)
target_link_libraries(test_leaf_8a0440 sotc_core)
add_test(NAME leaf_8a0440 COMMAND test_leaf_8a0440)

add_executable(test_leaf_c05980 tests/test_leaf_c05980.cpp)
target_link_libraries(test_leaf_c05980 sotc_core)
add_test(NAME leaf_c05980 COMMAND test_leaf_c05980)

add_executable(test_leaf_c6f520 tests/test_leaf_c6f520.cpp)
target_link_libraries(test_leaf_c6f520 sotc_core)
add_test(NAME leaf_c6f520 COMMAND test_leaf_c6f520)

add_executable(test_leaf_c6e9d0 tests/test_leaf_c6e9d0.cpp)
target_link_libraries(test_leaf_c6e9d0 sotc_core)
add_test(NAME leaf_c6e9d0 COMMAND test_leaf_c6e9d0)

add_executable(test_leaf_a816d0 tests/test_leaf_a816d0.cpp)
target_link_libraries(test_leaf_a816d0 sotc_core)
add_test(NAME leaf_a816d0 COMMAND test_leaf_a816d0)

add_executable(test_leaf_3ca7c0 tests/test_leaf_3ca7c0.cpp)
target_link_libraries(test_leaf_3ca7c0 sotc_core)
add_test(NAME leaf_3ca7c0 COMMAND test_leaf_3ca7c0)

add_executable(test_leaf_c0d380 tests/test_leaf_c0d380.cpp)
target_link_libraries(test_leaf_c0d380 sotc_core)
add_test(NAME leaf_c0d380 COMMAND test_leaf_c0d380)

add_executable(test_leaf_c675f0 tests/test_leaf_c675f0.cpp)
target_link_libraries(test_leaf_c675f0 sotc_core)
add_test(NAME leaf_c675f0 COMMAND test_leaf_c675f0)

add_executable(test_leaf_c16b30 tests/test_leaf_c16b30.cpp)
target_link_libraries(test_leaf_c16b30 sotc_core)
add_test(NAME leaf_c16b30 COMMAND test_leaf_c16b30)

add_executable(test_leaf_8f3d90 tests/test_leaf_8f3d90.cpp)
target_link_libraries(test_leaf_8f3d90 sotc_core)
add_test(NAME leaf_8f3d90 COMMAND test_leaf_8f3d90)

add_executable(test_leaf_163a40 tests/test_leaf_163a40.cpp)
target_link_libraries(test_leaf_163a40 sotc_core)
add_test(NAME leaf_163a40 COMMAND test_leaf_163a40)

add_executable(test_leaf_9b75a0 tests/test_leaf_9b75a0.cpp)
target_link_libraries(test_leaf_9b75a0 sotc_core)
add_test(NAME leaf_9b75a0 COMMAND test_leaf_9b75a0)

add_executable(test_leaf_575d40 tests/test_leaf_575d40.cpp)
target_link_libraries(test_leaf_575d40 sotc_core)
add_test(NAME leaf_575d40 COMMAND test_leaf_575d40)

add_executable(test_leaf_99350 tests/test_leaf_99350.cpp)
target_link_libraries(test_leaf_99350 sotc_core)
add_test(NAME leaf_99350 COMMAND test_leaf_99350)

add_executable(test_leaf_83d800 tests/test_leaf_83d800.cpp)
target_link_libraries(test_leaf_83d800 sotc_core)
add_test(NAME leaf_83d800 COMMAND test_leaf_83d800)

add_executable(test_leaf_7ce8d0 tests/test_leaf_7ce8d0.cpp)
target_link_libraries(test_leaf_7ce8d0 sotc_core)
add_test(NAME leaf_7ce8d0 COMMAND test_leaf_7ce8d0)

add_executable(test_leaf_e71990 tests/test_leaf_e71990.cpp)
target_link_libraries(test_leaf_e71990 sotc_core)
add_test(NAME leaf_e71990 COMMAND test_leaf_e71990)

add_executable(test_leaf_df94c0 tests/test_leaf_df94c0.cpp)
target_link_libraries(test_leaf_df94c0 sotc_core)
add_test(NAME leaf_df94c0 COMMAND test_leaf_df94c0)

add_executable(test_leaf_de0160 tests/test_leaf_de0160.cpp)
target_link_libraries(test_leaf_de0160 sotc_core)
add_test(NAME leaf_de0160 COMMAND test_leaf_de0160)

add_executable(test_fnv_cstr tests/test_fnv_cstr.cpp)
target_link_libraries(test_fnv_cstr sotc_core)
add_test(NAME fnv_cstr COMMAND test_fnv_cstr)

add_executable(test_fnv_path_cstr_162020 tests/test_fnv_path_cstr_162020.cpp)
target_link_libraries(test_fnv_path_cstr_162020 sotc_core)
add_test(NAME fnv_path_cstr_162020 COMMAND test_fnv_path_cstr_162020)

add_executable(test_hashed_name_ctors tests/test_hashed_name_ctors.cpp)
target_link_libraries(test_hashed_name_ctors sotc_core)
add_test(NAME hashed_name_ctors COMMAND test_hashed_name_ctors)

add_executable(test_guarded_static_helpers tests/test_guarded_static_helpers.cpp)
target_link_libraries(test_guarded_static_helpers sotc_core)
add_test(NAME guarded_static_helpers COMMAND test_guarded_static_helpers)
add_executable(test_const_table_ee1130 tests/test_const_table_ee1130.cpp)
target_link_libraries(test_const_table_ee1130 sotc_core)
add_test(NAME const_table_ee1130 COMMAND test_const_table_ee1130)

add_executable(test_input_table_551140 tests/test_input_table_551140.cpp)
target_link_libraries(test_input_table_551140 sotc_core)
add_test(NAME input_table_551140 COMMAND test_input_table_551140)

add_executable(test_descriptor_registration tests/test_descriptor_registration.cpp)
target_link_libraries(test_descriptor_registration sotc_core)
add_test(NAME descriptor_registration COMMAND test_descriptor_registration)

add_executable(test_save_chunk_header tests/test_save_chunk_header.cpp)
target_link_libraries(test_save_chunk_header sotc_core)
add_test(NAME save_chunk_header COMMAND test_save_chunk_header)

add_executable(test_save_checksum_edges tests/test_save_checksum_edges.cpp)
target_link_libraries(test_save_checksum_edges sotc_core)
add_test(NAME save_checksum_edges COMMAND test_save_checksum_edges)

add_executable(test_name_hash tests/test_name_hash.cpp)
target_link_libraries(test_name_hash sotc_core)
add_test(NAME name_hash COMMAND test_name_hash)

add_executable(test_save_stream tests/test_save_stream.cpp)
target_link_libraries(test_save_stream sotc_core)
add_test(NAME save_stream COMMAND test_save_stream)

add_executable(test_save_stream_file tests/test_save_stream_file.cpp)
target_link_libraries(test_save_stream_file sotc_core)
add_test(NAME save_stream_file COMMAND test_save_stream_file)

add_executable(test_save_chunk_codec tests/test_save_chunk_codec.cpp)
target_link_libraries(test_save_chunk_codec sotc_core)
add_test(NAME save_chunk_codec COMMAND test_save_chunk_codec)

add_executable(test_save_transaction tests/test_save_transaction.cpp)
target_link_libraries(test_save_transaction sotc_core)
add_test(NAME save_transaction COMMAND test_save_transaction)

add_executable(test_save_metadata tests/test_save_metadata.cpp)
target_link_libraries(test_save_metadata sotc_core)
add_test(NAME save_metadata COMMAND test_save_metadata)

add_executable(test_atomic_add_ref_14ccb0 tests/test_atomic_add_ref_14ccb0.cpp)
target_link_libraries(test_atomic_add_ref_14ccb0 sotc_core)
add_test(NAME atomic_add_ref_14ccb0 COMMAND test_atomic_add_ref_14ccb0)

add_executable(test_atomic_xadd_b30cf0 tests/test_atomic_xadd_b30cf0.cpp)
target_link_libraries(test_atomic_xadd_b30cf0 sotc_core)
add_test(NAME atomic_xadd_b30cf0 COMMAND test_atomic_xadd_b30cf0)

add_executable(test_store_value_clear_flag_158340 tests/test_store_value_clear_flag_158340.cpp)
target_link_libraries(test_store_value_clear_flag_158340 sotc_core)
add_test(NAME store_value_clear_flag_158340 COMMAND test_store_value_clear_flag_158340)

add_executable(test_fnv_span_161f80 tests/test_fnv_span_161f80.cpp)
target_link_libraries(test_fnv_span_161f80 sotc_core)
add_test(NAME fnv_span_161f80 COMMAND test_fnv_span_161f80)

add_executable(test_hashed_string_14c3a0 tests/test_hashed_string_14c3a0.cpp)
target_link_libraries(test_hashed_string_14c3a0 sotc_core)
add_test(NAME hashed_string_14c3a0 COMMAND test_hashed_string_14c3a0)

add_executable(test_shared_str_release_f590 tests/test_shared_str_release_f590.cpp)
target_link_libraries(test_shared_str_release_f590 sotc_core)
add_test(NAME shared_str_release_f590 COMMAND test_shared_str_release_f590)

add_executable(test_registry_object_14c9a0 tests/test_registry_object_14c9a0.cpp)
target_link_libraries(test_registry_object_14c9a0 sotc_core)
add_test(NAME registry_object_14c9a0 COMMAND test_registry_object_14c9a0)

add_executable(test_registry_object_2efd160 tests/test_registry_object_2efd160.cpp)
target_link_libraries(test_registry_object_2efd160 sotc_core)
add_test(NAME registry_object_2efd160 COMMAND test_registry_object_2efd160)

add_executable(test_registry_sorted_insert tests/test_registry_sorted_insert.cpp)
target_link_libraries(test_registry_sorted_insert sotc_core)
add_test(NAME registry_sorted_insert COMMAND test_registry_sorted_insert)

add_executable(test_inputtable_names_fixture tests/test_inputtable_names_fixture.cpp)
target_link_libraries(test_inputtable_names_fixture sotc_core)
add_test(NAME inputtable_names_fixture COMMAND test_inputtable_names_fixture)
set_tests_properties(inputtable_names_fixture PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")

add_executable(test_registry_fill_52a220 tests/test_registry_fill_52a220.cpp)
target_link_libraries(test_registry_fill_52a220 sotc_core)
add_test(NAME registry_fill_52a220 COMMAND test_registry_fill_52a220)
set_tests_properties(registry_fill_52a220 PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")

add_executable(test_registry_init_531d60 tests/test_registry_init_531d60.cpp)
target_link_libraries(test_registry_init_531d60 sotc_core)
add_test(NAME registry_init_531d60 COMMAND test_registry_init_531d60)
set_tests_properties(registry_init_531d60 PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")

add_executable(test_appmain_init_registry_slice tests/test_appmain_init_registry_slice.cpp)
target_link_libraries(test_appmain_init_registry_slice sotc_core)
add_test(NAME appmain_init_registry_slice COMMAND test_appmain_init_registry_slice)
set_tests_properties(appmain_init_registry_slice PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")

add_executable(test_appmain_init_remainder_slice tests/test_appmain_init_remainder_slice.cpp)
target_link_libraries(test_appmain_init_remainder_slice sotc_core)
add_test(NAME appmain_init_remainder_slice COMMAND test_appmain_init_remainder_slice)
set_tests_properties(appmain_init_remainder_slice PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")

add_executable(test_appmain_init_base_initialize_slice tests/test_appmain_init_base_initialize_slice.cpp)
target_link_libraries(test_appmain_init_base_initialize_slice sotc_core)
add_test(NAME appmain_init_base_initialize_slice COMMAND test_appmain_init_base_initialize_slice)
set_tests_properties(appmain_init_base_initialize_slice PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")

add_executable(test_appmain_init_prologue_slice tests/test_appmain_init_prologue_slice.cpp)
target_link_libraries(test_appmain_init_prologue_slice sotc_core)
add_test(NAME appmain_init_prologue_slice COMMAND test_appmain_init_prologue_slice)
set_tests_properties(appmain_init_prologue_slice PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")

add_executable(test_appmain_init_prologue_tail_slice tests/test_appmain_init_prologue_tail_slice.cpp)
target_link_libraries(test_appmain_init_prologue_tail_slice sotc_core)
add_test(NAME appmain_init_prologue_tail_slice COMMAND test_appmain_init_prologue_tail_slice)
set_tests_properties(appmain_init_prologue_tail_slice PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")

add_executable(test_stat_query_54b250 tests/test_stat_query_54b250.cpp)
target_link_libraries(test_stat_query_54b250 sotc_core)
add_test(NAME stat_query_54b250 COMMAND test_stat_query_54b250)

add_executable(test_savedata_stats_a54920 tests/test_savedata_stats_a54920.cpp)
target_link_libraries(test_savedata_stats_a54920 sotc_core)
add_test(NAME savedata_stats_a54920 COMMAND test_savedata_stats_a54920)

add_executable(test_savedata_family tests/test_savedata_family.cpp)
target_link_libraries(test_savedata_family sotc_core)
add_test(NAME savedata_family COMMAND test_savedata_family)

add_executable(test_init_531d60_entry_gaps tests/test_init_531d60_entry_gaps.cpp)
target_link_libraries(test_init_531d60_entry_gaps sotc_core)
add_test(NAME init_531d60_entry_gaps COMMAND test_init_531d60_entry_gaps)

add_executable(test_init_1290 tests/test_init_1290.cpp)
target_link_libraries(test_init_1290 sotc_core)
add_test(NAME init_1290 COMMAND test_init_1290)

add_executable(test_init_a5d990_body tests/test_init_a5d990_body.cpp)
target_link_libraries(test_init_a5d990_body sotc_core)
add_test(NAME init_a5d990_body COMMAND test_init_a5d990_body)

add_executable(test_init_5b9f90_slices tests/test_init_5b9f90_slices.cpp)
target_link_libraries(test_init_5b9f90_slices sotc_core)
add_test(NAME init_5b9f90_slices COMMAND test_init_5b9f90_slices)

add_executable(test_guest_entry_table tests/test_guest_entry_table.cpp)
target_link_libraries(test_guest_entry_table sotc_core)
add_test(NAME guest_entry_table COMMAND test_guest_entry_table)

add_executable(test_init_post_bodies tests/test_init_post_bodies.cpp)
target_link_libraries(test_init_post_bodies sotc_core)
add_test(NAME init_post_bodies COMMAND test_init_post_bodies)

add_executable(test_owner_dtor_1bdae0 tests/test_owner_dtor_1bdae0.cpp)
target_link_libraries(test_owner_dtor_1bdae0 sotc_core)
add_test(NAME owner_dtor_1bdae0 COMMAND test_owner_dtor_1bdae0)

add_executable(test_container_1bdb10 tests/test_container_1bdb10.cpp)
target_link_libraries(test_container_1bdb10 sotc_core)
add_test(NAME container_1bdb10 COMMAND test_container_1bdb10)

add_executable(test_insert_1bdb50 tests/test_insert_1bdb50.cpp)
target_link_libraries(test_insert_1bdb50 sotc_core)
add_test(NAME insert_1bdb50 COMMAND test_insert_1bdb50)

add_executable(test_app_info_registry tests/test_app_info_registry.cpp)
target_link_libraries(test_app_info_registry sotc_core)
add_test(NAME app_info_registry COMMAND test_app_info_registry)

add_executable(test_app_info_install tests/test_app_info_install.cpp)
target_link_libraries(test_app_info_install sotc_core)
add_test(NAME app_info_install COMMAND test_app_info_install)

add_executable(test_registry_owner tests/test_registry_owner.cpp)
target_link_libraries(test_registry_owner sotc_core)
add_test(NAME registry_owner COMMAND test_registry_owner)

add_executable(test_container_dtor_1bede0 tests/test_container_dtor_1bede0.cpp)
target_link_libraries(test_container_dtor_1bede0 sotc_core)
add_test(NAME container_dtor_1bede0 COMMAND test_container_dtor_1bede0)

add_executable(test_registry_slot tests/test_registry_slot.cpp)
target_link_libraries(test_registry_slot sotc_core)
add_test(NAME registry_slot COMMAND test_registry_slot)

add_executable(test_app_info_collect tests/test_app_info_collect.cpp)
target_link_libraries(test_app_info_collect sotc_core)
add_test(NAME app_info_collect COMMAND test_app_info_collect)

add_executable(test_appinfo_listb tests/test_appinfo_listb.cpp)
target_link_libraries(test_appinfo_listb sotc_core)
add_test(NAME appinfo_listb COMMAND test_appinfo_listb)

add_executable(test_app_info_snapshot tests/test_app_info_snapshot.cpp)
target_link_libraries(test_app_info_snapshot sotc_core)
add_test(NAME app_info_snapshot COMMAND test_app_info_snapshot)

add_executable(test_insert_1bdc80 tests/test_insert_1bdc80.cpp)
target_link_libraries(test_insert_1bdc80 sotc_core)
add_test(NAME insert_1bdc80 COMMAND test_insert_1bdc80)

add_executable(test_owner_properties_2569d0 tests/test_owner_properties_2569d0.cpp)
target_link_libraries(test_owner_properties_2569d0 sotc_core)
add_test(NAME owner_properties_2569d0 COMMAND test_owner_properties_2569d0)

add_executable(test_owner_prop_bag tests/test_owner_prop_bag.cpp)
target_link_libraries(test_owner_prop_bag sotc_core)
add_test(NAME owner_prop_bag COMMAND test_owner_prop_bag)

add_executable(test_owner_members tests/test_owner_members.cpp)
target_link_libraries(test_owner_members sotc_core)
add_test(NAME owner_members COMMAND test_owner_members)

add_executable(test_owner_simple_ctors tests/test_owner_simple_ctors.cpp)
target_link_libraries(test_owner_simple_ctors sotc_core)
add_test(NAME owner_simple_ctors COMMAND test_owner_simple_ctors)

add_executable(test_owner_ctor_b111e0_slot48 tests/test_owner_ctor_b111e0_slot48.cpp)
target_link_libraries(test_owner_ctor_b111e0_slot48 sotc_core)
add_test(NAME owner_ctor_b111e0_slot48 COMMAND test_owner_ctor_b111e0_slot48)

add_executable(test_owner_ctor_b11210_slot58 tests/test_owner_ctor_b11210_slot58.cpp)
target_link_libraries(test_owner_ctor_b11210_slot58 sotc_core)
add_test(NAME owner_ctor_b11210_slot58 COMMAND test_owner_ctor_b11210_slot58)

add_executable(test_owner_ctor_a5c850 tests/test_owner_ctor_a5c850.cpp)
target_link_libraries(test_owner_ctor_a5c850 sotc_core)
add_test(NAME owner_ctor_a5c850 COMMAND test_owner_ctor_a5c850)

add_executable(test_owner_dtor_a5cb30 tests/test_owner_dtor_a5cb30.cpp)
target_link_libraries(test_owner_dtor_a5cb30 sotc_core)
add_test(NAME owner_dtor_a5cb30 COMMAND test_owner_dtor_a5cb30)

add_executable(test_shared_str_ctor_f1a0 tests/test_shared_str_ctor_f1a0.cpp)
target_link_libraries(test_shared_str_ctor_f1a0 sotc_core)
add_test(NAME shared_str_ctor_f1a0 COMMAND test_shared_str_ctor_f1a0)

add_executable(test_shared_str_resize_f3c0 tests/test_shared_str_resize_f3c0.cpp)
target_link_libraries(test_shared_str_resize_f3c0 sotc_core)
add_test(NAME shared_str_resize_f3c0 COMMAND test_shared_str_resize_f3c0)

add_executable(test_resource_id_14c2f0 tests/test_resource_id_14c2f0.cpp)
target_link_libraries(test_resource_id_14c2f0 sotc_core)
add_test(NAME resource_id_14c2f0 COMMAND test_resource_id_14c2f0)

add_executable(test_resource_pool tests/test_resource_pool.cpp)
target_link_libraries(test_resource_pool sotc_core)
add_test(NAME resource_pool COMMAND test_resource_pool)

add_executable(test_app_info_run tests/test_app_info_run.cpp)
target_link_libraries(test_app_info_run sotc_core)
add_test(NAME app_info_run COMMAND test_app_info_run)

add_executable(test_snapshot_bug_data tests/test_snapshot_bug_data.cpp)
target_link_libraries(test_snapshot_bug_data sotc_core)
add_test(NAME snapshot_bug_data COMMAND test_snapshot_bug_data)

add_executable(test_screenshot_contract tests/test_screenshot_contract.cpp)
target_link_libraries(test_screenshot_contract sotc_core)
add_test(NAME screenshot_contract COMMAND test_screenshot_contract)

add_executable(test_frame_root tests/test_frame_root.cpp)
target_link_libraries(test_frame_root sotc_core)
add_test(NAME frame_root COMMAND test_frame_root)

add_executable(test_platform_tag_147e20 tests/test_platform_tag_147e20.cpp)
target_link_libraries(test_platform_tag_147e20 sotc_core)
add_test(NAME platform_tag_147e20 COMMAND test_platform_tag_147e20)

add_executable(test_camera_location_25a4d0 tests/test_camera_location_25a4d0.cpp)
target_link_libraries(test_camera_location_25a4d0 sotc_core)
add_test(NAME camera_location_25a4d0 COMMAND test_camera_location_25a4d0)

add_executable(test_list_dtor_1bdeb0 tests/test_list_dtor_1bdeb0.cpp)
target_link_libraries(test_list_dtor_1bdeb0 sotc_core)
add_test(NAME list_dtor_1bdeb0 COMMAND test_list_dtor_1bdeb0)

add_executable(test_alloc_wrappers_17070 tests/test_alloc_wrappers_17070.cpp)
target_link_libraries(test_alloc_wrappers_17070 sotc_core)
add_test(NAME alloc_wrappers_17070 COMMAND test_alloc_wrappers_17070)

add_executable(test_shared_release_f590 tests/test_shared_release_f590.cpp)
target_link_libraries(test_shared_release_f590 sotc_core)
add_test(NAME shared_release_f590 COMMAND test_shared_release_f590)

add_executable(test_insert_1bde30 tests/test_insert_1bde30.cpp)
target_link_libraries(test_insert_1bde30 sotc_core)
add_test(NAME insert_1bde30 COMMAND test_insert_1bde30)

add_executable(test_insert_1beeb0 tests/test_insert_1beeb0.cpp)
target_link_libraries(test_insert_1beeb0 sotc_core)
add_test(NAME insert_1beeb0 COMMAND test_insert_1beeb0)

add_executable(test_insert_21cd0 tests/test_insert_21cd0.cpp)
target_link_libraries(test_insert_21cd0 sotc_core)
add_test(NAME insert_21cd0 COMMAND test_insert_21cd0)

add_executable(test_capture_a473b0 tests/test_capture_a473b0.cpp)
target_link_libraries(test_capture_a473b0 sotc_core)
add_test(NAME capture_a473b0 COMMAND test_capture_a473b0)

add_executable(test_appmain_init_ctor_98a90_slice tests/test_appmain_init_ctor_98a90_slice.cpp)
target_link_libraries(test_appmain_init_ctor_98a90_slice sotc_core)
add_test(NAME appmain_init_ctor_98a90_slice COMMAND test_appmain_init_ctor_98a90_slice)
set_tests_properties(appmain_init_ctor_98a90_slice PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")

add_executable(test_page_owner_map tests/test_page_owner_map.cpp)
target_link_libraries(test_page_owner_map sotc_core)
add_test(NAME page_owner_map COMMAND test_page_owner_map)

add_executable(test_page_allocator tests/test_page_allocator.cpp)
target_link_libraries(test_page_allocator sotc_core)
add_test(NAME page_allocator COMMAND test_page_allocator)

add_executable(test_allocator_front tests/test_allocator_front.cpp)
target_link_libraries(test_allocator_front sotc_core)
add_test(NAME allocator_front COMMAND test_allocator_front)

add_executable(test_composite_allocator tests/test_composite_allocator.cpp)
target_link_libraries(test_composite_allocator sotc_core)
add_test(NAME composite_allocator COMMAND test_composite_allocator)

add_executable(test_bucket_allocator tests/test_bucket_allocator.cpp)
target_link_libraries(test_bucket_allocator sotc_core)
add_test(NAME bucket_allocator COMMAND test_bucket_allocator)

add_executable(test_heap_allocator tests/test_heap_allocator.cpp)
target_link_libraries(test_heap_allocator sotc_core)
add_test(NAME heap_allocator COMMAND test_heap_allocator)

add_executable(test_allocator_bootstrap tests/test_allocator_bootstrap.cpp)
target_link_libraries(test_allocator_bootstrap sotc_core)
add_test(NAME allocator_bootstrap COMMAND test_allocator_bootstrap)

add_executable(test_allocator_lazy_latch tests/test_allocator_lazy_latch.cpp)
target_link_libraries(test_allocator_lazy_latch sotc_core)
add_test(NAME allocator_lazy_latch COMMAND test_allocator_lazy_latch)

add_subdirectory(api)

# GPU-01-METAL-CAPTURE-PROVIDER-01: direct-Metal offscreen capture provider (Objective-C++).
# Appended so existing target order is untouched; project-wide arm64 + sanitizer flags apply.
enable_language(OBJCXX)
set(CMAKE_OBJCXX_STANDARD 20)
set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)
add_library(sotc_metal STATIC platform/metal_capture.mm)
target_compile_options(sotc_metal PRIVATE -Wall -Wextra -Werror)
target_link_libraries(sotc_metal PUBLIC sotc_core
  "-framework Metal"
  "-framework Foundation"
  "-framework ImageIO"
  "-framework CoreGraphics")
add_executable(test_metal_capture tests/test_metal_capture.cpp)
target_link_libraries(test_metal_capture sotc_metal sotc_core)
add_test(NAME metal_capture COMMAND test_metal_capture)
set_tests_properties(metal_capture PROPERTIES SKIP_RETURN_CODE 77)

# GPU-03-CMSH-RAW-POSIDX-01: raw POS0 float3 / U16 index subset for asset-derived frames.
add_library(sotc_cmsh_mesh STATIC platform/cmsh_mesh.cpp)
add_executable(test_cmsh_mesh tests/test_cmsh_mesh.cpp)
target_link_libraries(test_cmsh_mesh PRIVATE sotc_cmsh_mesh)
add_test(NAME cmsh_mesh COMMAND test_cmsh_mesh)

# GPU-03-RENDEROPS-ASSET-FRAME-01: host-handle typed render command list and fail-visible validation.
add_library(sotc_render_ops STATIC platform/render_ops.cpp)
add_executable(test_render_ops tests/test_render_ops.cpp)
target_link_libraries(test_render_ops PRIVATE sotc_render_ops)
add_test(NAME render_ops COMMAND test_render_ops)

# GPU-03-RENDEROPS-ASSET-FRAME-01: direct-Metal executor for typed indexed meshes and depth.
add_library(sotc_metal_asset_frame STATIC platform/metal_asset_frame.mm)
target_compile_options(sotc_metal_asset_frame PRIVATE -Wall -Wextra -Werror)
target_link_libraries(sotc_metal_asset_frame PUBLIC sotc_metal sotc_cmsh_mesh sotc_render_ops)
add_executable(test_metal_asset_frame tests/test_metal_asset_frame.cpp)
target_link_libraries(test_metal_asset_frame PRIVATE sotc_metal_asset_frame)
add_test(NAME metal_asset_frame COMMAND test_metal_asset_frame)
set_tests_properties(metal_asset_frame PROPERTIES
  SKIP_RETURN_CODE 77
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/.."
  ENVIRONMENT "MTL_DEBUG_LAYER=1;MTL_SHADER_VALIDATION=1;MTL_SHADER_VALIDATION_DEFAULT_STATE=all;MTL_SHADER_VALIDATION_ENABLE_ERROR_REPORTING=1;MTL_SHADER_VALIDATION_REPORT_TO_STDERR=1;MTL_SHADER_VALIDATION_ABORT_ON_FAULT=1;SOTC_DEFAULT_PAIR_MSL=${CMAKE_SOURCE_DIR}/../local/validation/gcn-default-root-v1/default-pair.metal;SOTC_ASSET_FRAME_DUMP=${CMAKE_SOURCE_DIR}/../local/validation/gpu03-r31/synthetic-asset-frame.png")

# GPU-03-RENDEROPS-TEXTURE-SAMPLER-01 and GPU-03-LOGO-PNG-TEXTURE-SAMPLE-01: test-only
# fragment texture/sampler binding, plus optional private PNG decode/replay fixture.
add_executable(test_metal_texture_binding
  tests/test_metal_texture_binding.cpp
  tests/asset_png_fixture.mm)
target_link_libraries(test_metal_texture_binding PRIVATE sotc_metal_asset_frame)
add_test(NAME metal_texture_binding COMMAND test_metal_texture_binding)
set_tests_properties(metal_texture_binding PROPERTIES
  SKIP_RETURN_CODE 77
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/.."
  ENVIRONMENT "MTL_DEBUG_LAYER=1;MTL_SHADER_VALIDATION=1;MTL_SHADER_VALIDATION_DEFAULT_STATE=all;MTL_SHADER_VALIDATION_ENABLE_ERROR_REPORTING=1;MTL_SHADER_VALIDATION_REPORT_TO_STDERR=1;MTL_SHADER_VALIDATION_ABORT_ON_FAULT=1")

# --- APP-01-SOCGAMECORE-EXE-01: native SoCGameCore auxiliary executable (protocol v1) -----------
# One JSON object per line on stdout; exit codes 0/2/3/4/5. Links the versioned ABI and the core.
add_library(soc_core_status_json STATIC app/core_status_json.cpp)
target_include_directories(soc_core_status_json PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/app)

add_executable(soc_game_core app/soc_game_core_main.cpp)

add_executable(sotc_init_window app/sotc_init_window_main.cpp)
target_link_libraries(sotc_init_window PRIVATE sotc_core)
add_test(NAME init_window_runtime COMMAND sotc_init_window)
set_tests_properties(init_window_runtime PROPERTIES
  ENVIRONMENT "SOTC_INPUTTABLE_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/inputtable_551140_names.bin")
set_target_properties(soc_game_core PROPERTIES OUTPUT_NAME SoCGameCore)
target_link_libraries(soc_game_core PRIVATE soc_core_status_json sotc_api sotc_core)

add_executable(test_core_status_json tests/test_core_status_json.cpp)
target_link_libraries(test_core_status_json PRIVATE soc_core_status_json)
add_test(NAME core_status_json COMMAND test_core_status_json)

# Spawn the real executable and pin protocol bytes, exit codes and serve framing.
find_program(SOTC_PYTHON3 NAMES python3)
if(SOTC_PYTHON3)
  set(SOTC_FCORE_CHECK ${CMAKE_CURRENT_SOURCE_DIR}/../tools/lift/check_fcore_status.py)
  foreach(sotc_case status coverage start serve validate)
    add_test(NAME soc_game_core_${sotc_case}
      COMMAND ${SOTC_PYTHON3} ${SOTC_FCORE_CHECK} --exe $<TARGET_FILE:soc_game_core> --only ${sotc_case})
  endforeach()
else()
  message(STATUS "python3 not found: SoCGameCore protocol ctests fall back to --status only")
  add_test(NAME soc_game_core_status COMMAND soc_game_core --status)
  set_tests_properties(soc_game_core_status PROPERTIES
    PASS_REGULAR_EXPRESSION "\"schema\":\"sotc.core.status.v1\"")
endif()

# CPU-01-CTOR-COVERAGE-BATCH-01: original-table coverage (573 rows), the last missing leaf_vec_copy
# module (index 95), and the production registration coverage test. Appended without reordering any
# existing target; the two new core sources are added with target_sources instead of editing the
# existing source list.
target_sources(sotc_core PRIVATE core/vec_d71f60.cpp core/ctor_original_table.cpp)

add_executable(test_vec_d71f60 tests/test_vec_d71f60.cpp)
target_link_libraries(test_vec_d71f60 sotc_core)
add_test(NAME vec_d71f60 COMMAND test_vec_d71f60)

add_executable(test_startup_ctor_coverage tests/test_startup_ctor_coverage.cpp)
target_link_libraries(test_startup_ctor_coverage sotc_core)
add_test(NAME startup_ctor_coverage COMMAND test_startup_ctor_coverage)
set_tests_properties(startup_ctor_coverage PROPERTIES
  ENVIRONMENT "SOTC_SLOT23_NAMES_FIXTURE=${CMAKE_SOURCE_DIR}/../local/fixtures/slot23_e71990_names.bin")

# CPU-01-OWNER-ROW14-01: row-14 constructor `0xa597d0` + its context-free owner_members adapter.
add_executable(test_owner_ctor_a597d0 tests/test_owner_ctor_a597d0.cpp)
target_link_libraries(test_owner_ctor_a597d0 sotc_core)
add_test(NAME owner_ctor_a597d0 COMMAND test_owner_ctor_a597d0)

# CPU-01-OWNER-ROW21-01: row-21 owner constructor `0x4a690` (appended so existing targets keep
# their order). Source list addition is deferred to target_sources for the same reason.
target_sources(sotc_core PRIVATE core/owner_ctor_4a690.cpp)
add_executable(test_owner_ctor_4a690 tests/test_owner_ctor_4a690.cpp)
target_link_libraries(test_owner_ctor_4a690 sotc_core)
add_test(NAME owner_ctor_4a690 COMMAND test_owner_ctor_4a690)

# SAV-02-RELAUNCH-01: cross-process persistence proof for the native save transaction.
add_executable(test_save_relaunch tests/test_save_relaunch.cpp)
target_link_libraries(test_save_relaunch sotc_core)
add_test(NAME save_relaunch COMMAND test_save_relaunch)

# CPU-01-STARTUP-SLOTS-296-297-300-01: three recovered startup constructors (TSC stamp thunk,
# 400-record table initializer, inert atexit thunk). Appended; existing target order is untouched.
target_sources(sotc_core PRIVATE
  core/ctor_slot_296_b2b6b0.cpp
  core/ctor_slot_297_b25470.cpp
  core/ctor_slot_300_acc880.cpp)

add_executable(test_ctor_slots_296_297_300 tests/test_ctor_slots_296_297_300.cpp)
target_link_libraries(test_ctor_slots_296_297_300 sotc_core)
add_test(NAME ctor_slots_296_297_300 COMMAND test_ctor_slots_296_297_300)

# CPU-01-OWNER-ROWS-3-18-01: row-3 (ResourcePool, `0x14db00`) and row-18 (`0x3c5cb0`) owner
# constructors + their context-free owner_members adapters. Appended; existing targets keep order.
target_sources(sotc_core PRIVATE
  core/owner_ctor_3c5cb0.cpp
  core/owner_ctor_14db00.cpp)

add_executable(test_owner_ctor_3c5cb0 tests/test_owner_ctor_3c5cb0.cpp)
target_link_libraries(test_owner_ctor_3c5cb0 sotc_core)
add_test(NAME owner_ctor_3c5cb0 COMMAND test_owner_ctor_3c5cb0)

add_executable(test_owner_ctor_14db00 tests/test_owner_ctor_14db00.cpp)
target_link_libraries(test_owner_ctor_14db00 sotc_core)
add_test(NAME owner_ctor_14db00 COMMAND test_owner_ctor_14db00)

# CPU-01-CONCMD-298-299-301-01: shared ConCmd base/registry, the bool and ranged console-variable
# shapes, the debug-menu item, and the startup slots 298 (shaderTypeToLoad), 299 (debug text scale)
# and 301 (rumbleEnabled). Appended; existing
# target order is untouched.
target_sources(sotc_core PRIVATE
  core/concmd_base.cpp
  core/concmd_vars.cpp
  core/debug_menu_item.cpp
  core/ctor_slot_298_b0e250.cpp
  core/ctor_slot_299_b0c7a0.cpp
  core/ctor_slot_301_ac9550.cpp)

add_executable(test_concmd_slots_298_299_301 tests/test_concmd_slots_298_299_301.cpp)
target_link_libraries(test_concmd_slots_298_299_301 sotc_core)
add_test(NAME concmd_slots_298_299_301 COMMAND test_concmd_slots_298_299_301)

# CPU-01-SLOT306-CONCMD-OBJECT-01: console-COMMAND object of startup slot 306 (vptr 0x1594dd0, flags 2,
# destructor 0x4b0, thunk 0x1990), its destructor chain and a behavioural manager model. Additive.
target_sources(sotc_core PRIVATE core/fconcmd_object_slot306.cpp)
add_executable(test_fconcmd_object_slot306 tests/test_fconcmd_object_slot306.cpp)
target_link_libraries(test_fconcmd_object_slot306 sotc_core)
add_test(NAME fconcmd_object_slot306 COMMAND test_fconcmd_object_slot306)

# CPU-01-SLOT306-INTVAR-CTORS-01: ranged int console-variable constructors 0xa624a0 / 0xa62860
# (+ workers 0xa62660 / 0xa62a20) and the three startup slot-306 construction sites. Additive.
target_sources(sotc_core PRIVATE core/intvar_a624a0.cpp)

add_executable(test_intvar_a624a0 tests/test_intvar_a624a0.cpp)
target_link_libraries(test_intvar_a624a0 sotc_core)
add_test(NAME intvar_a624a0 COMMAND test_intvar_a624a0)

# CPU-01-SLOT306-MENU-CTORS-01: debug-menu group/separator/command-item ctors and the complete
# registration helper 0x268b0 (kind-1 branch) used by startup slot 306. Additive.
target_sources(sotc_core PRIVATE core/fmenu_ctors_slot306.cpp)
add_executable(test_fmenu_ctors_slot306 tests/test_fmenu_ctors_slot306.cpp)
target_link_libraries(test_fmenu_ctors_slot306 sotc_core)
add_test(NAME fmenu_ctors_slot306 COMMAND test_fmenu_ctors_slot306)

# CPU-01-CONCMD-303-01: startup slot 303 (0xa82170), the `ProfileCPU` bool console variable and its
# "Profile CPU" debug-menu toggle. Reuses the bool-var shape (now parameterised on value/default)
# and the debug-menu toggle from the 298/299/301 batch. Appended; existing target order untouched.
target_sources(sotc_core PRIVATE
  core/ctor_slot_303_a82170.cpp)

add_executable(test_ctor_slot_303 tests/test_ctor_slot_303.cpp)
target_link_libraries(test_ctor_slot_303 sotc_core)
add_test(NAME ctor_slot_303 COMMAND test_ctor_slot_303)

# CPU-01-STARTUP-SLOT-306-ASSEMBLE-01: startup slot 306 (0xa633b0), the 46 `sndx_` objects assembled from
# the 306 int-var / command / menu modules, concmd_vars and debug_menu_item. Additive.
target_sources(sotc_core PRIVATE core/ctor_slot_306_a633b0.cpp)
add_executable(test_ctor_slot_306 tests/test_ctor_slot_306.cpp)
target_link_libraries(test_ctor_slot_306 sotc_core)
add_test(NAME ctor_slot_306 COMMAND test_ctor_slot_306)

# CPU-01-SLOT306-HANDLERS-NATIVE-01: native sndx_ console-command handler model (0xa5bf20 /
# 0xa5bf50 / 0xa5c020, the 0xba40 bool-var setter, the 0x1e4c0 lookup contract and the 0xa5f380
# reload walk) with explicit hooks for every unlifted callee.
target_sources(sotc_core PRIVATE core/sndx_command_handlers.cpp)
add_executable(test_sndx_command_handlers tests/test_sndx_command_handlers.cpp)
target_link_libraries(test_sndx_command_handlers sotc_core)
add_test(NAME sndx_command_handlers COMMAND test_sndx_command_handlers)

# CPU-01-SLOT306-CATEGORY-VARS-01: native model of the engine-init 0xa5d990 category loop —
# 32 Mute + 32 Filter sndxCategory_* bool vars at engine+0x8bd0/+0x72a8, stride 0xc8.
target_sources(sotc_core PRIVATE core/engine_category_vars.cpp)
add_executable(test_sndx_category_vars tests/test_sndx_category_vars.cpp)
target_link_libraries(test_sndx_category_vars sotc_core)
add_test(NAME sndx_category_vars COMMAND test_sndx_category_vars)

# CPU-01-STARTUP-302-303-305-01: ten handles/cleanup records and six guarded pair copies.
target_sources(sotc_core PRIVATE core/ctor_slot_302_aaeee0.cpp core/ctor_slot_305_a77b60.cpp)
add_executable(test_ctor_slot_302_305 tests/test_ctor_slot_302_305.cpp)
target_link_libraries(test_ctor_slot_302_305 sotc_core)
add_test(NAME ctor_slot_302_305 COMMAND test_ctor_slot_302_305)

# GPU-01-CSDR-READER-01: importer-only raw shader-container reader and metadata diagnostic.
target_sources(sotc_core PRIVATE core/csdr_container.cpp)
add_executable(test_csdr_container tests/test_csdr_container.cpp)
target_link_libraries(test_csdr_container sotc_core)
add_test(NAME csdr_container COMMAND test_csdr_container)
add_executable(csdr_inspect ../tools/dev/csdr_inspect.cpp)
target_link_libraries(csdr_inspect PRIVATE sotc_core)

# L5-RELEASE-FAMILY: 17 startup constructor slots (371 372 405 431 433 434 435 438 447 470 472 481 483 484
# 485 487 529) sharing the 0xf590 shared-str release + __cxa_atexit shape; table-driven, ELF-pinned by
# tools/lift/check_ctor_release_family_slots.py.
target_sources(sotc_core PRIVATE core/ctor_release_family_slots.cpp)
add_executable(test_ctor_release_family_slots tests/test_ctor_release_family_slots.cpp)
target_link_libraries(test_ctor_release_family_slots sotc_core)
add_test(NAME ctor_release_family_slots COMMAND test_ctor_release_family_slots)

# CPU-01-TINY-ATEXIT-SLOTS-01 (lane L1-TINY): 19 "zero static object + __cxa_atexit" constructor slots
# (311, 425, 432, 471, 515, 522..565 subset) as one table-driven module.
target_sources(sotc_core PRIVATE core/ctor_tiny_atexit_slots.cpp)
add_executable(test_ctor_tiny_atexit_slots tests/test_ctor_tiny_atexit_slots.cpp)
target_link_libraries(test_ctor_tiny_atexit_slots sotc_core)
add_test(NAME ctor_tiny_atexit_slots COMMAND test_ctor_tiny_atexit_slots)

# CPU-01-STARTUP-SLOTS-343-356 (lane L4): shared shapes + nine startup constructor slots.
target_sources(sotc_core PRIVATE
  core/ctor_shapes_343_356.cpp
  core/ctor_slot_343_7d7970.cpp
  core/ctor_slot_344_7d3810.cpp
  core/ctor_slot_345_7cec60.cpp
  core/ctor_slot_347_7c93a0.cpp
  core/ctor_slot_348_7c9150.cpp
  core/ctor_slot_350_7af0b0.cpp
  core/ctor_slot_353_7a14d0.cpp
  core/ctor_slot_355_7946d0.cpp
  core/ctor_slot_356_78e2f0.cpp)

# CPU-01-STARTUP-SLOT-313-01 (lane L2): update_entitlement_status command + Platform Services menu items.
target_sources(sotc_core PRIVATE core/ctor_slot_313_a447a0.cpp)
add_executable(test_ctor_slot_313 tests/test_ctor_slot_313.cpp)
target_link_libraries(test_ctor_slot_313 sotc_core)
add_test(NAME ctor_slot_313 COMMAND test_ctor_slot_313)

# CPU-01-TINY-MISC-SLOTS-01 (lane L1-TINY): 12 small constructor slots (404, 406, 409, 414, 494, 517, 519,
# 535, 541, 553, 562, 563) with explicit hooks for unlifted callees.
target_sources(sotc_core PRIVATE core/ctor_tiny_misc_slots.cpp)
add_executable(test_ctor_tiny_misc_slots tests/test_ctor_tiny_misc_slots.cpp)
target_link_libraries(test_ctor_tiny_misc_slots sotc_core)
add_test(NAME ctor_tiny_misc_slots COMMAND test_ctor_tiny_misc_slots)

# CPU-01-STARTUP-SLOTS-343-356 (lane L4): fixtures.
add_executable(test_ctor_slot_343 tests/test_ctor_slot_343.cpp)
target_link_libraries(test_ctor_slot_343 sotc_core)
add_test(NAME ctor_slot_343 COMMAND test_ctor_slot_343)
add_executable(test_ctor_slot_344 tests/test_ctor_slot_344.cpp)
target_link_libraries(test_ctor_slot_344 sotc_core)
add_test(NAME ctor_slot_344 COMMAND test_ctor_slot_344)
add_executable(test_ctor_slot_345 tests/test_ctor_slot_345.cpp)
target_link_libraries(test_ctor_slot_345 sotc_core)
add_test(NAME ctor_slot_345 COMMAND test_ctor_slot_345)
add_executable(test_ctor_slot_347 tests/test_ctor_slot_347.cpp)
target_link_libraries(test_ctor_slot_347 sotc_core)
add_test(NAME ctor_slot_347 COMMAND test_ctor_slot_347)
add_executable(test_ctor_slot_348 tests/test_ctor_slot_348.cpp)
target_link_libraries(test_ctor_slot_348 sotc_core)
add_test(NAME ctor_slot_348 COMMAND test_ctor_slot_348)
add_executable(test_ctor_slot_350 tests/test_ctor_slot_350.cpp)
target_link_libraries(test_ctor_slot_350 sotc_core)
add_test(NAME ctor_slot_350 COMMAND test_ctor_slot_350)
add_executable(test_ctor_slot_353 tests/test_ctor_slot_353.cpp)
target_link_libraries(test_ctor_slot_353 sotc_core)
add_test(NAME ctor_slot_353 COMMAND test_ctor_slot_353)
add_executable(test_ctor_slot_355 tests/test_ctor_slot_355.cpp)
target_link_libraries(test_ctor_slot_355 sotc_core)
add_test(NAME ctor_slot_355 COMMAND test_ctor_slot_355)
add_executable(test_ctor_slot_356 tests/test_ctor_slot_356.cpp)
target_link_libraries(test_ctor_slot_356 sotc_core)
add_test(NAME ctor_slot_356 COMMAND test_ctor_slot_356)

# CPU-01-STARTUP-SLOT-307-01 (lane L2): Trophy System block (bool var, two commands, group, two toggles).
target_sources(sotc_core PRIVATE core/ctor_slot_307_a5b090.cpp)
add_executable(test_ctor_slot_307 tests/test_ctor_slot_307.cpp)
target_link_libraries(test_ctor_slot_307 sotc_core)
add_test(NAME ctor_slot_307 COMMAND test_ctor_slot_307)
add_executable(test_ctor_shapes_343_356 tests/test_ctor_shapes_343_356.cpp)
target_link_libraries(test_ctor_shapes_343_356 sotc_core)
add_test(NAME ctor_shapes_343_356 COMMAND test_ctor_shapes_343_356)

# CPU-01-STARTUP-SLOTS-322-334 (lane L3): shared 0x26db0 named menu item + family helpers + nine startup
# constructor slots (322 325 327 328 330 331 332 333 334) in one module, one fixture per slot.
target_sources(sotc_core PRIVATE
  core/menu_item_26db0.cpp
  core/ctor_slot_family_items.cpp
  core/ctor_slots_322_334.cpp)
add_executable(test_ctor_slot_322 tests/test_ctor_slot_322.cpp)
target_link_libraries(test_ctor_slot_322 sotc_core)
add_test(NAME ctor_slot_322 COMMAND test_ctor_slot_322)
add_executable(test_ctor_slot_325 tests/test_ctor_slot_325.cpp)
target_link_libraries(test_ctor_slot_325 sotc_core)
add_test(NAME ctor_slot_325 COMMAND test_ctor_slot_325)
add_executable(test_ctor_slot_327 tests/test_ctor_slot_327.cpp)
target_link_libraries(test_ctor_slot_327 sotc_core)
add_test(NAME ctor_slot_327 COMMAND test_ctor_slot_327)
add_executable(test_ctor_slot_328 tests/test_ctor_slot_328.cpp)
target_link_libraries(test_ctor_slot_328 sotc_core)
add_test(NAME ctor_slot_328 COMMAND test_ctor_slot_328)
add_executable(test_ctor_slot_330 tests/test_ctor_slot_330.cpp)
target_link_libraries(test_ctor_slot_330 sotc_core)
add_test(NAME ctor_slot_330 COMMAND test_ctor_slot_330)
add_executable(test_ctor_slot_331 tests/test_ctor_slot_331.cpp)
target_link_libraries(test_ctor_slot_331 sotc_core)
add_test(NAME ctor_slot_331 COMMAND test_ctor_slot_331)
add_executable(test_ctor_slot_332 tests/test_ctor_slot_332.cpp)
target_link_libraries(test_ctor_slot_332 sotc_core)
add_test(NAME ctor_slot_332 COMMAND test_ctor_slot_332)
add_executable(test_ctor_slot_333 tests/test_ctor_slot_333.cpp)
target_link_libraries(test_ctor_slot_333 sotc_core)
add_test(NAME ctor_slot_333 COMMAND test_ctor_slot_333)
add_executable(test_ctor_slot_334 tests/test_ctor_slot_334.cpp)
target_link_libraries(test_ctor_slot_334 sotc_core)
add_test(NAME ctor_slot_334 COMMAND test_ctor_slot_334)

# CPU-01-STARTUP-SLOT-308-01: constructor index 308 (0xa58f10) and the native 0xc580 ranged-int constructor.
target_sources(sotc_core PRIVATE core/ranged_int_var_c580.cpp core/ctor_slot_308_a58f10.cpp)
target_sources(sotc_core PRIVATE core/ranged_bound_var_a4f4d0.cpp core/ctor_slot_309_a505e0.cpp)
add_executable(test_ctor_slot_308 tests/test_ctor_slot_308.cpp)
target_link_libraries(test_ctor_slot_308 sotc_core)
add_test(NAME ctor_slot_308 COMMAND test_ctor_slot_308)

# CPU-01-STARTUP-SLOT-317-01: constructor index 317 (0xa2c390).
target_sources(sotc_core PRIVATE core/ctor_slot_317_a2c390.cpp)
add_executable(test_ctor_slot_317 tests/test_ctor_slot_317.cpp)
target_link_libraries(test_ctor_slot_317 sotc_core)
add_test(NAME ctor_slot_317 COMMAND test_ctor_slot_317)
# CPU-01-STARTUP-SLOT-318-01: constructor index 318 (0xa20e80).
target_sources(sotc_core PRIVATE core/ctor_slot_318_a20e80.cpp)
add_executable(test_ctor_slot_318 tests/test_ctor_slot_318.cpp)
target_link_libraries(test_ctor_slot_318 sotc_core)
add_test(NAME ctor_slot_318 COMMAND test_ctor_slot_318)
# CPU-01-STARTUP-SLOT-310-01: constructor index 310 (0xa479d0) and the string-variable constructor 0x257a0.
target_sources(sotc_core PRIVATE core/string_var_257a0.cpp core/ctor_slot_310_a479d0.cpp)
add_executable(test_ctor_slot_310 tests/test_ctor_slot_310.cpp)
target_link_libraries(test_ctor_slot_310 sotc_core)
add_test(NAME ctor_slot_310 COMMAND test_ctor_slot_310)
# CPU-01-STARTUP-SLOT-312-CHARACTERIZATION-01 / -NATIVE-LIFT-01: constructor index 312 (0xa46130),
# the "Platform Services" / "TitleInfo" entitlement block (3 variables, 4 menu items, 1 record).
target_sources(sotc_core PRIVATE core/ctor_slot_312_a46130.cpp)
add_executable(test_ctor_slot_312 tests/test_ctor_slot_312.cpp)
target_link_libraries(test_ctor_slot_312 sotc_core)
add_test(NAME ctor_slot_312 COMMAND test_ctor_slot_312)
# CPU-01-STARTUP-SLOT-314-CHARACTERIZATION-01 / -NATIVE-LIFT-01: constructor index 314 (0xa41810),
# the physics debug block (1 page, 10 variables, 10 toggles, 22 atexit records).
target_sources(sotc_core PRIVATE core/ctor_slot_314_a41810.cpp)
add_executable(test_ctor_slot_314 tests/test_ctor_slot_314.cpp)
target_link_libraries(test_ctor_slot_314 sotc_core)
add_test(NAME ctor_slot_314 COMMAND test_ctor_slot_314)
# CPU-01-STARTUP-SLOT-316-CHARACTERIZATION-01 / -NATIVE-LIFT-01: constructor index 316 (0xa33280),
# the Localization block (4 language variables, 1 bool, the `loc_reload` command, 6 toggles,
# the "!!!Unlocalized!!!" string, 14 atexit records).
target_sources(sotc_core PRIVATE core/ctor_slot_316_a33280.cpp)
add_executable(test_ctor_slot_316 tests/test_ctor_slot_316.cpp)
target_link_libraries(test_ctor_slot_316 sotc_core)
add_test(NAME ctor_slot_316 COMMAND test_ctor_slot_316)
# CPU-01-STARTUP-SLOT-326-CHARACTERIZATION-01: constructor index 326 (0x837f30), the Colossus /
# Shadow eye-colour debug page (2 bools, 13 float variables, 5 groups, 3 toggles, 12 sliders,
# 1 kind-0 label row, 36 atexit records).
target_sources(sotc_core PRIVATE core/ctor_slot_326_837f30.cpp)
add_executable(test_ctor_slot_326 tests/test_ctor_slot_326.cpp)
target_link_libraries(test_ctor_slot_326 sotc_core)
add_test(NAME ctor_slot_326 COMMAND test_ctor_slot_326)
# CPU-01-STARTUP-SLOT-329-CHARACTERIZATION-01: constructor index 329 (0x829e90), the Shadow Path /
# path-finding debug page (7 bools, 2 integer ranges, 1 float range, 1 group, 10 toggles,
# 1 separator row, 22 atexit records) plus its two local ranged-variable constructors.
target_sources(sotc_core PRIVATE core/ctor_slot_329_829e90.cpp)
add_executable(test_ctor_slot_329 tests/test_ctor_slot_329.cpp)
target_link_libraries(test_ctor_slot_329 sotc_core)
add_test(NAME ctor_slot_329 COMMAND test_ctor_slot_329)

# CPU-01-STARTUP-SLOT-335-CHARACTERIZATION-01: constructor index 335 (0x7edf10), the "Shadow Cheat"
# assist page (4 unbounded floats, 3 bools, 2 FNV-1a name records, 2 bounded float sliders,
# 2 sliders, 11 atexit records) and the 0x7edd80 / 0xf2f0 / 0x7dbca0 / 0x52a990 helpers it needs.
target_sources(sotc_core PRIVATE core/ctor_slot_335_7edf10.cpp)
add_executable(test_ctor_slot_335 tests/test_ctor_slot_335.cpp)
target_link_libraries(test_ctor_slot_335 sotc_core)
add_test(NAME ctor_slot_335 COMMAND test_ctor_slot_335)

# Parallel-lane fragments: native/lanes/<lane>.cmake. Workers configure with -DSOTC_LANES=<lane>
# to build only their own fragment; the integrator configures with the default "*" (all lanes).
set(SOTC_LANES "*" CACHE STRING "Lane fragment glob under native/lanes (without .cmake)")
file(GLOB SOTC_LANE_FRAGMENTS CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/lanes/${SOTC_LANES}.cmake")
foreach(sotc_lane_fragment IN LISTS SOTC_LANE_FRAGMENTS)
  include("${sotc_lane_fragment}")
endforeach()

# GPU-01-PM4-TRANSLATOR-01 slice 1: PS4 GNM/PM4 packet framing (bounds-checked, no opcode semantics).
add_library(sotc_pm4_decoder STATIC platform/pm4_decoder.cpp)
add_executable(test_pm4_decoder tests/test_pm4_decoder.cpp)
target_link_libraries(test_pm4_decoder PRIVATE sotc_pm4_decoder)
add_test(NAME pm4_decoder COMMAND test_pm4_decoder)

# GPU-01-PM4-TRANSLATOR-01 slice 2: PM4 register-file state tracking with snapshot/digest.
add_library(sotc_pm4_state STATIC platform/pm4_state.cpp)
target_link_libraries(sotc_pm4_state PUBLIC sotc_pm4_decoder)
add_executable(test_pm4_state tests/test_pm4_state.cpp)
target_link_libraries(test_pm4_state PRIVATE sotc_pm4_state)
add_test(NAME pm4_state COMMAND test_pm4_state)

# GPU-01-PM4-TRANSLATOR-01 slice 3: policy-driven indirect traversal + typed draws.
add_library(sotc_pm4_indirect STATIC platform/pm4_indirect.cpp)
target_link_libraries(sotc_pm4_indirect PUBLIC sotc_pm4_decoder)
add_executable(test_pm4_indirect tests/test_pm4_indirect.cpp)
target_link_libraries(test_pm4_indirect PRIVATE sotc_pm4_indirect)
add_test(NAME pm4_indirect COMMAND test_pm4_indirect)

# GPU-01-PM4-TRANSLATOR-01 slice 5: the opcode table this title's own writers emit, with a
# validating sink on top of the framer.
add_library(sotc_gnm_opcode_table STATIC platform/gnm_opcode_table.cpp)
target_link_libraries(sotc_gnm_opcode_table PUBLIC sotc_pm4_decoder)
add_executable(test_gnm_opcode_table tests/test_gnm_opcode_table.cpp)
target_link_libraries(test_gnm_opcode_table PRIVATE sotc_gnm_opcode_table)
add_test(NAME gnm_opcode_table COMMAND test_gnm_opcode_table)

# GPU-01-PM4-TRANSLATOR-01 slice 4: encode the typed draw list into one Metal render pass.
add_library(sotc_metal_draw_encoder STATIC platform/metal_draw_encoder.mm)
target_compile_options(sotc_metal_draw_encoder PRIVATE -Wall -Wextra -Werror)
target_link_libraries(sotc_metal_draw_encoder PUBLIC sotc_pm4_indirect
  "-framework Metal"
  "-framework Foundation")
add_executable(test_metal_draw_encoder tests/test_metal_draw_encoder.cpp)
target_link_libraries(test_metal_draw_encoder PRIVATE sotc_metal_draw_encoder sotc_pm4_indirect)
add_test(NAME metal_draw_encoder COMMAND test_metal_draw_encoder)
set_tests_properties(metal_draw_encoder PROPERTIES SKIP_RETURN_CODE 77)

# CPU-01-AOT-CFG-SCALE-01: the one statically resolved vtable slot (0x1594dc0 + 0x20 -> 0x20d60).
add_library(sotc_vtable_1594dc0_slot20 STATIC core/vtable_1594dc0_slot20_20d60.cpp)
target_link_libraries(sotc_vtable_1594dc0_slot20 PUBLIC sotc_core)
add_executable(test_vtable_1594dc0_slot20 tests/test_vtable_1594dc0_slot20_20d60.cpp)
target_link_libraries(test_vtable_1594dc0_slot20 PRIVATE sotc_vtable_1594dc0_slot20)
add_test(NAME vtable_1594dc0_slot20 COMMAND test_vtable_1594dc0_slot20)

# CPU-01-AOT-CFG-SCALE-01: statically resolved vtable call sites (image + RELA verified).
add_library(sotc_vtable_resolved_entries STATIC core/vtable_resolved_entries.cpp)
target_link_libraries(sotc_vtable_resolved_entries PUBLIC sotc_core)
add_executable(test_vtable_resolved_entries tests/test_vtable_resolved_entries.cpp)
target_link_libraries(test_vtable_resolved_entries PRIVATE sotc_vtable_resolved_entries)
add_test(NAME vtable_resolved_entries COMMAND test_vtable_resolved_entries)

# CPU-01-AOT-CFG-SCALE-01: second resolved vtable slot (0x15949a0 + 0x88 -> 0x187c0).
add_library(sotc_vtable_15949a0_slot88 STATIC core/vtable_15949a0_slot88_187c0.cpp)
target_link_libraries(sotc_vtable_15949a0_slot88 PUBLIC sotc_core)
add_executable(test_vtable_15949a0_slot88 tests/test_vtable_15949a0_slot88_187c0.cpp)
target_link_libraries(test_vtable_15949a0_slot88 PRIVATE sotc_vtable_15949a0_slot88)
add_test(NAME vtable_15949a0_slot88 COMMAND test_vtable_15949a0_slot88)

# CPU-01-AOT-CFG-SCALE-01: the remaining resolved vtable slots (0x13d20, 0x358c0, 0xc740).
add_library(sotc_vtable_resolved_bodies_2 STATIC
  core/vtable_1594ae8_slot28_13d20.cpp
  core/vtable_15955c0_slot08_358c0.cpp
  core/vtable_15d1d80_slot08_c740.cpp)
target_link_libraries(sotc_vtable_resolved_bodies_2 PUBLIC sotc_core)
add_executable(test_vtable_resolved_bodies_2 tests/test_vtable_resolved_bodies_2.cpp)
target_link_libraries(test_vtable_resolved_bodies_2 PRIVATE sotc_vtable_resolved_bodies_2)
add_test(NAME vtable_resolved_bodies_2 COMMAND test_vtable_resolved_bodies_2)

# CPU-01-AOT-CFG-SCALE-01: the two smallest newly resolved targets (0xc2e0, 0xa40e20).
add_library(sotc_vtable_resolved_bodies_3 STATIC
  core/vtable_15d1678_slot_e8_c2e0.cpp
  core/vtable_15d1678_slot_68_a40e20.cpp)
target_link_libraries(sotc_vtable_resolved_bodies_3 PUBLIC sotc_core)
add_executable(test_vtable_resolved_bodies_3 tests/test_vtable_resolved_bodies_3.cpp)
target_link_libraries(test_vtable_resolved_bodies_3 PRIVATE sotc_vtable_resolved_bodies_3)
add_test(NAME vtable_resolved_bodies_3 COMMAND test_vtable_resolved_bodies_3)

# CPU-01-AOT-CFG-SCALE-01: resolved teardown target 0xa460f0 (vtable 0x15d1678 slot 0x280).
add_library(sotc_vtable_15d1678_slot280 STATIC core/vtable_15d1678_slot280_a460f0.cpp)
target_link_libraries(sotc_vtable_15d1678_slot280 PUBLIC sotc_core)
add_executable(test_vtable_15d1678_slot280 tests/test_vtable_15d1678_slot280_a460f0.cpp)
target_link_libraries(test_vtable_15d1678_slot280 PRIVATE sotc_vtable_15d1678_slot280)
add_test(NAME vtable_15d1678_slot280 COMMAND test_vtable_15d1678_slot280)

# CPU-01-AOT-CFG-SCALE-01: resolved teardown target 0x2687b0 (vtable 0x159c908 slot 0x58).
add_library(sotc_vtable_159c908_slot58 STATIC core/vtable_159c908_slot58_2687b0.cpp)
target_link_libraries(sotc_vtable_159c908_slot58 PUBLIC sotc_core)
add_executable(test_vtable_159c908_slot58 tests/test_vtable_159c908_slot58_2687b0.cpp)
target_link_libraries(test_vtable_159c908_slot58 PRIVATE sotc_vtable_159c908_slot58)
add_test(NAME vtable_159c908_slot58 COMMAND test_vtable_159c908_slot58)

# CPU-01-AOT-CFG-SCALE-01: resolved teardown target 0x1bf710 (vtable 0x1598e10 slot 0x18).
add_library(sotc_vtable_1598e10_slot18 STATIC core/vtable_1598e10_slot18_1bf710.cpp)
target_link_libraries(sotc_vtable_1598e10_slot18 PUBLIC sotc_core)
add_executable(test_vtable_1598e10_slot18 tests/test_vtable_1598e10_slot18_1bf710.cpp)
target_link_libraries(test_vtable_1598e10_slot18 PRIVATE sotc_vtable_1598e10_slot18)
add_test(NAME vtable_1598e10_slot18 COMMAND test_vtable_1598e10_slot18)

# CPU-01-AOT-CFG-SCALE-01: resolved manager-dispatch target 0xddf4b0 (vtable 0x15e0b48 slot 0x28).
add_library(sotc_vtable_15e0b48_slot28 STATIC core/vtable_15e0b48_slot28_ddf4b0.cpp)
target_link_libraries(sotc_vtable_15e0b48_slot28 PUBLIC sotc_core)
add_executable(test_vtable_15e0b48_slot28 tests/test_vtable_15e0b48_slot28_ddf4b0.cpp)
target_link_libraries(test_vtable_15e0b48_slot28 PRIVATE sotc_vtable_15e0b48_slot28)
add_test(NAME vtable_15e0b48_slot28 COMMAND test_vtable_15e0b48_slot28)

# CPU-01-AOT-CFG-SCALE-01: resolved dual-vcall target 0xe16a00 (vtable 0x15f2f08 slot 0x38).
add_library(sotc_vtable_15f2f08_slot38 STATIC core/vtable_15f2f08_slot38_e16a00.cpp)
target_link_libraries(sotc_vtable_15f2f08_slot38 PUBLIC sotc_core)
add_executable(test_vtable_15f2f08_slot38 tests/test_vtable_15f2f08_slot38_e16a00.cpp)
target_link_libraries(test_vtable_15f2f08_slot38 PRIVATE sotc_vtable_15f2f08_slot38)
add_test(NAME vtable_15f2f08_slot38 COMMAND test_vtable_15f2f08_slot38)

# CPU-01-AOT-CFG-SCALE-01: resolved lookup target 0xe1a5c0 (vtable 0x15e2028 slot 0x70).
add_library(sotc_vtable_15e2028_slot70 STATIC core/vtable_15e2028_slot70_e1a5c0.cpp)
target_link_libraries(sotc_vtable_15e2028_slot70 PUBLIC sotc_core)
add_executable(test_vtable_15e2028_slot70 tests/test_vtable_15e2028_slot70_e1a5c0.cpp)
target_link_libraries(test_vtable_15e2028_slot70 PRIVATE sotc_vtable_15e2028_slot70)
add_test(NAME vtable_15e2028_slot70 COMMAND test_vtable_15e2028_slot70)

# CPU-01-AOT-CFG-SCALE-01: resolved alignment-dispatch target 0xe19980 (vtable 0x15e1ee0 slot 0xb0).
add_library(sotc_vtable_15e1ee0_slot_b0 STATIC core/vtable_15e1ee0_slot_b0_e19980.cpp)
target_link_libraries(sotc_vtable_15e1ee0_slot_b0 PUBLIC sotc_core)
add_executable(test_vtable_15e1ee0_slot_b0 tests/test_vtable_15e1ee0_slot_b0_e19980.cpp)
target_link_libraries(test_vtable_15e1ee0_slot_b0 PRIVATE sotc_vtable_15e1ee0_slot_b0)
add_test(NAME vtable_15e1ee0_slot_b0 COMMAND test_vtable_15e1ee0_slot_b0)

# CPU-01-AOT-CFG-SCALE-01: resolved status-code lookup 0xe21420 (vtable 0x15e2028 slot 0x2a0).
add_library(sotc_vtable_15e2028_slot2a0 STATIC core/vtable_15e2028_slot2a0_e21420.cpp)
target_link_libraries(sotc_vtable_15e2028_slot2a0 PUBLIC sotc_core)
add_executable(test_vtable_15e2028_slot2a0 tests/test_vtable_15e2028_slot2a0_e21420.cpp)
target_link_libraries(test_vtable_15e2028_slot2a0 PRIVATE sotc_vtable_15e2028_slot2a0)
add_test(NAME vtable_15e2028_slot2a0 COMMAND test_vtable_15e2028_slot2a0)

# CPU-01-AOT-CFG-SCALE-01: resolved sub-dispatch target 0xe24900 (vtable 0x15e2d60 slot 0xd0).
add_library(sotc_vtable_15e2d60_slot_d0 STATIC core/vtable_15e2d60_slot_d0_e24900.cpp)
target_link_libraries(sotc_vtable_15e2d60_slot_d0 PUBLIC sotc_core)
add_executable(test_vtable_15e2d60_slot_d0 tests/test_vtable_15e2d60_slot_d0_e24900.cpp)
target_link_libraries(test_vtable_15e2d60_slot_d0 PRIVATE sotc_vtable_15e2d60_slot_d0)
add_test(NAME vtable_15e2d60_slot_d0 COMMAND test_vtable_15e2d60_slot_d0)

# CPU-01-AOT-CFG-SCALE-01: remaining slots of the already-resolved tables (vtable-closure sweep).
add_library(sotc_vtable_siblings_low STATIC
  core/vtable_1594ae8_slot40_13d50.cpp
  core/vtable_1594ae8_slot50_13d60.cpp
  core/vtable_15949a0_slot58_c2c0.cpp
  core/vtable_1595320_slot10_30e50.cpp)
target_link_libraries(sotc_vtable_siblings_low PUBLIC sotc_core)
add_executable(test_vtable_siblings_low tests/test_vtable_siblings_low.cpp)
target_link_libraries(test_vtable_siblings_low PRIVATE sotc_vtable_siblings_low)
add_test(NAME vtable_siblings_low COMMAND test_vtable_siblings_low)

# CPU-01-AOT-CFG-SCALE-01: container adapters reached through the 0x15e2028/0x15e1ee0 tables.
add_library(sotc_vtable_containers_e1 STATIC
  core/vtable_15e2028_slot40_e1a040.cpp
  core/vtable_15e2028_slot38_e19f70.cpp
  core/vtable_15e2028_slot18_e19cb0.cpp
  core/vtable_15e1ee0_slot10_e19440.cpp)
target_link_libraries(sotc_vtable_containers_e1 PUBLIC sotc_core)
add_executable(test_vtable_containers_e1 tests/test_vtable_containers_e1.cpp)
target_link_libraries(test_vtable_containers_e1 PRIVATE sotc_vtable_containers_e1)
add_test(NAME vtable_containers_e1 COMMAND test_vtable_containers_e1)

# CPU-01-AOT-CFG-SCALE-01: three-owner teardown 0xe52cb0 and the two plain tail frees.
add_library(sotc_vtable_teardown_tail STATIC
  core/vtable_15ed2a0_slot10_e52cb0.cpp
  core/vtable_15f2f08_slot88_ed4860.cpp
  core/vtable_1595b60_slot18_488d0.cpp)
target_link_libraries(sotc_vtable_teardown_tail PUBLIC sotc_core)
add_executable(test_vtable_teardown_tail tests/test_vtable_teardown_tail.cpp)
target_link_libraries(test_vtable_teardown_tail PRIVATE sotc_vtable_teardown_tail)
add_test(NAME vtable_teardown_tail COMMAND test_vtable_teardown_tail)

# CPU-01-AOT-CFG-SCALE-01: retail SysV argument contract for the resolved targets.
add_library(sotc_retail_abi STATIC core/retail_abi.cpp)
target_link_libraries(sotc_retail_abi PUBLIC sotc_core)
add_executable(test_retail_abi tests/test_retail_abi.cpp)
target_link_libraries(test_retail_abi PRIVATE sotc_retail_abi)
add_test(NAME retail_abi COMMAND test_retail_abi)

# CPU-01-AOT-CFG-SCALE-01: resolved status-gated helper target 0xe26f90 (vtable 0x15ed2a0 slot 0xc0).
add_library(sotc_vtable_15ed2a0_slot_c0 STATIC core/vtable_15ed2a0_slot_c0_e26f90.cpp)
target_link_libraries(sotc_vtable_15ed2a0_slot_c0 PUBLIC sotc_core)
add_executable(test_vtable_15ed2a0_slot_c0 tests/test_vtable_15ed2a0_slot_c0_e26f90.cpp)
target_link_libraries(test_vtable_15ed2a0_slot_c0 PRIVATE sotc_vtable_15ed2a0_slot_c0)
add_test(NAME vtable_15ed2a0_slot_c0 COMMAND test_vtable_15ed2a0_slot_c0)

# CPU-01-AOT-CFG-SCALE-01: checked guest-memory window for retail frame marshalling.
add_library(sotc_guest_memory STATIC core/guest_memory.cpp)
target_link_libraries(sotc_guest_memory PUBLIC sotc_core)
add_executable(test_guest_memory tests/test_guest_memory.cpp)
target_link_libraries(test_guest_memory PRIVATE sotc_guest_memory)
add_test(NAME guest_memory COMMAND test_guest_memory)

# CPU-01-AOT-CFG-SCALE-01: guest-address -> host-cell marshalling for the retail thunk layer.
add_library(sotc_retail_cells STATIC core/retail_cells.cpp)
target_link_libraries(sotc_retail_cells PUBLIC sotc_guest_memory sotc_vtable_1594dc0_slot20
  sotc_vtable_resolved_bodies_2 sotc_vtable_15949a0_slot88
  sotc_vtable_159c908_slot58 sotc_vtable_1598e10_slot18
  sotc_vtable_resolved_bodies_3
  sotc_vtable_15f2f08_slot38 sotc_vtable_15e0b48_slot28 sotc_vtable_15d1678_slot280
  sotc_vtable_15e2d60_slot_d0 sotc_vtable_15ed2a0_slot_c0 sotc_vtable_15e2028_slot70
  sotc_vtable_15e1ee0_slot_b0 sotc_vtable_15e2028_slot2a0)
add_executable(test_retail_cells tests/test_retail_cells.cpp)
target_link_libraries(test_retail_cells PRIVATE sotc_retail_cells)
add_test(NAME retail_cells COMMAND test_retail_cells)

# CPU-01-AOT-CFG-SCALE-01: single retail call entry point (frame + window -> model -> rax).
add_library(sotc_retail_call STATIC core/retail_call.cpp)
target_link_libraries(sotc_retail_call PUBLIC sotc_retail_abi sotc_retail_cells sotc_guest_memory
  sotc_guest_tls)
add_executable(test_retail_call tests/test_retail_call.cpp)
target_link_libraries(test_retail_call PRIVATE sotc_retail_call)
add_test(NAME retail_call COMMAND test_retail_call)

# CPU-01-AOT-CFG-SCALE-01: guest thread / TLS model (fs:0 thread pointer + checked per-thread block).
add_library(sotc_guest_tls STATIC core/guest_tls.cpp)
target_link_libraries(sotc_guest_tls PUBLIC sotc_core)
add_executable(test_guest_tls tests/test_guest_tls.cpp)
target_link_libraries(test_guest_tls PRIVATE sotc_guest_tls)
add_test(NAME guest_tls COMMAND test_guest_tls)

# CPU-01-AOT-CFG-SCALE-01: per-thread record stack (0x12300 push, 0x12350 pop) on the TLS model.
add_library(sotc_tls_record_stack STATIC core/tls_record_stack.cpp)
target_link_libraries(sotc_tls_record_stack PUBLIC sotc_guest_tls)
add_executable(test_tls_record_stack tests/test_tls_record_stack.cpp)
target_link_libraries(test_tls_record_stack PRIVATE sotc_tls_record_stack)
add_test(NAME tls_record_stack COMMAND test_tls_record_stack)

# CPU-01-AOT-CFG-SCALE-01: the TLS allocation-site path inside 0x11fb0 / 0x12820.
add_library(sotc_alloc_site_context STATIC core/alloc_site_context.cpp)
target_link_libraries(sotc_alloc_site_context PUBLIC sotc_tls_record_stack)
add_executable(test_alloc_site_context tests/test_alloc_site_context.cpp)
target_link_libraries(test_alloc_site_context PRIVATE sotc_alloc_site_context)
add_test(NAME alloc_site_context COMMAND test_alloc_site_context)

# CPU-01-AOT-CFG-SCALE-01: the two terminal paths of the 0xe21420 decision contract.
add_library(sotc_vtable_15e2028_slot2a0_tail STATIC core/vtable_15e2028_slot2a0_e21420_tail.cpp)
target_link_libraries(sotc_vtable_15e2028_slot2a0_tail PUBLIC sotc_core)
add_executable(test_vtable_15e2028_slot2a0_tail tests/test_vtable_15e2028_slot2a0_e21420_tail.cpp)
target_link_libraries(test_vtable_15e2028_slot2a0_tail PRIVATE sotc_vtable_15e2028_slot2a0_tail)
add_test(NAME vtable_15e2028_slot2a0_tail COMMAND test_vtable_15e2028_slot2a0_tail)

# CPU-01-AOT-CFG-SCALE-01: Fios read slots (0x31870/0x318b0) and the 0x4b0 array teardown.
add_library(sotc_vtable_15953f8_fios STATIC core/vtable_15953f8_slot_a0_a8_fios_reads.cpp
  core/vtable_15953f8_slot18_31310.cpp)
target_link_libraries(sotc_vtable_15953f8_fios PUBLIC sotc_core)
add_library(sotc_vtable_1594dc0_slot10 STATIC core/vtable_1594dc0_slot10_4b0.cpp)
target_link_libraries(sotc_vtable_1594dc0_slot10 PUBLIC sotc_core)
add_executable(test_vtable_15953f8_fios_and_4b0 tests/test_vtable_15953f8_fios_and_4b0.cpp)
target_link_libraries(test_vtable_15953f8_fios_and_4b0 PRIVATE sotc_vtable_15953f8_fios
  sotc_vtable_1594dc0_slot10)
add_test(NAME vtable_15953f8_fios_and_4b0 COMMAND test_vtable_15953f8_fios_and_4b0)

# CPU-01-STARTUP-SLOT-309-NATIVE-LIFT-01 / -RANGED-BOUND-VAR-A4F4D0-01: constructor index 309
# (0xa505e0), the Save System variable group (3 console variables through the shared 0xa4f4d0 family,
# 1 name object, 1 bool variable, 1 empty view, 4 toggles) and its 12 __cxa_atexit registrations.
# Registered in startup_ctors with default_slot309_hooks(). See core/ctor_slot_309_a505e0.hpp,
# core/ranged_bound_var_a4f4d0.hpp and tools/lift/check_consvar_slots.py.
add_executable(test_ctor_slot_309 tests/test_ctor_slot_309.cpp)
target_link_libraries(test_ctor_slot_309 PRIVATE sotc_core)
add_test(NAME ctor_slot_309 COMMAND test_ctor_slot_309)

# The shared console-variable constructor family (0xa4f4d0 / 0xa4fec0 / 0xa50280 wrappers over the
# 0xa4f690 / 0xa50080 / 0xa50440 leaves), used by startup slot 309.
add_executable(test_ranged_bound_var_a4f4d0 tests/test_ranged_bound_var_a4f4d0.cpp)
target_link_libraries(test_ranged_bound_var_a4f4d0 PRIVATE sotc_core)
add_test(NAME ranged_bound_var_a4f4d0 COMMAND test_ranged_bound_var_a4f4d0)

# CPU-01-AOT-CFG-SCALE-01: provenance-audited vtable-closure slice. Four targets whose call site
# loads the container address with its nearest register definition: 0x9b7b80 (float getter through
# 0x15cf248 + 0x68), 0xd7c4b0 (constant zero through 0x15de228 + 0x20), 0x6e92c0 (preset apply
# through 0x2dcb170 + 0x08) and 0x9804c0 (record-array builder through 0x2dd09b0 + 0x08).
add_library(sotc_vtable_frontier2 STATIC
  core/vtable_15cf248_slot68_9b7b80.cpp
  core/vtable_15de228_slot20_d7c4b0.cpp
  core/vtable_2dcb170_slot08_6e92c0.cpp
  core/vtable_2dd09b0_slot08_9804c0.cpp)
target_link_libraries(sotc_vtable_frontier2 PUBLIC sotc_core)
add_executable(test_vtable_frontier2 tests/test_vtable_frontier2.cpp)
target_link_libraries(test_vtable_frontier2 PRIVATE sotc_vtable_frontier2)
add_test(NAME vtable_frontier2 COMMAND test_vtable_frontier2)

# CPU-01-INIT-21620-LOOKUP-01: native `bpe::ConCmdManager::FindConCmd` (`0x21620`, 464 B pinned) —
# the locked manager-map path, the guarded static pending-list path, and the adapters for the two
# retail call sites (lazy_bind @ 0x91227, vslot-40 miss @ 0x53327d). Additive.
# See core/lookup_21620.hpp and tools/lift/check_lookup_21620.py.
target_sources(sotc_core PRIVATE core/lookup_21620.cpp)
add_executable(test_lookup_21620 tests/test_lookup_21620.cpp)
target_link_libraries(test_lookup_21620 sotc_core)
add_test(NAME lookup_21620 COMMAND test_lookup_21620)

# CPU-01-CONCMD-MANAGER-210B0-01: native manager constructor + pending drain (`0x210b0`, 273 B) —
# the eight header stores, the guarded static list (shared with 0x20d80/0x21620), the FIFO drain
# through the register BODY 0x20d80 with the manager as its argument, and the re-termination block
# that re-empties the head. Additive. See core/concmd_manager_210b0.hpp and
# tools/lift/check_concmd_manager_210b0.py.
target_sources(sotc_core PRIVATE core/concmd_manager_210b0.cpp)
add_executable(test_concmd_manager_210b0 tests/test_concmd_manager_210b0.cpp)
target_link_libraries(test_concmd_manager_210b0 sotc_core)
add_test(NAME concmd_manager_210b0 COMMAND test_concmd_manager_210b0)

# CPU-01-STARTUP-BATCH-DEBUG-PAGE-01: one parameterised model for the smallest uniform cluster of
# debug-page startup constructors (slots 361 / 467 / 379 / 536). The four bodies are byte-for-byte the
# same statement sequence (one or two bounded-float console variables through 0xc300, plus optionally one
# 0x27050 slider or 0x26ea0 toggle item), so core/debug_page_var_item carries the sequence and each
# ctor_slot_<N>_<rva>.hpp carries only the per-slot DATA. See core/debug_page_var_item.hpp.
target_sources(sotc_core PRIVATE core/debug_page_var_item.cpp)

target_sources(sotc_core PRIVATE core/ctor_slot_361_777390.cpp)
add_executable(test_ctor_slot_361 tests/test_ctor_slot_361.cpp)
target_link_libraries(test_ctor_slot_361 sotc_core)
add_test(NAME ctor_slot_361 COMMAND test_ctor_slot_361)

target_sources(sotc_core PRIVATE core/ctor_slot_467_3ca110.cpp)
add_executable(test_ctor_slot_467 tests/test_ctor_slot_467.cpp)
target_link_libraries(test_ctor_slot_467 sotc_core)
add_test(NAME ctor_slot_467 COMMAND test_ctor_slot_467)

target_sources(sotc_core PRIVATE core/ctor_slot_379_6d8d20.cpp)
add_executable(test_ctor_slot_379 tests/test_ctor_slot_379.cpp)
target_link_libraries(test_ctor_slot_379 sotc_core)
add_test(NAME ctor_slot_379 COMMAND test_ctor_slot_379)

target_sources(sotc_core PRIVATE core/ctor_slot_536_1342d0.cpp)
add_executable(test_ctor_slot_536 tests/test_ctor_slot_536.cpp)
target_link_libraries(test_ctor_slot_536 sotc_core)
add_test(NAME ctor_slot_536 COMMAND test_ctor_slot_536)

# CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 2): slot 374 is the batch's one-variable + 0x26ea0-toggle
# shape preceded by ONE leading `{name, fnv1a(name)}` record (core/debug_page_var_item HashedNameRow).
target_sources(sotc_core PRIVATE core/ctor_slot_374_701230.cpp)
add_executable(test_ctor_slot_374 tests/test_ctor_slot_374.cpp)
target_link_libraries(test_ctor_slot_374 sotc_core)
add_test(NAME ctor_slot_374 COMMAND test_ctor_slot_374)

# CPU-01-AOT-CFG-SCALE-01 (frontier audit E1095 follow-up, E1100): body models of the real targets of
# the eight withdrawn vptr-skew sites: 0x47860 (spin-locked qword store), 0x31900 (Fios op poll),
# 0x318f0 (op succeeded), 0x31520 (wait + FHCloseSync) and the 0x319c0 wait that 0x31520 reaches.
# FIXTURE_VERIFIED only; tools/lift/check_frontier_target_models.py pins the ELF facts.
add_library(sotc_frontier_targets STATIC
  core/frontier_47860_locked_store.cpp
  core/frontier_31900_fios_poll.cpp
  core/frontier_318f0_fios_succeeded.cpp
  core/frontier_31520_fios_close.cpp
  core/frontier_319c0_fios_wait.cpp)
target_link_libraries(sotc_frontier_targets PUBLIC sotc_core)
add_executable(test_frontier_target_models tests/test_frontier_target_models.cpp)
target_link_libraries(test_frontier_target_models PRIVATE sotc_frontier_targets sotc_vtable_15953f8_fios)
add_test(NAME frontier_target_models COMMAND test_frontier_target_models)

# CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 3): slot 518 is a one-variable data row plus a bounded static-float
# tail (the 8.5f cache-derived spacing cells at 0x2ee66c0..0x2ee66ec).
target_sources(sotc_core PRIVATE core/ctor_slot_518_1a57d0.cpp)
add_executable(test_ctor_slot_518 tests/test_ctor_slot_518.cpp)
target_link_libraries(test_ctor_slot_518 sotc_core)
add_test(NAME ctor_slot_518 COMMAND test_ctor_slot_518)

# CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 3): bool debug-page slots 410 / 362 / 477 / 423 / 449 (bool variables in
# the shared init_bool_var_static order, plus {name, fnv1a(name)} records at 362 / 423 and two 0x26ea0 toggles at 449).
target_sources(sotc_core PRIVATE
  core/ctor_slot_410_5ac580.cpp
  core/ctor_slot_362_76b8d0.cpp
  core/ctor_slot_477_3270d0.cpp
  core/ctor_slot_423_55f080.cpp
  core/ctor_slot_449_4b0200.cpp)
add_executable(test_ctor_slot_410 tests/test_ctor_slot_410.cpp)
target_link_libraries(test_ctor_slot_410 sotc_core)
add_test(NAME ctor_slot_410 COMMAND test_ctor_slot_410)
add_executable(test_ctor_slot_362 tests/test_ctor_slot_362.cpp)
target_link_libraries(test_ctor_slot_362 sotc_core)
add_test(NAME ctor_slot_362 COMMAND test_ctor_slot_362)
add_executable(test_ctor_slot_477 tests/test_ctor_slot_477.cpp)
target_link_libraries(test_ctor_slot_477 sotc_core)
add_test(NAME ctor_slot_477 COMMAND test_ctor_slot_477)
add_executable(test_ctor_slot_423 tests/test_ctor_slot_423.cpp)
target_link_libraries(test_ctor_slot_423 sotc_core)
add_test(NAME ctor_slot_423 COMMAND test_ctor_slot_423)
add_executable(test_ctor_slot_449 tests/test_ctor_slot_449.cpp)
target_link_libraries(test_ctor_slot_449 sotc_core)
add_test(NAME ctor_slot_449 COMMAND test_ctor_slot_449)

# CPU-01-HELPER-CALLEE-MODELS-01: family-admitting wrappers for the startup helper callees 0xc580 (ranged int),
# 0x3a980 (unbounded int), 0x6aa00 / 0xc300 (float) and a class-aware front end for the 0x26db0 named item. The tag
# tables are ELF-derived (37 int / 68 float functor class tables); tools/lift/check_helper_*.py pin them.
# FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED only; no original RVA is executed.
# The wrappers live in sotc_core (batch 6 startup slots 430 / 504 / 555 call them); sotc_helper_callees is kept as
# an INTERFACE alias so the helper tests keep their link line.
target_sources(sotc_core PRIVATE
  core/helper_functor_tags.cpp
  core/helper_c580_ranged_int.cpp
  core/helper_3a980_unbounded_int.cpp
  core/helper_6aa00_unbounded_float.cpp
  core/helper_26db0_named_item.cpp
  core/helper_int_var_sites.cpp
  core/helper_7ec160_enum_var.cpp
  core/helper_233ec0_unbounded_int_twin.cpp)
add_library(sotc_helper_callees INTERFACE)
target_link_libraries(sotc_helper_callees INTERFACE sotc_core)
add_executable(test_helper_c580 tests/test_helper_c580.cpp)
target_link_libraries(test_helper_c580 PRIVATE sotc_helper_callees)
add_test(NAME helper_c580 COMMAND test_helper_c580)
add_executable(test_helper_7ec160 tests/test_helper_7ec160.cpp)
target_link_libraries(test_helper_7ec160 PRIVATE sotc_helper_callees)
add_test(NAME helper_7ec160 COMMAND test_helper_7ec160)
add_executable(test_helper_233ec0 tests/test_helper_233ec0.cpp)
target_link_libraries(test_helper_233ec0 PRIVATE sotc_helper_callees)
add_test(NAME helper_233ec0 COMMAND test_helper_233ec0)
add_executable(test_helper_3a980 tests/test_helper_3a980.cpp)
target_link_libraries(test_helper_3a980 PRIVATE sotc_helper_callees)
add_test(NAME helper_3a980 COMMAND test_helper_3a980)
add_executable(test_helper_6aa00 tests/test_helper_6aa00.cpp)
target_link_libraries(test_helper_6aa00 PRIVATE sotc_helper_callees)
add_test(NAME helper_6aa00 COMMAND test_helper_6aa00)
add_executable(test_helper_26db0 tests/test_helper_26db0.cpp)
target_link_libraries(test_helper_26db0 PRIVATE sotc_helper_callees)
add_test(NAME helper_26db0 COMMAND test_helper_26db0)

# CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 4): debug-page slots 338 (nine bool variables), 368 (three bool variables,
# three 0x26ea0 toggles, 18 literal static-cell stores), 443 / 512 (two bool variables, a 0x26c10 group and two toggles)
# and 550 (one bool variable, two toggles, one 0x30-byte name holder).
target_sources(sotc_core PRIVATE
  core/ctor_slot_338_7e6c90.cpp
  core/ctor_slot_368_73df50.cpp
  core/ctor_slot_443_4de370.cpp
  core/ctor_slot_512_1d5e80.cpp
  core/ctor_slot_550_91290.cpp)
add_executable(test_ctor_slot_338 tests/test_ctor_slot_338.cpp)
target_link_libraries(test_ctor_slot_338 sotc_core)
add_test(NAME ctor_slot_338 COMMAND test_ctor_slot_338)
add_executable(test_ctor_slot_368 tests/test_ctor_slot_368.cpp)
target_link_libraries(test_ctor_slot_368 sotc_core)
add_test(NAME ctor_slot_368 COMMAND test_ctor_slot_368)
add_executable(test_ctor_slot_443 tests/test_ctor_slot_443.cpp)
target_link_libraries(test_ctor_slot_443 sotc_core)
add_test(NAME ctor_slot_443 COMMAND test_ctor_slot_443)
add_executable(test_ctor_slot_512 tests/test_ctor_slot_512.cpp)
target_link_libraries(test_ctor_slot_512 sotc_core)
add_test(NAME ctor_slot_512 COMMAND test_ctor_slot_512)
add_executable(test_ctor_slot_550 tests/test_ctor_slot_550.cpp)
target_link_libraries(test_ctor_slot_550 sotc_core)
add_test(NAME ctor_slot_550 COMMAND test_ctor_slot_550)

# CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 5): debug-page slot 437 (three bool variables and a 16-element array of
# 0xf2f0 borrowed string views registered through four NULL-argument __cxa_atexit chunk destructors).
target_sources(sotc_core PRIVATE
  core/ctor_slot_437_5070c0.cpp)
add_executable(test_ctor_slot_437 tests/test_ctor_slot_437.cpp)
target_link_libraries(test_ctor_slot_437 sotc_core)
add_test(NAME ctor_slot_437 COMMAND test_ctor_slot_437)

# CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): debug-page slots 366 and 411 (two bool variables and two 0x26db0 named
# toggles each; the bool class admits the functor tag and the named item takes slot3/slot6 of the same class).
target_sources(sotc_core PRIVATE
  core/ctor_slot_366_744ac0.cpp
  core/ctor_slot_411_591710.cpp
  core/ctor_slot_430_529df0.cpp
  core/ctor_slot_504_23f4d0.cpp
  core/ctor_slot_555_527f0.cpp
  core/ctor_slot_397_64d760.cpp
  core/ctor_slot_442_4e3760.cpp
  core/ctor_slot_450_4af4b0.cpp
  core/ctor_slot_453_48ac10.cpp
  core/ctor_slot_460_41c340.cpp
  core/ctor_slot_464_3db070.cpp
  core/ctor_slot_514_1c4c50.cpp
  core/ctor_slot_455_47bfa0.cpp
  core/ctor_slot_488_2ba5d0.cpp
  core/ctor_slot_369_734190.cpp
  core/ctor_slot_420_56f6e0.cpp
  core/ctor_slot_336_7ec4c0.cpp
  core/ctor_slot_506_2340b0.cpp
  core/ctor_slot_392_681690.cpp
  core/ctor_slot_396_650f20.cpp
  core/ctor_slot_337_7e81d0.cpp
  core/ctor_slot_444_4d3f10.cpp
  core/ctor_slot_521_17bf50.cpp
  core/ctor_slot_400_62b100.cpp
  core/ctor_slot_474_344850.cpp
  core/ctor_slot_351_7ae220.cpp
  core/ctor_slot_339_7e2fc0.cpp
  core/ctor_slot_395_65af60.cpp
  core/ctor_slot_380_6d71f0.cpp
  core/ctor_slot_375_6f1c20.cpp
  core/ctor_slot_382_6c8600.cpp
  core/ctor_slot_360_7806f0.cpp
  core/ctor_slot_373_7081e0.cpp
  core/ctor_slot_354_79e180.cpp
  core/ctor_slot_340_7de7e0.cpp
  core/name_cell_23fd00.cpp
  core/ctor_slot_342_7d8e10.cpp
  core/ctor_slot_349_7bde50.cpp
  core/ctor_slot_388_6a8c60.cpp
  core/ctor_slot_480_30daf0.cpp
  core/ctor_slot_352_7a2390.cpp
  core/ctor_slot_401_621530.cpp
  core/ctor_slot_398_63cf60.cpp
  core/ctor_slot_363_751c70.cpp
  core/ctor_slot_367_740980.cpp
  core/ctor_slot_358_786f50.cpp
  core/ctor_slot_391_68ebc0.cpp
  core/ctor_slot_381_6cd5b0.cpp
  core/ctor_slot_561_3ac00.cpp
  core/ctor_slot_399_632ca0.cpp
  core/ctor_slot_418_572a10.cpp
  core/ctor_slot_370_720b50.cpp
  core/ctor_slot_469_394a20.cpp
  core/ctor_slot_513_1d0770.cpp
  core/ctor_slot_508_2241e0.cpp
  core/ctor_slot_390_691c50.cpp
  core/ctor_slot_402_610430.cpp
  core/ctor_slot_572_1a20.cpp
  core/ctor_slot_493_277560.cpp
  core/ctor_slot_422_56b980.cpp
  core/ctor_slot_511_1d90c0.cpp
  core/ctor_slot_503_247f00.cpp
  core/ctor_slot_571_c780.cpp)
add_executable(test_ctor_slot_366 tests/test_ctor_slot_366.cpp)
target_link_libraries(test_ctor_slot_366 sotc_core)
add_test(NAME ctor_slot_366 COMMAND test_ctor_slot_366)
add_executable(test_ctor_slot_411 tests/test_ctor_slot_411.cpp)
target_link_libraries(test_ctor_slot_411 sotc_core)
add_test(NAME ctor_slot_411 COMMAND test_ctor_slot_411)
add_executable(test_ctor_slot_430 tests/test_ctor_slot_430.cpp)
target_link_libraries(test_ctor_slot_430 sotc_core)
add_test(NAME ctor_slot_430 COMMAND test_ctor_slot_430)
add_executable(test_ctor_slot_504 tests/test_ctor_slot_504.cpp)
target_link_libraries(test_ctor_slot_504 sotc_core)
add_test(NAME ctor_slot_504 COMMAND test_ctor_slot_504)
add_executable(test_ctor_slot_555 tests/test_ctor_slot_555.cpp)
target_link_libraries(test_ctor_slot_555 sotc_core)
add_test(NAME ctor_slot_555 COMMAND test_ctor_slot_555)
add_executable(test_ctor_slot_397 tests/test_ctor_slot_397.cpp)
target_link_libraries(test_ctor_slot_397 sotc_core)
add_test(NAME ctor_slot_397 COMMAND test_ctor_slot_397)
add_executable(test_ctor_slot_442 tests/test_ctor_slot_442.cpp)
target_link_libraries(test_ctor_slot_442 sotc_core)
add_test(NAME ctor_slot_442 COMMAND test_ctor_slot_442)
add_executable(test_ctor_slot_450 tests/test_ctor_slot_450.cpp)
target_link_libraries(test_ctor_slot_450 sotc_core)
add_test(NAME ctor_slot_450 COMMAND test_ctor_slot_450)
add_executable(test_ctor_slot_453 tests/test_ctor_slot_453.cpp)
target_link_libraries(test_ctor_slot_453 sotc_core)
add_test(NAME ctor_slot_453 COMMAND test_ctor_slot_453)
add_executable(test_ctor_slot_460 tests/test_ctor_slot_460.cpp)
target_link_libraries(test_ctor_slot_460 sotc_core)
add_test(NAME ctor_slot_460 COMMAND test_ctor_slot_460)
add_executable(test_ctor_slot_464 tests/test_ctor_slot_464.cpp)
target_link_libraries(test_ctor_slot_464 sotc_core)
add_test(NAME ctor_slot_464 COMMAND test_ctor_slot_464)
add_executable(test_ctor_slot_514 tests/test_ctor_slot_514.cpp)
target_link_libraries(test_ctor_slot_514 sotc_core)
add_test(NAME ctor_slot_514 COMMAND test_ctor_slot_514)
add_executable(test_ctor_slot_455 tests/test_ctor_slot_455.cpp)
target_link_libraries(test_ctor_slot_455 sotc_core)
add_test(NAME ctor_slot_455 COMMAND test_ctor_slot_455)
add_executable(test_ctor_slot_488 tests/test_ctor_slot_488.cpp)
target_link_libraries(test_ctor_slot_488 sotc_core)
add_test(NAME ctor_slot_488 COMMAND test_ctor_slot_488)
add_executable(test_ctor_slot_369 tests/test_ctor_slot_369.cpp)
target_link_libraries(test_ctor_slot_369 sotc_core)
add_test(NAME ctor_slot_369 COMMAND test_ctor_slot_369)
add_executable(test_ctor_slot_420 tests/test_ctor_slot_420.cpp)
target_link_libraries(test_ctor_slot_420 sotc_core)
add_test(NAME ctor_slot_420 COMMAND test_ctor_slot_420)
add_executable(test_ctor_slot_336 tests/test_ctor_slot_336.cpp)
target_link_libraries(test_ctor_slot_336 sotc_core)
add_test(NAME ctor_slot_336 COMMAND test_ctor_slot_336)
add_executable(test_ctor_slot_506 tests/test_ctor_slot_506.cpp)
target_link_libraries(test_ctor_slot_506 sotc_core)
add_test(NAME ctor_slot_506 COMMAND test_ctor_slot_506)
add_executable(test_ctor_slot_392 tests/test_ctor_slot_392.cpp)
target_link_libraries(test_ctor_slot_392 sotc_core)
add_test(NAME ctor_slot_392 COMMAND test_ctor_slot_392)
add_executable(test_ctor_slot_396 tests/test_ctor_slot_396.cpp)
target_link_libraries(test_ctor_slot_396 sotc_core)
add_test(NAME ctor_slot_396 COMMAND test_ctor_slot_396)
add_executable(test_ctor_slot_337 tests/test_ctor_slot_337.cpp)
target_link_libraries(test_ctor_slot_337 sotc_core)
add_test(NAME ctor_slot_337 COMMAND test_ctor_slot_337)
add_executable(test_ctor_slot_444 tests/test_ctor_slot_444.cpp)
target_link_libraries(test_ctor_slot_444 sotc_core)
add_test(NAME ctor_slot_444 COMMAND test_ctor_slot_444)
add_executable(test_ctor_slot_521 tests/test_ctor_slot_521.cpp)
target_link_libraries(test_ctor_slot_521 sotc_core)
add_test(NAME ctor_slot_521 COMMAND test_ctor_slot_521)
add_executable(test_ctor_slot_400 tests/test_ctor_slot_400.cpp)
target_link_libraries(test_ctor_slot_400 sotc_core)
add_test(NAME ctor_slot_400 COMMAND test_ctor_slot_400)
add_executable(test_ctor_slot_474 tests/test_ctor_slot_474.cpp)
target_link_libraries(test_ctor_slot_474 sotc_core)
add_test(NAME ctor_slot_474 COMMAND test_ctor_slot_474)
add_executable(test_ctor_slot_351 tests/test_ctor_slot_351.cpp)
target_link_libraries(test_ctor_slot_351 sotc_core)
add_test(NAME ctor_slot_351 COMMAND test_ctor_slot_351)
add_executable(test_ctor_slot_339 tests/test_ctor_slot_339.cpp)
target_link_libraries(test_ctor_slot_339 sotc_core)
add_test(NAME ctor_slot_339 COMMAND test_ctor_slot_339)
add_executable(test_ctor_slot_395 tests/test_ctor_slot_395.cpp)
target_link_libraries(test_ctor_slot_395 sotc_core)
add_test(NAME ctor_slot_395 COMMAND test_ctor_slot_395)
add_executable(test_ctor_slot_380 tests/test_ctor_slot_380.cpp)
target_link_libraries(test_ctor_slot_380 sotc_core)
add_test(NAME ctor_slot_380 COMMAND test_ctor_slot_380)
add_executable(test_ctor_slot_375 tests/test_ctor_slot_375.cpp)
target_link_libraries(test_ctor_slot_375 sotc_core)
add_test(NAME ctor_slot_375 COMMAND test_ctor_slot_375)
add_executable(test_ctor_slot_382 tests/test_ctor_slot_382.cpp)
target_link_libraries(test_ctor_slot_382 sotc_core)
add_test(NAME ctor_slot_382 COMMAND test_ctor_slot_382)
add_executable(test_ctor_slot_360 tests/test_ctor_slot_360.cpp)
target_link_libraries(test_ctor_slot_360 sotc_core)
add_test(NAME ctor_slot_360 COMMAND test_ctor_slot_360)
add_executable(test_ctor_slot_373 tests/test_ctor_slot_373.cpp)
target_link_libraries(test_ctor_slot_373 sotc_core)
add_test(NAME ctor_slot_373 COMMAND test_ctor_slot_373)
add_executable(test_ctor_slot_354 tests/test_ctor_slot_354.cpp)
target_link_libraries(test_ctor_slot_354 sotc_core)
add_test(NAME ctor_slot_354 COMMAND test_ctor_slot_354)
add_executable(test_ctor_slot_340 tests/test_ctor_slot_340.cpp)
target_link_libraries(test_ctor_slot_340 sotc_core)
add_test(NAME ctor_slot_340 COMMAND test_ctor_slot_340)
add_executable(test_name_cell_23fd00 tests/test_name_cell_23fd00.cpp)
target_link_libraries(test_name_cell_23fd00 sotc_core)
add_test(NAME name_cell_23fd00 COMMAND test_name_cell_23fd00)
add_executable(test_ctor_slot_342 tests/test_ctor_slot_342.cpp)
target_link_libraries(test_ctor_slot_342 sotc_core)
add_test(NAME ctor_slot_342 COMMAND test_ctor_slot_342)
add_executable(test_ctor_slot_349 tests/test_ctor_slot_349.cpp)
target_link_libraries(test_ctor_slot_349 sotc_core)
add_test(NAME ctor_slot_349 COMMAND test_ctor_slot_349)
add_executable(test_ctor_slot_352 tests/test_ctor_slot_352.cpp)
target_link_libraries(test_ctor_slot_352 sotc_core)
add_test(NAME ctor_slot_352 COMMAND test_ctor_slot_352)
add_executable(test_ctor_slot_401 tests/test_ctor_slot_401.cpp)
target_link_libraries(test_ctor_slot_401 sotc_core)
add_test(NAME ctor_slot_401 COMMAND test_ctor_slot_401)
add_executable(test_ctor_slot_398 tests/test_ctor_slot_398.cpp)
target_link_libraries(test_ctor_slot_398 sotc_core)
add_test(NAME ctor_slot_398 COMMAND test_ctor_slot_398)
add_executable(test_ctor_slot_363 tests/test_ctor_slot_363.cpp)
target_link_libraries(test_ctor_slot_363 sotc_core)
add_test(NAME ctor_slot_363 COMMAND test_ctor_slot_363)
add_executable(test_ctor_slot_367 tests/test_ctor_slot_367.cpp)
target_link_libraries(test_ctor_slot_367 sotc_core)
add_test(NAME ctor_slot_367 COMMAND test_ctor_slot_367)
add_executable(test_ctor_slot_358 tests/test_ctor_slot_358.cpp)
target_link_libraries(test_ctor_slot_358 sotc_core)
add_test(NAME ctor_slot_358 COMMAND test_ctor_slot_358)
add_executable(test_ctor_slot_391 tests/test_ctor_slot_391.cpp)
target_link_libraries(test_ctor_slot_391 sotc_core)
add_test(NAME ctor_slot_391 COMMAND test_ctor_slot_391)
add_executable(test_ctor_slot_381 tests/test_ctor_slot_381.cpp)
target_link_libraries(test_ctor_slot_381 sotc_core)
add_test(NAME ctor_slot_381 COMMAND test_ctor_slot_381)
add_executable(test_ctor_slot_561 tests/test_ctor_slot_561.cpp)
target_link_libraries(test_ctor_slot_561 sotc_core)
add_test(NAME ctor_slot_561 COMMAND test_ctor_slot_561)
add_executable(test_ctor_slot_399 tests/test_ctor_slot_399.cpp)
target_link_libraries(test_ctor_slot_399 sotc_core)
add_test(NAME ctor_slot_399 COMMAND test_ctor_slot_399)
add_executable(test_ctor_slot_418 tests/test_ctor_slot_418.cpp)
target_link_libraries(test_ctor_slot_418 sotc_core)
add_test(NAME ctor_slot_418 COMMAND test_ctor_slot_418)
add_executable(test_ctor_slot_370 tests/test_ctor_slot_370.cpp)
target_link_libraries(test_ctor_slot_370 sotc_core)
add_test(NAME ctor_slot_370 COMMAND test_ctor_slot_370)
add_executable(test_ctor_slot_469 tests/test_ctor_slot_469.cpp)
target_link_libraries(test_ctor_slot_469 sotc_core)
add_test(NAME ctor_slot_469 COMMAND test_ctor_slot_469)
add_executable(test_ctor_slot_513 tests/test_ctor_slot_513.cpp)
target_link_libraries(test_ctor_slot_513 sotc_core)
add_test(NAME ctor_slot_513 COMMAND test_ctor_slot_513)
add_executable(test_ctor_slot_508 tests/test_ctor_slot_508.cpp)
target_link_libraries(test_ctor_slot_508 sotc_core)
add_test(NAME ctor_slot_508 COMMAND test_ctor_slot_508)
add_executable(test_ctor_slot_390 tests/test_ctor_slot_390.cpp)
target_link_libraries(test_ctor_slot_390 sotc_core)
add_test(NAME ctor_slot_390 COMMAND test_ctor_slot_390)
add_executable(test_ctor_slot_402 tests/test_ctor_slot_402.cpp)
target_link_libraries(test_ctor_slot_402 sotc_core)
add_test(NAME ctor_slot_402 COMMAND test_ctor_slot_402)
add_executable(test_ctor_slot_572 tests/test_ctor_slot_572.cpp)
target_link_libraries(test_ctor_slot_572 sotc_core)
add_test(NAME ctor_slot_572 COMMAND test_ctor_slot_572)
add_executable(test_ctor_slot_493 tests/test_ctor_slot_493.cpp)
target_link_libraries(test_ctor_slot_493 sotc_core)
add_test(NAME ctor_slot_493 COMMAND test_ctor_slot_493)
add_executable(test_ctor_slot_422 tests/test_ctor_slot_422.cpp)
target_link_libraries(test_ctor_slot_422 sotc_core)
add_test(NAME ctor_slot_422 COMMAND test_ctor_slot_422)
add_executable(test_ctor_slot_511 tests/test_ctor_slot_511.cpp)
target_link_libraries(test_ctor_slot_511 sotc_core)
add_test(NAME ctor_slot_511 COMMAND test_ctor_slot_511)
add_executable(test_ctor_slot_503 tests/test_ctor_slot_503.cpp)
target_link_libraries(test_ctor_slot_503 sotc_core)
add_test(NAME ctor_slot_503 COMMAND test_ctor_slot_503)
add_executable(test_ctor_slot_571 tests/test_ctor_slot_571.cpp)
target_link_libraries(test_ctor_slot_571 sotc_core)
add_test(NAME ctor_slot_571 COMMAND test_ctor_slot_571)
add_executable(test_ctor_slot_388 tests/test_ctor_slot_388.cpp)
target_link_libraries(test_ctor_slot_388 sotc_core)
add_test(NAME ctor_slot_388 COMMAND test_ctor_slot_388)
add_executable(test_ctor_slot_480 tests/test_ctor_slot_480.cpp)
target_link_libraries(test_ctor_slot_480 sotc_core)
add_test(NAME ctor_slot_480 COMMAND test_ctor_slot_480)

# CPU-01-HELPER-CALLEE-MODELS-02 (helper-c): the local ranged-int variable wrapper family (0x620e10 / 0x6211d0 for startup slot 401, and 24
# byte-identical twins). Table-driven; tools/lift/check_helper_620e10.py pins every row. FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED only.
target_sources(sotc_core PRIVATE core/helper_620e10_local_int_var.cpp)
add_executable(test_helper_620e10 tests/test_helper_620e10.cpp)
target_link_libraries(test_helper_620e10 PRIVATE sotc_helper_callees)
add_test(NAME helper_620e10 COMMAND test_helper_620e10)

# CPU-01-HELPER-CALLEE-MODELS-03 (helper-d): bounded models of the callees that blocked batch 11 (0x8d650 store-one leaf, 0xf8f630 libc strncpy
# import contract, 0x56b4b0 node-reference builder, 0xa81700 named-record registrar with its two-vector push / reserve). Each is byte-pinned by
# tools/lift/check_helper_<rva>.py. FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED only; no startup slot is registered by these targets.
target_sources(sotc_core PRIVATE core/helper_8d650_store_one.cpp core/helper_f8f630_strncpy.cpp core/helper_56b4b0_node_ref.cpp core/helper_a81700_named_record.cpp)
target_sources(sotc_core PRIVATE core/startup_row_kinds.cpp)   # CPU-01-STARTUP-BATCH-14: empty-tree header and node-reference rows of the generated startup slots
add_executable(test_helper_8d650 tests/test_helper_8d650.cpp)
target_link_libraries(test_helper_8d650 PRIVATE sotc_helper_callees)
add_test(NAME helper_8d650 COMMAND test_helper_8d650)
add_executable(test_helper_f8f630 tests/test_helper_f8f630.cpp)
target_link_libraries(test_helper_f8f630 PRIVATE sotc_helper_callees)
add_test(NAME helper_f8f630 COMMAND test_helper_f8f630)
add_executable(test_helper_56b4b0 tests/test_helper_56b4b0.cpp)
target_link_libraries(test_helper_56b4b0 PRIVATE sotc_helper_callees)
add_test(NAME helper_56b4b0 COMMAND test_helper_56b4b0)
add_executable(test_helper_a81700 tests/test_helper_a81700.cpp)
target_link_libraries(test_helper_a81700 PRIVATE sotc_helper_callees)
add_test(NAME helper_a81700 COMMAND test_helper_a81700)

# CPU-01-HELPER-CALLEE-MODELS-04 (helper-e): bounded models of the callees that block the remaining startup constructor slots.
# 0x1580a0 perspective builder (startup slot 408, the cliff; tanf is an explicit host hook). Each model is byte-pinned by
# tools/lift/check_helper_<rva>.py. FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED only; no startup slot is registered by these targets.
target_sources(sotc_core PRIVATE core/helper_1580a0_perspective.cpp)
add_executable(test_helper_1580a0 tests/test_helper_1580a0.cpp)
target_link_libraries(test_helper_1580a0 PRIVATE sotc_helper_callees)
add_test(NAME helper_1580a0 COMMAND test_helper_1580a0)
# 0x274730 out-of-line bool console-variable constructor (startup slots 422 / 427 / 473 / 493) and the 143-table bool functor family it admits.
target_sources(sotc_core PRIVATE core/helper_bool_functor_tags.cpp core/helper_274730_bool_var.cpp)
add_executable(test_helper_274730 tests/test_helper_274730.cpp)
target_link_libraries(test_helper_274730 PRIVATE sotc_helper_callees)
add_test(NAME helper_274730 COMMAND test_helper_274730)
# 0x49ca60 / 0x49cc60 dword ranged console-variable constructors (startup slot 451), same body as the native float twin 0xc300.
target_sources(sotc_core PRIVATE core/helper_49ca60_ranged_dword_var.cpp)
add_executable(test_helper_49ca60 tests/test_helper_49ca60.cpp)
target_link_libraries(test_helper_49ca60 PRIVATE sotc_helper_callees)
add_test(NAME helper_49ca60 COMMAND test_helper_49ca60)
# 0x140b00 / 0x25ab0 unbounded-int constructor twins (startup slots 534 / 566): the 0x3a980 body with a private class pair.
target_sources(sotc_core PRIVATE core/helper_140b00_unbounded_int_twin.cpp)
add_executable(test_helper_140b00 tests/test_helper_140b00.cpp)
target_link_libraries(test_helper_140b00 PRIVATE sotc_helper_callees)
add_test(NAME helper_140b00 COMMAND test_helper_140b00)
# 0x1f220 ring-header leaf (startup slot 473, object 0x2ef2ba8).
target_sources(sotc_core PRIVATE core/helper_1f220_ring_header.cpp)
add_executable(test_helper_1f220 tests/test_helper_1f220.cpp)
target_link_libraries(test_helper_1f220 PRIVATE sotc_helper_callees)
add_test(NAME helper_1f220 COMMAND test_helper_1f220)
# Startup slot 498 member-hash getters (48 x `mov eax, imm ; ret`) and the 24 records the slot builds from them.
target_sources(sotc_core PRIVATE core/helper_498_member_hashes.cpp)
add_executable(test_helper_498 tests/test_helper_498.cpp)
target_link_libraries(test_helper_498 PRIVATE sotc_helper_callees)
add_test(NAME helper_498 COMMAND test_helper_498)
# CPU-01-SLOT408-PREREQS: leaf destructor / holder models for the open startup slots 408, 427, 451, 473, 534, 566.
# Each is tied to the ELF by tools/lift/check_helper_<rva>.py. FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED only; no startup slot is registered.
target_sources(sotc_core PRIVATE core/helper_8ef00_holder_dtor.cpp)
add_executable(test_helper_8ef00 tests/test_helper_8ef00.cpp)
target_link_libraries(test_helper_8ef00 PRIVATE sotc_helper_callees)
add_test(NAME helper_8ef00 COMMAND test_helper_8ef00)
target_sources(sotc_core PRIVATE core/helper_534580_cell_dtors.cpp)
add_executable(test_helper_534580 tests/test_helper_534580.cpp)
target_link_libraries(test_helper_534580 PRIVATE sotc_helper_callees)
add_test(NAME helper_534580 COMMAND test_helper_534580)
target_sources(sotc_core PRIVATE core/helper_499e90_slot451_dtors.cpp)
add_executable(test_helper_499e90 tests/test_helper_499e90.cpp)
target_link_libraries(test_helper_499e90 PRIVATE sotc_helper_callees)
add_test(NAME helper_499e90 COMMAND test_helper_499e90)
target_sources(sotc_core PRIVATE core/helper_498b20_ranged_var_dtors.cpp)
add_executable(test_helper_498b20 tests/test_helper_498b20.cpp)
target_link_libraries(test_helper_498b20 PRIVATE sotc_helper_callees)
add_test(NAME helper_498b20 COMMAND test_helper_498b20)
target_sources(sotc_core PRIVATE core/helper_354470_ring_dtor.cpp)
add_executable(test_helper_354470 tests/test_helper_354470.cpp)
target_link_libraries(test_helper_354470 PRIVATE sotc_helper_callees)
add_test(NAME helper_354470 COMMAND test_helper_354470)
# Startup slot 408 vector destructors 0x5b5530 / 0x5b55c0 / 0x5c7c00 (objects 0x2fda980 / 0x2fda9a0 / 0x2ff2af0).
target_sources(sotc_core PRIVATE core/helper_5b5530_vector_dtors.cpp)
add_executable(test_helper_5b5530 tests/test_helper_5b5530.cpp)
target_link_libraries(test_helper_5b5530 PRIVATE sotc_helper_callees)
add_test(NAME helper_5b5530 COMMAND test_helper_5b5530)
# 0xf7b0 SharedStr dtor: byte-identical twin of the shared release 0xf590 (startup slots 309 / 316 / 454).
target_sources(sotc_core PRIVATE core/helper_f7b0_shared_release_twin.cpp)
add_executable(test_helper_f7b0 tests/test_helper_f7b0.cpp)
target_link_libraries(test_helper_f7b0 PRIVATE sotc_helper_callees)
add_test(NAME helper_f7b0 COMMAND test_helper_f7b0)
target_sources(sotc_core PRIVATE core/ctor_slot_408_5c7f50.cpp)
add_executable(test_ctor_slot_408 tests/test_ctor_slot_408.cpp)
target_link_libraries(test_ctor_slot_408 sotc_core)
add_test(NAME ctor_slot_408 COMMAND test_ctor_slot_408)
target_sources(sotc_core PRIVATE core/ctor_slot_473_355a00.cpp)
add_executable(test_ctor_slot_473 tests/test_ctor_slot_473.cpp)
target_link_libraries(test_ctor_slot_473 sotc_core)
add_test(NAME ctor_slot_473 COMMAND test_ctor_slot_473)
target_sources(sotc_core PRIVATE core/ctor_slot_498_252ae0.cpp)
add_executable(test_ctor_slot_498 tests/test_ctor_slot_498.cpp)
target_link_libraries(test_ctor_slot_498 sotc_core)
add_test(NAME ctor_slot_498 COMMAND test_ctor_slot_498)
target_sources(sotc_core PRIVATE core/helper_49ca60_dword_var_site.cpp)
target_sources(sotc_core PRIVATE core/ctor_slot_451_49d230.cpp)
add_executable(test_ctor_slot_451 tests/test_ctor_slot_451.cpp)
target_link_libraries(test_ctor_slot_451 sotc_core)
add_test(NAME ctor_slot_451 COMMAND test_ctor_slot_451)
target_sources(sotc_core PRIVATE core/ctor_slot_427_5388f0.cpp)
add_executable(test_ctor_slot_427 tests/test_ctor_slot_427.cpp)
target_link_libraries(test_ctor_slot_427 sotc_core)
add_test(NAME ctor_slot_427 COMMAND test_ctor_slot_427)
target_sources(sotc_core PRIVATE core/helper_140b00_twin_site.cpp)
target_sources(sotc_core PRIVATE core/ctor_slot_534_1418d0.cpp)
add_executable(test_ctor_slot_534 tests/test_ctor_slot_534.cpp)
target_link_libraries(test_ctor_slot_534 sotc_core)
add_test(NAME ctor_slot_534 COMMAND test_ctor_slot_534)
target_sources(sotc_core PRIVATE core/ctor_slot_566_25de0.cpp)
add_executable(test_ctor_slot_566 tests/test_ctor_slot_566.cpp)
target_link_libraries(test_ctor_slot_566 sotc_core)
add_test(NAME ctor_slot_566 COMMAND test_ctor_slot_566)
target_sources(sotc_core PRIVATE core/store_table.cpp)
add_executable(test_store_table tests/test_store_table.cpp)
target_link_libraries(test_store_table sotc_core)
add_test(NAME store_table COMMAND test_store_table)
target_sources(sotc_core PRIVATE core/ctor_slot_424_551140.cpp)
add_executable(test_ctor_slot_424 tests/test_ctor_slot_424.cpp)
target_link_libraries(test_ctor_slot_424 sotc_core)
add_test(NAME ctor_slot_424 COMMAND test_ctor_slot_424)

# CPU-01-AOT-CFG-SCALE-01-BATCH19: bounded models of the callees that block startup slot 428 (0x52ec10).
# 0xa59620 id + string entry constructor with the doubling pointer-vector push (37 call sites in slot 428). Byte-pinned by
# tools/lift/check_helper_a59620.py. FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED only.
target_sources(sotc_core PRIVATE core/helper_a59620_id_entry.cpp)
add_executable(test_helper_a59620 tests/test_helper_a59620.cpp)
target_link_libraries(test_helper_a59620 PRIVATE sotc_helper_callees)
add_test(NAME helper_a59620 COMMAND test_helper_a59620)

# CPU-01-AOT-CFG-SCALE-01-BATCH19: availability entry constructor 0xa42960 (formatted-name bool var, V2 doubling push)
target_sources(sotc_core PRIVATE core/helper_a42960_avail_entry.cpp)
add_executable(test_helper_a42960 tests/test_helper_a42960.cpp)
target_link_libraries(test_helper_a42960 PRIVATE sotc_helper_callees)
add_test(NAME helper_a42960 COMMAND test_helper_a42960)
target_sources(sotc_core PRIVATE core/ctor_slot_428_52ec10.cpp)
add_executable(test_ctor_slot_428 tests/test_ctor_slot_428.cpp)
target_link_libraries(test_ctor_slot_428 sotc_core)
add_test(NAME ctor_slot_428 COMMAND test_ctor_slot_428)
target_sources(sotc_core PRIVATE core/ctor_slot_429_52e2b0.cpp)
add_executable(test_ctor_slot_429 tests/test_ctor_slot_429.cpp)
target_link_libraries(test_ctor_slot_429 sotc_core)
add_test(NAME ctor_slot_429 COMMAND test_ctor_slot_429)
target_sources(sotc_core PRIVATE core/ctor_slot_445_4cfe80.cpp)
add_executable(test_ctor_slot_445 tests/test_ctor_slot_445.cpp)
target_link_libraries(test_ctor_slot_445 sotc_core)
add_test(NAME ctor_slot_445 COMMAND test_ctor_slot_445)
target_sources(sotc_core PRIVATE core/hash_store_table.cpp)
target_sources(sotc_core PRIVATE core/ctor_slot_501_24db80.cpp)
add_executable(test_ctor_slot_501 tests/test_ctor_slot_501.cpp)
target_link_libraries(test_ctor_slot_501 sotc_core)
add_test(NAME ctor_slot_501 COMMAND test_ctor_slot_501)
target_sources(sotc_core PRIVATE core/ctor_slot_494_26f7c0.cpp)
add_executable(test_ctor_slot_494 tests/test_ctor_slot_494.cpp)
target_link_libraries(test_ctor_slot_494 sotc_core)
add_test(NAME ctor_slot_494 COMMAND test_ctor_slot_494)

target_sources(sotc_core PRIVATE core/ctor_slot_570_16d80.cpp)
add_executable(test_ctor_slot_570 tests/test_ctor_slot_570.cpp)
target_link_libraries(test_ctor_slot_570 sotc_core)
add_test(NAME ctor_slot_570 COMMAND test_ctor_slot_570)

target_sources(sotc_core PRIVATE core/ctor_slot_532_143f50.cpp)
add_executable(test_ctor_slot_532 tests/test_ctor_slot_532.cpp)
target_link_libraries(test_ctor_slot_532 sotc_core)
add_test(NAME ctor_slot_532 COMMAND test_ctor_slot_532)

target_sources(sotc_core PRIVATE core/ctor_slot_537_1324c0.cpp)
add_executable(test_ctor_slot_537 tests/test_ctor_slot_537.cpp)
target_link_libraries(test_ctor_slot_537 sotc_core)
add_test(NAME ctor_slot_537 COMMAND test_ctor_slot_537)

target_sources(sotc_core PRIVATE core/ctor_slot_452_498640.cpp)
add_executable(test_ctor_slot_452 tests/test_ctor_slot_452.cpp)
target_link_libraries(test_ctor_slot_452 sotc_core)
add_test(NAME ctor_slot_452 COMMAND test_ctor_slot_452)

target_sources(sotc_core PRIVATE core/blob_parser_a38960.cpp core/ctor_slot_495_268ee0.cpp)
add_executable(test_blob_parser_a38960 tests/test_blob_parser_a38960.cpp)
target_link_libraries(test_blob_parser_a38960 sotc_core)
add_test(NAME blob_parser_a38960 COMMAND test_blob_parser_a38960)
add_executable(test_ctor_slot_495 tests/test_ctor_slot_495.cpp)
target_link_libraries(test_ctor_slot_495 sotc_core)
add_test(NAME ctor_slot_495 COMMAND test_ctor_slot_495)

# CPU-01-AOT-CFG-SCALE-01-BATCH26 (slot 454): first-class RB erase 0x482a90 + network debug page
target_sources(sotc_core PRIVATE core/rb_tree_482a90.cpp core/ctor_slot_454_483e40.cpp)
add_executable(test_rb_tree_482a90 tests/test_rb_tree_482a90.cpp)
target_link_libraries(test_rb_tree_482a90 sotc_core)
add_test(NAME rb_tree_482a90 COMMAND test_rb_tree_482a90)
add_executable(test_ctor_slot_454 tests/test_ctor_slot_454.cpp)
target_link_libraries(test_ctor_slot_454 sotc_core)
add_test(NAME ctor_slot_454 COMMAND test_ctor_slot_454)

# CPU-01-STARTUP-SLOT-468: scripted-entry helper 0x3c2a00 + the automation command page
target_sources(sotc_core PRIVATE core/scripted_entry_3c2a00.cpp core/ctor_slot_468_3c3a90.cpp)
add_executable(test_scripted_entry_3c2a00 tests/test_scripted_entry_3c2a00.cpp)
target_link_libraries(test_scripted_entry_3c2a00 sotc_core)
add_test(NAME scripted_entry_3c2a00 COMMAND test_scripted_entry_3c2a00)
add_executable(test_ctor_slot_468 tests/test_ctor_slot_468.cpp)
target_link_libraries(test_ctor_slot_468 sotc_core)
add_test(NAME ctor_slot_468 COMMAND test_ctor_slot_468)

# CPU-01-AOT-CFG-SCALE-01-BATCH29 (slot 568): LoadCmdsFromFile command with two tokens
target_sources(sotc_core PRIVATE core/ctor_slot_568_207e0.cpp)
add_executable(test_ctor_slot_568 tests/test_ctor_slot_568.cpp)
target_link_libraries(test_ctor_slot_568 sotc_core)
add_test(NAME ctor_slot_568 COMMAND test_ctor_slot_568)

# CPU-01-AOT-CFG-SCALE-01-BATCH30 (slot 556): four app-info entries + sendEmail command
target_sources(sotc_core PRIVATE core/ctor_slot_556_4ccb0.cpp)
add_executable(test_ctor_slot_556 tests/test_ctor_slot_556.cpp)
target_link_libraries(test_ctor_slot_556 sotc_core)
add_test(NAME ctor_slot_556 COMMAND test_ctor_slot_556)

# CPU-01-AOT-CFG-SCALE-01-BATCH31 (slot 530): Resources page (5 bools, ranged int, command, 8 menu items)
target_sources(sotc_core PRIVATE core/ctor_slot_530_149f00.cpp)
add_executable(test_ctor_slot_530 tests/test_ctor_slot_530.cpp)
target_link_libraries(test_ctor_slot_530 sotc_core)
add_test(NAME ctor_slot_530 COMMAND test_ctor_slot_530)

target_sources(sotc_core PRIVATE core/vector_append_16a380.cpp core/ctor_slot_524_16a810.cpp)
add_executable(test_ctor_slot_524 tests/test_ctor_slot_524.cpp)
target_link_libraries(test_ctor_slot_524 sotc_core)
add_test(NAME ctor_slot_524 COMMAND test_ctor_slot_524)

# CPU-01-STARTUP-SLOT-505 (slot 505): 2,704-byte table copy + 226-step bounds min/max (0x1621b0/0x162970).
target_sources(sotc_core PRIVATE core/bounds_minmax_1621b0.cpp core/ctor_slot_505_236030.cpp)
add_executable(test_ctor_slot_505 tests/test_ctor_slot_505.cpp)
target_link_libraries(test_ctor_slot_505 sotc_core)
add_test(NAME ctor_slot_505 COMMAND test_ctor_slot_505)
target_sources(sotc_core PRIVATE core/ctor_slot_552_8c880.cpp)
add_executable(test_ctor_slot_552 tests/test_ctor_slot_552.cpp)
target_link_libraries(test_ctor_slot_552 sotc_core)
add_test(NAME ctor_slot_552 COMMAND test_ctor_slot_552)

# CPU-01-STARTUP-SLOT-567 (slot 567): manager cell + four console commands + five atexit records.
target_sources(sotc_core PRIVATE core/ctor_slot_567_235d0.cpp)
add_executable(test_ctor_slot_567 tests/test_ctor_slot_567.cpp)
target_link_libraries(test_ctor_slot_567 sotc_core)
add_test(NAME ctor_slot_567 COMMAND test_ctor_slot_567)

# CPU-01-STARTUP-SLOT-510 (slot 510): property cell + FollowGameCamera bool var + Debug Game
# toggle + pose constants + SyncCameraAcrossTools with seven tokens; four atexit records.
target_sources(sotc_core PRIVATE core/ctor_slot_497_262a80.cpp)
target_sources(sotc_core PRIVATE core/ctor_slot_510_211340.cpp)
add_executable(test_ctor_slot_510 tests/test_ctor_slot_510.cpp)
target_link_libraries(test_ctor_slot_510 sotc_core)
add_test(NAME ctor_slot_510 COMMAND test_ctor_slot_510)

add_executable(test_ctor_slot_497 tests/test_ctor_slot_497.cpp)
target_link_libraries(test_ctor_slot_497 sotc_core)
add_test(NAME ctor_slot_497 COMMAND test_ctor_slot_497)

# CPU-01-AOT-CFG-SCALE-01-BATCH27 (slot 462): functor command 0x6af60 / 0x6b0f0 + presentation page
target_sources(sotc_core PRIVATE core/concmd_functor_6af60.cpp core/ctor_slot_462_3e2730.cpp)
add_executable(test_concmd_functor_6af60 tests/test_concmd_functor_6af60.cpp)
target_link_libraries(test_concmd_functor_6af60 sotc_core)
add_test(NAME concmd_functor_6af60 COMMAND test_concmd_functor_6af60)
add_executable(test_ctor_slot_462 tests/test_ctor_slot_462.cpp)
target_link_libraries(test_ctor_slot_462 sotc_core)
add_test(NAME ctor_slot_462 COMMAND test_ctor_slot_462)

# CPU-01-AOT-CFG-SCALE-01-BATCH21 (slot 446): red-black named registry 0x3be2d0 / 0x3c0840 / 0x3c31b0 / 0x3c3710 / 0x3c3470 / 0x3c3930
target_sources(sotc_core PRIVATE core/rb_registry_3be2d0.cpp)
add_executable(test_rb_registry_3be2d0 tests/test_rb_registry_3be2d0.cpp)
target_link_libraries(test_rb_registry_3be2d0 sotc_core)
add_test(NAME rb_registry_3be2d0 COMMAND test_rb_registry_3be2d0)
target_sources(sotc_core PRIVATE core/ctor_slot_446_4c92a0.cpp)
add_executable(test_ctor_slot_446 tests/test_ctor_slot_446.cpp)
target_link_libraries(test_ctor_slot_446 sotc_core)
add_test(NAME ctor_slot_446 COMMAND test_ctor_slot_446)

# CPU-01-STARTUP-SLOT-554-BATCH-01: page header (0x53fc0, 187 stores) + first eight variables.
target_sources(sotc_core PRIVATE core/ctor_slot_554_6efe0.cpp)
add_executable(test_ctor_slot_554 tests/test_ctor_slot_554.cpp)
target_link_libraries(test_ctor_slot_554 sotc_core)
add_test(NAME ctor_slot_554 COMMAND test_ctor_slot_554)

# CPU-01-STARTUP-SLOT-554-BATCH-02A: five 0xc0-byte bool variables (0x6f793..0x6fc29).
target_sources(sotc_core PRIVATE core/ctor_slot_554_2a_6f793.cpp)
add_executable(test_ctor_slot_554_batch2a tests/test_ctor_slot_554_batch2a.cpp)
target_link_libraries(test_ctor_slot_554_batch2a sotc_core)
add_test(NAME ctor_slot_554_batch2a COMMAND test_ctor_slot_554_batch2a)

# CPU-01-STARTUP-SLOT-554-BATCH-02B: 0x6abb0 occluder-debug wrapper + seven bools (0x6fc29..0x7036d).
target_sources(sotc_core PRIVATE core/ctor_slot_554_2b_6fc29.cpp)
add_executable(test_ctor_slot_554_batch2b tests/test_ctor_slot_554_batch2b.cpp)
target_link_libraries(test_ctor_slot_554_batch2b sotc_core)
add_test(NAME ctor_slot_554_batch2b COMMAND test_ctor_slot_554_batch2b)

# CPU-01-STARTUP-SLOT-554-BATCH-02B-CONT: remaining 23 objects (0x7036d..0x718bb).
target_sources(sotc_core PRIVATE core/ctor_slot_554_2b_cont_7036d.cpp)
add_executable(test_ctor_slot_554_batch2b_cont tests/test_ctor_slot_554_batch2b_cont.cpp)
target_link_libraries(test_ctor_slot_554_batch2b_cont sotc_core)
add_test(NAME ctor_slot_554_batch2b_cont COMMAND test_ctor_slot_554_batch2b_cont)

# CPU-01-STARTUP-SLOT-554-BATCH-03: 36 objects (0x718bb..0x739e8).
target_sources(sotc_core PRIVATE core/ctor_slot_554_batch3_718bb.cpp)
add_executable(test_ctor_slot_554_batch3 tests/test_ctor_slot_554_batch3.cpp)
target_link_libraries(test_ctor_slot_554_batch3 sotc_core)
add_test(NAME ctor_slot_554_batch3 COMMAND test_ctor_slot_554_batch3)

# CPU-01-STARTUP-SLOT-554-BATCH-04: 36 objects (0x739e8..0x75d22).
target_sources(sotc_core PRIVATE core/ctor_slot_554_batch4_739e8.cpp)
add_executable(test_ctor_slot_554_batch4 tests/test_ctor_slot_554_batch4.cpp)
target_link_libraries(test_ctor_slot_554_batch4 sotc_core)
add_test(NAME ctor_slot_554_batch4 COMMAND test_ctor_slot_554_batch4)

# CPU-01-STARTUP-SLOT-554-BATCH-05: 36 objects (0x75d22..0x781c5).
target_sources(sotc_core PRIVATE core/ctor_slot_554_batch5_75d22.cpp)
add_executable(test_ctor_slot_554_batch5 tests/test_ctor_slot_554_batch5.cpp)
target_link_libraries(test_ctor_slot_554_batch5 sotc_core)
add_test(NAME ctor_slot_554_batch5 COMMAND test_ctor_slot_554_batch5)

# CPU-01-STARTUP-SLOT-554-BATCH-06: 36 objects (0x781c5..0x7a68c).
target_sources(sotc_core PRIVATE core/ctor_slot_554_batch6_781c5.cpp)
add_executable(test_ctor_slot_554_batch6 tests/test_ctor_slot_554_batch6.cpp)
target_link_libraries(test_ctor_slot_554_batch6 sotc_core)
add_test(NAME ctor_slot_554_batch6 COMMAND test_ctor_slot_554_batch6)

# CPU-01-STARTUP-SLOT-554-BATCH-07: 36 objects (0x7a68c..0x7cb5b).
target_sources(sotc_core PRIVATE core/ctor_slot_554_batch7_7a68c.cpp)
add_executable(test_ctor_slot_554_batch7 tests/test_ctor_slot_554_batch7.cpp)
target_link_libraries(test_ctor_slot_554_batch7 sotc_core)
add_test(NAME ctor_slot_554_batch7 COMMAND test_ctor_slot_554_batch7)

target_sources(sotc_core PRIVATE core/ctor_slot_554_batch8_7cb5b.cpp)
add_executable(test_ctor_slot_554_batch8 tests/test_ctor_slot_554_batch8.cpp)
target_link_libraries(test_ctor_slot_554_batch8 sotc_core)
add_test(NAME ctor_slot_554_batch8 COMMAND test_ctor_slot_554_batch8)

# CPU-01-STARTUP-SLOT-554-BATCH-09: 36 objects (0x7f098..0x8160c).
target_sources(sotc_core PRIVATE core/ctor_slot_554_batch9_7f098.cpp)
add_executable(test_ctor_slot_554_batch9 tests/test_ctor_slot_554_batch9.cpp)
target_link_libraries(test_ctor_slot_554_batch9 sotc_core)
add_test(NAME ctor_slot_554_batch9 COMMAND test_ctor_slot_554_batch9)

# CPU-01-STARTUP-SLOT-554-BATCH-10: 36 objects (0x8160c..0x83ad1).
target_sources(sotc_core PRIVATE core/ctor_slot_554_batch10_8160c.cpp)
add_executable(test_ctor_slot_554_batch10 tests/test_ctor_slot_554_batch10.cpp)
target_link_libraries(test_ctor_slot_554_batch10 sotc_core)
add_test(NAME ctor_slot_554_batch10 COMMAND test_ctor_slot_554_batch10)

# CPU-01-STARTUP-SLOT-554-BATCH-11: 36 objects (0x83ad1..0x86052).
target_sources(sotc_core PRIVATE core/ctor_slot_554_batch11_83ad1.cpp)
add_executable(test_ctor_slot_554_batch11 tests/test_ctor_slot_554_batch11.cpp)
target_link_libraries(test_ctor_slot_554_batch11 sotc_core)
add_test(NAME ctor_slot_554_batch11 COMMAND test_ctor_slot_554_batch11)

# CPU-01-STARTUP-SLOT-554-BATCH-12: 36 objects; body (0x86052..0x8849d).
target_sources(sotc_core PRIVATE core/ctor_slot_554_batch12_86052.cpp)
add_executable(test_ctor_slot_554_batch12 tests/test_ctor_slot_554_batch12.cpp)
target_link_libraries(test_ctor_slot_554_batch12 sotc_core)
add_test(NAME ctor_slot_554_batch12 COMMAND test_ctor_slot_554_batch12)
target_sources(sotc_core PRIVATE core/ctor_slot_554_batch13_8849d.cpp)
add_executable(test_ctor_slot_554_batch13 tests/test_ctor_slot_554_batch13.cpp)
target_link_libraries(test_ctor_slot_554_batch13 sotc_core)
add_test(NAME ctor_slot_554_batch13 COMMAND test_ctor_slot_554_batch13)
