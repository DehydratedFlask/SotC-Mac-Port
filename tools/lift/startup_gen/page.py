import sys, struct
import pathlib as _pl
_HERE=str(_pl.Path(__file__).resolve().parent)
_LIFT=str(_pl.Path(__file__).resolve().parents[1])
_ROOT=str(_pl.Path(__file__).resolve().parents[3])+'/'
sys.path.insert(0,_HERE)
from track import *
def cstr_at(a,n):
    return bytes(e.raw(a,n))
def cstr_z(a):
    b=bytearray()
    while True:
        c=e.raw(a+len(b),1)[0]
        if c==0: return bytes(b)
        b.append(c)
import re as _re
def _wrapper_rvas():
    # the 26 wrapper entry points of the local ranged-int family, read from the native table (first column of kLocalIntVarClasses)
    txt=open(_ROOT+'native/core/helper_620e10_local_int_var.hpp').read()
    body=txt.split('kLocalIntVarClasses{{',1)[1].split('}};',1)[0]
    return [int(m,16) for m in _re.findall(r'\{(0x[0-9a-f]+), 0x[0-9a-f]+,',body)]
WRAPPERS=set(_wrapper_rvas())
# The seven byte twins of the 23 B holder destructor (check_helper_8ef00.py pins every body per instruction)
HOLDER_DTORS=(0x8ef00,0x2fced0,0x2b5050,0x34e820,0x5b5470,0x5b5490,0x2c84f0,0x4cb9e0,0x4cba00)   # batch 20: slot 445 adds 0x4cb9e0 / 0x4cba00 (23 B twins; shape asserted by mk_page, bodies hash-pinned)
# zero-header vector destructors (check_helper_5b5530.py): obj+8 begin, +0x10 end, +0x18 cap
ZVEC_DTORS=(0x5b5530,0x5c7c00,0x52a0d0,0x52a140,0x52a1b0)   # batch 19: slot 429's three vectors
SENT_DTORS=(0x3941b0,0x5b55c0)

# ---- batch 18 (slot 424): the long straight-line store table. Two INDEPENDENT decoders replay the same range (objdump text -> replay.py; raw bytes -> store_bytes_replay.py) and must agree
# on every store (instruction address, instruction bytes, target, width, 64-bit lane values); a disagreement or any unsupported instruction class is a hard error.
sys.path.insert(0,_LIFT)
import replay as _rp
import store_bytes_replay as _sb
STAB_DTOR=0x54dbb0
STR_LO,STR_HI=0xf90000,0x1594000        # .rodata window that holds the C strings; relro window [0x1594000, 0x1604218) holds the vtable-like identity tags
ENTRY_DTOR=0x3c15e0; ENTRY_REGISTER=0x3be2d0; ENTRY_SELF_CELL=0x2ef63f8; ENTRY_HEAD_CELL=0x2ef6400; ENTRY_STRIDE=0x90
def _entry_row(xi,x,r,ev,INS):
    """Scripted-command registry entry (batch 21, slot 446): two 0xf2f0 name views, a 0x88 B object filled by a straight-line run (three {obj+0x88, functor} pairs,
    zeros, the first view copied to +0x60 with an atomic control increment, the previous list head at +0x80), a push on the global list (cell 0x2ef6400), a guarded
    0x3be2d0(*0x2ef63f8, obj), two 0xf590 on the views (second view first) and __cxa_atexit(0x3c15e0, obj, &__dso_handle). The run is evaluated SYMBOLICALLY here
    (no instruction is skipped silently: any unmodelled instruction is a hard error) and must reproduce exactly the canonical object image."""
    o=r['rsi']; assert isinstance(o,int) and r['rdx']==0x1604218, ('entry atexit arguments',{k:hex(v) for k,v in r.items() if isinstance(v,int)})
    prevat=max([y['addr'] for y in ev[:xi] if not isinstance(y,tuple) and y['t']==0xf8f390]+[0])
    start,end=prevat+5,x['addr']+5
    run_=[q for q in INS if start<=q['addr']<end]
    assert run_[0]['addr']==start and run_[-1]['addr']==x['addr'], 'entry run bounds'
    reg={}; xmm={}; stk={}; mem={}; events=[]; views=[]
    for q_ in INS:   # callee-saved registers keep the cell addresses loaded by an earlier entry (rbx / r14 are never clobbered by the in-run calls)
        if q_['addr']<start and q_['mnem']=='leaq' and q_['rip'] in(ENTRY_SELF_CELL,ENTRY_HEAD_CELL) and q_['ops'].split('#')[0].split(',')[-1].strip() in('%rbx','%r14'): reg[q_['ops'].split('#')[0].split(',')[-1].strip().lstrip('%')]=('cellp',q_['rip'])
    Z=('z',); OFF88=('off',0x88)
    def ops(q):
        t=q['ops'].split('#')[0].strip()
        return [a.strip() for a in re.split(r',\s*(?![^()]*\))',t)]
    def rn(a): return a.lstrip('%').replace('ymm','xmm')
    def mrip(q): return q['rip']
    def vec(n): return xmm.get(n,None)
    def setq(off,v):
        assert 0<=off<0x90 and off%8==0, ('entry store outside the object / unaligned',hex(off)); mem[off]=v
    i=0
    while i<len(run_):
        q=run_[i]; m=q['mnem']; a=ops(q); i+=1
        if m=='leaq':
            src,dst=a; d=rn(dst)
            if '(%rip)' in src:
                t=q['rip']
                reg[d]=OFF88 if t==o+0x88 else ('obj',) if t==o else ('cellp',t) if t in(ENTRY_SELF_CELL,ENTRY_HEAD_CELL) else ('code',t) if t==ENTRY_DTOR else ('str',t) if t<0x1100000 else ('data',t)
            elif '(%rbp)' in src: reg[d]=('sp',int(src.split('(')[0],16))
            else: raise AssertionError(('entry lea form',q['text']))
        elif m=='xorl' and a==['%esi','%esi']: reg['rsi']=('int',0)
        elif m=='callq':
            tg=int(q['ops'].split()[0],16)
            if tg==0xf2f0:
                assert reg.get('rdi',(0,))[0]=='sp' and reg.get('rsi')==('int',0) and reg.get('rdx',(0,))[0]=='str', ('entry name view call',q['text'])
                base=reg['rdi'][1]; vi=len(views); views.append(dict(sp=base,cstr=reg['rdx'][1]))
                for k in range(4): stk[base+8*k]=('view',vi,k)
            elif tg==0xf590:
                assert reg.get('rdi',(0,))[0]=='sp', ('entry release operand',q['text']); events.append(('release',reg['rdi'][1]))
            elif tg==ENTRY_REGISTER:
                events.append(('register',reg.get('rdi'),reg.get('rsi')))
            elif tg==0xf8f390:
                assert q['addr']==x['addr']; events.append(('atexit',reg.get('rdi'),reg.get('rsi'),reg.get('rdx')))
            else: raise AssertionError(('entry run: unmodelled call',q['text']))
        elif m=='vmovq' and '(%rip)' in a[0]: xmm[rn(a[1])]=[('q',q['rip']),Z,Z,Z]
        elif m=='vmovq' and a[0].startswith('%r') and a[1].startswith('%xmm'): xmm[rn(a[1])]=[reg[rn(a[0])],Z,Z,Z]
        elif m in('vxorps','vpxor'):
            assert len(set(a))==1 and a[0].startswith('%xmm'), ('entry zeroing form',q['text']); xmm[rn(a[0])]=[Z,Z,Z,Z]
        elif m=='vpunpcklqdq':
            s1,s2,dd=[xmm[rn(t)] for t in a[:2]]+[None]; xmm[rn(a[2])]=[s2[0],s1[0],Z,Z]   # dst = [second operand lo, first operand lo]
        elif m=='vinsertf128':
            assert a[0]=='$0x1' and a[1].startswith('%xmm') and a[2].startswith('%ymm') and a[3].startswith('%ymm'), ('entry vinsertf128 form',q['text'])
            hi=xmm[rn(a[1])]; lo=xmm[rn(a[2])]; xmm[rn(a[3])]=[lo[0],lo[1],hi[0],hi[1]]
        elif m=='movq' and a[0].startswith('-') and a[0].endswith('(%rbp)'):
            off=int(a[0].split('(')[0],16)
            assert off in stk, ('entry stack load outside the views',q['text']); reg[rn(a[1])]=stk[off]
        elif m=='movq' and a[0].startswith('$') and '(%rip)' in a[1]:
            assert int(a[0][1:],16)==0, 'entry immediate store is not zero'; setq(q['rip']-o,Z)
        elif m=='movq' and a[0].startswith('%r') and '(%rip)' in a[1]:
            setq(q['rip']-o,reg[rn(a[0])])
        elif m in('vmovdqu','vmovups') and '(%rip)' in a[1]:
            v=xmm[rn(a[0])]; n=4 if a[0].startswith('%ymm') else 2
            for k in range(n): setq(q['rip']-o+8*k,v[k])
        elif m=='movq' and a[0].startswith('(%') and a[1].startswith('%r'):
            c=reg[rn(a[0].strip('()'))]; assert c[0]=='cellp', ('entry cell load',q['text']); reg[rn(a[1])]=('ld',c[1],q['addr'])
        elif m=='movq' and a[0].startswith('%r') and a[1].startswith('(%'):
            c=reg[rn(a[1].strip('()'))]; assert c[0]=='cellp', ('entry cell store',q['text']); events.append(('cellstore',c[1],reg[rn(a[0])],q['addr']))
        elif m=='testq':
            assert a[0]==a[1] and run_[i]['mnem']=='je', ('entry test form',q['text']); v=reg[rn(a[0])]; je=run_[i]; i+=1; tgt=int(je['ops'].split()[0],16)
            if run_[i]['mnem']=='lock':
                assert run_[i+1]['mnem']=='incq' and run_[i+1]['ops'].replace(' ','').split('#')[0]=='(%rax)' and run_[i+2]['addr']==tgt and v==reg['rax'], ('entry ctl increment shape',q['text'])
                events.append(('incctl',v)); i+=2
            else:
                assert run_[i]['mnem']=='leaq' and run_[i+1]['mnem']=='callq' and run_[i+2]['addr']==tgt, ('entry guarded call shape',q['text'])
                events.append(('guard',v))
        else: raise AssertionError(('entry run: unmodelled instruction',q['text']))
    # the canonical image
    qs=[m_ for m_ in mem.values() if m_[0]=='q']; qaddrs=[]
    for off in (0x08,0x28,0x48):
        assert mem.get(off,(0,))[0]=='q', ('entry functor slot',hex(off)); qaddrs.append(mem[off][1])
    want={0x00:OFF88,0x10:Z,0x18:Z,0x20:OFF88,0x30:Z,0x38:Z,0x40:OFF88,0x50:Z,0x58:Z,0x60:('view',0,0),0x68:('view',0,1),0x70:('view',0,2),0x78:('view',0,3)}
    for off,v in want.items(): assert mem.get(off)==v, ('entry image mismatch',hex(off),mem.get(off),v)
    lastld=[ev_ for ev_ in mem.items() if ev_[0]==0x80]; assert len(lastld)==1 and lastld[0][1][0]=='ld' and lastld[0][1][1]==ENTRY_HEAD_CELL, 'entry previous list head at +0x80'
    assert set(mem)=={0x00,0x08,0x10,0x18,0x20,0x28,0x30,0x38,0x40,0x48,0x50,0x58,0x60,0x68,0x70,0x78,0x80}, ('entry written offsets',sorted(hex(k) for k in mem))
    assert len(views)==2 and views[0]['sp']!=views[1]['sp'], 'two name views'
    kinds=[ev_[0] for ev_ in events]
    assert kinds==['incctl','cellstore','guard','register','release','release','atexit'], ('entry event order',kinds)
    inc,cst,grd,reg_,rel1,rel2,atx=events
    assert inc[1]==('view',0,3), 'the control increment is on the copied view control word'
    assert cst[1]==ENTRY_HEAD_CELL and cst[2]==('obj',) and cst[3]>lastld[0][1][2], 'list push: the previous head is read BEFORE the cell is overwritten'
    assert grd[1][0]=='ld' and grd[1][1]==ENTRY_SELF_CELL and reg_[1]==grd[1] and reg_[2]==('obj',), 'guarded registration of obj in *0x2ef63f8'
    assert rel1[1]==views[1]['sp'] and rel2[1]==views[0]['sp'], 'views are released description first, then name'
    assert atx[1]==('code',ENTRY_DTOR) and atx[2]==('obj',) and atx[3]==('data',0x1604218), 'atexit arguments'
    return dict(kind='entry',obj=o,dtor=ENTRY_DTOR,name=cstr_z(views[0]['cstr']),desc=cstr_z(views[1]['cstr']),name_ptr=views[0]['cstr'],desc_ptr=views[1]['cstr'],fns=[e.u64(a_) for a_ in qaddrs],fn_slots=qaddrs,fn_raw=[e.raw(a_,8).hex() for a_ in qaddrs],
                seq_start=start,seq_end=end,seq_bytes=e.raw(start,end-start).hex(),call=x['addr'])
