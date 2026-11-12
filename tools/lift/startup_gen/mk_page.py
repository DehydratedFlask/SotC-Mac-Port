import sys,hashlib
import pathlib as _pl
_HERE=str(_pl.Path(__file__).resolve().parent)
_LIFT=str(_pl.Path(__file__).resolve().parents[1])
_ROOT=str(_pl.Path(__file__).resolve().parents[3])+'/'
sys.path.insert(0,_HERE)
from genpage import *
from mkchecker import write_checker
from genpins import e
def h(r,s): return hashlib.sha256(e.raw(r,s)).hexdigest()
INT_M6={
    'ranged_int_ctor_c580': (0xc580, 351, "a93bc0f34cfd49fbbcbc7f2186b2668de6e49f5f54012caafca7f17dfb396bce"),
    'int_fn_copy_c720': (0xc720, 20, "5b63116e7f29674de81acb2a9317619a1c1dcbdbc53f98785a3029c04aa7a914"),
    'int_fn_clone_c740': (0xc740, 20, "3b5a8a9fd4b3e15b1f5f14f73127ac98c5becb3a88fd096f8ccfb2015e991593"),
    'int_fn_move_c760': (0xc760, 9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    'int_fn_destroy_c770': (0xc770, 1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
    'int_dtor_18a60': (0x18a60, 54, "4de4f3993b02c84c9c20824690e8ab9d4f7a1b84babfa6cbe0d89c9dd558b50b"),
    'int_slot3_bf70': (0xbf70, 6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
    'int_slot6_bfc0': (0xbfc0, 6, "7140f35dee6220b79b12aecc27acf5105bf3b77d1588e89fce345de7c16c72b7"),
}
STR_M6={
    'string_ctor_257a0': (0x257a0, 0x14c, h(0x257a0,0x14c)), 'string_inner_258f0': (0x258f0, 0x157, h(0x258f0,0x157)), 'empty_ctor_f180': (0xf180, 0x1c, h(0xf180,0x1c)),
    'sfn_copy_25a50': (0x25a50, 0x14, h(0x25a50,0x14)), 'sfn_clone_25a70': (0x25a70, 0x14, h(0x25a70,0x14)), 'sfn_move_25a90': (0x25a90, 9, h(0x25a90,9)), 'sfn_destroy_25aa0': (0x25aa0, 1, h(0x25aa0,1)),
    'str_slot3_246a0': (0x246a0, 6, h(0x246a0,6)), 'str_slot6_24780': (0x24780, 6, h(0x24780,6)), 'str_dtor_mid_24610': (0x24610, 0x28, h(0x24610,0x28)),
    'str_dtor_23c90': (0x23c90, fde(0x23c90), h(0x23c90,fde(0x23c90))),
}
def build(n, what, theme_note, summary, doc):
    d=gen_page(n,what,theme_note)
    k=d['kinds']
    callees=['concmd_base_ctor','concmd_register','release_f590']+(['menu_item_dtor'] if not d['menu_less'] else [])
    if k['bool']: callees+=['bool_dtor','bool_fn_copy','bool_fn_clone','bool_fn_move','bool_fn_destroy','bool_slot3','bool_slot6']
    if k['bool'] or k['boolx']: callees+=['bool_dtor','bool_fn_copy','bool_fn_clone','bool_fn_move','bool_fn_destroy','bool_slot3','bool_slot6']
    if k['ufloat']: callees+=['unbounded_float_ctor','float_dtor','float_fn_copy','float_fn_clone','float_fn_move','float_fn_destroy']
    if k['float']: callees+=['float_dtor','bounded_float_ctor','float_fn_copy','float_fn_clone','float_fn_move','float_fn_destroy']
    if k['tog']: callees+=['menu_toggle_from_var_ctor']
    if k['toge']: callees+=['menu_toggle_ctor']
    if k['cmd']: callees+=['cmd_dtor']
    callees=list(dict.fromkeys(callees))
    m6=None; m6b=[]; extras=['bp_atexit_imports']
    if k['bool'] or k['boolx']: extras.append('bp_object_vtables')
    if k['tog'] or k['toge']: extras.append('toggle_gate')
    tkinds={next(q for q in d['rows'] if q['obj']==r['target'])['kind'] for r in d['rows'] if r['kind'] in('tog','toge')}
    if tkinds&{'float','ufloat'}: extras.append('float_menu')
    if 'cmd' in tkinds: extras.append('cmd_target_gate')
    if k['int']:
        m6=dict(INT_M6); extras.append('int_site')
    if k['uint']:
        m6=dict(m6 or {}); m6.update({kk:vv for kk,vv in INT_M6.items() if kk!='ranged_int_ctor_c580'}); m6['int_var_worker_3a980']=(0x3a980,322,h(0x3a980,322))
        if 'int_site' not in extras: extras.append('int_site')
    if k['uint'] and any(r_.get('twin') for r_ in d['rows'] if r_['kind']=='uint'):
        m6=dict(m6 or {})
        for _tw in sorted({r_['twin'] for r_ in d['rows'] if r_['kind']=='uint' and r_.get('twin')}):
            _tc={'a':0x140b00,'b':0x25ab0}[_tw]; _sc={'a':0x15983c8,'b':0x1595100}[_tw]
            m6['twin_ctor_%x'%_tc]=(_tc,fde(_tc),h(_tc,fde(_tc)))
            for _q in range(4):
                _t=e.reloc_target(_sc+8*_q); m6['twin_seed_slot%d_%x'%(_q,_t)]=(_t,fde(_t),h(_t,fde(_t)))
            m6b.append((_sc,bytes(e.raw(_sc,32)).hex(),'twin seed class %#x: the four slot pointers (relocation addends)'%_sc))
        for _tw in sorted({r_['twin'] for r_ in d['rows'] if r_['kind']=='uint' and r_.get('twin')}):
            _td={'a':0x13a090,'b':0x23d20}[_tw]; m6['twin_dtor_%x'%_td]=(_td,fde(_td),h(_td,fde(_td)))   # the twin class destructor (A: slot 534; B: slot 566; asserted by genpage)
            if _tw=='b':
                # twin B final class: the menu item over this variable calls slots 3 and 6 (`mov eax, 1 ; ret`, 6 B each); pin the eight address-point slots and those two bodies
                _fv=sorted({r_['vptr'] for r_ in d['rows'] if r_['kind']=='uint' and r_.get('twin')=='b'}); assert len(_fv)==1
                for q_ in (3,6):
                    _t=e.reloc_target(_fv[0]+8*q_); assert fde(_t)==6 and bytes(e.raw(_t,6))==bytes.fromhex('b801000000c3'), ('twin B final class slot is not mov eax,1 ; ret',q_)
                    m6[f'twin_final_slot{q_}_{_t:x}']=(_t,6,h(_t,6))
                m6b.append((_fv[0],bytes(e.raw(_fv[0],64)).hex(),'twin B final class: the eight address-point slots (relocation addends): dtor 0x23d20, slot 3 and slot 6 (mov eax,1 ; ret) are the bodies pinned above'))
    if k['cmd']:
        m6=dict(m6 or {}); m6['name_view_f2f0']=(0xf2f0,40,h(0xf2f0,40)); extras+=['bp_strlen_import','fcmd_vtable']
        for r in d['rows']:
            if r['kind']=='cmd': m6[f'handler_tag_{r["handler"]:x}']=(r['handler'],fde(r['handler']),h(r['handler'],fde(r['handler'])))
    spec_extra=None; edefs=''
    if k['lint']:
        extras.append('lint_sites'); edefs='import local_int_site_lib  # noqa: E402,F401\n'
        spec_extra={'lint_sites':[dict(s) for s in d['lsites']]}
        m6=dict(m6 or {}); m6.update({'wrapper_template_7ec160': (0x7ec160, 435, h(0x7ec160,435)), 'ranged_int_ctor_c580': (0xc580, 351, h(0xc580,351)),
            'int_fn_copy_c720': (0xc720, 20, h(0xc720,20)), 'int_fn_clone_c740': (0xc740, 20, h(0xc740,20)), 'int_fn_move_c760': (0xc760, 9, h(0xc760,9)), 'int_fn_destroy_c770': (0xc770, 1, h(0xc770,1))})
        for s_ in d['lsites']:
            m6[f'wrapper_{s_["wrapper"]:x}']=(s_['wrapper'],435,h(s_['wrapper'],435)); m6[f'inner_{s_["wrapper"]+0x1c0:x}']=(s_['wrapper']+0x1c0,351,h(s_['wrapper']+0x1c0,351)); m6[f'dtor_{s_["dtor"]:x}']=(s_['dtor'],54,h(s_['dtor'],54))
    if k['str']:
        m6=dict(m6 or {}); m6.update(STR_M6); extras.append('bp_string_var')
        if any(r_['kind']=='str' and r_.get('init_empty') for r_ in d['rows']): m6['empty_string_f730']=(0xf730,fde(0xf730),h(0xf730,fde(0xf730)))
    if k['sent']:
        _sd=next(r_['dtor'] for r_ in d['rows'] if r_['kind']=='sent')
        m6=dict(m6 or {}); m6['sentinel_dtor_%x'%_sd]=(_sd,fde(_sd),h(_sd,fde(_sd)))
        for r_ in d['rows']:
            if r_['kind']=='sent':
                m6b.append((r_['seq_start'],r_['seq_bytes'],'sentinel container: lea obj, lea dtor, mov rdx, the self-referential header stores (+0x20, +0x00/+0x08, +0x10) and the __cxa_atexit call'))
    if k['gettable']:
        m6=dict(m6 or {})
        for r_ in d['rows']:
            if r_['kind']=='gettable':
                for g_ in r_['lo_rva']+r_['hi_rva']: m6['member_getter_%x'%g_]=(g_,6,h(g_,6))
                m6b.append((r_['seq_start'],r_['seq_bytes'],'member-hash table: 24 pairs of `call lo ; mov ebx, eax ; call hi ; mov [cell], ebx ; mov [cell+4], eax ; mov dword [cell+8], size` (the last pair has the stack-canary load interleaved)'))
    if k['sep']:
        m6=dict(m6 or {})
        if any(r_['kind']=='sep' and r_.get('fn',0x26b60)==0x26b60 for r_ in d['rows']): m6['menu_separator_ctor_26b60']=(0x26b60,166,h(0x26b60,166))
        if any(r_['kind']=='sep' and r_.get('fn')==0x26800 for r_ in d['rows']): m6['menu_label_ctor_26800']=(0x26800,173,h(0x26800,173))
    if k['ringhdr']:
        m6=dict(m6 or {}); m6['ring_header_1f220']=(0x1f220,27,h(0x1f220,27)); m6['ring_dtor_354470']=(0x354470,116,h(0x354470,116))
        for r_ in d['rows']:
            if r_['kind']=='ringhdr':
                m6b.append((r_['seq_start'],r_['seq_bytes'],'ring header: lea obj, mov rdi, call 0x1f220, lea dtor 0x354470, mov rsi / rdx and the __cxa_atexit call'))
    if k['zvec']:
        m6=dict(m6 or {})
        for r_ in d['rows']:
            if r_['kind']=='zvec':
                m6['zero_vector_dtor_%x'%r_['dtor']]=(r_['dtor'],fde(r_['dtor']),h(r_['dtor'],fde(r_['dtor'])))
                m6b.append((r_['seq_start'],r_['seq_bytes'],'zero-header vector: lea obj, lea dtor, mov rdx, the zeroing of begin / end / cap (obj+8..obj+0x20) and the __cxa_atexit call'))
    if k['pre']:
        m6=dict(m6 or {}); m6['perspective_builder_1580a0']=(0x1580a0,fde(0x1580a0),h(0x1580a0,fde(0x1580a0)))
        for r_ in d['rows']:
            if r_['kind']=='pre':
                m6b.append((r_['aspect_addr'],e.raw(r_['aspect_addr'],4).hex(),'perspective aspect (.rodata dword, loaded by vmovss into xmm0)'))
                m6b.append((r_['near_addr'],e.raw(r_['near_addr'],4).hex(),'perspective near plane (.rodata dword, xmm1)'))
                m6b.append((r_['far_addr'],e.raw(r_['far_addr'],4).hex(),'perspective far plane (.rodata dword, xmm2)'))
    if k['recs'] or k['rec1']:
        m6=dict(m6 or {})
        for nm_,a_,sz_ in (('named_record_ctor_a81700',0xa81700,190),('record_registry_push_a81840',0xa81840,265),('vec_reserve_a82050',0xa82050,245),('record_registry_ctor_a81b50',0xa81b50,34)):
            m6[nm_]=(a_,sz_,h(a_,sz_))
        _pairs=[('record_unregister_a81950',0xa81950),('record_registry_dtor_a81b80',0xa81b80)]
        if k['recs']:
            _rd=next(r_['dtor'] for r_ in d['rows'] if r_['kind']=='recs'); _pairs.insert(0,('records_dtor_%x'%_rd,_rd))
        for nm_,a_ in _pairs:
            m6[nm_]=(a_,fde(a_),h(a_,fde(a_)))
    if k['entry']:
        # scripted-command registry entries: the registration body 0x3be2d0 (its red-black callees are pinned by tools/lift/check_rb_registry_3be2d0.py), the atexit destructor, the
        # whole straight-line run (object image, list push, guarded registration, releases, atexit call) byte for byte, the three functor-tag slots and both name strings
        m6=dict(m6 or {}); m6['name_view_f2f0']=(0xf2f0,40,h(0xf2f0,40)); m6['registry_register_3be2d0']=(0x3be2d0,192,h(0x3be2d0,192)); m6['entry_dtor_3c15e0']=(0x3c15e0,fde(0x3c15e0),h(0x3c15e0,fde(0x3c15e0)))
        extras+=['bp_strlen_import']
        for r_ in d['rows']:
            if r_['kind']!='entry': continue
            m6b.append((r_['seq_start'],r_['seq_bytes'],'registry entry %#x: two 0xf2f0 name views, the 0x88 B image stores, the list push, the guarded 0x3be2d0, two 0xf590 and the __cxa_atexit call (whole run byte for byte)'%r_['obj']))
            for q_,raw_ in zip(r_['fn_slots'],r_['fn_raw']): m6b.append((q_,raw_,'registry entry %#x functor tag: relocated code address read by vmovq'%r_['obj']))
            m6b.append((r_['name_ptr'],bytes(r_['name']+b'\0').hex(),'registry entry %#x name literal (NUL terminated)'%r_['obj']))
            m6b.append((r_['desc_ptr'],bytes(r_['desc']+b'\0').hex(),'registry entry %#x description literal (NUL terminated)'%r_['obj']))
    if k['zcell']:
        m6=dict(m6 or {})
        for _zd in sorted({r_['dtor'] for r_ in d['rows'] if r_['kind']=='zcell'}): m6['zero_cell_dtor_%x'%_zd]=(_zd,fde(_zd),h(_zd,fde(_zd)))
        for r_ in d['rows']:
            if r_['kind']=='zcell':
                m6b.append((r_['seq_start'],r_['seq_bytes'],'zero cell (%d qword%s): lea obj, lea dtor, mov rdx, the zero store%s and the __cxa_atexit call'%(r_['words'],'' if r_['words']==1 else 's','s' if r_['words']==3 else '')))
    if k['tree']:
        m6=dict(m6 or {})
        _td=next(r_['dtor'] for r_ in d['rows'] if r_['kind']=='tree')
        for nm_,a_ in ((('alloc_wrapper_17070',0x17070),('tree_dtor_6918e0',0x6918e0),('tree_erase_691930',0x691930)) if _td==0x6918e0 else (('alloc_wrapper_17070',0x17070),('tree_dtor_%x'%_td,_td),('tree_erase_1d7fe0',0x1d7fe0),('tree_free_170b0',0x170b0))):
            m6[nm_]=(a_,fde(a_),h(a_,fde(a_)))
        for r_ in d['rows']:
            if r_['kind']=='tree':
                m6b.append((r_['seq_start'],r_['seq_bytes'],'empty tree header: zero [obj+8..+0x18), 0x17070(0x30), the self-linked node stores (+0x00/+0x08/+0x10, bytes +0x18/+0x19 = 1), [obj+8] = node and the __cxa_atexit call'))
    if k['ncell']:
        m6=dict(m6 or {}); m6['strncpy_plt_thunk_f8f630']=(0xf8f630,6,h(0xf8f630,6))
        for r_ in d['rows']:
            if r_['kind']=='ncell':
                m6b.append((r_['seq_start'],r_['seq_bytes'],'null-name record: lea obj ; "NULL" source and count 15 ; stack-guard load ; [obj] = 0 ; strncpy(obj+4, src, 15) ; byte +0x13 = 0 (no atexit record)'))
    if k['cmdi']:
        m6=dict(m6 or {}); m6['menu_command_ctor_26f90']=(0x26f90,fde(0x26f90),h(0x26f90,fde(0x26f90)))
    if k['ienum']:
        m6=dict(m6 or {})
        _fv=sorted({r_['vptr'] for r_ in d['rows'] if r_['kind']=='ienum'}); _mv=sorted({r_['mid'] for r_ in d['rows'] if r_['kind']=='ienum'})
        assert len(_fv)==1 and len(_mv)==1
        for q_ in range(8):
            _t=e.reloc_target(_fv[0]+8*q_)
            if q_ in (0,3,6): m6[f'enum_final_slot{q_}_{_t:x}']=(_t,fde(_t),h(_t,fde(_t)))
        m6b.append((_fv[0],bytes(e.raw(_fv[0],64)).hex(),'inline enum final class: the eight address-point slots (relocation addends): dtor, slot 3 and slot 6 are the bodies pinned above'))
        m6b.append((_mv[0],bytes(e.raw(_mv[0],64)).hex(),'inline enum mid class: the eight address-point slots'))
    if k['nref']:
        m6=dict(m6 or {})
        for nm_,a_ in (('node_ref_ctor_56b4b0',0x56b4b0),('node_ref_dtor_560540',0x560540)):
            m6[nm_]=(a_,fde(a_),h(a_,fde(a_)))
    if k['zcont']:
        m6=dict(m6 or {}); m6['zero_container_dtor_1c5090']=(0x1c5090,fde(0x1c5090),h(0x1c5090,fde(0x1c5090)))
        for r_ in d['rows']:
            if r_['kind']=='zcont':
                m6b.append((r_['seq_start'],r_['seq_bytes'],'zero container: lea obj, lea dtor, mov rdx, the two zero stores (+0x08..+0x18, +0x18..+0x20) and the __cxa_atexit call'))
    if k['zq']:
        m6=dict(m6 or {})
        for _zd in sorted({r_['dtor'] for r_ in d['rows'] if r_['kind']=='zq'}): m6['zero_pointer_dtor_%x'%_zd]=(_zd,fde(_zd),h(_zd,fde(_zd)))
        if any(r_['dtor']==0x499e90 for r_ in d['rows'] if r_['kind']=='zq'):
            # the 0x499e90 node holder: its body calls the two free thunks and the critical-section destroy (all pinned)
            for _nm,_ad in (('free_type1_170b0',0x170b0),('free_type2_170c0',0x170c0),('critsec_destroy_3b380',0x3b380)): m6[_nm]=(_ad,fde(_ad),h(_ad,fde(_ad)))
        for r_ in d['rows']:
            if r_['kind']=='zq':
                m6b.append((r_['seq_start'],r_['seq_bytes'],'zero pointer slot: lea obj, lea dtor, mov rdx, movq $0 into the slot and the __cxa_atexit call'))
    if k['dwr']:
        # dword-ranged console variables (0x49ca60 / 0x49cc60): constructor, the four functor-class slot bodies of the used class and the atexit destructor
        m6=dict(m6 or {})
        for r_ in d['rows']:
            if r_['kind']!='dwr': continue
            _cb=0x49ca60 if r_['variant']=='a' else 0x49cc60
            m6['rdword_ctor_%x'%_cb]=(_cb,352,h(_cb,352))
            for _nm,_ad,_sz in (('copy',0x49cc00,20),('clone',0x49cc20,20),('move',0x49cc40,9),('destroy',0x49cc50,1)) if r_['variant']=='a' else (('copy',0x49ce00,20),('clone',0x49ce20,20),('move',0x49ce40,9),('destroy',0x49ce50,1)):
                m6['rdword_%s_fn_%s_%x'%(r_['variant'],_nm,_ad)]=(_ad,_sz,h(_ad,_sz))
            m6['rdword_dtor_%x'%r_['dtor']]=(r_['dtor'],fde(r_['dtor']),h(r_['dtor'],fde(r_['dtor'])))
    if k['crit']:
        # critical-section objects: the 0x3b310 init, the 0x3b380 destroy and each atexit destructor body; the whole init + store run is pinned byte for byte
        m6=dict(m6 or {}); m6['critsec_init_3b310']=(0x3b310,107,h(0x3b310,107)); m6['critsec_destroy_3b380']=(0x3b380,5,h(0x3b380,5))
        for r_ in d['rows']:
            if r_['kind']!='crit': continue
            m6['crit_dtor_%x'%r_['dtor']]=(r_['dtor'],fde(r_['dtor']),h(r_['dtor'],fde(r_['dtor'])))
            m6b.append((r_['seq_start'],r_['seq_bytes'],'critical-section object %#x: lea obj, call 0x3b310, the observed field stores (vector zero + qword/dword stores) and the __cxa_atexit call'%r_['obj']))
        if any(r_['dtor']==0x499e50 for r_ in d['rows'] if r_['kind']=='crit'):
            m6['six_buffer_dtor_49c9e0']=(0x49c9e0,113,h(0x49c9e0,113))
        if any(r_['dtor']==0x499e60 for r_ in d['rows'] if r_['kind']=='crit'):
            m6['free_type2_170c0']=(0x170c0,fde(0x170c0),h(0x170c0,fde(0x170c0)))
    if k['holder']:
        m6=dict(m6 or {})
        hcls=[]
        for r_ in d['rows']:
            if r_['kind']=='holder' and (r_['vptr'],r_['dtor']) not in hcls: hcls.append((r_['vptr'],r_['dtor']))
        if any(v_ in(0x15976B0,) for v_,_ in hcls):
            m6.update({'name_holder_dtor': (0x8ef00,23,h(0x8ef00,23))}); extras.append('bp_holder_vtables')
        newc=[(v_,d_) for v_,d_ in hcls if v_!=0x15976B0]
        if newc:
            # a holder class not covered by bp_holder_vtables: pin its address-point slots (relocation targets, or null), the destructor body and the base vptr the destructor rewrites
            hc=[]
            for v_,d_ in newc:
                slots=[]
                for q_ in range(7):
                    try: slots.append(e.reloc_target(v_+8*q_))
                    except BaseException:
                        assert e.u64(v_+8*q_)==0, ('holder class slot is neither relocated nor null',hex(v_),q_); slots.append(0)
                dsz=fde(d_); ins_=disasm(d_,dsz)
                lea=[x for x in ins_ if x['mnem'].startswith('lea') and x['rip'] is not None]; assert len(lea)==1 and ins_[-1]['mnem']=='jmp' and int(ins_[-1]['ops'].split()[0],16)==0xf590, ('holder destructor shape',hex(d_))
                base=lea[0]['rip']; bslots=[]
                for q_ in range(7):
                    try: bslots.append(e.reloc_target(base+8*q_))
                    except BaseException:
                        assert e.u64(base+8*q_)==0; bslots.append(0)
                hc.append(dict(vptr=v_,slots=slots,dtor=d_,base_vptr=base,base_slots=bslots,dtor_lea=lea[0]['addr']))
                m6[f'holder_dtor_{d_:x}']=(d_,dsz,h(d_,dsz))
            extras.append('bp_holder_classes'); spec_extra=dict(spec_extra or {}); spec_extra['holder_classes']=hc
    if k['slider'] or k['slider50']:
        m6=dict(m6 or {}); m6["menu_register_268b0"]=(0x268b0,0x2a7,h(0x268b0,0x2a7))
        if k['slider']: m6["slider_from_var_ctor_27120"]=(0x27120,210,h(0x27120,210))
        if k['slider50']:
            m6["menu_float_ctor_27050"]=(0x27050,208,h(0x27050,208))
            m6b+=[(0x270aa,"c7434802000000",'0x27050 kind = 2'),(0x2711b,"e990f7ffff",'0x27050 tail jmp 0x268b0')]
        sl=[r for r in d['rows'] if r['kind'] in('slider','slider50')]
        seen={}
        for r in sl:
            seen[r['lo_addr']]=(r['lo'],'slider min (.rodata)'); seen[r['hi_addr']]=(r['hi'],'slider max (.rodata)')
        for a,(v,w) in sorted(seen.items()):
            m6b.append((a, v.to_bytes(4,'little').hex(), w))
    if k['boolx']:
        m6=dict(m6 or {}); m6['bool_var_ctor_274730']=(0x274730,661,h(0x274730,661))
        for r_ in d['rows']:
            if r_['kind']=='boolx' and r_['q1src'] is not None:
                m6b.append((r_['q1src'],e.raw(r_['q1src'],8).hex(),'0x274730 args q[1]: a relocated .text RVA read from a global (vmovq), upper qword zeroed by the VEX load'))
    if k['stab']:
        m6=dict(m6 or {})
        for r_ in d['rows']:
            if r_['kind']!='stab': continue
            m6['store_table_dtor_%x'%r_['dtor']]=(r_['dtor'],fde(r_['dtor']),h(r_['dtor'],fde(r_['dtor'])))
            for _tg in r_['tags']:
                if _tg < 0xf80000: m6['store_table_tagfn_%x'%_tg]=(_tg,fde(_tg),h(_tg,fde(_tg)))
            m6b.append((r_['seed_at'],r_['seed_bytes'],'store table seed: lea r14 = &__dso_handle (0x1604218), the register the table of address-point stores is tracked from'))
            spec_extra=dict(spec_extra or {}); spec_extra['store_table']=dict(sym=d['S'],obj=r_['obj'],size=r_['size'],dso=r_['dso'],dtor=r_['dtor'],seed_at=r_['seed_at'],seed_end=r_['seed_end'],start=r_['start'],end=r_['end'],nins=r_['nins'],nstores=len(r_['stores']),nstrings=len(r_['strs']),tags=list(r_['tags']),str_lo=0xf90000,str_hi=0x1594000,text_hi=0xf80000,relro_lo=0x1594000,relro_hi=0x1604218)
            m6b.append((r_['call'],e.raw(r_['call'],5).hex(),'store table: the __cxa_atexit call (call rel32 to the PLT stub 0xf8f390) that registers the table destructor'))
            m6b.append((r_['start'],r_['seq_bytes'],'store table: %d straight-line instructions (%d stores into the 0x%x B object at %#x): the whole range is pinned byte for byte'%(r_['nins'],len(r_['stores']),r_['size'],r_['obj'])))
        extras.append('store_table')
    if k['hstab']:
        m6=dict(m6 or {})
        m6['fnv_entry_a2ddc0']=(0xa2ddc0,10,h(0xa2ddc0,10)); m6['fnv_path_162020']=(0x162020,fde(0x162020),h(0x162020,fde(0x162020)))
        for r_ in d['rows']:
            if r_['kind']!='hstab': continue
            m6b.append((0xa2ddc0,bytes(e.raw(0xa2ddc0,10)).hex(),'0xa2ddc0: mov esi, 0x811c9dc5 ; jmp 0x162020 (the path-folding FNV entry)'))
            m6b.append((0x162020,bytes(e.raw(0x162020,11)).hex(),'0x162020 prologue: push rbp ; mov rbp,rsp ; push r15 ; push r14 ; push r12 ; push rbx (callee-saved registers rbx / r12 / r14 / r15 are saved)'))
            m6b.append((0x162079,bytes(e.raw(0x162079,9)).hex(),'0x162020 epilogue: pop rbx ; pop r12 ; pop r14 ; pop r15 ; pop rbp ; ret (the single exit restores them)'))
            m6b.append((r_['start'],r_['seq_bytes'],'hash-store run: %d instructions (%d x lea rdi / call 0xa2ddc0 / mov [rbx+d], eax, plus table-base and string-register loads and the interleaved stack-guard load): the whole range is pinned byte for byte'%(r_['nins'],r_['n'])))
            spec_extra=dict(spec_extra or {}); spec_extra['hash_store']=dict(sym=d['S'],obj=r_['obj'],size=r_['size'],start=r_['start'],end=r_['end'],nins=r_['nins'],nstores=r_['n'],nstrings=len(r_['strs']),expected=list(r_['expected']),str_lo=0xf90000,str_hi=0x1594000)
        extras.append('hash_store')
    ftags=([d['ftag']] if d['floats'] else [])+([d['utag']] if d['utag'] else []); btags=list(dict.fromkeys(([d['btag']] if d['bools'] else [])+list(d['bxtags'])))
    write_checker(n, doc, callees, d['native'], summary, bool_tags=btags, float_tags=ftags, int_tags=([d['itag']] if d['ints'] else [])+([d['wtag']] if d['uints'] and d['wtag']!=d['itag'] else []), extras=extras, m6_callees=m6, m6_bytes=m6b, extra_defs=edefs, spec_extra=spec_extra, drop_strings=tuple(r_['src'] for r_ in d['rows'] if r_['kind']=='ncell')+tuple(v_ for r_ in d['rows'] if r_['kind'] in ('stab','hstab') for v_,_t in r_['strs']))   # pooled store-table strings are tied by x_store_table (pool text re-read at its .rodata address)
    return d
if __name__=='__main__':
    pass