def _store_table_row(xi,x,r,ev,INS):
    assert r['rdi']==STAB_DTOR and r['rsi']==0 and r['rdx']==0x1604218, ('table atexit arguments',{k:hex(v) for k,v in r.items() if isinstance(v,int)})
    dsz=fde(STAB_DTOR); di=disasm(STAB_DTOR,dsz)
    shape=[(q['mnem'],q['ops'].split('#')[0].strip()) for q in di]
    assert shape==[('pushq','%rbp'),('movq','%rsp, %rbp'),('pushq','%r14'),('pushq','%rbx'),('leaq','0x29b1d22(%rip), %r14'),('movl','$0x6720, %ebx'),('nopw','%cs:(%rax,%rax)'),('movq','-0x20(%r14,%rbx), %rdi'),('testq','%rdi, %rdi'),('je','0x54dbf7 <PT_LOAD'),('movq','(%rdi), %rax'),('leaq','-0x40(%r14,%rbx), %rcx'),('xorl','%esi, %esi'),('cmpq','%rdi, %rcx'),('setne','%sil'),('callq','*0x20(%rax)'),('movq','$0x0, -0x20(%r14,%rbx)'),('addq','$-0x60, %rbx'),('jne','0x54dbd0 <PT_LOAD'),('popq','%rbx'),('popq','%r14'),('popq','%rbp'),('retq','')], ('table destructor shape',shape)
    base=di[4]['rip']; size=0x6720; stride=0x60
    assert size%stride==0
    prevat=max(y['addr'] for y in ev[:xi] if not isinstance(y,tuple) and y['t']==0xf8f390)
    start,end=prevat+5,x['addr']
    seeds=[q for q in INS if q['mnem']=='leaq' and q['ops'].split('#')[0].split(',')[-1].strip()=='%r14' and prevat-0x20<q['addr']<prevat]
    assert len(seeds)==1 and seeds[0]['rip']==r['rdx'], ('r14 seed lea',seeds)
    seed=seeds[0]; seed_end=seed['addr']+7
    a=_rp.Replay({'r14':('a',seed['rip'])}); a.run(INS,start,end)
    b=_sb.ByteReplay(e.raw); assert b.step(seed['addr'])==7; b.nins=0; b.run(start,end)
    assert a.nins==b.nins, ('instruction counts of the two decoders',a.nins,b.nins)
    assert len(a.stores)==len(b.stores), ('store counts of the two decoders',len(a.stores),len(b.stores))
    for sa,sb_ in zip(a.stores,b.stores):
        hb=sa['bytes'] if isinstance(sa['bytes'],str) else bytes(sa['bytes']).hex()
        assert (sa['addr'],hb,sa['target'],sa['width'],sa['lanes'])==(sb_['addr'],sb_['bytes'],sb_['target'],sb_['width'],sb_['lanes']), ('the two decoders disagree on a store',hex(sa['addr']),sa,sb_)
    assert b.g[7]==('a',STAB_DTOR) and b.g[6]==('c',0) and b.g[2]==('a',r['rdx']) and a.g['rdi']==b.g[7] and a.g['rsi']==b.g[6] and a.g['rdx']==b.g[2], 'final __cxa_atexit argument registers'
    ca=e.raw(end,5); assert ca[0]==0xe8 and end+5+int.from_bytes(ca[1:],'little',signed=True)==0xf8f390, 'range end is the __cxa_atexit PLT call'
    strs={}; tags=set(); stores=[]
    for s_ in b.stores:
        off=s_['target']-base
        assert 0<=off and off+s_['width']<=size, ('store outside the table',hex(s_['addr']),hex(s_['target']))
        lanes=[]
        for (k,v) in s_['lanes']:
            if k=='c': lanes.append((0,v)); continue
            assert s_['width'] in (8,16), ('sub-qword address store',hex(s_['addr']))
            if base<=v<base+size: lanes.append((1,v-base)); continue
            if STR_LO<=v<STR_HI:
                t_=cstr_z(v); assert t_ and all(0x20<=c<0x7f for c in t_), ('rodata pointer is not a printable NUL-terminated string',hex(v))
                strs[v]=t_; lanes.append((2,v)); continue
            if v<0xf80000: fde(v); tags.add(v); lanes.append((3,v)); continue
            if 0x1594000<=v<0x1604218: tags.add(v); lanes.append((3,v)); continue
            raise AssertionError(('address lane outside the known windows',hex(v),hex(s_['addr'])))
        stores.append(dict(addr=s_['addr'],ln=len(s_['bytes'])//2,off=off,width=s_['width'],lanes=lanes))
    sv=sorted(strs); sidx={v:i for i,v in enumerate(sv)}
    for st in stores: st['lanes']=[(k,sidx[v] if k==2 else v) for k,v in st['lanes']]
    return dict(kind='stab',obj=base,size=size,stride=stride,dtor=STAB_DTOR,call=x['addr'],atexit_obj=0,start=start,end=end,seed_at=seed['addr'],seed_end=seed_end,dso=r['rdx'],
                stores=stores,strs=[(v,strs[v]) for v in sv],tags=sorted(tags),nins=b.nins,
                seq_bytes=e.raw(start,end-start).hex(),seed_bytes=e.raw(seed['addr'],7).hex())

import hash_store_bytes as _hb
def fnv_a2ddc0_py(text):
    """Python port of 0xa2ddc0 -> 0x162020 (offset basis 0x811c9dc5, FNV prime 0x1000193; each byte ASCII-upper-cased, '\\' folded to '/'). Used only as an INDEPENDENT oracle for the native hash."""
    h=0x811c9dc5
    for c in text:
        if 0x61<=c<=0x7a: c-=0x20
        if c==0x5c: c=0x2f
        h=((h^c)*0x1000193)&0xffffffff
    return h
def _hash_store_row(ev,hcalls,INS):
    """Row 'hstab' (batch 20, slot 445): n x [lea rdi,str ; call 0xa2ddc0 ; mov [rbx+d],eax]. Two INDEPENDENT decoders (objdump text -> this function; raw bytes ->
    hash_store_bytes.replay) replay the SAME range and must agree store for store; any unsupported instruction is a hard error."""
    first=hcalls[0]['addr']; last=hcalls[-1]['addr']
    prev=max([y['addr'] for y in ev if not isinstance(y,tuple) and y['addr']<first]+[0])
    assert prev, 'no event before the hash run'
    start=prev+5
    stores_after=[q for q in INS if q['mnem']=='movl' and q['ops'].replace(' ','').endswith(',%eax') is False and q['addr']>last]
    lastst=[q for q in INS if q['mnem']=='movl' and q['ops'].split('#')[0].replace(' ','').startswith('%eax,') and '(%rbx)' in q['ops'] and q['addr']>last]
    assert lastst, 'no store after the last hash call'
    end=lastst[0]['addr']+lastst[0]['len']
    # decoder A: objdump text
    g={}; eax=None; pend=None; A=[]; nins=0
    for q in INS:
        if not (start<=q['addr']<end): continue
        nins+=1
        m=q['mnem']; ops=q['ops'].split('#')[0].replace(' ','')
        if m=='leaq' and q['rip'] is not None and ops.split(',')[-1] in ('%rdi','%rbx','%r14','%r15'): g[ops.split(',')[-1]]=q['rip']
        elif m=='movq' and ops in ('%r14,%rdi','%r15,%rdi'): g['%rdi']=g[ops.split(',')[0]]
        elif m=='callq' and ops.startswith('0xa2ddc0'):
            assert pend is None and isinstance(g.get('%rdi'),int), ('hash call without a string / result still pending',hex(q['addr']))
            pend=(q['addr'],g['%rdi']); g.pop('%rdi',None)
        elif m=='movl' and ops.startswith('%eax,') and ops.endswith('(%rbx)'):
            assert pend is not None and isinstance(g.get('%rbx'),int), ('store without a pending result / rbx',hex(q['addr']))
            d=ops.split(',')[1][:-6]; disp=int(d,16) if d else 0
            A.append((pend[0],q['addr'],g['%rbx']+disp,pend[1])); pend=None
        elif m=='movq' and q['rip']==0x15fe518 and ops.endswith(',%rcx'): pass
        else: raise AssertionError(('hash-store run: unsupported instruction',hex(q['addr']),m,ops))
    assert pend is None, 'last hash result not stored'
    Bd=_hb.replay(e.raw,start,end)
    assert Bd['nins']==nins and Bd['stores']==A, ('the two decoders disagree on the hash-store run',nins,Bd['nins'])
    assert len(A)==len(hcalls) and [a[0] for a in A]==[x['addr'] for x in hcalls], 'every hash call has exactly one store'
    assert all(x['regs']['rdi']==a[3] for x,a in zip(hcalls,A)), 'tracker rdi equals the decoded string address'
    dests=[a[2] for a in A]; assert len(set(dests))==len(dests) and all(d%4==0 for d in dests), 'hash-store destinations: distinct, dword aligned'
    obj=min(dests); size=max(dests)+4-obj
    sv=sorted({a[3] for a in A}); strs=[]
    for v in sv:
        t_=cstr_z(v); assert t_ and all(0x20<=c<0x7f for c in t_), ('hash string is not printable',hex(v)); strs.append((v,t_))
    sidx={v:i for i,v in enumerate(sv)}
    return dict(kind='hstab',obj=obj,size=size,n=len(A),stores=[dict(call=a[0],addr=a[1],off=a[2]-obj,sidx=sidx[a[3]],sva=a[3]) for a in A],strs=strs,
                expected=[fnv_a2ddc0_py(dict(strs)[a[3]]) for a in A],start=start,end=end,nins=nins,seq_bytes=e.raw(start,end-start).hex())

def rows_of(idx):
    lastrel=None
    t,ev=run(idx)
    _rva=e.u64(g.TABLE_TOP-8*idx); INS=disasm(_rva,fde(_rva))
    rows=[]; atexit=[]
    ab=lambda a,sz: t.read(('abs',a),sz)
    urow=None
    prev=None
    recs=[]
    pend_tree=pend_nref=pend_ring=pend_crit=None
    gets=[]; getpos=None
    hcalls=[]; hpos=None
    for xi,x in enumerate(ev):
        if isinstance(x,tuple): continue
        tg=x['t']; r=x['regs']
        pv=prev; prev=x
        if tg not in _TOK and tg not in WRAPPERS and e.raw(tg,1)==b'\xb8' and e.raw(tg+5,1)==b'\xc3' and fde(tg)==6:
            # `mov eax, imm32 ; ret` getter (6 B, no calls, no loads): consumed with its neighbours into one 'gettable' row after the loop
            if getpos is None: getpos=len(rows)
            gets.append(x); continue
        if tg in WRAPPERS:
            nm=x['view_rsi']
            if pv is not None and pv['t']==0xf730:
                nxt=[y for y in ev[xi+1:xi+4]]
                assert isinstance(nxt[0],tuple) and nxt[1]['t']==0xf590 and nxt[1]['view_rdi']==nm and nxt[2]['t']==0xf8f390 and nxt[2]['regs']['rsi']==r['rdi'], 'unexpected call-site shape after the wrapper'
                rows.append(dict(kind='lint',obj=r['rdi'],wrapper=tg,name=cstr_at(*nm),name_ptr=nm[0],len=nm[1],flags=r['r9'],value=x['f_rdx'],lo=x['f_rcx'],hi=x['f_r8'],
                   tag=x['lint_tag'],dtor=nxt[2]['regs']['rdi'],call=x['addr'],ind=nxt[0][1],atexit_call=nxt[2]['addr'],f730=pv['addr'],desc_text=None))
            else:
                # described site (7ec160 order): wrapper ; f590(description view) ; call [tag + 0x18] ; f590(name) ; atexit
                ds=x.get('lint_desc'); assert ds and ds[0] and ds[1] is not None, ('local-int description view unresolved',hex(x['addr']))
                nxt=[y for y in ev[xi+1:xi+5]]
                assert nxt[0]['t']==0xf590 and nxt[0]['view_rdi']==ds and isinstance(nxt[1],tuple) and nxt[2]['t']==0xf590 and nxt[2]['view_rdi']==nm and nxt[3]['t']==0xf8f390 and nxt[3]['regs']['rsi']==r['rdi'], 'unexpected described call-site shape after the wrapper'
                rows.append(dict(kind='lint',obj=r['rdi'],wrapper=tg,name=cstr_at(*nm),name_ptr=nm[0],len=nm[1],flags=r['r9'],value=x['f_rdx'],lo=x['f_rcx'],hi=x['f_r8'],
                   tag=x['lint_tag'],dtor=nxt[3]['regs']['rdi'],call=x['addr'],ind=nxt[1][1],atexit_call=nxt[3]['addr'],f730=None,desc_text=cstr_at(*ds),desc_len=ds[1],desc_ptr=ds[0],desc_f590=nxt[0]['addr']))
            continue
        if tg==0x1580a0:
            # perspective-projection builder (helper_1580a0_perspective): out matrix, fov cell on the stack, aspect / near / far loaded from .rodata, edx = flag
            assert isinstance(r['rdi'],int) and issp(r['rsi']) and r['rdx']==1, ('1580a0 arguments unresolved',hex(x['addr']))
            fv=x['fov']; assert isinstance(fv,int), ('1580a0 fov cell unresolved',hex(x['addr']))
            fl=[x['xmm0'],x['xmm1'],x['xmm2']]
            assert all(isinstance(v_,tuple) and v_[0]=='f' for v_ in fl), ('1580a0 aspect / near / far are not rodata loads',fl)
            rows.append(dict(kind='pre',obj=r['rdi'],fov=fv,aspect_addr=fl[0][1],near_addr=fl[1][1],far_addr=fl[2][1],flag=r['rdx'],call=x['addr'],
                             aspect=int.from_bytes(e.raw(fl[0][1],4),'little'),near=int.from_bytes(e.raw(fl[1][1],4),'little'),far=int.from_bytes(e.raw(fl[2][1],4),'little')))
            continue
        if tg==0xa81700:
            # named-record registrar: 13-style run of records, then ONE group atexit (0x1c8990) over the block
            v4=x.get('v4_rsi'); assert v4 and None not in v4 and v4[2:]==(0,0), ('record name view is not a borrowed 4-qword view',hex(x['addr']))
            assert isinstance(r['rdi'],int) and isinstance(r['rdx'],int) and isinstance(r['rcx'],int), ('record kind/flag unresolved',hex(x['addr']))
            nm=x['view_rsi']; assert nm[1]==v4[1]
            recs.append(dict(obj=r['rdi'],name=cstr_at(*nm),name_ptr=nm[0],len=nm[1],rkind=r['rdx'],flag=r['rcx']&0xff,call=x['addr']))
            continue
        if tg==0x274730:
            # out-of-line bool console-variable ctor: 0x274730(obj, &name, &initial, &args) ; call [argfn + 0x18] ; f590(name) ; __cxa_atexit(0x20b0, obj)
            o=r['rdi']; nm=x['view_rsi']
            assert isinstance(o,int) and nm and nm[0] and nm[1] is not None, ('0x274730 object / name unresolved',hex(x['addr']))
            assert issp(r['rdx']) and issp(r['rcx']), ('0x274730 initial / args are not stack objects',hex(x['addr']))
            bx_=x['bx']; init_=bx_['init']; b_=r['rcx'][1]; q_=list(bx_['q'])
            assert isinstance(init_,int) and init_ in (0,1), ('0x274730 initial byte',init_)
            assert q_[0]==('sp',b_), ('0x274730 args q0 is not the self reference',q_[0])
            if isinstance(q_[1],tuple) and q_[1][0]=='vq':
                try: q1src=q_[1][1]; q_[1]=e.reloc_target(q1src)
                except BaseException: q_[1]=e.u64(q1src)
            else: q1src=None
            assert all(isinstance(v_,int) for v_ in q_[1:]), ('0x274730 args q[1..3] unresolved',q_)
            pay_=bx_['pay']; cls_=bx_['cls']; aux_=bx_['aux']
            assert all(isinstance(v_,int) for v_ in (pay_,cls_,aux_)), ('0x274730 functor block unresolved',pay_,cls_,aux_)
            nx=ev[xi+1:xi+4]
            assert len(nx)==3 and isinstance(nx[0],tuple) and nx[1]['t']==0xf590 and nx[1]['view_rdi']==nm and nx[2]['t']==0xf8f390 and nx[2]['regs']['rdi']==0x20b0 and nx[2]['regs']['rsi']==o, ('0x274730 call-site shape',hex(x['addr']))
            rows.append(dict(kind='boolx',obj=o,name=cstr_at(*nm),name_ptr=nm[0],len=nm[1],initial=init_,q=q_[1:],q1src=q1src,payload=pay_,tag=cls_,aux=aux_,call=x['addr'],ind=nx[0][1]))
            continue
        if urow is not None and urow['early'] is None: urow['early']=(tg==0xf590)
        if tg==0x20af0 and r['rcx']==2:
            # console command: 0xf2f0 name view, 0x20af0 flags 2, register-once through [0x1594dc0 + 0x20], two 0xf590, __cxa_atexit(0x4b0, obj)
            o=r['rdi']; nm=x['view_rsi']; ds=x['view_rdx']
            assert nm and nm[0] and nm[1] is not None and ds and ds[0] and ds[1] is not None, ('command views unresolved',hex(x['addr']))
            nx=ev[xi+1]
            assert isinstance(nx,tuple) and nx[2]==('ld',0x1594de0), ('command register-once is not [0x1594dc0+0x20]',hex(x['addr']),nx)
            lo_=pv_addr=None
            prevcall=max([y['addr'] for y in ev[:xi] if not isinstance(y,tuple) and y['t']==0xf2f0]+[0])
            hs=[q['rip'] for q in INS if prevcall<q['addr']<x['addr'] and q['mnem'].startswith('lea') and q['rip'] is not None and q['rip']<0xf80000]
            assert len(hs)==1, ('command handler lea', [hex(h) for h in hs], hex(x['addr']))
            rows.append(dict(kind='cmd',obj=o,name=cstr_at(*nm),name_ptr=nm[0],len=nm[1],desc=ds,desc_text=cstr_at(*ds),flags=r['rcx'],handler=hs[0],call=x['addr'],ind=nx[1]))
        elif tg==0x20af0:
            o=r['rdi']; nm=x['view_rsi']; ds=x['view_rdx']
            if ab(o,8)!=0x1594218:
                # an inline-built ranged enum variable (the compiler inlined a 0xd8 byte ranged-int constructor): bool call grammar, ranged-int stores
                regc=next(y['addr'] for y in ev[xi+1:] if not isinstance(y,tuple) and y['t']==0x20d60)
                lea_={}; mid=None
                for q in INS:
                    if q['addr']<=x['addr'] or q['addr']>=regc: continue
                    if q['mnem']=='leaq' and q['rip'] is not None: lea_[q['ops'].split(',')[-1].strip().split()[0]]=q['rip']
                    elif q['mnem']=='movq' and q['rip']==o: mid=lea_.get(q['ops'].split(',')[0].strip()); break
                assert mid, ('inline enum: mid vptr store not found',hex(o))
                f_=dict(vref=ab(o+0x60,8),dflt=ab(o+0x68,4),lo=ab(o+0x6c,4),elo=ab(o+0x70,1),hi=ab(o+0x74,4),ehi=ab(o+0x78,1),payload=ab(o+0xb0,8),tag=ab(o+0xc0,8),aux=ab(o+0xc8,8),value=ab(o+0xd0,4),vptr=ab(o,8))
                if f_['lo'] is None and f_['hi'] is None and f_['elo']==0 and f_['ehi']==0:
                    # unbounded inline enum: the constructor stores ONE zero byte at +0x6c / +0x74 and clears the engaged bytes (+0x70 / +0x78); the other bytes are never written
                    assert ab(o+0x6c,1)==0 and ab(o+0x74,1)==0 and ab(o+0x6d,1) is None and ab(o+0x75,1) is None, ('inline enum: unbounded bound stores',f_)
                    f_['lo']=0; f_['hi']=0; f_['bounded']=False
                else: f_['bounded']=True
                assert all(v_ is not None for v_ in f_.values()), ('inline enum: unresolved store',f_)
                assert f_['vref']==o+0xd0 and f_['elo']==int(f_['bounded']) and f_['ehi']==int(f_['bounded']) and f_['payload']==0 and f_['aux']==0 and all(ab(o+0x80+8*q_,8)==0 for q_ in range(4)), ('inline enum: layout facts',f_)
                assert ds and ds[1]==0 and r['rcx']==1, 'inline enum: description / flags'
                _ea=next(y for y in ev[xi+1:] if not isinstance(y,tuple) and y['t']==0xf8f390 and y['regs']['rsi']==o)
                rows.append(dict(kind='ienum',obj=o,name=cstr_at(*nm),name_ptr=nm[0],len=nm[1],desc=ds,flags=r['rcx'],mid=mid,call=x['addr'],dtor=_ea['regs']['rdi'],**f_))
            else:
              rows.append(dict(kind='bool',obj=o,name=cstr_at(*nm),name_ptr=nm[0],len=nm[1],desc=ds,flags=r['rcx'],
               vptr=ab(o,8),tag=ab(o+0xb0,8),value=ab(o+0xc0,1),default=ab(o+0x68,1),early=None))
              urow=rows[-1]
        elif tg in (0x49ca60,0x49cc60):
            # dword-valued ranged console variable (helper_49ca60): ctor(obj, &name, value_ref, &lo, &hi, flags=1, args, description) ; call [class + 0x18] ; register 0x20d60 ;
            # f590(description) ; f590(name) ; __cxa_atexit(dtor, obj). The 7th/8th arguments are the two stack words (args block, description view).
            o=r['rdi']; nm=x['view_rsi']; ds=x.get('lint_desc')
            assert isinstance(o,int) and nm and nm[0] and nm[1] is not None and ds and ds[0] and ds[1] is not None, ('dword ranged variable name / description unresolved',hex(x['addr']))
            assert issp(r['rcx']) and issp(r['r8']) and r['r9']==1 and r['rdx']==o+0xd0, ('dword ranged variable arguments',hex(x['addr']))
            nx=ev[xi+1:xi+6]
            assert len(nx)==5 and isinstance(nx[0],tuple) and nx[1]['t']==0x20d60 and nx[2]['t']==0xf590 and nx[2]['view_rdi']==ds and nx[3]['t']==0xf590 and nx[3]['view_rdi']==nm and nx[4]['t']==0xf8f390 and nx[4]['regs']['rsi']==o, ('dword ranged variable call-site shape',hex(x['addr']))
            assert isinstance(x['f_rcx'],int) and isinstance(x['f_r8'],int) and isinstance(x['lint_tag'],int), ('dword ranged variable bounds / class unresolved',hex(x['addr']))
            rows.append(dict(kind='dwr',variant='a' if tg==0x49ca60 else 'b',tgt=tg,obj=o,name=cstr_at(*nm),name_ptr=nm[0],len=nm[1],desc=ds,desc_text=cstr_at(*ds),flags=r['r9'],lo=x['f_rcx'],hi=x['f_r8'],tag=x['lint_tag'],
               value=ab(o+0xd0,4),default=ab(o+0x68,4),vptr=ab(o,8),value_ref=r['rdx'],call=x['addr'],ind=nx[0][1],dtor=nx[4]['regs']['rdi'],atexit_call=nx[4]['addr']))
        elif tg==0xa2ddc0:
            # batch 20 (slot 445): a path-folding FNV hash of a rodata string whose result is stored as a dword; every consecutive call is folded into ONE 'hstab' row after the loop
            if hpos is None: hpos=len(rows)
            hcalls.append(x)
        elif tg==0x3b310:
            # critical-section handle init (core/critsec_3b310): 0x3b310(obj) ; zero-store run ; __cxa_atexit(dtor, obj) -- see the 'crit' branch of the atexit handler
            assert isinstance(r['rdi'],int), ('critical section object unresolved',hex(x['addr']))
            pend_crit=dict(call=x['addr'],obj=r['rdi'])
        elif tg==0xc300:
            o=r['rdi']; nm=x['view_rsi']
            rows.append(dict(kind='float',obj=o,name=cstr_at(*nm),name_ptr=nm[0],len=nm[1],desc=x.get('descview'),flags=r['r9'],
               lo=x['f_rcx'],hi=x['f_r8'],tag=x.get('tag'),value=ab(o+0xd0,4),default=ab(o+0x68,4),vptr=ab(o,8),value_ref=r['rdx']))
        elif tg==0xc580:
            o=r['rdi']; nm=x['view_rsi']
            rows.append(dict(kind='int',obj=o,name=cstr_at(*nm),name_ptr=nm[0],len=nm[1],desc=x.get('descview'),flags=r['r9'],
               lo=x['f_rcx'],hi=x['f_r8'],tag=x.get('tag'),value=ab(o+0xd0,4),default=ab(o+0x68,4),vptr=ab(o,8),value_ref=r['rdx']))
        elif tg==0x26db0:
            pv=x['view_rsi']
            rows.append(dict(kind='tog',obj=r['rdi'],parent=cstr_at(*pv),parent_len=pv[1],target=r['rdx'],flags=r['rcx']))
        elif tg==0x26ea0:
            pv=x['view_rsi']; tv=x['view_rdx']
            rows.append(dict(kind='toge',obj=r['rdi'],parent=cstr_at(*pv),parent_len=pv[1],text=cstr_at(*tv),text_len=tv[1],target=r['rcx'],flags=r['r8']))
        elif tg==0x27050:
            pv=x['view_rsi']; tv=x['view_rdx']
            def fl(v):
                if v and v[0]=='f': return struct.unpack('<I',e.raw(v[1],4))[0]
                return None
            rows.append(dict(kind='slider50',obj=r['rdi'],parent=cstr_at(*pv),parent_len=pv[1],text=cstr_at(*tv),text_len=tv[1],target=r['rcx'],flags=r['r8'],lo=fl(x['xmm0']),hi=fl(x['xmm1']),lo_addr=x['xmm0'][1] if x['xmm0'] else None,hi_addr=x['xmm1'][1] if x['xmm1'] else None,call=x['addr']))
        elif tg==0x27120:
            pv=x['view_rsi']
            def fl(v):
                if v and v[0]=='f': return struct.unpack('<I',e.raw(v[1],4))[0]
                return None
            rows.append(dict(kind='slider',obj=r['rdi'],parent=cstr_at(*pv),parent_len=pv[1],target=r['rdx'],flags=r['rcx'],lo=fl(x['xmm0']),hi=fl(x['xmm1']),lo_addr=x['xmm0'][1] if x['xmm0'] else None,hi_addr=x['xmm1'][1] if x['xmm1'] else None))
        elif tg==0x26f90:
            pv=x['view_rsi']; cv=x['view_rdx']; mv=x['view_r8']
            assert pv and cv and mv and None not in (pv[0],pv[1],cv[0],cv[1],mv[0],mv[1]), ('command item views unresolved',hex(x['addr']))
            assert isinstance(r['rcx'],int) and isinstance(r['r9'],int), ('command item target / flags unresolved',hex(x['addr']))
            rows.append(dict(kind='cmdi',obj=r['rdi'],parent=cstr_at(*pv),parent_len=pv[1],caption=cstr_at(*cv),caption_len=cv[1],command=cstr_at(*mv),command_len=mv[1],target=r['rcx'],flags=r['r9'],call=x['addr']))
        elif tg==0x1f220:
            # ring / vector header leaf (helper_1f220_ring_header): 0x1f220(obj) ; __cxa_atexit(0x354470, obj) -- see the 'ringhdr' branch of the atexit handler
            assert isinstance(r['rdi'],int), ('ring header object unresolved',hex(x['addr']))
            pend_ring=dict(call=x['addr'],obj=r['rdi'])
        elif tg==0x26b60:
            # debug-menu separator item (kind 3, builds "MenuSeparator" through the allocator): 0x26b60(obj, &parent, 0, flags) ; f590(parent) ; atexit(0x272d0, obj)
            pv=x['view_rsi']; assert pv and pv[0] and pv[1] is not None and r['rdx']==0 and isinstance(r['rcx'],int), ('separator arguments unresolved',hex(x['addr']))
            rows.append(dict(kind='sep',obj=r['rdi'],parent=cstr_at(*pv),parent_len=pv[1],flags=r['rcx'],call=x['addr']))
        elif tg==0x26800:
            # debug-menu label item (batch 20, slots 501 / 497; kind 0 after a transient 3, builds "MenuSeparator" through the allocator): 0x26800(obj, &label, 0, flags) ; f590(label) ; atexit(0x272d0, obj)
            pv=x['view_rsi']; assert pv and pv[0] and pv[1] is not None and r['rdx']==0 and isinstance(r['rcx'],int), ('label item arguments unresolved',hex(x['addr']))
            rows.append(dict(kind='sep',fn=0x26800,obj=r['rdi'],parent=cstr_at(*pv),parent_len=pv[1],flags=r['rcx'],call=x['addr']))
        elif tg==0x26c10:
            pv=x['view_rsi']; cv=x['view_rdx']
            rows.append(dict(kind='group',obj=r['rdi'],parent=cstr_at(*pv),parent_len=pv[1],caption=cstr_at(*cv),caption_len=cv[1],flags=r['rcx']))
        elif tg in (0x6aa00,0x3a980,0x140b00,0x25ab0):
            o=r['rdi']; nm=x['view_rsi']; ds=x['view_r9']
            urow=dict(kind='ufloat' if tg==0x6aa00 else 'uint',twin={0x140b00:'a',0x25ab0:'b'}.get(tg),obj=o,name=cstr_at(*nm),len=nm[1],flags=r['rcx'],desc=ds,tag=x['tag'],value_ref=r['rdx'],early=None)
            rows.append(urow)
        elif tg==0x257a0:
            # string variable (0x257a0 -> 0x258f0): name view and initial view are plain borrowed stack views (aux = ctl = 0), released by two f590 after the call
            o=r['rdi']; nm=x['view_rsi']; iv=x['view_rdx']
            if iv is None and r['rdx'] is None and pv is not None and pv['t']==0xf730:
                # batch 20 (slots 501 / 497): the initial view is the RETURN VALUE of 0xf730 (the function-local static empty string, never released): the only instructions between
                # the call and this call that write rax / rdx must be exactly `movq %rax, %rdx` (rax still the 0xf730 result), so rdx == &static empty string.
                between=[q for q in INS if pv['addr']<q['addr']<x['addr']]
                wr=[q for q in between if q['ops'].split('#')[0].replace(' ','').split(',')[-1] in ('%rax','%eax','%ax','%al','%rdx','%edx','%dx','%dl') and q['mnem'] not in ('cmpq','testq','cmpl','testl')]
                assert [(q['mnem'],q['ops'].split('#')[0].replace(' ','')) for q in wr]==[('movq','%rax,%rdx')], ('empty-view string variable: rdx is not the plain 0xf730 result',hex(x['addr']),wr)
                assert nm and nm[0] and nm[1] is not None, ('string variable name view unresolved',hex(x['addr']))
                assert x['v4_rsi'][2:]==(0,0), ('string variable name view with aux/ctl',hex(x['addr']))
                rows.append(dict(kind='str',obj=o,name=cstr_at(*nm),name_ptr=nm[0],len=nm[1],init=b'',init_ptr=0,init_len=0,init_empty=True,f730=pv['addr'],call=x['addr']))
                continue
            assert nm and nm[0] and nm[1] is not None and iv and iv[0] is not None and iv[1] is not None, ('string variable views unresolved',hex(x['addr']))
            assert x['v4_rsi'][2:]==(0,0) and x['v4_rdx'][2:]==(0,0), ('string variable view with aux/ctl',hex(x['addr']))
            rows.append(dict(kind='str',obj=o,name=cstr_at(*nm),name_ptr=nm[0],len=nm[1],init=cstr_at(*iv),init_ptr=iv[0],init_len=iv[1],call=x['addr']))
        elif tg==0x17070:
            # empty tree header: 0x17070(0x30) -> node; node self-linked; see the 'tree' branch of the atexit handler
            assert isinstance(r['rdi'],int) and r['rdi'] in (0x30,0x38,0x48,0x50,424), ('tree node allocation size',r['rdi'])
            pend_tree=dict(call=x['addr'],size=r['rdi'])
        elif tg==0x56b4b0:
            assert isinstance(r['rdi'],int), ('node-ref object unresolved',hex(x['addr']))
            pend_nref=dict(call=x['addr'],obj=r['rdi'])
        elif tg==0xf590:
            lastrel=x['view_rdi']
        elif tg==0xf8f630:
            # strncpy(obj+4, "NULL", 15) of the 0x14 byte null-name record: [obj] = 0 before the call, byte +0x13 = 0 after it; no atexit record
            assert isinstance(r['rdi'],int) and isinstance(r['rsi'],int) and r['rdx']==15, ('strncpy arguments unresolved',hex(x['addr']))
            o=r['rdi']-4
            assert cstr_z(r['rsi'])==b'NULL', 'strncpy source literal'
            assert ab(o,4)==0 and ab(o+0x13,1)==0, ('null-name record stores',hex(o))
            ins_=[q for q in INS if q['addr']<=x['addr']]
            lea_=[q for q in ins_ if q['mnem']=='leaq' and q['rip']==o]; assert len(lea_)==1, 'null-name record lea'
            _mb=next(j_ for j_,q in enumerate(INS) if q['addr']>x['addr'] and q['mnem']=='movb' and q['ops'].replace(' ','').startswith('$0x0,0x13(')); endaddr=INS[_mb+1]['addr']
            seq=[q for q in INS if lea_[0]['addr']<=q['addr']<endaddr]
            rows.append(dict(kind='ncell',obj=o,src=r['rsi'],seq_start=seq[0]['addr'],seq_end=endaddr,seq_bytes=e.raw(seq[0]['addr'],endaddr-seq[0]['addr']).hex(),call=x['addr']))
        elif tg==0x52a990:
            assert x['cell'], ('no cell store before the hash call', hex(x['addr']))
            rows.append(dict(kind='cell',obj=x['cell'][0],name=cstr_at(r['rdi'],len(cstr_z(r['rdi']))),name_ptr=r['rdi'],hash_ok=True))
        elif tg==0xf8f390:
            atexit.append((r['rdi'],r['rsi']))
            if r['rdi'] in (0x1c8990,0x48a250,0x3d9b40):
                assert recs and r['rsi']==recs[0]['obj'] and [q['obj'] for q in recs]==[recs[0]['obj']+0x38*k for k in range(len(recs))], ('record group is not one contiguous 0x38 stride block',[hex(q['obj']) for q in recs])
                ins_=disasm(r['rdi'],fde(r['rdi']))
                offs=[int(q['ops'].split('(')[0],16) for q in ins_ if q['mnem']=='leaq' and '(%rbx)' in q['ops']]
                tg_=[int(q['ops'].split()[0],16) for q in ins_ if q['mnem'] in('callq','jmp','jmpq')]
                assert offs==[0x38*k for k in range(len(recs)-1,0,-1)] and tg_==[0xa81950]*len(recs) and ins_[-1]['mnem'] in('jmp','jmpq'), (hex(r['rdi'])+' does not unregister exactly the block records, last first',offs,tg_)
                rows.append(dict(kind='recs',obj=recs[0]['obj'],dtor=r['rdi'],recs=list(recs),call=x['addr']))
                recs.clear()
            elif r['rdi']==ENTRY_DTOR:
                rows.append(_entry_row(xi,x,r,ev,INS))
            elif r['rdi']==0x1c5090:
                # zero-filled container: [+8, +0x18) and [+0x18, +0x20) zeroed just before the atexit call
                o=r['rsi']; assert isinstance(o,int), ('zero container object unresolved',hex(x['addr']))
                prevat=max([y['addr'] for y in ev[:xi] if not isinstance(y,tuple) and y['t']==0xf8f390]+[0])
                run_=[q for q in INS if prevat<q['addr']<=x['addr']]
                st=[q for q in run_ if q['mnem'].startswith('lea') and q['rip']==o]; assert len(st)==1, ('zero container object lea',hex(o))
                seq=[q for q in run_ if q['addr']>=st[0]['addr']]
                mn=[q['mnem'] for q in seq]; ops_=[q['ops'].split('#')[0].replace(' ','') for q in seq]
                assert mn==['leaq','leaq','vxorps','movq','vmovups','movq','callq'] and ops_[2:6]==['%xmm0,%xmm0,%xmm0','%r13,%rdx','%xmm0,0x8(%rsi)','$0x0,0x18(%rsi)'], ('zero container store run',mn,ops_)
                endaddr=x['addr']+5
                rows.append(dict(kind='zcont',obj=o,dtor=r['rdi'],seq_start=seq[0]['addr'],seq_end=endaddr,seq_bytes=e.raw(seq[0]['addr'],endaddr-seq[0]['addr']).hex(),call=x['addr']))
            elif r['rdi'] in (0x499e50,0x499e60):
                o=r['rsi']; assert pend_crit and pend_crit['obj']==o, ('critical section atexit object is not the initialised object',hex(x['addr']))
                _lea=[q for q in INS if q['addr']<pend_crit['call'] and q['mnem']=='leaq' and q['rip']==o]; assert _lea, 'critical section object lea'
                endaddr=x['addr']+5
                seq=[q for q in INS if _lea[-1]['addr']<=q['addr']<endaddr]
                size_={0x499e50:0xd8,0x499e60:0x30}[r['rdi']]
                st=sorted(set((m_[1]-o,m_[2],m_[3]) for m_ in t.mem if m_[0]=='abs' and o<=m_[1]<o+size_))
                assert st and all(isinstance(v_,int) and m_+s_<=size_ for m_,s_,v_ in st), ('critical section store run unresolved',hex(o),st)
                assert all(off_>=8 for off_,_,_ in st), ('a store reaches the handle cell +0 (written only by 0x3b310)',st)
                rows.append(dict(kind='crit',obj=o,dtor=r['rdi'],size=size_,stores=st,init_call=pend_crit['call'],seq_start=seq[0]['addr'],seq_end=endaddr,seq_bytes=e.raw(seq[0]['addr'],endaddr-seq[0]['addr']).hex(),call=x['addr']))
                pend_crit=None
            elif r['rdi'] in (0x565e80,0x499e90,0x52a010):
                # zero pointer slot: lea obj ; lea dtor ; mov rdx ; movq $0,(%rsi) ; call atexit  (the dtor frees the pointee and re-zeroes the slot)
                o=r['rsi']; assert isinstance(o,int), ('zero pointer object unresolved',hex(x['addr']))
                prevat=max([y['addr'] for y in ev[:xi] if not isinstance(y,tuple) and y['t']==0xf8f390]+[0])
                run_=[q for q in INS if prevat<q['addr']<=x['addr']]
                st=[q for q in run_ if q['mnem'].startswith('lea') and q['rip']==o]; assert len(st)==1, ('zero pointer object lea',hex(o))
                seq=[q for q in run_ if q['addr']>=st[0]['addr']]
                mn=[q['mnem'] for q in seq]; ops_=[q['ops'].split('#')[0].replace(' ','') for q in seq]
                assert mn==['leaq','leaq','movq','movq','callq'] and ops_[3]=='$0x0,(%rsi)', ('zero pointer store run',mn,ops_)
                endaddr=x['addr']+5
                rows.append(dict(kind='zq',obj=o,dtor=r['rdi'],seq_start=seq[0]['addr'],seq_end=endaddr,seq_bytes=e.raw(seq[0]['addr'],endaddr-seq[0]['addr']).hex(),call=x['addr']))
            elif r['rdi']==0xa81950:
                # single named record (rec1): 0xa81700(obj, &name, kind, flag) ; f590(name) ; __cxa_atexit(0xa81950, obj) -- the record's own unregister, no group atexit
                assert len(recs)==1 and recs[0]['obj']==r['rsi'], ('single record atexit object is not the one pending record',hex(r['rsi']),[hex(q['obj']) for q in recs])
                rows.append(dict(kind='rec1',obj=recs[0]['obj'],dtor=r['rdi'],rec=dict(recs[0]),call=x['addr']))
                recs.clear()
            elif r['rdi']==0x534580:
                # zero 3-qword cell: lea obj ; lea dtor ; vxorps ; mov rdx ; mov r13 ; vmovups xmm0,(obj) ; movq $0,0x10(obj) ; call atexit (the dtor frees [obj] and keeps the cell)
                o=r['rsi']; assert isinstance(o,int), ('zero cell object unresolved',hex(x['addr']))
                prevat=max([y['addr'] for y in ev[:xi] if not isinstance(y,tuple) and y['t']==0xf8f390]+[0])
                run_=[q for q in INS if prevat<q['addr']<=x['addr']]
                st=[q for q in run_ if q['mnem'].startswith('lea') and q['rip']==o]; assert len(st)==1, ('zero cell object lea',hex(o))
                seq=[q for q in run_ if q['addr']>=st[0]['addr']]
                mn=[q['mnem'] for q in seq]; ops_=[q['ops'].split('#')[0].replace(' ','') for q in seq]
                assert mn==['leaq','leaq','vxorps','movq','movq','vmovups','movq','callq'] and ops_[5]=='%xmm0,(%rsi)' and ops_[6]=='$0x0,0x10(%rsi)', ('zero cell store run',mn,ops_)
                endaddr=x['addr']+5
                rows.append(dict(kind='zcell',words=3,obj=o,dtor=r['rdi'],seq_start=seq[0]['addr'],seq_end=endaddr,seq_bytes=e.raw(seq[0]['addr'],endaddr-seq[0]['addr']).hex(),call=x['addr']))
            elif r['rdi']==0x13a0d0:
                # zero 1-qword cell: lea obj ; lea dtor ; mov rdx ; movq $0,(obj) ; call atexit (the dtor deletes through the pointer and CLEARS the cell)
                o=r['rsi']; assert isinstance(o,int), ('zero cell object unresolved',hex(x['addr']))
                prevat=max([y['addr'] for y in ev[:xi] if not isinstance(y,tuple) and y['t']==0xf8f390]+[0])
                run_=[q for q in INS if prevat<q['addr']<=x['addr']]
                st=[q for q in run_ if q['mnem'].startswith('lea') and q['rip']==o]; assert len(st)==1, ('zero cell object lea',hex(o))
                seq=[q for q in run_ if q['addr']>=st[0]['addr']]
                mn=[q['mnem'] for q in seq]; ops_=[q['ops'].split('#')[0].replace(' ','') for q in seq]
                assert mn==['leaq','leaq','movq','movq','callq'] and ops_[3]=='$0x0,(%rsi)', ('zero cell1 store run',mn,ops_)
                endaddr=x['addr']+5
                rows.append(dict(kind='zcell',words=1,obj=o,dtor=r['rdi'],seq_start=seq[0]['addr'],seq_end=endaddr,seq_bytes=e.raw(seq[0]['addr'],endaddr-seq[0]['addr']).hex(),call=x['addr']))
            elif r['rdi'] in ZVEC_DTORS:
                # zero-header vector: begin / end / cap (obj+8 .. obj+0x20) zeroed by one 16 B store and one qword store, in the run just before the atexit call
                o=r['rsi']; assert isinstance(o,int), ('zero vector object unresolved',hex(x['addr']))
                prevat=max([y['addr'] for y in ev[:xi] if not isinstance(y,tuple) and y['t']==0xf8f390]+[0])
                run_=[q for q in INS if prevat+5<=q['addr']<x['addr']]
                canon=[]
                for q in run_:
                    ops_=q['ops'].split('#')[0].replace(' ','')
                    if q['mnem'].startswith('lea') and q['rip']==o: canon.append('obj')
                    elif q['mnem'].startswith('lea') and q['rip']==r['rdi']: canon.append('dtor')
                    elif q['mnem'] in('vxorps','vpxor') and ops_=='%xmm0,%xmm0,%xmm0': canon.append('zero')
                    elif q['mnem']=='movq' and ops_ in('%r12,%rdx','%r13,%rdx','%r14,%rdx','%r15,%rdx','%rbx,%rdx'): canon.append('rdx')
                    elif q['mnem'] in('vmovups','vmovdqu') and ops_ in('%xmm0,0x8(%rsi)',) : canon.append('st8')
                    elif q['mnem'] in('vmovups','vmovdqu') and q['rip']==o+8 and ops_.startswith('%xmm0,'): canon.append('st8')
                    elif q['mnem']=='movq' and ops_=='$0x0,0x18(%rsi)': canon.append('st18')
                    elif q['mnem']=='movq' and q['rip']==o+0x18 and ops_.startswith('$0x0,'): canon.append('st18')
                    else: canon.append('?'+q['mnem']+' '+ops_)
                assert sorted(canon)==sorted(['obj','dtor','zero','rdx','st8','st18']) and canon.index('zero')<canon.index('st8'), ('zero vector store run',canon)
                endaddr=x['addr']+5
                rows.append(dict(kind='zvec',obj=o,dtor=r['rdi'],seq_start=run_[0]['addr'],seq_end=endaddr,seq_bytes=e.raw(run_[0]['addr'],endaddr-run_[0]['addr']).hex(),call=x['addr']))
            elif r['rdi'] in SENT_DTORS:
                # sentinel container: [0]=[8]=[0x20]=obj+0x28, [0x10]=obj+0xa8 (self-referential, written by the instruction run just before the atexit call)
                o=r['rsi']; assert isinstance(o,int), ('sentinel container object unresolved',hex(x['addr']))
                prevat=max([q for q in [y['addr'] for y in ev[:xi] if not isinstance(y,tuple) and y['t']==0xf8f390]]+[0])
                run_=[q for q in INS if prevat<q['addr']<=x['addr']]
                st=[q for q in run_ if q['mnem'].startswith('lea') and q['rip']==o]; assert len(st)==1, ('sentinel object lea',hex(o))
                seq=[q for q in run_ if q['addr']>=st[0]['addr']]
                shape=[(q['mnem'],q['ops'].split(',')[0] if q['mnem'] in ('movq','vmovq','vmovdqu') else None) for q in seq]
                mn=[q['mnem'] for q in seq]
                assert mn==['leaq','leaq','movq','leaq','vmovq','movq','leaq','vpshufd','vmovdqu','movq','callq'], ('sentinel store run shape',mn)
                ops_=[q['ops'].split('#')[0].replace(' ','') for q in seq]
                assert ops_[3:10]==['0x28(%rsi),%rax','%rax,%xmm0','%rax,0x20(%rsi)','0xa8(%rsi),%rax','$0x44,%xmm0,%xmm0','%xmm0,(%rsi)','%rax,0x10(%rsi)'], ('sentinel store operands',ops_)
                endaddr=x['addr']+5
                rows.append(dict(kind='sent',obj=o,dtor=r['rdi'],seq_start=seq[0]['addr'],seq_end=endaddr,seq_bytes=e.raw(seq[0]['addr'],endaddr-seq[0]['addr']).hex(),call=x['addr']))
            elif r['rdi'] in (0x6918e0,0x1d6210):
                # empty tree header: [obj+8] = [obj+0x10] zeroed, node = 0x17070(0x30) with left = parent = right = node, [node+0x18] = [node+0x19] = 1, [obj+8] = node, then the atexit call
                o=r['rsi']; assert isinstance(o,int) and pend_tree, ('tree object / allocation unresolved',hex(x['addr']))
                st=[q for q in INS if q['addr']<pend_tree['call'] and q['mnem']=='movl' and q['ops'].replace(' ','')==f"${pend_tree['size']:#x},%edi"]; assert st, 'tree size immediate'; st=[st[-1]]
                seq=[q for q in INS if st[0]['addr']<=q['addr']<=x['addr']]
                # register-based header stores (slot 511): the object lea and the zeroing vmovups precede `movl $size,%edi`; the run then starts at that lea
                _lea=[q for q in INS if q['addr']<st[0]['addr'] and q['mnem']=='leaq' and q['rip']==o and q['ops'].split(',')[-1].strip().split()[0] in ('%rbx','%r12','%r13','%r14','%r15')]
                regtree=False
                if _lea and not any(q['mnem']=='vmovups' and q['rip']==o+8 for q in seq):
                    _l=_lea[-1]; _reg=_l['ops'].split(',')[-1].strip().split()[0]
                    seq=[q for q in INS if _l['addr']<=q['addr']<=x['addr']]
                    _pre=[q['ops'].split('#')[0].replace(' ','') for q in seq if q['addr']<st[0]['addr']]
                    assert _pre[1:]==['%xmm0,%xmm0,%xmm0'] and any(q['mnem']=='vmovups' and q['ops'].replace(' ','')=='%xmm0,0x8('+_reg+')' for q in seq), ('register tree header prelude',_pre)
                    regtree=_reg
                mn=[q['mnem'] for q in seq]; ops_=[q['ops'].split('#')[0].replace(' ','') for q in seq]
                if regtree: assert any(q['mnem']=='movl' and q['ops'].replace(' ','')==f"${pend_tree['size']:#x},%edi" for q in seq)
                else: assert pend_tree['call'] in [q['addr'] for q in seq] and ops_[0]==f"${pend_tree['size']:#x},%edi", ('tree store run',mn)
                tail=[(q['mnem'],o_) for q,o_ in zip(seq,ops_) if q['addr']>pend_tree['call']]
                assert [o_ for _,o_ in tail if 'rax' in o_ and '(%rax)' in o_]==['%rax,(%rax)','%rax,0x8(%rax)','%rax,0x10(%rax)','$0x1,0x18(%rax)','$0x1,0x19(%rax)'], ('tree node stores',tail)
                assert (regtree and any(q['mnem']=='movq' and q['ops'].replace(' ','')=='%rax,0x8('+regtree+')' for q in seq)) or (any(q['mnem']=='vmovups' and q['rip']==o+8 for q in seq) and any(q['mnem']=='movq' and q['rip']==o+8 and q['ops'].startswith('%rax') for q in seq)), ('tree head stores',hex(o))
                endaddr=x['addr']+5
                rows.append(dict(kind='tree',obj=o,dtor=r['rdi'],seq_start=seq[0]['addr'],seq_end=endaddr,seq_bytes=e.raw(seq[0]['addr'],endaddr-seq[0]['addr']).hex(),call=x['addr'],alloc_call=pend_tree['call'],node_size=pend_tree['size']))
                pend_tree=None
            elif r['rdi']==0x354470:
                o=r['rsi']; assert pend_ring and pend_ring['obj']==o, ('ring atexit object is not the header object',hex(x['addr']))
                _lea=[q for q in INS if q['addr']<pend_ring['call'] and q['mnem']=='leaq' and q['rip']==o]; assert len(_lea)==1, 'ring header object lea'
                endaddr=x['addr']+5
                seq=[q for q in INS if _lea[0]['addr']<=q['addr']<endaddr]
                rows.append(dict(kind='ringhdr',obj=o,dtor=r['rdi'],hdr_call=pend_ring['call'],seq_start=seq[0]['addr'],seq_end=endaddr,seq_bytes=e.raw(seq[0]['addr'],endaddr-seq[0]['addr']).hex(),call=x['addr']))
                pend_ring=None
            elif r['rdi']==0x560540:
                o=r['rsi']; assert pend_nref and pend_nref['obj']==o, ('node-ref atexit object is not the constructed object',hex(x['addr']))
                rows.append(dict(kind='nref',obj=o,dtor=r['rdi'],call=x['addr'],ctor_call=pend_nref['call']))
                pend_nref=None
            elif r['rdi']==0x54dbb0:
                rows.append(_store_table_row(xi,x,r,ev,INS))
            elif r['rdi'] in HOLDER_DTORS:
                o=r['rsi']; nm=lastrel
                rows.append(dict(kind='holder',dtor=r['rdi'],obj=o,name=cstr_at(*nm),len=nm[1],vptr=ab(o,8),name_ptr=nm[0],tail=ab(o+0x28,8)))
    if hcalls:
        rows.insert(hpos,_hash_store_row(ev,hcalls,INS))
    if gets:
        # table of lo / hi getter pairs stored as 12 byte {lo, hi, size} records (helper_498_member_hashes): every instruction between the first call and the last store is pinned
        assert len(gets)%2==0 and all(gets[2*k+1]['t']==gets[2*k]['t']+0x10 for k in range(len(gets)//2)), 'getter pairs'
        npair=len(gets)//2; a0=gets[0]['addr']
        cells=[(q['rip'],q['ops'].split(',')[0].strip()) for q in INS if q['addr']>=a0 and q['mnem']=='movl' and q['rip'] is not None and (q['ops'].split(',')[0].strip() in('%ebx','%eax') or q['ops'].split(',')[0].strip().startswith('$'))]
        assert len(cells)==3*npair, ('record stores',len(cells),npair)
        base_=cells[0][0]
        for k_ in range(npair):
            c3=cells[3*k_:3*k_+3]
            assert [c[0] for c in c3]==[base_+12*k_,base_+12*k_+4,base_+12*k_+8] and c3[0][1]=='%ebx' and c3[1][1]=='%eax' and c3[2][1].startswith('$'), ('record store order',k_,c3)
        sizes_=[int(cells[3*k_+2][1][1:],16) for k_ in range(npair)]
        laststore=[q for q in INS if q['addr']>=a0 and q['mnem']=='movl' and q['rip']==base_+12*(npair-1)+8][0]
        endaddr=INS[INS.index(laststore)+1]['addr']
        callsites=[gets[2*k_]['addr'] for k_ in range(npair)]
        assert all(c==gets[2*k_]['addr'] for k_,c in enumerate(callsites))
        lo_=[gets[2*k_]['t'] for k_ in range(npair)]; hi_=[gets[2*k_+1]['t'] for k_ in range(npair)]
        val=lambda r_: int.from_bytes(e.raw(r_+1,4),'little')
        rows.insert(getpos,dict(kind='gettable',obj=base_,n=npair,lo_rva=lo_,hi_rva=hi_,call_site=callsites,lo=[val(r_) for r_ in lo_],hi=[val(r_) for r_ in hi_],size=sizes_,
                                seq_start=a0,seq_end=endaddr,seq_bytes=e.raw(a0,endaddr-a0).hex()))
    for rw in rows:
        if rw['kind'] in ('ufloat','uint'):
            rw['value']=ab(rw['obj']+0xd0,4); rw['default']=ab(rw['obj']+0x68,4); rw['vptr']=ab(rw['obj'],8)
            if rw['kind']=='uint' and rw.get('twin')=='b':
                # twin B (0x25ab0, slot 566): the 0xd0 B object keeps its value in an EXTERNAL dword at obj-8; the site never writes it (no value store) and loads it into [obj+0x68]
                cell=rw['value_ref']
                assert isinstance(cell,int) and cell==rw['obj']-8 and rw['value'] is None and rw['default']==('mut',cell,4), ('twin B external value cell facts',hex(rw['obj']),rw['value'],rw['default'])
                try: img=int.from_bytes(e.raw(cell,4),'little'); rw['cell_backed']=True
                except Exception: img=0; rw['cell_backed']=False   # .bss: no initial image (zero before any writer runs)
                rw['cell']=cell; rw['cell_image']=img; rw['value']=rw['default']=img
                continue
            if isinstance(rw['value'],tuple) and rw['value'][0]=='mut':
                # initial value = a 4-byte load of a global at run time (stored to both +0xd0 and +0x68): a source address, not a constant of the body
                assert rw['kind']=='ufloat' and rw['value']==rw['default'], ('run-time loaded initial value is not stored identically to +0xd0 and +0x68', hex(rw['obj']))
                rw['src']=rw['value'][1]
                try: rw['src_image']=int.from_bytes(e.raw(rw['src'],4),'little'); rw['src_backed']=True
                except Exception: rw['src_image']=0; rw['src_backed']=False   # .bss: no initial image (zero before any writer runs)
                rw['value']=rw['default']=rw['src_image']
    account(idx,rows,ev,atexit)
    return rows,atexit,t

# ---- ELF-tie row accounting: the call/atexit event stream of the whole body must be EXACTLY the concatenation of the per-row call
# grammars below. A row the tracker does not recognise (or recognises wrongly) leaves events over -> hard error, never a silent drop.
_TOK={0x20af0:'B',0x20d60:'R',0xf590:'f',0xf8f390:'A',0xf2f0:'N',0xf730:'E',0xc300:'F',0xc580:'I',0x6aa00:'U',0x3a980:'W',0x26db0:'t',0x26ea0:'e',0x27120:'s',0x27050:'S',0x26c10:'g',0x26f90:'x',0x52a990:'H',0x257a0:'V',0xa81700:'Q',0x17070:'M',0x56b4b0:'K',0x274730:'Y',0x140b00:'D',0x25ab0:'G',0xf8f630:'P',0x1580a0:'Z'}
def _pattern(r):
    k=r['kind']
    if k=='ienum': return 'BfRfA'
    if k=='bool':
        assert r['early'] is not None, ('bool without a release order',hex(r['obj']))
        return 'BfRfA' if r['early'] else 'BRffA'
    if k=='float': return 'FfiRfA'
    if k in ('ufloat','uint'):
        assert r['early'] is not None, ('row without a release order',k,hex(r['obj']))
        return ('U' if k=='ufloat' else {None:'W','a':'D','b':'G'}[r.get('twin')])+('fiRfA' if r['early'] else 'iRffA')
    if k=='int': return 'IfiRfA'
    if k=='dwr': return '<%x>iRffA'%r['tgt']
    if k=='crit': return '<3b310>A'
    if k=='lint': return ('E<%x>ifA' if r['desc_text'] is None else '<%x>fifA')%r['wrapper']
    if k=='boolx': return 'YifA'
    if k=='tog': return 'tfA'
    if k=='toge': return 'effA'
    if k=='group': return 'gffA'
    if k=='sep': return '<%x>fA'%r.get('fn',0x26b60)
    if k=='gettable': return ''.join('<%x><%x>'%(a,b) for a,b in zip(r['lo_rva'],r['hi_rva']))
    if k=='ringhdr': return '<1f220>A'
    if k=='cmdi': return 'xfffA'
    if k=='slider': return 'sfA'
    if k=='slider50': return 'SffA'
    if k=='cell': return 'H'
    if k=='ncell': return 'P'
    if k=='holder': return 'fA'
    if k in('sent','zcont','zq','zvec','zcell'): return 'A'
    if k=='entry': return 'NN<3be2d0>ffA'
    if k=='rec1': return 'QfA'
    if k=='pre': return 'Z'
    if k=='tree': return 'MA'
    if k=='nref': return 'KA'
    if k=='recs': return 'Qf'*len(r['recs'])+'A'
    if k=='cmd': return 'NBiffA'
    if k=='stab': return 'A'
    if k=='hstab': return '<a2ddc0>'*r['n']
    if k=='str': return 'EVfA' if r.get('init_empty') else 'VffA'
    raise AssertionError(('no call grammar for row kind',k))
def _tokens(ev):
    out=[]
    for x in ev:
        if isinstance(x,tuple): out.append('i'); continue
        if x['t']==0xf8f3a0: continue   # __stack_chk_fail on the failure path
        out.append(_TOK.get(x['t'],'<%x>'%x['t']))
    return ''.join(out)
def account(idx,rows,ev,atexit):
    got=_tokens(ev); want=''.join(_pattern(r) for r in rows)
    if got!=want:
        n=next((i for i,(a,b) in enumerate(zip(got,want)) if a!=b),min(len(got),len(want)))
        raise AssertionError('slot %d row accounting failed (rows would silently drop calls): first mismatch at token %d; events %r vs rows %r'%(idx,n,got[max(0,n-12):n+24],want[max(0,n-12):n+24]))
    # every atexit record belongs to a row (one per non-cell row)
    assert len(atexit)==sum(1 for r in rows if r['kind'] not in ('cell','ncell','pre','gettable','hstab')), ('atexit count',len(atexit))
if __name__=='__main__':
    rows,at,t=rows_of(int(sys.argv[1]))
    for r in rows: print({k:(hex(v) if isinstance(v,int) and not isinstance(v,bool) and v>9 else v) for k,v in r.items()})
    print(len(at))
