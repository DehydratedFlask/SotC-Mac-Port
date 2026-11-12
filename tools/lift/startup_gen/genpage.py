"""Generate native header/cpp/test + checker for a debug-page startup slot from the ELF (via track.py)."""
import sys, struct, hashlib, re
import pathlib as _pl
_HERE=str(_pl.Path(__file__).resolve().parent)
_LIFT=str(_pl.Path(__file__).resolve().parents[1])
_ROOT=str(_pl.Path(__file__).resolve().parents[3])+'/'
sys.path.insert(0,_HERE)
from page import *
from mkchecker import write_checker
ROOT=_ROOT
SZ={'bool':0xc8,'boolx':0xc8,'zq':8,'int':0xd8,'ienum':0xd8,'lint':0xd8,'uint':0xd8,'float':0xd8,'ufloat':0xd8,'cmd':0xa0,'tog':0xa8,'toge':0xa8,'slider':0xa8,'slider50':0xa8,'group':0xa8,'cmdi':0xa8,'cell':0x10,'ncell':0x14,'holder':0x30,'str':0x148,'sent':0xa8,'recs':0,'zcont':0x20,'tree':0x18,'nref':0x38,'pre':0x40,'zvec':0x20,'sep':0xa8,'ringhdr':0x2628,'gettable':0x120,'dwr':0xd8,'crit':0,'rec1':0x38,'zcell':0,'stab':0,'hstab':0,'entry':0x90}
def twinb(r): return r['kind']=='uint' and r.get('twin')=='b'
def szof(r): return 0xd0 if twinb(r) else 0x38*len(r['recs']) if r['kind']=='recs' else r['size'] if r['kind'] in('crit','stab','hstab') else 8*r['words'] if r['kind']=='zcell' else SZ[r['kind']]   # twin B: the real object is only 0xd0 B (its value is the external dword at obj-8)
ITEMS=('tog','toge','slider','slider50','group','cmdi','sep')
TY={'bool':'ConCmdBoolVar','boolx':'ConCmdBoolVar','zq':'std::uint64_t','int':'ConCmdRangedVar','ienum':'ConCmdRangedVar','lint':'ConCmdRangedVar','uint':'ConCmdRangedVar','float':'ConCmdRangedVar','ufloat':'ConCmdRangedVar','cmd':'ConCmdCommand','tog':'DebugMenuItem','toge':'DebugMenuItem','slider':'DebugMenuItem','slider50':'DebugMenuItem','group':'DebugMenuItem','cmdi':'DebugMenuItem','cell':'NameHashCell','ncell':'NullNameCell247f47','holder':'NameHolder30','str':'ConCmdStringVar','sent':'@SENT@','recs':'@RECS@','zcont':'@ZCONT@','tree':'EmptyTreeHeader','nref':'NodeRef56b4b0','pre':'@PRE@','zvec':'@ZCONT@','sep':'DebugMenuItem','ringhdr':'@RING@','gettable':'@GT@','dwr':'ConCmdRangedVar','crit':'@CRIT@','rec1':'NamedRecord','zcell':'@ZCELL@','stab':'@STAB@','hstab':'@HST@','entry':'@ENTRY@'}
def cs(b): return b.decode('latin1')
def lit(b):
    s=cs(b)
    assert all(32<=ord(c)<127 and c not in '"\\' for c in s), s
    return '"'+s+'"'
def cond(text,hb,hf,hu,hi=False,hm=True,hl=False,hw=False,hc=False,hd=False,hv=False,hq=False,hx=False,hy=False):
    out=[]
    for ln in text.split('\n'):
        if ln.startswith('@B '):
            if hb: out.append(ln[3:])
        elif ln.startswith('@F '):
            if hf: out.append(ln[3:])
        elif ln.startswith('@U '):
            if hu: out.append(ln[3:])
        elif ln.startswith('@I '):
            if hi: out.append(ln[3:])
        elif ln.startswith('@M '):
            if hm: out.append(ln[3:])
        elif ln.startswith('@N '):
            if not hm: out.append(ln[3:])
        elif ln.startswith('@L '):
            if hl: out.append(ln[3:])
        elif ln.startswith('@W '):
            if hw: out.append(ln[3:])
        elif ln.startswith('@C '):
            if hc: out.append(ln[3:])
        elif ln.startswith('@D '):
            if hd: out.append(ln[3:])
        elif ln.startswith('@V '):
            if hv: out.append(ln[3:])
        elif ln.startswith('@X '):
            if hx: out.append(ln[3:])
        elif ln.startswith('@X'):
            if hx: out.append(ln[2:])
        elif ln.startswith('@Q '):
            if hq: out.append(ln[3:])
        elif ln.startswith('@Y '):
            if hy: out.append(ln[3:])
        else: out.append(ln)
    return '\n'.join(out)
def gen_page(n, what, theme_note, doc_extra=''):
    rva=e.u64(g.TABLE_TOP-8*n); size=fde(rva)
    stem=f'ctor_slot_{n}_{rva:x}'; S=f'Slot{n}'
    rows,atexit,t=rows_of(n)
    R=len(rows)
    # objects by address
    byaddr=sorted(range(R),key=lambda i:rows[i]['obj'])
    for a,b in zip(byaddr,byaddr[1:]):
        assert rows[a]['obj']+szof(rows[a])<=rows[b]['obj'], ('overlap',a,b)
    assert len({r['obj'] for r in rows})==R
    obj0=rows[byaddr[0]]['obj']
    npre=sum(1 for r in rows if r['kind']=='pre')
    nsep=sum(1 for r in rows if r['kind']=='sep')
    base0=obj0&~7   # layout base: the window starts at an 8-byte boundary so that every row keeps its natural alignment
    objmap={r['obj']:i for i,r in enumerate(rows)}
    bools=[i for i,r in enumerate(rows) if r['kind']=='bool']; bx=[i for i,r in enumerate(rows) if r['kind']=='boolx']; bxidx={i:k for k,i in enumerate(bx)}; floats=[i for i,r in enumerate(rows) if r['kind']=='float']
    ufl=[i for i,r in enumerate(rows) if r['kind']=='ufloat']; uidx={i:k for k,i in enumerate(ufl)}
    ints=[i for i,r in enumerate(rows) if r['kind']=='int']; iidx={i:k for k,i in enumerate(ints)}
    ienums=[i for i,r in enumerate(rows) if r['kind']=='ienum']; eidx={i:k for k,i in enumerate(ienums)}
    lints=[i for i,r in enumerate(rows) if r['kind']=='lint']; lidx={i:k for k,i in enumerate(lints)}
    uints=[i for i,r in enumerate(rows) if r['kind']=='uint']; widx={i:k for k,i in enumerate(uints)}
    cmds=[i for i,r in enumerate(rows) if r['kind']=='cmd']; cidx={i:k for k,i in enumerate(cmds)}
    dwrs=[i for i,r in enumerate(rows) if r['kind']=='dwr']; didx={i:k for k,i in enumerate(dwrs)}
    crits=[i for i,r in enumerate(rows) if r['kind']=='crit']; critidx={i:k for k,i in enumerate(crits)}
    strv=[i for i,r in enumerate(rows) if r['kind']=='str']; zidx={i:k for k,i in enumerate(strv)}
    # per-site facts of the local-int wrapper calls, resolved against the native family table (the checker re-proves them from the ELF)
    fam={}
    _txt=open(ROOT+'native/core/helper_620e10_local_int_var.hpp').read().split('kLocalIntVarClasses{{',1)[1].split('}};',1)[0]
    for m_ in re.finditer(r'\{(0x[0-9a-f]+), (0x[0-9a-f]+), (0x[0-9a-f]+), (0x[0-9a-f]+), (0x[0-9a-f]+), (0x[0-9a-f]+), (0x[0-9a-f]+), (0x[0-9a-f]+)\}',_txt):
        v_=[int(x,16) for x in m_.groups()]; fam[v_[0]]=dict(inner=v_[1],mid=v_[2],fn=v_[3],final=v_[4],dtor=v_[5])
    _th=open(ROOT+'native/core/helper_620e10_local_int_var.hpp').read().split('kLocalIntCallerTwins{{',1)[1].split('}};',1)[0]
    TWINS={(int(a,16),int(b,16)) for a,b in re.findall(r'\{(0x[0-9a-f]+), (0x[0-9a-f]+)\}',_th)}
    _ins=disasm(rva,size)
    lsites=[]
    for i in lints:
        r_=rows[i]; f_=fam[r_['wrapper']]
        twin=(r_['tag'],f_['fn']) in TWINS
        assert (r_['tag']==f_['fn'] or twin) and r_['dtor']==f_['dtor'] and r_['flags'] in (1,8), ('per-site tag proof failed',hex(r_['wrapper']),hex(r_['tag']),hex(f_['fn']))
        tl=[x for x in _ins if x['addr']<r_['call'] and x['mnem'].startswith('lea') and x['rip']==r_['tag']]
        dl=[x for x in _ins if x['addr']<r_['atexit_call'] and x['mnem'].startswith('lea') and x['rip']==r_['dtor']]
        ol=[x for x in _ins if x['addr']<r_['call'] and x['mnem'].startswith('lea') and x['rip']==r_['obj']]
        assert tl and dl and ol and all(x['len']==7 for x in (tl[-1],dl[-1],ol[-1])), 'site operand leas not found'
        lsites.append(dict(wrapper=r_['wrapper'],own=f_['fn'],call=r_['call'],ind=r_['ind'],atexit_call=r_['atexit_call'],f730=r_['f730'],desc_f590=r_.get('desc_f590'),tag=r_['tag'],tag_lea=tl[-1]['addr'],dtor=r_['dtor'],dtor_lea=dl[-1]['addr'],obj=r_['obj'],obj_lea=ol[-1]['addr']))
    for i in lints:
        r_=rows[i]
        assert (r_['dtor'],r_['obj']) in [a for a in atexit], 'atexit record of the site'
    LSITES=lsites
    itags={rows[i]['tag'] for i in ints}; assert len(itags)<=1, itags
    itag=next(iter(itags)) if itags else 0
    assert all(rows[i]['flags']==1 and rows[i]['value']==rows[i]['default'] and rows[i]['vptr']==0x1594c08 and rows[i]['desc'][1]==0 for i in ints)
    TWSEED={'a':0x15983c8,'b':0x1595100}
    twinu=[i for i in uints if rows[i].get('twin')]; plainu=[i for i in uints if not rows[i].get('twin')]; hastwin=bool(twinu)
    assert all(rows[i]['tag']==TWSEED[rows[i]['twin']] for i in twinu), 'twin uint: the argument functor is the twin\'s own seed class'
    assert not (any(rows[i]['twin']=='a' for i in twinu) and any(rows[i]['twin']=='b' for i in twinu)), 'twin A and twin B in one slot: one set of twin constants per slot'
    twb=[i for i in twinu if rows[i]['twin']=='b']; hasB=bool(twb)
    assert all(rows[i]['kind']=='uint' and rows[i]['vptr']==0x1595098 and rows[i]['cell']==rows[i]['obj']-8 for i in twb), 'twin B site facts'
    wtags={rows[i]['tag'] for i in plainu}; assert len(wtags)<=1, wtags
    wtag=next(iter(wtags)) if wtags else 0
    assert all(rows[i]['flags']==1 and rows[i]['value']==rows[i]['default'] and (rows[i]['vptr']==0x1594c08 or rows[i].get('twin')) and rows[i]['value_ref']==(rows[i]['obj']-8 if rows[i].get('twin')=='b' else rows[i]['obj']+0xd0) and rows[i]['desc'][1]==len(cstr_at(*rows[i]['desc'])) and rows[i]['early'] is not None for i in uints), 'uint site facts'
    assert len({rows[i]['vptr'] for i in twinu})<=1, 'one twin final class per slot'
    hasud=any(rows[i]['desc'][1] for i in uints)
    atx_by_obj={o_:d_ for d_,o_ in atexit}
    assert all(atx_by_obj[rows[i]['obj']]==({'a':0x13a090,'b':0x23d20}[rows[i]['twin']] if rows[i].get('twin') else 0x18a60) for i in uints), 'uint atexit destructor'
    assert all(rows[i]['flags']==2 and rows[i]['handler'] and rows[i]['len']==len(rows[i]['name']) and rows[i]['desc'][1]==len(rows[i]['desc_text']) for i in cmds), 'cmd site facts'
    assert all(rows[i]['name'] and rows[i]['len']==len(rows[i]['name']) and rows[i]['init_len']==len(rows[i]['init']) for i in strv), 'string variable facts'
    _ecls={(rows[i]['mid'],rows[i]['vptr'],rows[i]['tag']) for i in ienums}; assert len(_ecls)<=1, ('one enum class per slot',_ecls)
    emid,efinal,etag=next(iter(_ecls)) if _ecls else (0,0,0)
    _edt={rows[i]['dtor'] for i in ienums}; assert len(_edt)<=1, ('one enum dtor per slot',_edt); edtor=next(iter(_edt)) if _edt else 0x60d5d0
    _ebd={rows[i]['bounded'] for i in ienums}; assert len(_ebd)<=1, 'bounded and unbounded enums in one slot'; ebounded=next(iter(_ebd)) if _ebd else True
    assert all(rows[i]['flags']==1 and rows[i]['desc'][1]==len(rows[i]['desc_text']) and rows[i]['desc'][1]>0 and rows[i]['tag']==(0x15bfce8 if rows[i]['variant']=='a' else 0x15bfd80) and rows[i]['dtor'] in (0x498b20,0x498b60) for i in dwrs), 'dword-ranged site facts'
    assert all(len(rows[i]['stores'])>0 and all(sz<=8 or v==0 for (_o,sz,v) in rows[i]['stores']) for i in crits), 'critical-section store facts'
    assert all(rows[i]['value']==rows[i]['dflt'] for i in ienums), 'enum value / default'
    allvars=sorted(bools+bx+floats+ufl+ints+ienums+lints+uints+cmds+strv+dwrs)
    utags={rows[i]['tag'] for i in ufl}; assert len(utags)<=1; utag=next(iter(utags)) if utags else 0
    assert all(rows[i]['value'] is not None and rows[i]['default'] is not None for i in ufl), ('unbounded float whose initial value is not a constant of the body and not a 4-byte global load: not modelled', [hex(rows[i]['obj']) for i in ufl if rows[i]['value'] is None])
    # Run-time loaded initial values (a 4-byte load of a global the body never wrote): modelled as an explicit INPUT field per distinct source address;
    # the default is the pinned ELF's initial image of that global (zero for .bss), NOT a claim about the value at the moment the original runs.
    srcs=[]
    for i in ufl:
        if 'src' in rows[i] and rows[i]['src'] not in [q[0] for q in srcs]: srcs.append((rows[i]['src'],rows[i]['src_image'],rows[i]['src_backed']))
    srcidx={i:[q[0] for q in srcs].index(rows[i]['src']) if 'src' in rows[i] else -1 for i in ufl}
    assert all(rows[i]['flags']==1 and rows[i]['value']==rows[i]['default'] and rows[i]['vptr']==0x15942b0 and rows[i]['early'] is not None for i in ufl)
    bidx={i:k for k,i in enumerate(bools)}; fidx={i:k for k,i in enumerate(floats)}
    btags={rows[i]['tag'] for i in bools}; ftags={rows[i]['tag'] for i in floats}
    assert len(btags)<=1 and len(ftags)<=1, (btags,ftags)
    btag=next(iter(btags)) if btags else 0; ftag=next(iter(ftags)) if ftags else 0
    assert all(rows[i]['flags']==1 for i in bools+floats)
    # out-of-line bool variables (0x274730): per-site facts; q[3] != 0 is the self-reference rule (q[0] is the block's own address, rewritten into the object)
    assert all(rows[i]['initial'] in (0,1) and rows[i]['q'][2]!=0 and rows[i]['len']==len(rows[i]['name']) for i in bx), '0x274730 site facts'
    bxtags=sorted({rows[i]['tag'] for i in bx})
    assert all(rows[i]['value']==rows[i]['default'] for i in bools+floats)
    assert all(rows[i]['desc'][1]==0 for i in floats)
    assert all(rows[i]['desc'][1]==len(cstr_at(*rows[i]['desc'])) for i in bools), 'bool description length'
    assert all(r['flags']==0x30 for r in rows if r['kind'] in ITEMS and r['kind']!='group' and not (r['kind']=='sep' and r.get('fn')==0x26800)), 'item flags'
    hcl=[]
    for r in rows:
        if r['kind']=='holder' and (r['vptr'],r['dtor']) not in hcl: hcl.append((r['vptr'],r['dtor']))
    assert len(hcl)<=4 and all(r['tail']==0 for r in rows if r['kind']=='holder'), hcl
    hvptr=hcl[0][0] if hcl else 0
    HSFX=['','B','C','D']
    assert all(rows[i]['vptr'] in(0x1594218,0x15942b0) for i in bools+floats)
    # slider targets that are not objects of this slot (built by another slot): identity pointers the host supplies
    exts=[]
    for r in rows:
        if r['kind'] in('tog','toge','slider','slider50','cmdi') and r['target'] not in objmap:
            assert r['kind'] in('slider50','cmdi'), ('external target of a non-0x27050 item',r['kind'],hex(r['target']))
            if r['target'] not in exts: exts.append(r['target'])
    # menu strings table
    nzv_pre=sum(1 for r in rows if r['kind']=='zvec')
    strs=[]
    def sid(b):
        if b not in strs: strs.append(b)
        return strs.index(b)
    ents=[i for i,r in enumerate(rows) if r['kind']=='entry']; entidx={i:k for k,i in enumerate(ents)}; nent=len(ents)
    r1idx={i:k for k,i in enumerate(i_ for i_,r_ in enumerate(rows) if r_['kind']=='rec1')}
    rowinfo=[]; ZVD_idx=list(range(nzv_pre))
    for i,r in enumerate(rows):
        k=r['kind']; d={'kind':k}
        if k=='bool': d['v']=bidx[i]; d['sd']=sid(cstr_at(*rows[i]['desc'])) if rows[i]['desc'][1] else -1
        elif k=='boolx': d['v']=bxidx[i]
        elif k=='int': d['v']=iidx[i]
        elif k=='ienum': d['v']=eidx[i]
        elif k=='lint': d['v']=lidx[i]
        elif k=='uint': d['v']=widx[i]; d['sd']=sid(cstr_at(*rows[i]['desc'])) if rows[i]['desc'][1] else -1
        elif k=='dwr': d['v']=didx[i]; d['sd']=sid(r['desc_text'])
        elif k=='crit': d['v']=critidx[i]
        elif k=='rec1': d['v']=r1idx[i]
        elif k=='zcell': pass
        elif k=='entry': d['v']=entidx[i]
        elif k=='cmd': d['v']=cidx[i]; d['sd']=sid(r['desc_text'])
        elif k=='str': d['v']=zidx[i]; d['s0']=-1 if r.get('init_empty') else sid(r['init'])
        elif k=='float': d['v']=fidx[i]
        elif k=='ufloat': d['v']=uidx[i]; d['sd']=sid(cstr_at(*rows[i]['desc'])) if rows[i]['desc'][1] else None
        elif k=='zvec': d['v']=ZVD_idx.pop(0)
        elif k in('sent','recs','zcont','tree','nref','zq','ncell','pre','ringhdr','gettable','stab','hstab'): pass
        elif k in('cell','holder'):
            d['s0']=sid(r['name']); assert k=='cell' or len(r['name'])==r['len']
            if k=='holder': d['v']=hcl.index((r['vptr'],r['dtor']))
        else:
            d['s0']=sid(r['parent']); assert len(r['parent'])==r['parent_len']
            if k=='sep' and r.get('fn')==0x26800: d['v']=r['flags']   # label item (0x26800): the per-row flags travel in Row::var
            if k in('tog','toge','slider','slider50'):
                if r['target'] in objmap:
                    tg=objmap[r['target']]; d['t']=tg; assert rows[tg]['kind'] in('bool','boolx','float','ufloat','lint','int','ienum','uint','cmd','str'), rows[tg]['kind']
                    assert k!='slider' or rows[tg]['kind'] in('float','ufloat'), 'slider target'
                else: d['t']=-1; d['x']=exts.index(r['target'])
            if k in('toge','slider50'): d['s1']=sid(r['text']); assert len(r['text'])==r['text_len']
            if k=='group': d['s1']=sid(r['caption']); assert len(r['caption'])==r['caption_len']; d['v']=r['flags']   # per-row group flags travel in Row::var
            if k=='cmdi':
                assert r['target'] not in objmap, 'command item target is an object of this slot'
                d['t']=-1; d['x']=exts.index(r['target']); d['s1']=sid(r['caption']); d['v']=sid(r['command'])
                assert len(r['caption'])==r['caption_len'] and len(r['command'])==r['command_len']
        rowinfo.append(d)
    sliders=[i for i,r in enumerate(rows) if r['kind'] in('slider','slider50')]
    sidx={i:k for k,i in enumerate(sliders)}
    firstitem=next((i for i,r in enumerate(rows) if r['kind'] in ITEMS),-1)
    menu_less=firstitem<0
    MENUP='' if menu_less else ', DebugMenuRegistry& menu'
    MENUA='' if menu_less else ', menu'
    PRESET='        o.perspective_tanf = &test_tanf;\n' if npre else ''
    if allvars:
        PENDCHK=f'CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r{allvars[0]:02d}.base.pending);'
        FAILBLK=('// First variable registration fails: nothing after it is reached and nothing is queued.\n'
          '        ConCmdRegistry creg;\n        int token = 0;\n        creg.manager.manager = &token;\n'
          '@M         DebugMenuRegistry menu;\n'
          f'        {S}Objects o;\n{PRESET}@@RECSET@@@@TREESET@@        const {S}Result r = init_slot{n}_{rva:x}(o, creg{MENUA});\n'
          f'        CHECK(!r.ok && r.rows_done == {allvars[0]});\n'
          '@M         CHECK(menu.root.count == 0 && creg.pending.size() == 0);\n'
          '@N         CHECK(creg.pending.size() == 0);')
    else:
        # cells-only page: no console variable is registered, so nothing can queue and there is no registration to fail
        PENDCHK='CHECK(vars == 0 && creg.pending.size() == 0);'
        FAILBLK=('// Cells-only page: no variable registration exists; the rows are name-hash stores (no manager contract is read).\n'
          '        ConCmdRegistry creg;\n'
          f'        {S}Objects o;\n@@RECSET@@        const {S}Result r = init_slot{n}_{rva:x}(o, creg{MENUA});\n'
          '        CHECK(r.ok && creg.pending.size() == 0);')
    nitems=sum(1 for r in rows if r['kind'] in ITEMS); ncells=sum(1 for r in rows if r['kind'] in ('cell','ncell','pre','gettable','hstab')); nzv=sum(1 for r in rows if r['kind']=='zvec'); ZVD=[r['dtor'] for r in rows if r['kind']=='zvec']; PRE=next((r for r in rows if r['kind']=='pre'),None); nreal=sum(1 for r in rows if r['kind']=='cell'); nnc=sum(1 for r in rows if r['kind']=='ncell'); nhold=sum(1 for r in rows if r['kind']=='holder'); nsent=sum(1 for r in rows if r['kind']=='sent'); nring=sum(1 for r in rows if r['kind']=='ringhdr'); nsep=sum(1 for r in rows if r['kind']=='sep')
    nstab=sum(1 for r in rows if r['kind']=='stab'); STAB=next((r for r in rows if r['kind']=='stab'),None); assert nstab<=1, 'one store table per slot'
    nhst=sum(1 for r in rows if r['kind']=='hstab'); HST=next((r for r in rows if r['kind']=='hstab'),None); assert nhst<=1, 'one hash-store table per slot'
    ngt=sum(1 for r in rows if r['kind']=='gettable'); GT=next((r for r in rows if r['kind']=='gettable'),None); assert ngt<=1, 'one getter table per slot'
    assert nring<=1 and all(r['dtor']==0x354470 for r in rows if r['kind']=='ringhdr') and all(r['flags']==0x30 for r in rows if r['kind']=='sep' and r.get('fn',0x26b60)==0x26b60), 'ring header / separator facts'
    assert nsent<=1 and all(r['dtor'] in (0x3941b0,0x5b55c0) for r in rows if r['kind']=='sent')
    SENT_DTOR=next((r['dtor'] for r in rows if r['kind']=='sent'),0x3941b0)
    assert npre<=1 and all(r['flag']==1 for r in rows if r['kind']=='pre'), 'perspective row facts'
    assert all(r['dtor'] in (0x5b5530,0x5c7c00,0x52a0d0,0x52a140,0x52a1b0) for r in rows if r['kind']=='zvec'), 'zero-vector dtor'
    ntree=sum(1 for r in rows if r['kind']=='tree'); nnref=sum(1 for r in rows if r['kind']=='nref')
    _tds={r['dtor'] for r in rows if r['kind']=='tree'}; assert len(_tds)<=1 and _tds<={0x6918e0,0x1d6210}, ('one tree destructor per slot',_tds)
    TREE_DTOR=_tds.pop() if _tds else 0x6918e0
    assert nnref<=1 and all(r['dtor']==0x560540 for r in rows if r['kind']=='nref'), 'tree / node-ref facts'
    _tns={r['node_size'] for r in rows if r['kind']=='tree'}; assert len(_tns)<=1, _tns
    TREE_NODE=_tns.pop() if _tns else 0x30
    nzq=sum(1 for r in rows if r['kind']=='zq'); _zqd={r['dtor'] for r in rows if r['kind']=='zq'}; assert len(_zqd)<=1 and _zqd<={0x565e80,0x499e90,0x52a010}, ('one zero-pointer dtor per slot',_zqd)
    ZQ_DTOR=next(iter(_zqd)) if _zqd else 0x565e80
    nrecs=sum(1 for r in rows if r['kind']=='recs'); nzc=sum(1 for r in rows if r['kind']=='zcont')
    assert nrecs<=1 and nzc<=1 and all(r['dtor']==0x1c5090 for r in rows if r['kind']=='zcont')
    RECS=next((r for r in rows if r['kind']=='recs'),None); NREC=len(RECS['recs']) if RECS else 0
    rec1s=[i for i,r in enumerate(rows) if r['kind']=='rec1']; r1idx={i:k for k,i in enumerate(rec1s)}; nrec1=len(rec1s); nrr=nrecs+nrec1
    assert not (nrecs and nrec1), 'a slot with both a record group and single records is not modelled'
    assert all(rows[i]['dtor']==0xa81950 and rows[i]['rec']['len']==len(rows[i]['rec']['name']) for i in rec1s), 'single record facts'
    zcells=[i for i,r in enumerate(rows) if r['kind']=='zcell']; nzcell=len(zcells)
    _zcd={(rows[i]['dtor'],rows[i]['words']) for i in zcells}; assert len(_zcd)<=1 and _zcd<={(0x534580,3),(0x13a0d0,1)}, ('one zero-cell flavour per slot',_zcd)
    zcdtor,zcwords=next(iter(_zcd)) if _zcd else (0,0)
    ents=[i for i,r in enumerate(rows) if r['kind']=='entry']; entidx={i:k for k,i in enumerate(ents)}; nent=len(ents)
    assert all(rows[i]['dtor']==0x3c15e0 and len(rows[i]['fns'])==3 and rows[i]['name'] and rows[i]['desc'] for i in ents), 'registry entry facts'
    assert all(q['len']==len(q['name']) for q in (RECS['recs'] if RECS else [])), 'record name lengths'
    maxn=max([len(rows[i]['name']) for i in bools+bx+floats+ufl+ints+ienums+lints+uints+cmds+strv+dwrs]+[1])+1
    maxs=max([len(s) for s in strs]+[1])+1
    # ---------------- header
    H=[]
    STINC=('#include "store_table.hpp"\n' if nstab else '')+('#include "hash_store_table.hpp"\n#include "fnv_path_cstr_162020.hpp"\n' if nhst else '')
    DWINC=('#include "helper_49ca60_dword_var_site.hpp"\n' if dwrs else '')+('#include "critsec_3b310.hpp"\n' if crits else '')+('#include "helper_140b00_twin_site.hpp"\n' if hastwin else '')
    BXINC=('#include "rb_registry_3be2d0.hpp"\n#include "ctor_98a90_3be050.hpp"\n' if nent else '')+('#include "helper_274730_bool_var.hpp"\n' if bx else '')+('#include "helper_1580a0_perspective.hpp"\n' if npre else '')+('#include "helper_f8f630_strncpy.hpp"\n' if nnc else '')+('#include "helper_1f220_ring_header.hpp"\n' if nring else '')+('#include "helper_498_member_hashes.hpp"\n' if ngt else '')
    H.append(f'''// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot {n} (RVA {rva:#x}, {size} bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * {n}. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_{n}.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// {what}
// Rows, in constructor order ({R} objects at {obj0:#x}..): {len(bools)} bool variables (caller functor tag {btag:#x}, dtor 0x20b0), {len(floats)} BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag {ftag:#x}, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {{0.1, 0.5}}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// {theme_note}
//
// NOT established: what reads the variables at run time. Any reading of the names is INFERENCE only.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "ctor_slot_539_121940.hpp"   // NameHolder30
#include "string_var_257a0.hpp"
#include "startup_row_kinds.hpp"
#include "ctor_slot_family_items.hpp"
#include "fnv_cstr_161fb0.hpp"
#include "hashed_name_ctors.hpp"
#include "hashed_string_14c3a0.hpp"
#include "helper_620e10_local_int_var.hpp"
#include "helper_a81700_named_record.hpp"
#include "helper_int_var_sites.hpp"
#include "l2_slot_common.hpp"
#include "lane_m6_shapes.hpp"
{BXINC}{DWINC}{STINC}
namespace sotc {{
''')
    toks=[]
    def const(name,val,typ='std::uint64_t'):
        H.append(f'inline constexpr {typ} k{S}{name} = {val};'); 
    const('Rva',f'{rva:#x}'); const('Size',str(size),'std::size_t')
    const('BoolFnTag',f'{btag:#x}'); const('FloatFnTag',f'{ftag:#x}')
    const('ItemFlags','0x30','std::uint32_t'); const('VarFlags','1','std::uint32_t')
    const('UFloatFnTag',f'{utag:#x}'); const('UFloatCount',str(len(ufl)),'std::size_t'); const('BoolCount',str(len(bools)),'std::size_t'); const('FloatCount',str(len(floats)),'std::size_t'); const('RowCount',str(R),'std::size_t')
    const('StringCount',str(len(strs)),'std::size_t'); const('IntFnTag',f'{itag:#x}'); const('IntCount',str(len(ints)),'std::size_t'); const('LocalIntCount',str(len(lints)),'std::size_t'); const('SliderCount',str(len(sliders)),'std::size_t')
    if hastwin: const('UIntTwinFinalVptr',f'{next(rows[i]["vptr"] for i in twinu):#x}'); const('UIntTwinDtorRva','0x23d20' if hasB else '0x13a090')
    if hasB: const('UIntTwinSlot3Result','1','std::uint32_t'); const('UIntTwinSlot6Result','1','std::uint32_t')   # slots 3 / 6 of the final class are `mov eax, 1 ; ret` (pinned bodies)
    const('UIntFnTag',f'{wtag:#x}'); const('UIntCount',str(len(uints)),'std::size_t'); const('CmdCount',str(len(cmds)),'std::size_t'); const('StrVarCount',str(len(strv)),'std::size_t')
    const('Object0Rva',f'{obj0:#x}')
    if bx: const('BoolXCount',str(len(bx)),'std::size_t')
    if dwrs: const('DwrCount',str(len(dwrs)),'std::size_t')
    if crits: const('CritCount',str(len(crits)),'std::size_t'); const('CritStoreCount',str(sum(len(rows[i]['stores']) for i in crits)),'std::size_t')
    for hi,(hvp,hdt) in enumerate(hcl): const('HolderVptr'+HSFX[hi],f'{hvp:#x}'); const('HolderDtorRva'+HSFX[hi],f'{hdt:#x}')
    const('CellCount',str(ncells),'std::size_t'); const('HolderCount',str(nhold),'std::size_t'); const('SentinelCount',str(nsent),'std::size_t')
    if nsent: const('SentinelDtorRva',f'{SENT_DTOR:#x}')
    if ngt: const('MemberCount',str(GT['n']),'std::size_t')
    if nstab: const('StoreCount',str(len(STAB['stores'])),'std::size_t'); const('StoreTableSize',f'{STAB["size"]:#x}','std::size_t'); const('StoreTableRva',f'{STAB["obj"]:#x}'); const('StoreDtorRva',f'{STAB["dtor"]:#x}'); const('StoreStringCount',str(len(STAB['strs'])),'std::size_t'); const('StoreFirstInsnRva',f'{STAB["stores"][0]["addr"]:#x}'); const('StoreLastInsnRva',f'{STAB["stores"][-1]["addr"]:#x}')
    if nhst: const('HashStoreCount',str(HST['n']),'std::size_t'); const('HashTableSize',f'{HST["size"]:#x}','std::size_t'); const('HashTableRva',f'{HST["obj"]:#x}'); const('HashStringCount',str(len(HST['strs'])),'std::size_t'); const('HashFirstCallRva',f'{HST["stores"][0]["call"]:#x}'); const('HashLastStoreRva',f'{HST["stores"][-1]["addr"]:#x}')
    if nring: const('RingCount',str(nring),'std::size_t'); const('RingDtorRva','0x354470'); const('RingHeaderRva','0x1f220')
    if nrecs: const('RecsCount',str(nrecs),'std::size_t'); const('RecordCount',str(NREC),'std::size_t')
    if nzc: const('ZeroCount',str(nzc),'std::size_t')
    if nzv: const('ZVecCount',str(nzv),'std::size_t')
    if npre: const('PerspectiveFovBits',f'{PRE["fov"]:#x}','std::uint32_t'); const('PerspectiveAspectBits',f'{PRE["aspect"]:#x}','std::uint32_t'); const('PerspectiveNearBits',f'{PRE["near"]:#x}','std::uint32_t'); const('PerspectiveFarBits',f'{PRE["far"]:#x}','std::uint32_t'); const('PerspectiveFlag',str(PRE['flag']),'std::uint32_t')
    if nzq: const('ZeroPtrCount',str(nzq),'std::size_t'); const('ZeroPtrDtorRva',f'{ZQ_DTOR:#x}')
    if srcs: const('UFloatSourceCount',str(len(srcs)),'std::size_t')
    if ntree: const('TreeCount',str(ntree),'std::size_t'); const('TreeNodeSize',f'{TREE_NODE:#x}','std::size_t'); const('TreeDtorRva',f'{TREE_DTOR:#x}')
    if nnref: const('NodeRefCount',str(nnref),'std::size_t'); const('NodeRefDtorRva','0x560540')
    if nrecs: const('RecsDtorRva',hex(RECS['dtor']))
    if ienums: const('EnumCount',str(len(ienums)),'std::size_t'); const('EnumMidVptr',f'{emid:#x}'); const('EnumFinalVptr',f'{efinal:#x}'); const('EnumFnTag',f'{etag:#x}'); const('EnumDtorRva',f'{edtor:#x}'); const('EnumBounded','1' if ebounded else '0','std::uint32_t'); const('EnumSlot3Result','1','std::uint32_t'); const('EnumSlot6Result','1','std::uint32_t')
    if nzc: const('ZeroDtorRva','0x1c5090')
    if nrec1: const('Rec1Count',str(nrec1),'std::size_t'); const('Rec1DtorRva','0xa81950')
    if nent: const('EntryCount',str(nent),'std::size_t'); const('EntryDtorRva','0x3c15e0'); const('EntrySize','0x90','std::size_t'); const('EntryListHeadCellRva','0x2ef6400'); const('EntrySelfCellRva','0x2ef63f8')
    if nzcell: const('ZeroCellCount',str(nzcell),'std::size_t'); const('ZeroCellDtorRva',f'{zcdtor:#x}'); const('ZeroCellWords',str(zcwords),'std::size_t')
    tabs=[]
    def table(decl,vals,fmt):
        line=f'inline constexpr {decl} = {{'+', '.join(fmt(v) for v in vals)+'};'
        H.append(line); tabs.append(line.split(' = ',1)[0].split(' ',2)[-1] if False else decl)
        return line
    tline=[]
    if bools:
        H.append(f'inline constexpr char k{S}BoolNames[k{S}BoolCount][{maxn}] = {{'+', '.join(lit(rows[i]['name']) for i in bools)+'};')
        tline.append(table(f'std::size_t k{S}BoolNameLengths[k{S}BoolCount]',[rows[i]['len'] for i in bools],lambda v:f'{v:#x}'))
        tline.append(table(f'std::uint8_t k{S}BoolValues[k{S}BoolCount]',[rows[i]['value'] for i in bools],lambda v:str(v)))
        tline.append(table(f'std::int32_t k{S}BoolDesc[k{S}BoolCount]',[rowinfo[i]['sd'] for i in bools],lambda v:str(v)))
        tline.append(table(f'std::uint8_t k{S}BoolEarly[k{S}BoolCount]',[1 if rows[i]['early'] else 0 for i in bools],lambda v:str(v)))
    if bx:
        H.append('// Out-of-line bool variables (0x274730): the call site\'s name, initial byte, REAL argument-functor class tag (a bool-family table) and argument block {self, q1, q2, q3, payload, aux};')
        H.append('// q[0] is the block\'s own address (rewritten into the object by the self-reference rule), q[1] is a .text RVA read from a relocated global, q[3] a callback RVA. All identity-only.')
        _bl='inline constexpr BoolVar274730SiteSpec k%sBoolXSites[k%sBoolXCount] = {' % (S,S)+', '.join('{'+f'{lit(rows[i]["name"])}, {rows[i]["len"]:#x}, {rows[i]["initial"]}, {rows[i]["tag"]:#x}, {{0, {rows[i]["q"][0]:#x}, {rows[i]["q"][1]:#x}, {rows[i]["q"][2]:#x}}}, {rows[i]["payload"]:#x}, {rows[i]["aux"]:#x}'+'}' for i in bx)+'};'
        H.append(_bl); tline.append(_bl)
    if floats:
        H.append(f'inline constexpr char k{S}FloatNames[k{S}FloatCount][{maxn}] = {{'+', '.join(lit(rows[i]['name']) for i in floats)+'};')
        tline.append(table(f'std::size_t k{S}FloatNameLengths[k{S}FloatCount]',[rows[i]['len'] for i in floats],lambda v:f'{v:#x}'))
        tline.append(table(f'std::uint32_t k{S}FloatLoBits[k{S}FloatCount]',[rows[i]['lo'] for i in floats],lambda v:f'{v:#010x}'))
        tline.append(table(f'std::uint32_t k{S}FloatHiBits[k{S}FloatCount]',[rows[i]['hi'] for i in floats],lambda v:f'{v:#010x}'))
        tline.append(table(f'std::uint32_t k{S}FloatValueBits[k{S}FloatCount]',[rows[i]['value'] for i in floats],lambda v:f'{v:#010x}'))
    if ufl:
        H.append(f'inline constexpr char k{S}UFloatNames[k{S}UFloatCount][{maxn}] = {{'+', '.join(lit(rows[i]['name']) for i in ufl)+'};')
        tline.append(table(f'std::size_t k{S}UFloatNameLengths[k{S}UFloatCount]',[rows[i]['len'] for i in ufl],lambda v:f'{v:#x}'))
        tline.append(table(f'std::uint32_t k{S}UFloatValueBits[k{S}UFloatCount]',[rows[i]['value'] for i in ufl],lambda v:f'{v:#010x}'))
        if srcs:
            H.append('// Run-time loaded initial values: the original executes `mov eax, [global]` and stores it to the variable (+0xd0 and +0x68). The global is NOT a constant of the body, so the model takes it as an INPUT')
            H.append('// (Objects::source_bits[k]); the default is the pinned ELF initial image of that global (a .bss global has no image: zero). It is the host\'s job to supply the value in effect when the original constructor runs.')
            tline.append(table(f'std::int32_t k{S}UFloatSource[k{S}UFloatCount]',[srcidx[i] for i in ufl],lambda v:str(v)))
            tline.append(table(f'std::uint64_t k{S}SourceAddr[k{S}UFloatSourceCount]',[q[0] for q in srcs],lambda v:f'{v:#x}'))
            tline.append(table(f'std::uint32_t k{S}SourceImageBits[k{S}UFloatSourceCount]',[q[1] for q in srcs],lambda v:f'{v:#010x}'))
            tline.append(table(f'std::uint8_t k{S}SourceFileBacked[k{S}UFloatSourceCount]',[1 if q[2] else 0 for q in srcs],lambda v:str(v)))
        tline.append(table(f'std::uint8_t k{S}UFloatEarly[k{S}UFloatCount]',[1 if rows[i]['early'] else 0 for i in ufl],lambda v:str(v)))
        tline.append(table(f'std::int32_t k{S}UFloatDesc[k{S}UFloatCount]',[(rowinfo[i]['sd'] if rowinfo[i]['sd'] is not None else -1) for i in ufl],lambda v:str(v)))
    if ints:
        H.append(f'inline constexpr char k{S}IntNames[k{S}IntCount][{maxn}] = {{'+', '.join(lit(rows[i]['name']) for i in ints)+'};')
        tline.append(table(f'std::size_t k{S}IntNameLengths[k{S}IntCount]',[rows[i]['len'] for i in ints],lambda v:f'{v:#x}'))
        tline.append(table(f'std::int32_t k{S}IntMin[k{S}IntCount]',[rows[i]['lo'] for i in ints],lambda v:str(v)))
        tline.append(table(f'std::int32_t k{S}IntMax[k{S}IntCount]',[rows[i]['hi'] for i in ints],lambda v:str(v)))
        tline.append(table(f'std::int32_t k{S}IntValue[k{S}IntCount]',[rows[i]['value'] for i in ints],lambda v:str(v)))
    if ienums:
        H.append(f'inline constexpr char k{S}EnumNames[k{S}EnumCount][{maxn}] = {{'+', '.join(lit(rows[i]['name']) for i in ienums)+'};')
        tline.append(table(f'std::size_t k{S}EnumNameLengths[k{S}EnumCount]',[rows[i]['len'] for i in ienums],lambda v:f'{v:#x}'))
        tline.append(table(f'std::int32_t k{S}EnumMin[k{S}EnumCount]',[rows[i]['lo'] for i in ienums],lambda v:str(v)))
        tline.append(table(f'std::int32_t k{S}EnumMax[k{S}EnumCount]',[rows[i]['hi'] for i in ienums],lambda v:str(v)))
        tline.append(table(f'std::int32_t k{S}EnumValue[k{S}EnumCount]',[rows[i]['value'] for i in ienums],lambda v:str(v)))
    if uints:
        H.append(f'inline constexpr char k{S}UIntNames[k{S}UIntCount][{maxn}] = {{'+', '.join(lit(rows[i]['name']) for i in uints)+'};')
        tline.append(table(f'std::size_t k{S}UIntNameLengths[k{S}UIntCount]',[rows[i]['len'] for i in uints],lambda v:f'{v:#x}'))
        tline.append(table(f'std::int32_t k{S}UIntValue[k{S}UIntCount]',[rows[i]['value'] for i in uints],lambda v:str(v-(1<<32) if v>=(1<<31) else v)))
        tline.append(table(f'std::uint8_t k{S}UIntEarly[k{S}UIntCount]',[1 if rows[i]['early'] else 0 for i in uints],lambda v:str(v)))
        if hastwin: tline.append(table(f'std::uint8_t k{S}UIntTwin[k{S}UIntCount]',[1 if rows[i].get('twin') else 0 for i in uints],lambda v:str(v)))
        if hasud: tline.append(table(f'std::int32_t k{S}UIntDesc[k{S}UIntCount]',[rowinfo[i]['sd'] for i in uints],lambda v:str(v)))
    if dwrs:
        H.append('// Dword-ranged variables (0x49ca60 / 0x49cc60): per-site name, description (borrowed view), flags, the REAL argument-functor tag (the constructor\'s own class), the dword range, the initial value and default,')
        H.append('// the final vtable pointer (a literal lea of the site) and the atexit destructor (identity tag). Variant 0 = 0x49ca60, 1 = 0x49cc60.')
        H.append(f'inline constexpr char k{S}DwrNames[k{S}DwrCount][{maxn}] = {{'+', '.join(lit(rows[i]['name']) for i in dwrs)+'};')
        tline.append(table(f'std::size_t k{S}DwrNameLengths[k{S}DwrCount]',[rows[i]['len'] for i in dwrs],lambda v:f'{v:#x}'))
        tline.append(table(f'std::uint8_t k{S}DwrVariant[k{S}DwrCount]',[0 if rows[i]['variant']=='a' else 1 for i in dwrs],lambda v:str(v)))
        tline.append(table(f'std::int32_t k{S}DwrDesc[k{S}DwrCount]',[rowinfo[i]['sd'] for i in dwrs],lambda v:str(v)))
        tline.append(table(f'std::uint64_t k{S}DwrFnTag[k{S}DwrCount]',[rows[i]['tag'] for i in dwrs],lambda v:f'{v:#x}'))
        tline.append(table(f'std::int32_t k{S}DwrMin[k{S}DwrCount]',[rows[i]['lo'] for i in dwrs],lambda v:str(v)))
        tline.append(table(f'std::int32_t k{S}DwrMax[k{S}DwrCount]',[rows[i]['hi'] for i in dwrs],lambda v:str(v)))
        tline.append(table(f'std::int32_t k{S}DwrValue[k{S}DwrCount]',[rows[i]['value'] for i in dwrs],lambda v:str(v)))
        tline.append(table(f'std::int32_t k{S}DwrDefault[k{S}DwrCount]',[rows[i]['default'] for i in dwrs],lambda v:str(v)))
        tline.append(table(f'std::uint64_t k{S}DwrFinalVptr[k{S}DwrCount]',[rows[i]['vptr'] for i in dwrs],lambda v:f'{v:#x}'))
        tline.append(table(f'std::uint64_t k{S}DwrDtorRva[k{S}DwrCount]',[rows[i]['dtor'] for i in dwrs],lambda v:f'{v:#x}'))
    if crits:
        H.append('// Critical-section objects (0x3b310 at +0 -> the 8-byte opaque handle, then the observed field stores in order; 0x499e50 / 0x499e60 are atexit destructors, IDENTITY TAGS). Stores are flattened per row:')
        H.append('// offset, width and value (a store wider than 8 bytes is an all-zero vector store). Nothing else in the object is written.')
        _flat=[(o_,sz_,v_) for i in crits for (o_,sz_,v_) in rows[i]['stores']]
        _first=[0]
        for i in crits: _first.append(_first[-1]+len(rows[i]['stores']))
        tline.append(table(f'std::uint32_t k{S}CritSize[k{S}CritCount]',[rows[i]['size'] for i in crits],lambda v:f'{v:#x}'))
        tline.append(table(f'std::uint64_t k{S}CritDtorRva[k{S}CritCount]',[rows[i]['dtor'] for i in crits],lambda v:f'{v:#x}'))
        tline.append(table(f'std::size_t k{S}CritStoreFirst[k{S}CritCount + 1]',_first,lambda v:str(v)))
        tline.append(table(f'std::uint32_t k{S}CritStoreOffset[k{S}CritStoreCount]',[q[0] for q in _flat],lambda v:f'{v:#x}'))
        tline.append(table(f'std::uint32_t k{S}CritStoreWidth[k{S}CritStoreCount]',[q[1] for q in _flat],lambda v:str(v)))
        tline.append(table(f'std::uint64_t k{S}CritStoreValue[k{S}CritStoreCount]',[q[2] for q in _flat],lambda v:f'{v:#x}'))
    if ents:
        H.append('// Scripted-command registry entries (0x3c15e0 is the atexit destructor, an IDENTITY TAG): name, description, the three functor tags stored at +0x08 / +0x28 / +0x48 (relocated code addresses, never run).')
        maxe=max(len(rows[i]['name']) for i in ents)+1; maxd=max(len(rows[i]['desc']) for i in ents)+1
        H.append(f'inline constexpr char k{S}EntryNames[k{S}EntryCount][{maxe}] = {{'+', '.join(lit(rows[i]['name']) for i in ents)+'};')
        tline.append(table(f'std::size_t k{S}EntryNameLengths[k{S}EntryCount]',[len(rows[i]['name']) for i in ents],lambda v:f'{v:#x}'))
        H.append(f'inline constexpr char k{S}EntryDescs[k{S}EntryCount][{maxd}] = {{'+', '.join(lit(rows[i]['desc']) for i in ents)+'};')
        tline.append(table(f'std::size_t k{S}EntryDescLengths[k{S}EntryCount]',[len(rows[i]['desc']) for i in ents],lambda v:f'{v:#x}'))
        tline.append(table(f'std::uint64_t k{S}EntryFns[3 * k{S}EntryCount]',[f_ for i in ents for f_ in rows[i]['fns']],lambda v:f'{v:#x}'))
    if cmds:
        H.append(f'inline constexpr char k{S}CmdNames[k{S}CmdCount][{maxn}] = {{'+', '.join(lit(rows[i]['name']) for i in cmds)+'};')
        tline.append(table(f'std::size_t k{S}CmdNameLengths[k{S}CmdCount]',[rows[i]['len'] for i in cmds],lambda v:f'{v:#x}'))
        tline.append(table(f'std::int32_t k{S}CmdDesc[k{S}CmdCount]',[rowinfo[i]['sd'] for i in cmds],lambda v:str(v)))
        tline.append(table(f'std::uint64_t k{S}CmdHandler[k{S}CmdCount]',[rows[i]['handler'] for i in cmds],lambda v:f'{v:#x}'))
    if strv:
        H.append(f'inline constexpr char k{S}StrNames[k{S}StrVarCount][{maxn}] = {{'+', '.join(lit(rows[i]['name']) for i in strv)+'};')
        tline.append(table(f'std::size_t k{S}StrNameLengths[k{S}StrVarCount]',[rows[i]['len'] for i in strv],lambda v:f'{v:#x}'))
        tline.append(table(f'std::int32_t k{S}StrInit[k{S}StrVarCount]',[rowinfo[i]['s0'] for i in strv],lambda v:str(v)))
    if lints:
        H.append(f'// Local-int wrapper sites (0x620e10 family): wrapper RVA, name, init value, range, flags and the call site\'s own argument-functor tag; the description is the static empty string 0xf730 (never released).')
        _ld=[rows[i]['desc_text'] for i in lints]
        H.append(f'// Per-site description: nullptr = the static empty string 0xf730 (never released); a literal = a hand-built borrowed view that the site RELEASES between the wrapper and the functor destroy (the 7ec160 order, release_description = true).')
        H.append(f'inline constexpr const char* k{S}LintDesc[k{S}LocalIntCount] = {{'+', '.join('nullptr' if d is None else lit(d) for d in _ld)+'};')
        H.append(f'inline constexpr std::size_t k{S}LintDescLength[k{S}LocalIntCount] = {{'+', '.join('0' if d is None else str(len(d)) for d in _ld)+'};')
        H.append(f'inline constexpr LocalIntSiteSpec k{S}LocalIntSites[k{S}LocalIntCount] = {{'+', '.join('{'+f'{rows[i]["wrapper"]:#x}, {lit(rows[i]["name"])}, {rows[i]["len"]}, {rows[i]["value"]}, {rows[i]["lo"]}, {rows[i]["hi"]}, {rows[i]["flags"]}, {rows[i]["tag"]:#x}, '+('true' if rows[i]['desc_text'] is not None else 'false')+'}' for i in lints)+'};')
    if nrecs:
        H.append(f'// Named records (0xa81700 registrar, 0x38 B stride): name, kind (edx) and flag (cl) of each record, in constructor order. Names are borrowed views; the registrar copies the view and the block is unregistered by 0x1c8990 (identity tag).')
        H.append(f'inline constexpr char k{S}RecName[k{S}RecordCount][{max(len(q["name"]) for q in RECS["recs"])+1}] = {{'+', '.join(lit(q['name']) for q in RECS['recs'])+'};')
        tline.append(table(f'std::size_t k{S}RecNameLength[k{S}RecordCount]',[q['len'] for q in RECS['recs']],lambda v:f'{v:#x}'))
        tline.append(table(f'std::uint32_t k{S}RecKind[k{S}RecordCount]',[q['rkind'] for q in RECS['recs']],lambda v:str(v)))
        tline.append(table(f'std::uint8_t k{S}RecFlag[k{S}RecordCount]',[q['flag'] for q in RECS['recs']],lambda v:str(v)))
    if nrec1:
        H.append(f'// Single named records (0xa81700 registrar, one record per object, atexit 0xa81950 = the record\'s own unregister): name (borrowed view), kind (edx) and flag (cl) per row.')
        H.append(f'inline constexpr char k{S}Rec1Name[k{S}Rec1Count][{max(len(rows[i]["rec"]["name"]) for i in rec1s)+1}] = {{'+', '.join(lit(rows[i]['rec']['name']) for i in rec1s)+'};')
        tline.append(table(f'std::size_t k{S}Rec1NameLength[k{S}Rec1Count]',[rows[i]['rec']['len'] for i in rec1s],lambda v:f'{v:#x}'))
        tline.append(table(f'std::uint32_t k{S}Rec1Kind[k{S}Rec1Count]',[rows[i]['rec']['rkind'] for i in rec1s],lambda v:str(v)))
        tline.append(table(f'std::uint8_t k{S}Rec1Flag[k{S}Rec1Count]',[rows[i]['rec']['flag'] for i in rec1s],lambda v:str(v)))
    if nzv:
        tline.append(table(f'std::uint64_t k{S}ZVecDtorRva[k{S}ZVecCount]',ZVD,lambda v:f'{v:#x}'))
    if strs:
        H.append(f'inline constexpr char k{S}MenuStrings[k{S}StringCount][{maxs}] = {{'+', '.join(lit(s) for s in strs)+'};')
        tline.append(table(f'std::size_t k{S}MenuStringLengths[k{S}StringCount]',[len(s) for s in strs],lambda v:f'{v:#x}'))
    else:
        H.append(f'inline constexpr char k{S}MenuStrings[1][1] = {{""}};   // no strings in this slot')
        H.append(f'inline constexpr std::size_t k{S}MenuStringLengths[1] = {{0}};')
    if sliders:
        tline.append(table(f'std::uint32_t k{S}SliderLoBits[k{S}SliderCount]',[rows[i]['lo'] for i in sliders],lambda v:f'{v:#010x}'))
        tline.append(table(f'std::uint32_t k{S}SliderHiBits[k{S}SliderCount]',[rows[i]['hi'] for i in sliders],lambda v:f'{v:#010x}'))
    if ngt:
        tline.append(table(f'std::uint32_t k{S}MemberLoRva[k{S}MemberCount]',GT['lo_rva'],lambda v:f'{v:#x}'))
        tline.append(table(f'std::uint32_t k{S}MemberHiRva[k{S}MemberCount]',GT['hi_rva'],lambda v:f'{v:#x}'))
        tline.append(table(f'std::uint32_t k{S}MemberLo[k{S}MemberCount]',GT['lo'],lambda v:f'{v:#x}'))
        tline.append(table(f'std::uint32_t k{S}MemberHi[k{S}MemberCount]',GT['hi'],lambda v:f'{v:#x}'))
        tline.append(table(f'std::uint32_t k{S}MemberSize[k{S}MemberCount]',GT['size'],lambda v:f'{v:#x}'))
    # struct
    H.append('')
    if nsent:
        H.append(f'''// Sentinel container (0x3941b0 is its atexit destructor, an IDENTITY TAG). The original writes {{[0x00] = [0x08] = [0x20] = this + 0x28, [0x10] = this + 0xa8}} and nothing else:
// a self-referential header whose sentinel node starts at +0x28 and whose 0x80 B area ends at +0xa8 (the size and the element type are INFERENCE; +0x18 and the area are never written).
struct {S}Sentinel {{
    std::uintptr_t q00;   // = this + 0x28
    std::uintptr_t q08;   // = this + 0x28
    std::uintptr_t q10;   // = this + 0xa8
    std::uintptr_t q18;   // never written
    std::uintptr_t q20;   // = this + 0x28
    std::uint8_t area[0x80];   // never written
}};
static_assert(sizeof({S}Sentinel) == 0xa8);
''')
    if ngt:
        H.append(f'''// Member-hash block (helper_498_member_hashes): the original calls 24 pairs of six byte getters (`mov eax, imm32 ; ret`, lo at rva, hi at rva + 0x10) and stores {{lo, hi, size}} into 24 consecutive 12 byte records.
// What the hashes are is INFERENCE only; the model copies them and does not interpret them. The block is {GT["n"]*12:#x} bytes at {GT["obj"]:#x}.
struct {S}MemberBlock {{
    std::uint32_t dw[3 * k{S}MemberCount];
}};
static_assert(sizeof({S}MemberBlock) == 12 * k{S}MemberCount);
''')
    if nstab:
        def _esc(b): return b.decode('latin1').replace('\\','\\\\').replace('"','\\"')
        offs=[]; cur_=0
        for _v,_t in STAB['strs']: offs.append(cur_); cur_+=len(_t)+1
        H.append(f'''// Static store table (inference only: ~{STAB["size"]//STAB["stride"]} records of {STAB["stride"]:#x} B; the destructor {STAB["dtor"]:#x} walks them last to first and releases a pointer at +0x40 of each record, so the object is an
// array of records with a small-buffer callable at +0x20; the contents below are the OBSERVED stores, their meaning is NOT established). The original is {STAB["nins"]} straight-line instructions ({len(STAB["stores"])} stores) with no call
// between them (dword / qword immediates, qword register stores and 16 B vector stores built from vmovq / vpunpcklqdq / vpshufd of address registers). Bytes of the {STAB["size"]:#x} B object that no store covers are NEVER written.
// Each record is one store instruction (insn_rva = identity tag, never run). Lane kinds: 0 constant; 1 self pointer (the original stores its own address inside the object); 2 string
// (the original stores the address of a .rodata C string: the model stores a pointer to a NATIVE copy, the original address is kept in k{S}StoreStringRva); 3 tag (a function / vtable-like address, kept as the INTEGER, never followed).
struct {S}StoreTable {{
    alignas(8) std::uint8_t bytes[{STAB["size"]:#x}];
}};
static_assert(sizeof({S}StoreTable) == {STAB["size"]:#x});''')
        H.append(f'inline constexpr StoreTableRecord k{S}Stores[k{S}StoreCount] = {{')
        for st in STAB['stores']:
            l0=st['lanes'][0]; l1=st['lanes'][1] if len(st['lanes'])>1 else (0,0)
            H.append('    {'+f'{st["addr"]:#x}, {st["off"]:#x}, {st["width"]}, {l0[0]}, {l1[0]}, 0, {l0[1]:#x}, {l1[1]:#x}'+'},')
        H.append('};')
        H.append(f'inline constexpr std::uint32_t k{S}StoreStringRva[k{S}StoreStringCount] = {{'+', '.join(f'{_v:#x}' for _v,_t in STAB['strs'])+'};')
        H.append(f'inline constexpr std::uint32_t k{S}StoreStringOffset[k{S}StoreStringCount] = {{'+', '.join(f'{_o:#x}' for _o in offs)+'};')
        H.append(f'inline constexpr char k{S}StoreStringBlob[] =')
        H.append('\n'.join('    "'+_esc(_t)+'\\0"' for _v,_t in STAB['strs'])+';')
        H.append(f'static_assert(sizeof(k{S}StoreStringBlob) == {cur_ + 1});   // the pool plus the literal\'s own terminator\n')
    if nhst:
        def _esc2(b): return b.decode('latin1').replace('\\','\\\\').replace('"','\\"')
        offs2=[]; cur2=0
        for _v,_t in HST['strs']: offs2.append(cur2); cur2+=len(_t)+1
        H.append(f'''// Hash-to-dword store table (batch 20, inference only: UI option / button-code string ids). The original runs {HST["n"]} times `lea rdi, <rodata C string> ; call 0xa2ddc0 ; mov [rbx+d], eax` (0xa2ddc0 = the path-folding FNV hash,
// native fnv_cstr_a2ddc0); the {HST["size"]:#x} B window at {HST["obj"]:#x} is NOT one array: bytes between the written dwords are NEVER written. Each record is one site (call_rva / store_rva = identity tags, never run).
// The expected hash of every site is an INDEPENDENT oracle (Python port of 0x162020, ASCII upper-case policy) used by the fixture test only; the model computes the hash natively.
struct {S}HashTable {{
    alignas(4) std::uint8_t bytes[{HST["size"]:#x}];
}};
static_assert(sizeof({S}HashTable) == {HST["size"]:#x});
inline constexpr HashStoreRecord k{S}HashStores[k{S}HashStoreCount] = {{''')
        for st in HST['stores']: H.append('    {'+f'{st["call"]:#x}, {st["addr"]:#x}, {st["off"]:#x}, {st["sidx"]}'+'},')
        H.append('};')
        H.append(f'inline constexpr std::uint32_t k{S}HashStringRva[k{S}HashStringCount] = {{'+', '.join(f'{_v:#x}' for _v,_t in HST['strs'])+'};')
        H.append(f'inline constexpr std::uint32_t k{S}HashStringOffset[k{S}HashStringCount] = {{'+', '.join(f'{_o:#x}' for _o in offs2)+'};')
        H.append(f'inline constexpr std::uint32_t k{S}HashExpected[k{S}HashStoreCount] = {{'+', '.join(f'{_x:#x}' for _x in HST['expected'])+'};')
        H.append(f'inline constexpr char k{S}HashStringBlob[] =')
        H.append('\n'.join('    "'+_esc2(_t)+'\\0"' for _v,_t in HST['strs'])+';')
        H.append(f'static_assert(sizeof(k{S}HashStringBlob) == {cur2 + 1});   // the pool plus the literal\'s own terminator\n')
    if nring:
        H.append(f'''// Ring / vector header object (0x354470 is its atexit destructor, an IDENTITY TAG; helper_1f220_ring_header writes exactly four qwords: [+0x00] = [+0x08] = [+0x20] = this + 0x28 and [+0x10] = this + 0x2628).
// The 0x2600 B inline storage area, +0x18 and everything else are never written (the element type and the ring semantics are INFERENCE).
struct {S}Ring {{
    std::uintptr_t q00;
    std::uintptr_t q08;
    std::uintptr_t q10;
    std::uintptr_t q18;   // never written
    std::uintptr_t q20;
    std::uint8_t area[0x2600];   // never written
}};
static_assert(sizeof({S}Ring) == 0x2628);
''')
    if nrecs:
        H.append(f'''struct {S}Records {{
    NamedRecord rec[k{S}RecordCount];   // consecutive 0x38 B records
}};
static_assert(sizeof({S}Records) == 0x38 * k{S}RecordCount);
''')
    if nzc or nzv:
        H.append(f'''// Zero-filled container (0x1c5090 is its atexit destructor, an IDENTITY TAG): the original zeroes [+0x08, +0x18) and [+0x18, +0x20) and nothing else (+0x00 is never written; the object may extend beyond +0x20: INFERENCE).
struct {S}Zero {{
    std::uint64_t q00;   // never written
    std::uint64_t q08;   // = 0
    std::uint64_t q10;   // = 0
    std::uint64_t q18;   // = 0
}};
static_assert(sizeof({S}Zero) == 0x20);
''')
    if nzcell:
        H.append(f'''// Zero cell ({zcdtor:#x} is its atexit destructor, an IDENTITY TAG; {"the original frees the pointer at +0 and keeps the cell" if zcwords==3 else "the original deletes through the pointer at +0 and CLEARS the cell"}): the original zeroes the {"three qwords [+0x00, +0x18)" if zcwords==3 else "qword at +0"} and nothing else.
struct {S}ZeroCell {{
    std::uint64_t q00;   // = 0
{"    std::uint64_t q08;   // = 0"+chr(10)+"    std::uint64_t q10;   // = 0"+chr(10) if zcwords==3 else ""}}};
static_assert(sizeof({S}ZeroCell) == {8*zcwords:#x});
''')
    if nent:
        H.append(f'''// Scripted-command registry entry (0x90 bytes of which only [+0x00, +0x88) are ever written; +0x88 is never touched): three {{obj+0x88, functor}} pairs at +0x00 / +0x20 / +0x40, zeros in between,
// the name SharedStr copied to +0x60 (atomic control increment when a control block is present) and the previous list head at +0x80. The destructor 0x3c15e0 is an IDENTITY TAG.
struct {S}Entry {{
    alignas(8) std::uint8_t b[0x90];
}};
static_assert(sizeof({S}Entry) == 0x90);
''')
    if npre:
        H.append(f'''// Perspective block (0x1580a0 writes all sixteen floats of a row-major matrix at {PRE["obj"]:#x}; the process-wide cell is the original's .bss, the host owns the storage).
struct {S}Perspective {{
    float m[16];
}};
static_assert(sizeof({S}Perspective) == 0x40);
''')
    if crits:
        H.append(f'''// Opaque bytes of a critical-section object: only the 8 byte handle at +0 (0x3b310) and the listed stores are ever written; the rest is never touched.
template <std::size_t N>
struct {S}Crit {{
    alignas(8) std::uint8_t b[N];
}};
''')
    def tyof(r):
        return TY[r["kind"]].replace("@CRIT@",f"{S}Crit<{r['size']:#x}>" if r["kind"]=="crit" else "").replace("@SENT@",S+"Sentinel").replace("@RECS@",S+"Records").replace("@ZCELL@",S+"ZeroCell").replace("@ENTRY@",S+"Entry").replace("@ZCONT@",S+"Zero").replace("@PRE@",S+"Perspective").replace("@RING@",S+"Ring").replace("@GT@",S+"MemberBlock").replace("@STAB@",S+"StoreTable").replace("@HST@",S+"HashTable")
    H.append(f'struct {S}Objects {{')
    cur=base0
    for k,i in enumerate(byaddr):
        r=rows[i]
        if twinb(r):
            # Twin B: the original object is only 0xd0 B and its value is the EXTERNAL dword at obj-8 (the bytes right after the previous row). The window keeps both
            # facts address-tied (cell + 0xd0 B reserved); the 0xd8 B ConCmdRangedVar model type lives right after the window (its +0xd0 tail is never read or written here).
            if r['cell']>cur: H.append(f'    std::uint8_t gap{k:02d}[{r["cell"]-cur:#x}];')
            H.append(f'    std::uint32_t cell{i:02d} = {r["cell_image"]:#x};   // {r["cell"]:#x}: the external value dword (INPUT; default = the ELF initial image, .bss has none)')
            H.append(f'    std::uint32_t cell{i:02d}_unused;   // {r["cell"]+4:#x}: never touched by this constructor')
            H.append(f'    std::uint8_t head{i:02d}[0xd0];   // {r["obj"]:#x}: the 0xd0 B object (model: r{i:02d} below)')
            cur=r['obj']+0xd0
            continue
        if r['obj']>cur: H.append(f'    std::uint8_t gap{k:02d}[{r["obj"]-cur:#x}];')
        H.append(f'    {tyof(r)} r{i:02d};   // {r["obj"]:#x} {r["kind"]}')
        cur=r['obj']+szof(r)
    for i in twb: H.append(f'    ConCmdRangedVar r{i:02d};   // {rows[i]["obj"]:#x} uint (twin B): relocated model of the 0xd0 B object above; value_ref = &cell{i:02d}; the +0xd0 tail of the model type is not part of the original')
    if nrr:
        H.append('    // Host services of the named-record registrar 0xa81700 (the process registry cell and the allocator pair behind 0x17070 / 0x170b0 are NOT lifted):')
        H.append('    RecordRegistryCell* record_registry = nullptr;   // the registry object (the original: 0x32fd398 + guard 0x33053f0)')
        H.append('    AllocDispatchContract record_alloc{};            // growth of the two record-pointer vectors; see default_slot'+str(n)+'_record_alloc()')
    if nent:
        H.append('    RbHooks entry_hooks{};   // HOST INPUT: the allocator behind 0x17070 / 0x170b0 and the strcasecmp import (0xf8f500); used only when the registry object *0x2ef63f8 exists (not at the first startup)')
    if nsep:
        H.append('    SharedStrCtorHooks separator_alloc{};   // HOST INPUT: the allocator behind 0xf1a0 ("MenuSeparator"; 0x12820 / 0x12af0 are not lifted); empty refuses the separator rows. See default_family_separator_alloc().')
    if npre:
        H.append('    TanfFn perspective_tanf = nullptr;   // HOST INPUT: libc tanf (the original reaches it through the PLT thunk 0xf8f410); nullptr refuses the perspective row. See helper_1580a0_perspective.')
    if ntree:
        H.append('    EmptyTreeAlloc tree_alloc{};   // HOST INPUT: the allocator behind 0x17070 (not lifted); nullptr refuses the empty-tree row. See default_empty_tree_alloc().')
    if srcs:
        H.append('    // INPUT: the run-time value of each global the original loads as an initial value (see k'+S+'SourceAddr); the default is the ELF initial image (zero for .bss).')
        H.append(f'    std::uint32_t source_bits[k{S}UFloatSourceCount] = {{'+', '.join(f'{q[1]:#010x}' for q in srcs)+'};')
    for xi_,xa_ in enumerate(exts): H.append(f'    const void* external_target_{xi_} = nullptr;   // identity of the object at {xa_:#x} (built by another slot; the host supplies it, the model only stores the pointer)')
    H.append('};')
    DEFALLOC_H=(f'\n// Host allocator stand-in (malloc / free) for the registrar vectors. NOT the engine allocator (0x12820 / 0x12af0 are not lifted); a host replaces it.\n[[nodiscard]] AllocDispatchContract default_slot{n}_record_alloc() noexcept;\n' if nrr else '')
    H.append(f'''
struct {S}Result {{
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
}};

[[nodiscard]] {S}Result init_slot{n}_{rva:x}({S}Objects& objects, ConCmdRegistry& concmd{MENUP}) noexcept;
{DEFALLOC_H}
}}  // namespace sotc''')
    header='\n'.join(H)+'\n'
    # ---------------- cpp
    CELL_SRC='// `[cell] = name; cell->hash = 0x52a990(name)`; the four bytes at +0x0c are never written.\nbool name_cell(NameHashCell& cell, const char* name) noexcept {{\n    cell.name = name;\n    cell.hash = fnv_cstr_52a990(name);\n    return true;\n}}\n\n'
    HOLD_SRC='// The 0x30 byte holder: view copy at +8, zero tail at +0x28, vptr identity tag; the stack view is released by an exact no-op f590 and\n// __cxa_atexit(dtor, holder) is recorded (dtor 0x8ef00 or 0x2fced0 per holder class).\nbool name_holder(NameHolder30& holder, const char* name, std::size_t length, std::uint64_t vptr, std::uint64_t dtor, ConCmdRegistry& concmd) noexcept {{\n    const SharedStr view = family_view(name, length);\n    holder.name = view;\n    holder.tail = 0;\n    holder.vptr = vptr;\n    const bool inert = release_inert_view_f590(view);\n    const bool recorded = concmd.atexit_log.record(dtor, &holder);\n    return inert && recorded;\n}}\n\n'
    unesc=lambda t:t.replace('{S}',S).replace('{{','{').replace('}}','}')
    NC_SRC='// The 0x14 byte null-name record: [obj] = 0 ; strncpy(obj+4, "NULL", 15) (ISO C strncpy, helper_f8f630_strncpy) ; byte +0x13 = 0. No atexit record.\nbool null_name_cell(NullNameCell247f47& cell) noexcept {{\n    store_null_name_cell_247f47(cell);\n    return true;\n}}\n\n'
    CELLFN=(unesc(CELL_SRC) if nreal else '')+(unesc(NC_SRC) if nnc else '')
    ENUMFN=(f'''[[maybe_unused]] bool enum_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {{
    const InlineEnumSpec spec{{k{S}EnumNames[n], k{S}EnumNameLengths[n], k{S}VarFlags, k{S}EnumMidVptr, k{S}EnumFinalVptr, k{S}EnumFnTag,
                              k{S}EnumMin[n], k{S}EnumMax[n], k{S}EnumValue[n], k{S}EnumValue[n], k{S}EnumDtorRva, k{S}EnumBounded}};
    return init_inline_enum_var(var, spec, concmd).ok;
}}

''' if ienums else '')
    SENT_SRC='// Sentinel container: the self-referential header stores of the original (q20, then q00 = q08, then q10); the +0x18 field and the 0x80 B area are never written. __cxa_atexit(dtor, &container) is recorded.\nbool sentinel_container(%sSentinel& c, std::uint64_t dtor, ConCmdRegistry& concmd) noexcept {{\n    const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(&c);\n    c.q20 = base + 0x28;\n    c.q00 = base + 0x28;\n    c.q08 = base + 0x28;\n    c.q10 = base + 0xa8;\n    return concmd.atexit_log.record(dtor, &c);\n}}\n\n' % S
    HOLDFN=unesc(HOLD_SRC) if nhold else ''
    if nsent: HOLDFN+=unesc(SENT_SRC)
    RING_SRC=f'// Ring / vector header: helper_1f220_ring_header (four qword stores), then __cxa_atexit(0x354470, header) is recorded (identity tag; the destructor frees the buffer, never run).\nbool ring_header(%sRing& c, ConCmdRegistry& concmd) noexcept {{{{\n    ring_header_init_1f220(&c);\n    return concmd.atexit_log.record(k%sRingDtorRva, &c);\n}}}}\n\n' % (S,S)
    GT_SRC=f'// The 24 getter pairs and their {{lo, hi, size}} record stores (helper_498_member_hashes getters; no atexit record). Fails closed if a getter is not one of the pinned 48.\nbool member_hash_block(%sMemberBlock& b) noexcept {{{{\n    for (std::size_t k = 0; k < k%sMemberCount; ++k) {{{{\n        std::uint32_t lo = 0;\n        std::uint32_t hi = 0;\n        if (!member_hash_getter_498(k%sMemberLoRva[k], lo) || !member_hash_getter_498(k%sMemberHiRva[k], hi)) return false;\n        b.dw[3 * k] = lo;\n        b.dw[3 * k + 1] = hi;\n        b.dw[3 * k + 2] = k%sMemberSize[k];\n    }}}}\n    return true;\n}}}}\n\n' % (S,S,S,S,S)
    if nring: HOLDFN+=unesc(RING_SRC)
    if ngt: HOLDFN+=unesc(GT_SRC)
    RECS_SRC="""// Named records (0xa81700 x N, each followed by f590 on the name view), then __cxa_atexit(0x1c8990, records). On first use the registrar initialises the
// registry and records the identity-tag atexit 0xa81b80. A missing registry or allocator is a visible refusal (the original would fault).
bool named_records({S}Records& recs, RecordRegistryCell* cell, const AllocDispatchContract& alloc, ConCmdRegistry& concmd) noexcept {{
    if (cell == nullptr) return false;
    for (std::size_t n = 0; n < k{S}RecordCount; ++n) {{
        const SharedStr view = family_view(k{S}RecName[n], k{S}RecNameLength[n]);
        const NamedRecordRegistration reg = register_named_record_a81700(*cell, recs.rec[n], view, k{S}RecKind[n], k{S}RecFlag[n], alloc);
        if (!reg.ok) return false;
        if (reg.registry_initialised_now && !concmd.atexit_log.record(kRecordRegistryDtorRva, &cell->registry)) return false;
        if (!release_inert_view_f590(view)) return false;
    }}
    return concmd.atexit_log.record(k{S}RecsDtorRva, &recs);
}}

"""
    ZERO_SRC="""// Zero-filled container: [+0x08, +0x18) and [+0x18, +0x20) are zeroed, nothing else is written; __cxa_atexit(0x1c5090, container) is recorded (identity tag).
bool zero_container({S}Zero& c, std::uint64_t dtor, ConCmdRegistry& concmd) noexcept {{
    c.q08 = 0;
    c.q10 = 0;
    c.q18 = 0;
    return concmd.atexit_log.record(dtor, &c);
}}

"""
    PRE_SRC="""// Perspective row: 0x1580a0(out, &fov, aspect, near, far, flag). Every argument is a constant of the slot body (the fov immediate, three .rodata dwords, edx = 1); tanf is the host hook.
bool perspective_row({S}Perspective& p, TanfFn tanf_hook) noexcept {{
    float fov = 0.0f;
    float aspect = 0.0f;
    float near_plane = 0.0f;
    float far_plane = 0.0f;
    std::memcpy(&fov, &k{S}PerspectiveFovBits, sizeof fov);
    std::memcpy(&aspect, &k{S}PerspectiveAspectBits, sizeof aspect);
    std::memcpy(&near_plane, &k{S}PerspectiveNearBits, sizeof near_plane);
    std::memcpy(&far_plane, &k{S}PerspectiveFarBits, sizeof far_plane);
    return helper_1580a0_perspective(p.m, &fov, aspect, near_plane, far_plane, k{S}PerspectiveFlag, tanf_hook).ok;
}}

"""
    ZQ_SRC="""// Zero pointer slot: the original stores qword 0 and records __cxa_atexit(0x565e80, &slot) (identity tag; the destructor frees the pointee, never run).
bool zero_pointer(std::uint64_t& slot, std::uint64_t dtor, ConCmdRegistry& concmd) noexcept {{
    slot = 0;
    return concmd.atexit_log.record(dtor, &slot);
}}

"""
    DWR_SRC="""// Dword-ranged variable (0x49ca60 / 0x49cc60 -> bare-ret functor destroy -> final vptr -> value/default -> 0x20d60 -> f590(description) -> f590(name) -> __cxa_atexit(0x498b20 / 0x498b60)).
bool dwr_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {{
    const std::int32_t d = k{S}DwrDesc[n];
    const HelperDwordSiteSpec spec{{k{S}DwrVariant[n] == 0 ? RangedDwordVariant::a_49ca60 : RangedDwordVariant::b_49cc60, k{S}DwrNames[n], k{S}DwrNameLengths[n],
                                   k{S}MenuStrings[d], k{S}MenuStringLengths[d], k{S}VarFlags, k{S}DwrFnTag[n], k{S}DwrMin[n], k{S}DwrMax[n], k{S}DwrValue[n], k{S}DwrDefault[n],
                                   k{S}DwrFinalVptr[n], k{S}DwrDtorRva[n]}};
    return helper_init_ranged_dword_var_site(var, spec, concmd).ok;
}}

"""
    CRIT_SRC="""// Critical-section object: 0x3b310(obj) fills the 8 byte handle at +0, then the observed field stores (offset / width / value tables) are replayed; __cxa_atexit(dtor, obj) is recorded
// (0x499e50 / 0x499e60 are identity tags and never run). Nothing else in the object is written.
template <std::size_t N>
bool crit_row({S}Crit<N>& c, std::size_t n, ConCmdRegistry& concmd) noexcept {{
    if (k{S}CritSize[n] != N) return false;
    if (critsec_init_at(c.b) != 0) return false;
    for (std::size_t s = k{S}CritStoreFirst[n]; s < k{S}CritStoreFirst[n + 1]; ++s) {{
        const std::uint32_t off = k{S}CritStoreOffset[s];
        const std::uint32_t width = k{S}CritStoreWidth[s];
        if (off < 8 || off + width > N) return false;
        if (width <= 8) {{
            const std::uint64_t value = k{S}CritStoreValue[s];
            std::memcpy(c.b + off, &value, width);
        }} else {{
            std::memset(c.b + off, 0, width);   // a vector store of zero (the tables only admit zero past 8 bytes)
        }}
    }}
    return concmd.atexit_log.record(k{S}CritDtorRva[n], &c);
}}

"""
    if dwrs: HOLDFN+=unesc(DWR_SRC)
    if crits: HOLDFN+=unesc(CRIT_SRC)
    if nzq: HOLDFN+=unesc(ZQ_SRC)
    REC1_SRC="""// Single named record (0xa81700 + f590 on the name view + __cxa_atexit(0xa81950, record)). On first use the registrar initialises the registry and records the identity-tag atexit 0xa81b80
// (BEFORE the record's own). A missing registry or allocator is a visible refusal (the original would fault).
bool named_record1(NamedRecord& rec, std::size_t n, RecordRegistryCell* cell, const AllocDispatchContract& alloc, ConCmdRegistry& concmd) noexcept {{
    if (cell == nullptr) return false;
    const SharedStr view = family_view(k{S}Rec1Name[n], k{S}Rec1NameLength[n]);
    const NamedRecordRegistration reg = register_named_record_a81700(*cell, rec, view, k{S}Rec1Kind[n], k{S}Rec1Flag[n], alloc);
    if (!reg.ok) return false;
    if (reg.registry_initialised_now && !concmd.atexit_log.record(kRecordRegistryDtorRva, &cell->registry)) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(k{S}Rec1DtorRva, &rec);
}}

"""
    ZCELL_SRC=f"""// Zero cell: {"the three qwords [+0x00, +0x18)" if zcwords==3 else "the qword at +0"} {"are" if zcwords==3 else "is"} zeroed, nothing else is written; __cxa_atexit({zcdtor:#x}, cell) is recorded (identity tag; the destructor {"frees [+0] and keeps the cell" if zcwords==3 else "deletes through [+0] and clears the cell"}, never run).
bool zero_cell({S}ZeroCell& c, ConCmdRegistry& concmd) noexcept {{
    c.q00 = 0;
{"    c.q08 = 0;"+chr(10)+"    c.q10 = 0;"+chr(10) if zcwords==3 else ""}    return concmd.atexit_log.record(k{S}ZeroCellDtorRva, &c);
}}

"""
    ENTRY_SRC="""// Scripted-command registry entry: two borrowed name views (0xf2f0), the 0x88 B object image (three {obj+0x88, functor tag} pairs, zeros, the name SharedStr at +0x60 with an atomic control
// increment when a control block exists, the previous list head at +0x80), the push on the global list (cell 0x2ef6400), the registration 0x3be2d0(*0x2ef63f8, obj) when the registry
// object exists, the two view releases (description first) and __cxa_atexit(0x3c15e0, obj) (identity tag, never run). +0x88 is never written.
bool entry_row({S}Entry& e, std::size_t n, const RbHooks& hooks, ConCmdRegistry& concmd) noexcept {{
    const SharedStr name = family_view(k{S}EntryNames[n], k{S}EntryNameLengths[n]);
    const SharedStr desc = family_view(k{S}EntryDescs[n], k{S}EntryDescLengths[n]);
    const std::uint64_t self88 = static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(e.b)) + 0x88;
    auto put = [&](std::size_t off, std::uint64_t v) {{ std::memcpy(e.b + off, &v, sizeof v); }};
    put(0x00, self88);
    put(0x08, k{S}EntryFns[3 * n + 0]);
    put(0x10, 0);
    put(0x18, 0);
    put(0x20, self88);
    put(0x28, k{S}EntryFns[3 * n + 1]);
    put(0x30, 0);
    put(0x38, 0);
    put(0x40, self88);
    put(0x48, k{S}EntryFns[3 * n + 2]);
    put(0x50, 0);
    put(0x58, 0);
    std::memcpy(e.b + 0x60, &name, sizeof name);
    if (name.ctl != nullptr) reinterpret_cast<std::atomic<std::int64_t>*>(name.ctl)->fetch_add(1, std::memory_order_seq_cst);   // lock incq
    void*& head_cell = ctor_98a90_list_head_cell_2ef6400();
    void* const registry = ctor_98a90_self_cell_2ef63f8();
    void* const previous_head = head_cell;
    head_cell = e.b;
    std::memcpy(e.b + 0x80, &previous_head, sizeof previous_head);
    if (registry != nullptr && !rb_register_entry_3be2d0(*static_cast<RbMap*>(registry), e.b, hooks).ok) return false;
    if (!release_inert_view_f590(desc)) return false;
    if (!release_inert_view_f590(name)) return false;
    return concmd.atexit_log.record(k{S}EntryDtorRva, &e);
}}

"""
    if nrecs: HOLDFN+=unesc(RECS_SRC)
    if nent: HOLDFN+=unesc(ENTRY_SRC)
    if nrec1: HOLDFN+=unesc(REC1_SRC)
    if nzc or nzv: HOLDFN+=unesc(ZERO_SRC)
    if nzcell: HOLDFN+=unesc(ZCELL_SRC)
    if nhst: HOLDFN+=unesc(f"""// Hash-to-dword store table (batch 20): every site hashes its rodata string natively (fnv_cstr_a2ddc0) and stores the dword; no atexit record exists for these sites.
bool hash_store_row({S}HashTable& t, [[maybe_unused]] ConCmdRegistry& concmd) noexcept {{
    const HashStoreSpec spec{{k{S}HashStores, k{S}HashStoreCount, k{S}HashStringBlob, sizeof k{S}HashStringBlob, k{S}HashStringOffset, k{S}HashStringCount}};
    return replay_hash_stores(t.bytes, sizeof t.bytes, spec);
}}

""")
    if nstab: HOLDFN+=unesc(f"""// Store table (batch 18): the straight-line stores of the original, applied from the ELF-tied record table (core/store_table; nothing is executed). The destructor {STAB["dtor"]:#x} takes the object
// from a fixed address and is recorded as an identity tag with a null object, exactly like the original __cxa_atexit({STAB["dtor"]:#x}, 0, &__dso_handle).
bool store_table_row({S}StoreTable& t, ConCmdRegistry& concmd) noexcept {{
    const StoreTableSpec spec{{k{S}Stores, k{S}StoreCount, k{S}StoreStringBlob, sizeof k{S}StoreStringBlob, k{S}StoreStringOffset, k{S}StoreStringCount}};
    if (!replay_store_table(t.bytes, sizeof t.bytes, spec)) return false;
    return concmd.atexit_log.record(k{S}StoreDtorRva, nullptr);
}}

""")
    if npre: HOLDFN+=unesc(PRE_SRC)
    UINTFN=('@W [[maybe_unused]] bool uint_var(ConCmdRangedVar& var, std::size_t n, '+('std::uint32_t* cell, ' if hasB else '')+'ConCmdRegistry& concmd) noexcept {\n'
        +('@W     const std::int32_t d = k{S}UIntDesc[n];   // d < 0: the empty description; otherwise a borrowed view of the menu string literal\n'
          '@W     const HelperIntSiteSpec spec{k{S}UIntNames[n], k{S}UIntNameLengths[n], d < 0 ? "" : k{S}MenuStrings[d], d < 0 ? 0 : k{S}MenuStringLengths[d], 0, 0, k{S}VarFlags, k{S}UIntFnTag, k{S}UIntValue[n], k{S}UIntValue[n], k{S}UIntEarly[n] != 0};\n' if hasud else
          '@W     const HelperIntSiteSpec spec{k{S}UIntNames[n], k{S}UIntNameLengths[n], "", 0, 0, 0, k{S}VarFlags, k{S}UIntFnTag, k{S}UIntValue[n], k{S}UIntValue[n], k{S}UIntEarly[n] != 0};\n')
        +'@W     return helper_init_unbounded_int_var_site(var, spec, concmd).ok;\n@W }\n\n').replace('{S}',S)
    if hastwin:
        UINTFN=UINTFN.replace('@W     const HelperIntSiteSpec spec','''@W     if (k{S}UIntTwin[n] != 0) {
@W         const HelperIntTwinSiteSpec twin{k{S}UIntNames[n], k{S}UIntNameLengths[n], @TWD, k{S}VarFlags, k{S}UIntValue[n], k{S}UIntValue[n], k{S}UIntTwinFinalVptr, k{S}UIntTwinDtorRva, k{S}UIntEarly[n] != 0@TWB};\n@W         return helper_init_unbounded_int_twin_var_site(var, UnboundedIntTwinVariant::@TWV, twin, @TWC, concmd).ok;
@W     }
@W     const HelperIntSiteSpec spec'''.replace('{S}',S).replace('@TWB',', false, false' if hasB else '').replace('@TWV','b_25ab0' if hasB else 'a_140b00').replace('@TWC','cell' if hasB else 'nullptr').replace('@TWD','d < 0 ? "" : k'+S+'MenuStrings[d], d < 0 ? 0 : k'+S+'MenuStringLengths[d]' if hasud else '"", 0'),1)
    UFLOATFN=(
        "@U [[maybe_unused]] bool ufloat_var(ConCmdRangedVar& var, std::size_t n, "+("const std::uint32_t* source_bits, " if srcs else "")+"ConCmdRegistry& concmd) noexcept {{\n"
        "@U     const std::int32_t d = k{S}UFloatDesc[n];\n"
        "@U     const char* const desc = d < 0 ? \"\" : k{S}MenuStrings[d];\n"
        "@U     const std::size_t desc_length = d < 0 ? 0 : k{S}MenuStringLengths[d];\n"
        +("@U     const std::int32_t src = k{S}UFloatSource[n];   // >= 0: the initial value is a run-time load of a global (an input), else a constant of the body\n"
          "@U     const std::uint32_t value_bits = src < 0 ? k{S}UFloatValueBits[n] : source_bits[src];\n" if srcs else "")
        +"@U     const HelperFloatSiteSpec spec{{k{S}UFloatNames[n], k{S}UFloatNameLengths[n], desc, desc_length, k{S}VarFlags, k{S}UFloatFnTag, "+("value_bits" if srcs else "k{S}UFloatValueBits[n]")+", k{S}UFloatEarly[n] != 0}};\n"
        "@U     return helper_init_unbounded_float_var_site(var, spec, concmd).ok;\n"
        "@U }}\n\n").replace('{S}',S).replace('{{','{').replace('}}','}')
    C=[f'#include "{stem}.hpp"\n\n{"#include <atomic>"+chr(10) if nent else ""}{"#include <cstdlib>"+chr(10) if nrr else ""}#include <cstring>\n\nnamespace sotc {{\n\nnamespace {{\n']
    C.append(f'''// The 0x27120 slider: ctor(item, label, variable, 0x30, lo, hi) (text = the variable's own name view), 0x268b0, 0xf590(label), __cxa_atexit(0x272d0, item).
[[maybe_unused]] bool slider_item(DebugMenuItem& item, const char* label, std::size_t label_length, const ConCmdRangedVar& var, std::uint32_t lo_bits,
                 std::uint32_t hi_bits, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {{
    float lo = 0.0f;
    float hi = 0.0f;
    std::memcpy(&lo, &lo_bits, sizeof lo);
    std::memcpy(&hi, &hi_bits, sizeof hi);
    const SharedStr view = family_view(label, label_length);
    if (!init_debug_menu_item_27120(item, view, var.base, k{S}ItemFlags, lo, hi)) return false;
    if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
    if (!release_inert_view_f590(view)) return false;
    return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
}}

@D // The 0x27050 slider: ctor(item, label, text, target, 0x30, lo, hi) (explicit text, any target object), 0x268b0, f590(text), f590(label), __cxa_atexit(0x272d0, item).
@D [[maybe_unused]] bool slider50_item(DebugMenuItem& item, const char* label, std::size_t label_length, const char* text, std::size_t text_length, const void* target,
@D                                     std::uint32_t lo_bits, std::uint32_t hi_bits, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept {{
@D     float lo = 0.0f;
@D     float hi = 0.0f;
@D     std::memcpy(&lo, &lo_bits, sizeof lo);
@D     std::memcpy(&hi, &hi_bits, sizeof hi);
@D     const SharedStr label_view = family_view(label, label_length);
@D     const SharedStr text_view = family_view(text, text_length);
@D     if (!init_debug_menu_item_27050(item, label_view, text_view, target, k{S}ItemFlags, lo, hi)) return false;
@D     if (!register_debug_menu_item_268b0(item, menu, concmd.atexit_log).ok) return false;
@D     if (!release_inert_view_f590(text_view) || !release_inert_view_f590(label_view)) return false;
@D     return concmd.atexit_log.record(kDebugMenuItemDtorRva, &item);
@D }}

{UINTFN}
@V // String variable (0x257a0 -> 0x258f0): borrowed name and initial views, then f590(init), f590(name), __cxa_atexit(0x23c90, var).
@V [[maybe_unused]] bool str_var(ConCmdStringVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {{
@V     const SharedStr name = family_view(k{S}StrNames[n], k{S}StrNameLengths[n]);
@V     const std::int32_t d = k{S}StrInit[n];
@V     const SharedStr init = d < 0 ? empty_string_f730() : family_view(k{S}MenuStrings[d], k{S}MenuStringLengths[d]);   // d < 0: the static empty string returned by 0xf730 (never released)
@V     if (!init_string_var_257a0(var, name, init, concmd).ok) return false;
@V     if ((d >= 0 && !release_inert_view_f590(init)) || !release_inert_view_f590(name)) return false;
@V     return concmd.atexit_log.record(kStringVarDtorRva, &var);
@V }}

@C // Console command (0xf2f0 name view, 0x20af0 flags 2, register-once [0x1594dc0 + 0x20], two f590, __cxa_atexit(0x4b0)); the handler is an identity tag, never called.
@C [[maybe_unused]] bool cmd_var(ConCmdCommand& command, std::size_t n, ConCmdRegistry& concmd) noexcept {{
@C     const std::int32_t d = k{S}CmdDesc[n];
@C     const FConCmdCommandSpec spec{{k{S}CmdNames[n], k{S}MenuStrings[d], k{S}MenuStringLengths[d], k{S}CmdHandler[n]}};
@C     const FConCmdBuildResult built = init_fconcmd_object(command, spec, concmd);
@C     return built.ok && built.vtable_register_slot_ok && built.views_inert && built.atexit_recorded && !built.handler_invoked;
@C }}

@F [[maybe_unused]] bool float_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {{
@F     const FloatVarSpec spec{{k{S}FloatNames[n], "", true, k{S}FloatLoBits[n], k{S}FloatHiBits[n], k{S}FloatValueBits[n], k{S}FloatFnTag, k{S}VarFlags}};
@F     return init_float_var_static(var, spec, concmd).ok;
@F }}

{UFLOATFN}
@I [[maybe_unused]] bool int_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {{
@I     const HelperIntSiteSpec spec{{k{S}IntNames[n], k{S}IntNameLengths[n], "", 0, k{S}IntMin[n], k{S}IntMax[n], k{S}VarFlags, k{S}IntFnTag, k{S}IntValue[n], k{S}IntValue[n]}};
@I     return helper_init_ranged_int_var_site(var, spec, concmd).ok;
@I }}

@L [[maybe_unused]] bool lint_var(ConCmdRangedVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {{
@L     const SharedStr view = k{S}LintDesc[n] == nullptr ? empty_string_f730() : family_view(k{S}LintDesc[n], k{S}LintDescLength[n]);
@L     return helper_init_local_int_var_site(var, k{S}LocalIntSites[n], view, concmd).ok;
@L }}

@B [[maybe_unused]] bool bool_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {{
@B     const std::int32_t d = k{S}BoolDesc[n];   // d < 0: the empty description; otherwise the description is a borrowed view of the menu string literal
@B     return lane_m6::init_bool_var_literal(var, k{S}BoolNames[n], k{S}BoolNameLengths[n], d < 0 ? "" : k{S}MenuStrings[d], d < 0 ? 0 : k{S}MenuStringLengths[d],
@B                                           k{S}BoolFnTag, concmd, k{S}BoolValues[n], k{S}BoolValues[n], k{S}BoolEarly[n] != 0).ok;
@B }}

@Y [[maybe_unused]] bool boolx_var(ConCmdBoolVar& var, std::size_t n, ConCmdRegistry& concmd) noexcept {{
@Y     const BoolVar274730SiteResult r = helper_init_bool_var_274730_site(var, k{S}BoolXSites[n], concmd);   // 0x274730 ; call [argfn + 0x18] ; f590(name) ; __cxa_atexit(0x20b0)
@Y     return r.ok && r.refusal == HelperRefusal::none;
@Y }}

{ENUMFN}{CELLFN}{HOLDFN}[[maybe_unused]] DebugMenuTarget target_of(const ConCmdBoolVar& var) noexcept {{ return debug_menu_target_for(var); }}
[[maybe_unused]] DebugMenuTarget target_of(const ConCmdRangedVar& var) noexcept {{ return lane_m6::ranged_float_menu_target(var); }}

}}  // namespace
''')
    C.append(f'{S}Result init_slot{n}_{rva:x}({S}Objects& objects, ConCmdRegistry&{" concmd" if (allvars or ntree or nnref or nsent or nzc or nzq or nzv or nrr or nzcell or nent or nhold or nring or nstab) else ""}{MENUP}) noexcept {{\n    {S}Result out{{}};\n')
    order=[]
    def TOF(ti,tv):
        tk=rows[ti]['kind']
        return {'lint':f'local_int_var_menu_target({tv})','int':f'ranged_int_c580_menu_target({tv})','ienum':f'inline_enum_menu_target({tv}, k{S}EnumFinalVptr, k{S}EnumSlot3Result, k{S}EnumSlot6Result)','uint':(f'inline_enum_menu_target({tv}, k{S}UIntTwinFinalVptr, k{S}UIntTwinSlot3Result, k{S}UIntTwinSlot6Result)' if twinb(rows[ti]) else f'unbounded_int_menu_target({tv})'),'cmd':f'l2::command_menu_target({tv})','str':f'string_var_menu_target({tv})'}.get(tk,f'target_of({tv})')
    def S_(s): return f'k{S}MenuStrings[{s}], k{S}MenuStringLengths[{s}]'
    for i,d in enumerate(rowinfo):
        k=d['kind']; o=f'objects.r{i:02d}'
        if k=='bool': call=f'bool_var({o}, {d["v"]}, concmd)'
        elif k=='boolx': call=f'boolx_var({o}, {d["v"]}, concmd)'
        elif k=='int': call=f'int_var({o}, {d["v"]}, concmd)'
        elif k=='ienum': call=f'enum_var({o}, {d["v"]}, concmd)'
        elif k=='lint': call=f'lint_var({o}, {d["v"]}, concmd)'
        elif k=='uint': call=f'uint_var({o}, {d["v"]}, '+((f'&objects.cell{i:02d}, ' if twinb(rows[i]) else 'nullptr, ') if hasB else '')+'concmd)'
        elif k=='cmd': call=f'cmd_var({o}, {d["v"]}, concmd)'
        elif k=='dwr': call=f'dwr_var({o}, {d["v"]}, concmd)'
        elif k=='crit': call=f'crit_row({o}, {d["v"]}, concmd)'
        elif k=='rec1': call=f'named_record1({o}, {d["v"]}, objects.record_registry, objects.record_alloc, concmd)'
        elif k=='zcell': call=f'zero_cell({o}, concmd)'
        elif k=='entry': call=f'entry_row({o}, {d["v"]}, objects.entry_hooks, concmd)'
        elif k=='str': call=f'str_var({o}, {d["v"]}, concmd)'
        elif k=='float': call=f'float_var({o}, {d["v"]}, concmd)'
        elif k=='ufloat': call=f'ufloat_var({o}, {d["v"]}, '+('objects.source_bits, ' if srcs else '')+'concmd)'
        elif k=='tog':
            tv=f'objects.r{d["t"]:02d}'; tof=TOF(d["t"],tv); call=f'family_named_item_26db0({o}, {S_(d["s0"])}, {tv}.base, {tof}, k{S}ItemFlags, concmd, menu).ok()'
        elif k=='toge':
            tv=f'objects.r{d["t"]:02d}'; tof=TOF(d["t"],tv); call=f'family_toggle_item_26ea0({o}, {S_(d["s0"])}, {S_(d["s1"])}, {tof}, k{S}ItemFlags, concmd, menu).ok()'
        elif k=='slider':
            tv=f'objects.r{d["t"]:02d}'; si=sidx[i]; call=f'slider_item({o}, {S_(d["s0"])}, {tv}, k{S}SliderLoBits[{si}], k{S}SliderHiBits[{si}], concmd, menu)'
        elif k=='slider50':
            si=sidx[i]
            if d['t']<0:
                tv=f'objects.external_target_{d["x"]}'; call=f'({tv} != nullptr && slider50_item({o}, {S_(d["s0"])}, {S_(d["s1"])}, {tv}, k{S}SliderLoBits[{si}], k{S}SliderHiBits[{si}], concmd, menu))'
            else:
                tv=f'objects.r{d["t"]:02d}'; call=f'slider50_item({o}, {S_(d["s0"])}, {S_(d["s1"])}, &{tv}, k{S}SliderLoBits[{si}], k{S}SliderHiBits[{si}], concmd, menu)'
        elif k=='cell': call=f'name_cell({o}, k{S}MenuStrings[{d["s0"]}])'
        elif k=='ncell': call=f'null_name_cell({o})'
        elif k=='sent': call=f'sentinel_container({o}, k{S}SentinelDtorRva, concmd)'
        elif k=='recs': call=f'named_records({o}, objects.record_registry, objects.record_alloc, concmd)'
        elif k=='zcont': call=f'zero_container({o}, k{S}ZeroDtorRva, concmd)'
        elif k=='zvec': call=f'zero_container({o}, k{S}ZVecDtorRva[{d["v"]}], concmd)'
        elif k=='pre': call=f'perspective_row({o}, objects.perspective_tanf)'
        elif k=='gettable': call=f'member_hash_block({o})'
        elif k=='stab': call=f'store_table_row({o}, concmd)'
        elif k=='hstab': call=f'hash_store_row({o}, concmd)'
        elif k=='ringhdr': call=f'ring_header({o}, concmd)'
        elif k=='sep' and rows[i].get('fn')==0x26800: call=f'family_label_item_26800({o}, {S_(d["s0"])}, {rows[i]["flags"]:#x}, objects.separator_alloc, concmd, menu).ok()'
        elif k=='sep': call=f'family_separator_item_26b60({o}, {S_(d["s0"])}, k{S}ItemFlags, objects.separator_alloc, concmd, menu).ok()'
        elif k=='zq': call=f'zero_pointer({o}, k{S}ZeroPtrDtorRva, concmd)'
        elif k=='tree': call=f'build_empty_tree({o}, k{S}TreeNodeSize, objects.tree_alloc, k{S}TreeDtorRva, concmd)'
        elif k=='nref': call=f'build_node_ref_row({o}, k{S}NodeRefDtorRva, concmd)'
        elif k=='holder': call=f'name_holder({o}, {S_(d["s0"])}, k{S}HolderVptr{HSFX[d["v"]]}, k{S}HolderDtorRva{HSFX[d["v"]]}, concmd)'
        elif k=='cmdi':
            tv=f'objects.external_target_{d["x"]}'; call=f'({tv} != nullptr && family_command_item_26f90({o}, {S_(d["s0"])}, {S_(d["s1"])}, {S_(d["v"])}, {tv}, k{S}ItemFlags, concmd, menu).ok())'
        elif k=='group':
            call=f'family_group_item_26c10({o}, {S_(d["s0"])}, {S_(d["s1"])}, {"k"+S+"ItemFlags" if d["v"]==0x30 else format(d["v"],"#x")}, concmd, menu).ok()'
        line=f'    if (!{call}) return out;\n    ++out.rows_done;   // row {i:02d} {k}'
        # group flags: the original group flags may differ -> checked at generation
        C.append(line); order.append(call.split('(')[0]+'('+o)
    C.append('    out.ok = true;\n    return out;\n}\n')
    if nrr:
        C.append(f'''
namespace {{
void* record_host_allocate(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) noexcept {{ return std::malloc(size); }}
bool record_host_deallocate(void* ptr, std::uint32_t) noexcept {{
    std::free(ptr);
    return true;
}}
}}  // namespace

AllocDispatchContract default_slot{n}_record_alloc() noexcept {{
    AllocDispatchContract c;
    c.allocate = &record_host_allocate;
    c.deallocate = &record_host_deallocate;
    return c;
}}
''')
    C.append('\n}  // namespace sotc\n')
    cpp=cond('\n'.join(C),bool(bools),bool(floats),bool(ufl),bool(ints),not menu_less,bool(lints),bool(uints),bool(cmds),bool([i for i in sliders if rows[i]['kind']=='slider50']),bool(strv),hq=bool(nsent),hx=bool(exts),hy=bool(bx))
    # ---------------- test
    def _hchain(var,pfx):
        e_=f"k{S}{pfx}{HSFX[len(hcl)-1]}"
        for q_ in range(len(hcl)-2,-1,-1): e_=f"({var} == {q_} ? k{S}{pfx}{HSFX[q_]} : {e_})"
        return e_
    HVEXPR=_hchain('row.var','HolderVptr') if len(hcl)>1 else f"k{S}HolderVptr"
    SDEXPR=(f'k{S}SentinelDtorRva' if nsent else '0')
    GDEXPR=(f'k{S}RingDtorRva' if nring else '0')
    HDEXPR=("0" if not hcl else _hchain('rows[i].var','HolderDtorRva') if len(hcl)>1 else f"k{S}HolderDtorRva")
    HOLDCHK=(f"            CHECK(h.vptr == {HVEXPR} && h.tail == 0 && same_view(h.name, k{S}MenuStrings[row.s0], k{S}MenuStringLengths[row.s0]));" if nhold else "            (void)h;")
    EXTDEF=''.join(f'int g_external_{k_} = 0;   // stands for the object at {a_:#x}, which another slot builds\n' for k_,a_ in enumerate(exts))
    EXTSET=''.join(f'    o.external_target_{k_} = &g_external_{k_};\n' for k_ in range(len(exts)))
    if npre:
        EXTSET+='    o.perspective_tanf = &test_tanf;\n'
        EXTDEF+='float test_tanf(float x) noexcept { return std::tan(x); }   // the host tanf hook (libc)\n'
    EXTSET_NT=EXTSET
    if ntree:
        EXTSET+='    o.tree_alloc.allocate_17070 = &test_tree_alloc;\n'
        EXTDEF+=f'std::size_t g_tree_requested = 0;       // size the model passed to the allocator\nstd::uint8_t* g_tree_blocks[8] = {{}};    // the blocks the test allocator returned (poison-filled; freed at exit)\nstd::size_t g_tree_count = 0;\nvoid* test_tree_alloc(std::size_t size) noexcept {{\n    g_tree_requested = size;\n    if (g_tree_count >= 8) return nullptr;\n    std::uint8_t* b = static_cast<std::uint8_t*>(std::malloc(size));\n    std::memset(b, 0xa5, size);\n    g_tree_blocks[g_tree_count++] = b;\n    return b;\n}}\nbool is_tree_block(const std::uint8_t* p) noexcept {{\n    for (std::size_t i = 0; i < g_tree_count; ++i) if (g_tree_blocks[i] == p) return true;\n    return false;\n}}\n'
    if srcs:
        EXTSET+=(f'    for (std::size_t k = 0; k < k{S}UFloatSourceCount; ++k) {{\n'
                 '        g_source_bits[k] = 0x3f800000u + static_cast<std::uint32_t>(k);   // distinct marker INPUTS (1.0f, ...), deliberately not the ELF image\n'
                 '        o.source_bits[k] = g_source_bits[k];\n    }\n')
        EXTDEF+=f'std::uint32_t g_source_bits[k{S}UFloatSourceCount] = {{}};   // the inputs section_alone supplied, read back by the row check\n'
    UEXP=(f'(k{S}UFloatSource[row.var] < 0 ? k{S}UFloatValueBits[row.var] : g_source_bits[k{S}UFloatSource[row.var]])' if srcs else f'k{S}UFloatValueBits[row.var]')
    SRCCHK=(f'''    {{
        // A default-constructed Objects carries the ELF initial image of every run-time loaded global (zero for .bss), not a value this model invented.
        {S}Objects fresh;
        for (std::size_t k = 0; k < k{S}UFloatSourceCount; ++k) CHECK(fresh.source_bits[k] == k{S}SourceImageBits[k] && k{S}SourceAddr[k] != 0);
    }}
''' if srcs else '')
    _fe=next((i for i,d_ in enumerate(rowinfo) if d_.get('t')==-1 and 'x' in d_),0)
    EXTNULL=(f'''    {{
        // An external slider target the host did not supply: that row is refused visibly (rows before it were built).
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        {S}Objects o;
        poison(o);
        o.external_target_0 = nullptr;
        const {S}Result r = init_slot{n}_{rva:x}(o, creg, menu);
        CHECK(!r.ok && r.rows_done == {_fe});
    }}
''' if exts else '')
    SLIDCHK=(f"                CHECK(it.range_min == bits_to_float(k{S}SliderLoBits[row.slider]) && it.range_max == bits_to_float(k{S}SliderHiBits[row.slider]));" if sliders else "                CHECK(false);   // no slider rows in this slot")
    BXVARS=f' + k{S}BoolXCount' if bx else ''
    DWVARS=f' + k{S}DwrCount' if dwrs else ''
    DWEXPR=f'k{S}DwrDtorRva[rows[i].var]' if dwrs else '0'
    CREXPR=f'k{S}CritDtorRva[rows[i].var]' if crits else '0'
    DWCONST=(f'    for (std::size_t n = 0; n < k{S}DwrCount; ++n) CHECK(std::strlen(k{S}DwrNames[n]) == k{S}DwrNameLengths[n] && k{S}DwrMin[n] <= k{S}DwrMax[n]);\n' if dwrs else '')+(f'    CHECK(k{S}CritStoreFirst[k{S}CritCount] == k{S}CritStoreCount);\n' if crits else '')
    JDEXPR=f'k{S}EnumDtorRva' if ienums else '0'; ENUMCNT=f' + k{S}EnumCount' if ienums else ''
    JCASE=(f"""        case 'j': {{
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == k{S}EnumFinalVptr && v.base.flags == k{S}VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, k{S}EnumNames[row.var], k{S}EnumNameLengths[row.var]) && empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(k{S}EnumValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(k{S}EnumValue[row.var]));
            if (k{S}EnumBounded) {{
                CHECK(dword(v, 0x6c) == static_cast<std::uint32_t>(k{S}EnumMin[row.var]) && v.min_engaged == 1 && dword(v, 0x74) == static_cast<std::uint32_t>(k{S}EnumMax[row.var]) && v.max_engaged == 1);
            }} else {{
                CHECK(v.min_value[0] == 0 && v.min_engaged == 0 && v.max_value[0] == 0 && v.max_engaged == 0);   // one byte stored per bound, engaged bytes cleared
                for (std::size_t q = 1; q < 4; ++q) CHECK(v.min_value[q] == 0xa5 && v.max_value[q] == 0xa5);   // never written
            }}
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == k{S}EnumFnTag && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }}
""" if ienums else '')
    NDEXPR=f'k{S}UIntTwinDtorRva'; NFINAL=f"(row.kind == 'N' ? k{S}UIntTwinFinalVptr : kUnboundedIntFinalVptr)" if hastwin else 'kUnboundedIntFinalVptr'
    NFNTAG=f"(row.kind == 'N' ? kUnboundedIntTwinASeedClass : kIntFunctorClassTag)" if hastwin else 'kIntFunctorClassTag'
    if not hastwin: NDEXPR='0x18a60'
    if hasB:
        NFINAL=f"k{S}UIntTwinFinalVptr"; NFNTAG='kUnboundedIntTwinBSeedClass'
    NVREF='v.value_ref == &v.value'; NVAL='dword(v, 0xd0)'
    if hasB: NVREF='v.value_ref == row.ext'; NVAL='*static_cast<const std::uint32_t*>(row.ext)'   # twin B: the value is the external dword (INPUT), the +0xd0 tail of the model type is not part of the original
    UINTTAGCHK=(f'@W     CHECK(is_int_functor_tag(k{S}UIntFnTag));' if plainu else ('@W     CHECK(true);' if uints else ''))
    UINTCHK=('@W             if (k{S}UIntDesc[row.var] < 0) CHECK(empty_view(v.base.description));\n@W             else CHECK(same_view(v.base.description, k{S}MenuStrings[k{S}UIntDesc[row.var]], k{S}MenuStringLengths[k{S}UIntDesc[row.var]]));'.replace('{S}',S) if hasud else '@W             CHECK(empty_view(v.base.description));')
    DCASE=(f"""        case 'd': {{
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            const RangedDwordVariant variant = k{S}DwrVariant[row.var] == 0 ? RangedDwordVariant::a_49ca60 : RangedDwordVariant::b_49cc60;
            CHECK(k{S}DwrFnTag[row.var] == ranged_dword_class(variant) && k{S}DwrFinalVptr[row.var] != ranged_dword_vptr(variant));   // the site overwrites the constructor's mid vptr with its own literal final vptr
            CHECK(v.base.vptr == k{S}DwrFinalVptr[row.var] && v.base.flags == k{S}VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, k{S}DwrNames[row.var], k{S}DwrNameLengths[row.var]));
            CHECK(same_view(v.base.description, k{S}MenuStrings[k{S}DwrDesc[row.var]], k{S}MenuStringLengths[k{S}DwrDesc[row.var]]));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(k{S}DwrValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(k{S}DwrDefault[row.var]));
            CHECK(dword(v, 0x6c) == static_cast<std::uint32_t>(k{S}DwrMin[row.var]) && v.min_engaged == 1 && dword(v, 0x74) == static_cast<std::uint32_t>(k{S}DwrMax[row.var]) && v.max_engaged == 1);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == k{S}DwrFnTag[row.var] && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }}
""" if dwrs else '')
    CCASE=(f"""        case 'C': {{
            // Only the handle (+0, filled by 0x3b310) and the listed stores were written; every other byte still carries the poison.
            const auto* bytes = static_cast<const std::uint8_t*>(row.obj);
            const std::size_t size = k{S}CritSize[row.var];
            std::uint8_t want[0x100];
            CHECK(size <= sizeof want);
            std::memset(want, 0xa5, sizeof want);
            for (std::size_t q = k{S}CritStoreFirst[row.var]; q < k{S}CritStoreFirst[row.var + 1]; ++q) {{
                const std::uint64_t value = k{S}CritStoreValue[q];
                if (k{S}CritStoreWidth[q] <= 8) std::memcpy(want + k{S}CritStoreOffset[q], &value, k{S}CritStoreWidth[q]);
                else std::memset(want + k{S}CritStoreOffset[q], 0, k{S}CritStoreWidth[q]);
            }}
            for (std::size_t b = 8; b < size; ++b) CHECK(bytes[b] == want[b]);
            std::uint64_t handle = 0;
            std::memcpy(&handle, bytes, 8);
            CHECK(handle != 0xa5a5a5a5a5a5a5a5ull && handle != 0);   // 0x3b310 wrote a live token
            CHECK(critsec_destroy_at(const_cast<std::uint8_t*>(bytes)) == 0);   // fixture cleanup (0x3b380 model)
            break;
        }}
""" if crits else '')
    TDEXPR=f'k{S}TreeDtorRva' if ntree else '0'; KDEXPR=f'k{S}NodeRefDtorRva' if nnref else '0'
    ATX=f' + k{S}RecsCount' if nrecs else ' + 1' if nrec1 else ''
    RDEXPR=f'k{S}RecsDtorRva' if nrecs else '0'
    RECSET=(f'    RecordRegistryCell cell{{}};   // the process registry cell: zero = the original\'s BSS state\n    o.record_registry = &cell;\n    o.record_alloc = default_slot{n}_record_alloc();\n' if nrr else '')+('    o.separator_alloc = default_family_separator_alloc();\n' if nsep else '')+('    ctor_98a90_self_cell_2ef63f8() = nullptr;\n    ctor_98a90_list_head_cell_2ef6400() = nullptr;\n' if nent else '')
    TCASE=(f"""        case 'T': {{
            const auto& t = *static_cast<const EmptyTreeHeader*>(row.obj);
            CHECK(t.q00 == 0xa5a5a5a5a5a5a5a5ull && t.size == 0);   // +0x00 is never written; the size is zeroed
            const auto* node = static_cast<const std::uint8_t*>(t.head);
            CHECK(node != nullptr && g_tree_requested == k{S}TreeNodeSize && is_tree_block(node));
            std::uint64_t left = 0;
            std::uint64_t parent = 0;
            std::uint64_t right = 0;
            std::memcpy(&left, node + 0x00, 8);
            std::memcpy(&parent, node + 0x08, 8);
            std::memcpy(&right, node + 0x10, 8);
            const std::uint64_t self = reinterpret_cast<std::uintptr_t>(node);
            CHECK(left == self && parent == self && right == self && node[0x18] == 1 && node[0x19] == 1);
            for (std::size_t b = 0x1a; b < k{S}TreeNodeSize; ++b) CHECK(node[b] == 0xa5);   // never written by the original
            break;
        }}
""" if ntree else '')+(f"""        case 'k': {{
            const auto& n = *static_cast<const NodeRef56b4b0*>(row.obj);
            const HashedStr& empty = static_empty_hashed_2ed3640();
            CHECK(n.node != nullptr && n.flag == 0 && n.id.hash == empty.hash && n.id.str.ctl == empty.str.ctl);   // 0x14c3a0 copy of the static empty string
            const auto* bytes = reinterpret_cast<const std::uint8_t*>(&n);
            for (int b = 4; b < 8; ++b) CHECK(bytes[b] == 0xa5);                  // HashedStr padding is never written
            for (std::size_t b = 0x31; b < 0x38; ++b) CHECK(bytes[b] == 0xa5);    // past the flag byte: never written
            break;
        }}
""" if nnref else '')
    _ti=next((i for i,d_ in enumerate(rowinfo) if d_['kind']=='tree'),0)
    TREEFAIL=(f"""    {{
        // No host allocator for 0x17070: the empty-tree row is refused visibly (header zeroed, no atexit record for it); every row before it was built.
        ConCmdRegistry creg;
@M         DebugMenuRegistry menu;
        {S}Objects o;
        poison(o);
        o.tree_alloc = EmptyTreeAlloc{{}};
{EXTSET_NT}{RECSET}        const {S}Result r = init_slot{n}_{rva:x}(o, creg{MENUA});
        CHECK(!r.ok && r.rows_done == {_ti});
        CHECK(o.r{_ti:02d}.head == nullptr && o.r{_ti:02d}.size == 0);
    }}
    {{
        // The allocator answers null: same visible refusal.
        ConCmdRegistry creg;
@M         DebugMenuRegistry menu;
        {S}Objects o;
        poison(o);
        o.tree_alloc.allocate_17070 = [](std::size_t) noexcept -> void* {{ return nullptr; }};
{EXTSET_NT}{RECSET}        const {S}Result r = init_slot{n}_{rva:x}(o, creg{MENUA});
        CHECK(!r.ok && r.rows_done == {_ti});
        CHECK(o.r{_ti:02d}.head == nullptr && o.r{_ti:02d}.size == 0);
    }}
""" if ntree else '')
    _pi=next((i for i,d_ in enumerate(rowinfo) if d_['kind']=='pre'),0)
    PRE_EXT=EXTSET_NT.replace("    o.perspective_tanf = &test_tanf;\n","")
    PREFAIL=(f"""    {{
        // No tanf hook: the perspective row is refused visibly (nothing written for it, no later row runs).
        ConCmdRegistry creg;
@M         DebugMenuRegistry menu;
        {S}Objects o;
        poison(o);
        o.perspective_tanf = nullptr;
{PRE_EXT}{RECSET}        const {S}Result r = init_slot{n}_{rva:x}(o, creg{MENUA});
        CHECK(!r.ok && r.rows_done == {_pi});
        const auto* pb = reinterpret_cast<const std::uint8_t*>(&o.r{_pi:02d});
        for (std::size_t b = 0; b < sizeof(o.r{_pi:02d}); ++b) CHECK(pb[b] == 0xa5);   // untouched
    }}
""" if npre else '')
    WDEXPR=f'k{S}ZeroDtorRva' if nzc else '0'
    VDEXPR=f'k{S}ZVecDtorRva[rows[i].var]' if nzv else '0'
    ZPEXPR=f'k{S}ZeroPtrDtorRva' if nzq else '0'
    _ri=next((i for i,d_ in enumerate(rowinfo) if d_['kind'] in ('recs','rec1')),0)
    RCASE=(f"""        case 'r': {{
            const auto& rs = *static_cast<const {S}Records*>(row.obj);
            for (std::size_t n = 0; n < k{S}RecordCount; ++n) {{
                const NamedRecord& rec = rs.rec[n];
                CHECK(same_view(rec.name, k{S}RecName[n], k{S}RecNameLength[n]) && rec.name.ctl == nullptr);   // borrowed view: no control block to count
                CHECK(rec.kind == k{S}RecKind[n] && rec.q28 == 0 && rec.b30 == 0 && rec.flag == k{S}RecFlag[n]);
                for (std::uint8_t b : rec.gap24) CHECK(b == 0xa5);   // never written
                for (std::uint8_t b : rec.gap32) CHECK(b == 0xa5);   // never written
            }}
            break;
        }}
""" if nrecs else '')
    ACASE=(f"""        case 'a': {{
            const auto& p = *static_cast<const {S}Perspective*>(row.obj);
            // Independent evaluation of the documented 0x1580a0 arithmetic (single precision, instruction order): left-handed projection, m[11] = 1.
            float fov = 0.0f;
            float aspect = 0.0f;
            float near_plane = 0.0f;
            float far_plane = 0.0f;
            std::memcpy(&fov, &k{S}PerspectiveFovBits, 4);
            std::memcpy(&aspect, &k{S}PerspectiveAspectBits, 4);
            std::memcpy(&near_plane, &k{S}PerspectiveNearBits, 4);
            std::memcpy(&far_plane, &k{S}PerspectiveFarBits, 4);
            const float t = std::tan(fov * 0.5f);
            const float c = 1.0f / t;
            float want[16] = {{}};
            want[0] = c / aspect;
            want[5] = c;
            want[11] = 1.0f;
            const float d = k{S}PerspectiveFlag != 0 ? near_plane - far_plane : far_plane - near_plane;
            want[10] = (k{S}PerspectiveFlag != 0 ? near_plane : far_plane) / d;
            want[14] = -(near_plane * far_plane) / d;
            for (int q = 0; q < 16; ++q) CHECK(std::memcmp(&p.m[q], &want[q], 4) == 0);   // all sixteen elements written, bit-exact
            CHECK(p.m[11] == 1.0f && p.m[1] == 0.0f && p.m[15] == 0.0f);
            break;
        }}
""" if npre else '')
    GCASE=(f"""        case 'G': {{
            const auto& b = *static_cast<const {S}MemberBlock*>(row.obj);
            for (std::size_t k = 0; k < k{S}MemberCount; ++k) CHECK(b.dw[3 * k] == k{S}MemberLo[k] && b.dw[3 * k + 1] == k{S}MemberHi[k] && b.dw[3 * k + 2] == k{S}MemberSize[k]);   // all 72 dwords written
            break;
        }}
""" if ngt else '')
    RGCASE=(f"""        case 'R': {{
            const auto& q = *static_cast<const {S}Ring*>(row.obj);
            const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(&q);
            CHECK(q.q00 == base + 0x28 && q.q08 == base + 0x28 && q.q20 == base + 0x28 && q.q10 == base + 0x2628);
            CHECK(q.q18 == 0xa5a5a5a5a5a5a5a5ull);   // never written
            for (std::uint8_t b : q.area) CHECK(b == 0xa5);   // never written
            break;
        }}
""" if nring else '')
    VCASE=(f"""        case 'v': {{
            const auto& z = *static_cast<const {S}Zero*>(row.obj);
            CHECK(z.q00 == 0xa5a5a5a5a5a5a5a5ull && z.q08 == 0 && z.q10 == 0 && z.q18 == 0);   // begin / end / cap zeroed; +0x00 is never written
            break;
        }}
""" if nzv else '')
    PCASE=(f"""        case 'p': {{
            CHECK(*static_cast<const std::uint64_t*>(row.obj) == 0);   // the one store of the original (poison would read 0xa5..)
            break;
        }}
""" if nzq else '')
    WCASE=(f"""        case 'w': {{
            const auto& z = *static_cast<const {S}Zero*>(row.obj);
            CHECK(z.q00 == 0xa5a5a5a5a5a5a5a5ull && z.q08 == 0 && z.q10 == 0 && z.q18 == 0);   // +0x00 is never written
            break;
        }}
""" if nzc else '')
    RLOOP=(f"""        if (rows[i].kind == 'r') {{
            // The registrar initialises the registry on the first record and records its destructor 0xa81b80 once, BEFORE the records' own 0x1c8990.
            const auto& rs = *static_cast<const {S}Records*>(rows[i].obj);
            check_atexit(creg, at, kRecordRegistryDtorRva, &cell.registry);
            check_atexit(creg, at + 1, dtor, rows[i].obj);
            ++reg_shift;
            CHECK(cell.guard != 0);
            for (RecordPtrVec* vec : {{&cell.registry.first, &cell.registry.second}}) {{
                CHECK(static_cast<std::size_t>(vec->end - vec->begin) == k{S}RecordCount && vec->cap >= vec->end);
                for (std::size_t n = 0; n < k{S}RecordCount; ++n) CHECK(vec->begin[n] == &rs.rec[n]);   // constructor order, in both vectors
            }}
            std::free(cell.registry.first.begin);
            std::free(cell.registry.second.begin);
            continue;
        }}
""" if nrecs else '')
    RC1CASE=(f"""        case 'E': {{
            const auto& rec = *static_cast<const NamedRecord*>(row.obj);
            CHECK(same_view(rec.name, k{S}Rec1Name[row.var], k{S}Rec1NameLength[row.var]) && rec.name.ctl == nullptr);   // borrowed view: no control block to count
            CHECK(rec.kind == k{S}Rec1Kind[row.var] && rec.q28 == 0 && rec.b30 == 0 && rec.flag == k{S}Rec1Flag[row.var]);
            for (std::uint8_t b : rec.gap24) CHECK(b == 0xa5);   // never written
            for (std::uint8_t b : rec.gap32) CHECK(b == 0xa5);   // never written
            break;
        }}
""" if nrec1 else '')
    LCASE=(f"""        case 'L': {{
            const auto& z = *static_cast<const {S}ZeroCell*>(row.obj);
            CHECK(z.q00 == 0{" && z.q08 == 0 && z.q10 == 0" if zcwords==3 else ""});   // every qword zeroed
            break;
        }}
""" if nzcell else '')
    QCASE=(f"""        case 'Q': {{
            const auto* b = static_cast<const std::uint8_t*>(row.obj);
            auto q = [&](std::size_t off) {{ std::uint64_t v = 0; std::memcpy(&v, b + off, sizeof v); return v; }};
            const std::uint64_t self88 = static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(b)) + 0x88;
            const std::size_t v = static_cast<std::size_t>(row.var);
            CHECK(q(0x00) == self88 && q(0x20) == self88 && q(0x40) == self88);
            CHECK(q(0x08) == k{S}EntryFns[3 * v + 0] && q(0x28) == k{S}EntryFns[3 * v + 1] && q(0x48) == k{S}EntryFns[3 * v + 2]);
            CHECK(q(0x10) == 0 && q(0x18) == 0 && q(0x30) == 0 && q(0x38) == 0 && q(0x50) == 0 && q(0x58) == 0);
            SharedStr key{{}};
            std::memcpy(&key, b + 0x60, sizeof key);
            CHECK(same_view(key, k{S}EntryNames[v], k{S}EntryNameLengths[v]) && key.ctl == nullptr);
            CHECK(q(0x88) == 0xa5a5a5a5a5a5a5a5ull);   // never written
            std::uint64_t expect_prev = 0;   // the list head cell is null before the first entry (test fixture)
            for (std::size_t j = 0; j < k{S}RowCount; ++j) if (all[j].kind == 'Q' && static_cast<std::size_t>(all[j].var) + 1 == v) expect_prev = static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(all[j].obj));
            CHECK(q(0x80) == expect_prev);
            if (v + 1 == k{S}EntryCount) CHECK(ctor_98a90_list_head_cell_2ef6400() == row.obj);   // the last entry is the list head
            break;
        }}
""" if nent else '')
    HTCASE=(f"""        case 'H': {{
            const auto& t = *static_cast<const {S}HashTable*>(row.obj);
            // Independent evaluation: own loop, expected dwords from the generator's Python port of the hash, into a second poisoned buffer; every byte (written or never written) must match.
            alignas(4) std::uint8_t want[sizeof t.bytes];
            std::memset(want, 0xa5, sizeof want);
            for (std::size_t n = 0; n < k{S}HashStoreCount; ++n) {{
                const std::uint32_t v = k{S}HashExpected[n];
                std::memcpy(want + k{S}HashStores[n].offset, &v, 4);
                CHECK(fnv_cstr_a2ddc0(k{S}HashStringBlob + k{S}HashStringOffset[k{S}HashStores[n].string]) == v);   // the native hash agrees with the independent oracle
            }}
            CHECK(std::memcmp(want, t.bytes, sizeof want) == 0);
            CHECK(k{S}HashStores[0].call_rva == {HST["stores"][0]["call"]:#x} && k{S}HashStores[k{S}HashStoreCount - 1].store_rva == {HST["stores"][-1]["addr"]:#x});
            for (std::size_t n = 0; n < k{S}HashStringCount; ++n) {{
                const char* text = k{S}HashStringBlob + k{S}HashStringOffset[n];
                CHECK(std::strlen(text) != 0 && text[0] >= 0x20 && text[0] < 0x7f);
            }}
            break;
        }}
""" if nhst else '')
    ZTEXPR=f'k{S}StoreDtorRva' if nstab else '0'
    if nstab:
        _cov=set(); _kc=[0,0,0,0]
        for _st in STAB['stores']:
            _cov.update(range(_st['off'],_st['off']+_st['width']))
            for _lk,_lv in _st['lanes']: _kc[_lk]+=1
        _f=STAB['stores'][0]; _l=STAB['stores'][-1]
    ZTCASE=(f"""        case 'Z': {{
            const auto& t = *static_cast<const {S}StoreTable*>(row.obj);
            // Independent evaluation of the table: an own loop and own lane resolution into a second poisoned buffer; every byte (written or never written) must match.
            alignas(8) std::uint8_t want[sizeof t.bytes];
            std::memset(want, 0xa5, sizeof want);
            std::size_t kinds[4] = {{0, 0, 0, 0}};
            for (std::size_t n = 0; n < k{S}StoreCount; ++n) {{
                const StoreTableRecord& s = k{S}Stores[n];
                const std::uint8_t lane_kind[2] = {{s.kind0, s.kind1}};
                const std::uint64_t lane_val[2] = {{s.value0, s.value1}};
                for (int ln = 0; ln < (s.width == 16 ? 2 : 1); ++ln) {{
                    std::uint64_t v = lane_val[ln];
                    ++kinds[lane_kind[ln]];
                    if (lane_kind[ln] == 1) v = reinterpret_cast<std::uintptr_t>(t.bytes) + v;
                    if (lane_kind[ln] == 2) v = reinterpret_cast<std::uintptr_t>(k{S}StoreStringBlob) + k{S}StoreStringOffset[v];
                    std::memcpy(want + s.offset + 8 * ln, &v, s.width == 4 ? 4 : 8);
                }}
            }}
            CHECK(std::memcmp(want, t.bytes, sizeof want) == 0);
            CHECK(kinds[0] == {_kc[0]} && kinds[1] == {_kc[1]} && kinds[2] == {_kc[2]} && kinds[3] == {_kc[3]});   // lane census of the ELF replay
            std::size_t covered = 0;
            for (std::size_t off = 0; off < sizeof want; ++off) {{
                std::uint8_t b = t.bytes[off];
                if (b != 0xa5) ++covered;
            }}
            CHECK(covered <= {len(_cov)});   // a written byte may equal the poison value (never the reverse): never more than the {len(_cov)} bytes the stores cover
            CHECK(k{S}Stores[0].insn_rva == {_f['addr']:#x} && k{S}Stores[k{S}StoreCount - 1].insn_rva == {_l['addr']:#x});
            for (std::size_t n = 0; n < k{S}StoreStringCount; ++n) {{
                const char* text = k{S}StoreStringBlob + k{S}StoreStringOffset[n];
                CHECK(std::strlen(text) != 0 && text[0] >= 0x20 && text[0] < 0x7f);   // every pooled string is a printable C string
            }}
            break;
        }}
""" if nstab else '')
    REC1DECL=(f'    bool rec_registry_seen = false;\n    const NamedRecord* rec1_ptr[k{S}Rec1Count] = {{}};\n    std::size_t rec1_seen = 0;\n' if nrec1 else '')
    REC1LOOP=(f"""        if (rows[i].kind == 'E') {{
            // The registrar initialises the registry on the first record and records its destructor 0xa81b80 once, BEFORE the record's own 0xa81950.
            std::size_t own = at;
            if (!rec_registry_seen) {{
                check_atexit(creg, at, kRecordRegistryDtorRva, &cell.registry);
                rec_registry_seen = true;
                ++reg_shift;
                own = at + 1;
            }}
            check_atexit(creg, own, dtor, rows[i].obj);
            CHECK(rec1_seen < k{S}Rec1Count);
            rec1_ptr[rec1_seen++] = static_cast<const NamedRecord*>(rows[i].obj);
            continue;
        }}
""" if nrec1 else '')
    REC1POST=(f"""    CHECK(rec1_seen == k{S}Rec1Count && cell.guard != 0);
    for (RecordPtrVec* vec : {{&cell.registry.first, &cell.registry.second}}) {{
        CHECK(static_cast<std::size_t>(vec->end - vec->begin) == k{S}Rec1Count && vec->cap >= vec->end);
        for (std::size_t q = 0; q < k{S}Rec1Count; ++q) CHECK(vec->begin[q] == rec1_ptr[q]);   // constructor order, in both vectors
    }}
    std::free(cell.registry.first.begin);
    std::free(cell.registry.second.begin);
""" if nrec1 else '')
    RC1EXPR=f'k{S}Rec1DtorRva' if nrec1 else '0'; LEXPR=f'k{S}ZeroCellDtorRva' if nzcell else '0'; QEXPR=f'k{S}EntryDtorRva' if nent else '0'
    RECFAIL=(f"""    {{
        // No registry cell: the records row is refused visibly; every row before it was built.
        ConCmdRegistry creg;
@M         DebugMenuRegistry menu;
        {S}Objects o;
        poison(o);
        o.record_registry = nullptr;
        o.record_alloc = default_slot{n}_record_alloc();
{EXTSET}        const {S}Result r = init_slot{n}_{rva:x}(o, creg{MENUA});
        CHECK(!r.ok && r.rows_done == {_ri});
    }}
    {{
        // No allocator: the first record's vector growth is refused (the original would fault), nothing is recorded for the records.
        ConCmdRegistry creg;
@M         DebugMenuRegistry menu;
        {S}Objects o;
        poison(o);
        RecordRegistryCell cell{{}};
        o.record_registry = &cell;
        o.record_alloc = AllocDispatchContract{{}};
{EXTSET}        const {S}Result r = init_slot{n}_{rva:x}(o, creg{MENUA});
        CHECK(!r.ok && r.rows_done == {_ri});
        CHECK(cell.registry.first.begin == nullptr);
    }}
""" if nrr else '')
    T=[f'''// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index {n} ({rva:#x}): {len(bools)} bool and {len(floats)} bounded float variables and {nitems} menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {{1, 10}}, float {{0.1, 0.5}}) are checked against the model.
#include "l4_test_util.hpp"

{"#include <cstdlib>"+chr(10)+chr(10) if (nrr or ntree) else ""}{"#include <cmath>"+chr(10)+chr(10) if npre else ""}#include "core/{stem}.hpp"{chr(10)+"#include "+chr(34)+"core/helper_f8f630_strncpy.hpp"+chr(34) if True else ""}

namespace {{
using namespace l4;

{EXTDEF}
struct Row {{
    char kind;            // b bool, i/l/n ranged / local / unbounded int, f/u bounded / unbounded float, m console command, t 0x26db0 toggle, e 0x26ea0 toggle, s 0x27120 slider, S 0x27050 slider, g group
    const void* obj;
    int var;              // index into the bool / float tables (variables)
    int target;           // row index of the target variable (items)
    int s0;               // menu string: parent / label
    int s1;               // menu string: explicit text / caption
    int slider;           // slider table index
    const void* ext;      // identity pointer of an external slider target (target == -1), otherwise nullptr
}};

Row rows_of({S}Objects& o, Row* out) {{
''']
    T.append('    Row r{};\n')
    for i,d in enumerate(rowinfo):
        k={'bool':'b','int':'i','ienum':'j','lint':'l','uint':'n','cmd':'m','str':'z','float':'f','ufloat':'u','tog':'t','toge':'e','slider':'s','slider50':'S','group':'g','cmdi':'x','cell':'c','ncell':'o','boolx':'y','holder':'h','sent':'q','recs':'r','zcont':'w','zq':'p','tree':'T','nref':'k','pre':'a','zvec':'v','sep':'K','ringhdr':'R','gettable':'G','dwr':'d','crit':'C','rec1':'E','zcell':'L','stab':'Z','hstab':'H','entry':'Q'}[d['kind']]
        if d['kind']=='uint' and rows[i].get('twin'): k='P' if twinb(rows[i]) else 'N'
        if d['kind']=='sep' and rows[i].get('fn')==0x26800: k='J'
        T.append(f'    out[{i}] = Row{{\'{k}\', &o.r{i:02d}, {d.get("v",-1)}, {d.get("t",-1)}, {d.get("s0",-1)}, {d.get("s1",-1)}, {sidx.get(i,-1)}, {("o.external_target_%d"%d["x"]) if "x" in d else ("&o.cell%02d"%i) if twinb(rows[i]) else "nullptr"}}};\n')
    T.append('    return r;\n}\n')
    T.append(f'''
[[maybe_unused]] float bits_to_float(std::uint32_t b) {{
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}}

[[maybe_unused]] std::uint64_t lint_dtor([[maybe_unused]] std::size_t var) {{
    std::uint64_t dtor = 0;
@L     dtor = find_local_int_var_class(k{S}LocalIntSites[var].wrapper_rva)->dtor_rva;
    return dtor;
}}

void section_constants() {{
    CHECK(k{S}Rva == {rva:#x} && k{S}Size == {size} && k{S}RowCount == {R});
@F     CHECK(is_known_float_arg_functor(k{S}FloatFnTag));
@I     CHECK(is_int_functor_tag(k{S}IntFnTag));
@L     for (const LocalIntSiteSpec& site : k{S}LocalIntSites) CHECK(find_local_int_var_class(site.wrapper_rva) != nullptr && (find_local_int_var_class(site.wrapper_rva)->fn_vtbl == site.arg_fn_tag || is_local_int_caller_twin_tag(site.arg_fn_tag, find_local_int_var_class(site.wrapper_rva)->fn_vtbl)));   // per-site tag = the wrapper's own class or its proven alias
@U     CHECK(is_float_functor_tag(k{S}UFloatFnTag));
{UINTTAGCHK}
@C     CHECK(sizeof(ConCmdCommand) == 0xa0);
{DWCONST}@V     CHECK(sizeof(ConCmdStringVar) == 0x148);
@V     for (std::size_t n = 0; n < k{S}StrVarCount; ++n) CHECK(std::strlen(k{S}StrNames[n]) == k{S}StrNameLengths[n]);
''')
    cur=0
    for k,i in enumerate(byaddr):
        if twinb(rows[i]):
            # address-tied: the external value dword at obj-8 and the 0xd0 B window; the relocated model follows the window
            T.append(f'    CHECK(offsetof({S}Objects, cell{i:02d}) == {rows[i]["cell"]-base0:#x} && offsetof({S}Objects, head{i:02d}) == {rows[i]["obj"]-base0:#x});\n')
            T.append(f'    CHECK(offsetof({S}Objects, r{i:02d}) == {rows[byaddr[-1]]["obj"]+szof(rows[byaddr[-1]])-base0:#x});   // relocated 0xd8 B model: right after the window\n')
            continue
        T.append(f'    CHECK(offsetof({S}Objects, r{i:02d}) == {rows[i]["obj"]-base0:#x});\n')
    _tot=(rows[byaddr[-1]]["obj"]+szof(rows[byaddr[-1]])-base0)+0xd8*len(twb)+8*len(exts)+(24 if nrr else 0)+(24 if nent else 0)+(8 if ntree else 0)+(8 if npre else 0)+(16 if nsep else 0)+((4*len(srcs)+7)//8*8 if srcs else 0)
    if nhst: _tot=(_tot+7)//8*8   # the 4-aligned hash window may end off an 8-byte boundary: the struct tail is padded to its 8-byte alignment
    T.append(f'''    CHECK(sizeof({S}Objects) == {_tot:#x});
{SRCCHK}
@B     for (std::size_t n = 0; n < k{S}BoolCount; ++n) CHECK(std::strlen(k{S}BoolNames[n]) == k{S}BoolNameLengths[n]);
@Y     for (const BoolVar274730SiteSpec& site : k{S}BoolXSites) CHECK(std::strlen(site.name) == site.name_length && site.q[3] != 0 && is_bool_functor_tag(site.arg_fn_tag));
@F     for (std::size_t n = 0; n < k{S}FloatCount; ++n) CHECK(std::strlen(k{S}FloatNames[n]) == k{S}FloatNameLengths[n]);
@U     for (std::size_t n = 0; n < k{S}UFloatCount; ++n) CHECK(std::strlen(k{S}UFloatNames[n]) == k{S}UFloatNameLengths[n]);
@I     for (std::size_t n = 0; n < k{S}IntCount; ++n) CHECK(std::strlen(k{S}IntNames[n]) == k{S}IntNameLengths[n]);
@L     for (const LocalIntSiteSpec& site : k{S}LocalIntSites) CHECK(std::strlen(site.name) == site.name_length);
@W     for (std::size_t n = 0; n < k{S}UIntCount; ++n) CHECK(std::strlen(k{S}UIntNames[n]) == k{S}UIntNameLengths[n]);
@C     for (std::size_t n = 0; n < k{S}CmdCount; ++n) CHECK(std::strlen(k{S}CmdNames[n]) == k{S}CmdNameLengths[n] && k{S}CmdHandler[n] != 0);
    for (std::size_t n = 0; n < k{S}StringCount; ++n) CHECK(std::strlen(k{S}MenuStrings[n]) == k{S}MenuStringLengths[n]);
}}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {{
    switch (row.kind) {{
@B         case 'b': {{
@B             const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
@B             if (k{S}BoolDesc[row.var] < 0) check_bool_with_description(v, k{S}BoolFnTag, k{S}BoolValues[row.var], "", 0);
@B             else check_bool_with_description(v, k{S}BoolFnTag, k{S}BoolValues[row.var], k{S}MenuStrings[k{S}BoolDesc[row.var]], k{S}MenuStringLengths[k{S}BoolDesc[row.var]]);
@B             CHECK(same_view(v.base.name, k{S}BoolNames[row.var], k{S}BoolNameLengths[row.var]));
@B             break;
@B         }}
@Y         case 'y': {{
@Y             const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
@Y             const BoolVar274730SiteSpec& site = k{S}BoolXSites[row.var];
@Y             CHECK(is_bool_functor_tag(site.arg_fn_tag));
@Y             CHECK(v.base.vptr == kBoolVarFinalVptr && v.value_ref == &v.value && v.base.registered == 1 && v.base.flags == 1);
@Y             for (std::uint8_t byte : v.zero69) CHECK(byte == 0);
@Y             std::uint64_t w[4] = {{}};
@Y             std::memcpy(w, v.zero70, sizeof w);
@Y             CHECK(w[0] == reinterpret_cast<std::uint64_t>(v.zero70) && w[1] == site.q[1] && w[2] == site.q[2] && w[3] == site.q[3]);   // q[0] = self reference (q[3] != 0)
@Y             CHECK(v.fn_vptr == kBoolVarCtorFinalFnClass && v.fn_payload == site.fn_payload && v.fn_aux == site.fn_aux);   // the net functor install: the family constant, not the site tag
@Y             CHECK(v.value == site.initial && v.default_value == site.initial);
@Y             CHECK(same_view(v.base.name, site.name, site.name_length) && empty_view(v.base.description));
@Y             check_untouched(v, {{{{0x00, 8}}, {{0x08, 0x10}}, {{0x18, 1}}, {{0x1c, 4}}, {{0x20, 0x40}}, {{0x60, 8}}, {{0x68, 1}}, {{0x69, 4}}, {{0x70, 0x20}}, {{0xa0, 8}}, {{0xb0, 8}}, {{0xb8, 8}}, {{0xc0, 1}}}});
@Y             break;
@Y         }}
@I         case 'i': {{
@I             const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
@I             CHECK(v.base.vptr == kRangedIntFinalVptr && v.base.flags == k{S}VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
@I             CHECK(same_view(v.base.name, k{S}IntNames[row.var], k{S}IntNameLengths[row.var]) && empty_view(v.base.description));
@I             CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(k{S}IntValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(k{S}IntValue[row.var]));
@I             CHECK(dword(v, 0x6c) == static_cast<std::uint32_t>(k{S}IntMin[row.var]) && v.min_engaged == 1 && dword(v, 0x74) == static_cast<std::uint32_t>(k{S}IntMax[row.var]) && v.max_engaged == 1);
@I             for (std::uint64_t q : v.args) CHECK(q == 0);
@I             CHECK(v.fn_vptr == kRangedIntFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
@I             break;
@I         }}
@L         case 'l': {{
@L             const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
@L             const LocalIntSiteSpec& site = k{S}LocalIntSites[row.var];
@L             const LocalIntVarClass* cls = find_local_int_var_class(site.wrapper_rva);
@L             CHECK(cls != nullptr && (site.arg_fn_tag == cls->fn_vtbl || is_local_int_caller_twin_tag(site.arg_fn_tag, cls->fn_vtbl)));
@L             CHECK(v.base.vptr == cls->final_vptr && v.base.flags == site.flags && v.base.registered == 1 && v.value_ref == &v.value);
@L             CHECK(same_view(v.base.name, site.name, site.name_length));
@L             if (k{S}LintDesc[row.var] == nullptr) CHECK(empty_view(v.base.description));
@L             else CHECK(same_view(v.base.description, k{S}LintDesc[row.var], k{S}LintDescLength[row.var]));
@L             CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(site.value) && dword(v, 0x68) == static_cast<std::uint32_t>(site.value));
@L             CHECK(dword(v, 0x6c) == static_cast<std::uint32_t>(site.min_value) && v.min_engaged == 1 && dword(v, 0x74) == static_cast<std::uint32_t>(site.max_value) && v.max_engaged == 1);
@L             for (std::uint64_t q : v.args) CHECK(q == 0);
@L             CHECK(v.fn_vptr == cls->fn_vtbl && v.fn_payload == 0 && v.fn_aux == 0);
@L             break;
@L         }}
@F         case 'f': {{
@F             const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
@F             std::uint32_t lo = 0;
@F             std::uint32_t hi = 0;
@F             std::memcpy(&lo, v.min_value, 4);
@F             std::memcpy(&hi, v.max_value, 4);
@F             CHECK(v.base.flags == k{S}VarFlags && v.base.registered == 1 && same_view(v.base.name, k{S}FloatNames[row.var], k{S}FloatNameLengths[row.var]));
@F             CHECK(v.value == k{S}FloatValueBits[row.var] && v.default_value == k{S}FloatValueBits[row.var] && v.min_engaged == 1 && v.max_engaged == 1);
@F             CHECK(lo == k{S}FloatLoBits[row.var] && hi == k{S}FloatHiBits[row.var]);
@F             break;
@F         }}
@U         case 'u': {{
@U             const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
@U             CHECK(v.base.vptr == kRangedFloatFinalVptr && v.value_ref == &v.value && v.base.registered == 1 && v.base.flags == k{S}VarFlags);
@U             CHECK(v.value == {UEXP} && v.default_value == {UEXP} && v.min_engaged == 0 && v.max_engaged == 0);
@U             CHECK(same_view(v.base.name, k{S}UFloatNames[row.var], k{S}UFloatNameLengths[row.var]));
@U             if (k{S}UFloatDesc[row.var] < 0) CHECK(empty_view(v.base.description));
@U             else CHECK(same_view(v.base.description, k{S}MenuStrings[k{S}UFloatDesc[row.var]], k{S}MenuStringLengths[k{S}UFloatDesc[row.var]]));
@U             CHECK(v.fn_vptr == kRangedFloatFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
@U             break;
@U         }}
@W         case 'n': case 'N': case 'P': {{
@W             const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
@W             CHECK(v.base.vptr == {NFINAL} && v.base.flags == k{S}VarFlags && v.base.registered == 1 && {NVREF});
@W             CHECK(same_view(v.base.name, k{S}UIntNames[row.var], k{S}UIntNameLengths[row.var]));
{UINTCHK}
@W             CHECK({NVAL} == static_cast<std::uint32_t>(k{S}UIntValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(k{S}UIntValue[row.var]));
@W             CHECK(v.min_engaged == 0 && v.max_engaged == 0);
@W             CHECK(v.fn_vptr == {NFNTAG} && v.fn_payload == 0 && v.fn_aux == 0);
@W             break;
@W         }}
@C         case 'm': {{
@C             const auto& c = *static_cast<const ConCmdCommand*>(row.obj);
@C             check_command(c, k{S}CmdNames[row.var], k{S}MenuStrings[k{S}CmdDesc[row.var]], k{S}MenuStringLengths[k{S}CmdDesc[row.var]], k{S}CmdHandler[row.var]);
@C             break;
@C         }}
@V         case 'z': {{
@V             const auto& v = *static_cast<const ConCmdStringVar*>(row.obj);
@V             const std::int32_t d = k{S}StrInit[row.var];
@V             CHECK(v.base.vptr == kStringVarFinalVptr && v.base.flags == kStringVarFlags && v.base.registered == 1 && v.value_ref == &v.value);
@V             CHECK(same_view(v.base.name, k{S}StrNames[row.var], k{S}StrNameLengths[row.var]) && empty_view(v.base.description));
@V             if (d < 0) CHECK(v.default_value.data == empty_string_f730().data && v.default_value.size == 0 && empty_view(v.default_value) && empty_view(v.value) && v.value.data == empty_string_f730().data);
@V             else CHECK(same_view(v.default_value, k{S}MenuStrings[d], k{S}MenuStringLengths[d]) && same_view(v.value, k{S}MenuStrings[d], k{S}MenuStringLengths[d]));
@V             CHECK(v.byte88 == 0 && v.bytea8 == 0 && v.byteb0 == 0 && v.byted0 == 0);
@V             for (std::uint64_t q : v.args) CHECK(q == 0);
@V             CHECK(v.fn_vptr == kStringVarFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
@V             break;
@V         }}
        case 'c': {{
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == k{S}MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(k{S}MenuStrings[row.s0]));
            CHECK(c.unwritten == 0xa5a5a5a5u);   // +0x0c is never written
            break;
        }}
        case 'o': {{
            const auto& c = *static_cast<const NullNameCell247f47*>(row.obj);
            CHECK(c.id == 0 && c.name[0] == 'N' && c.name[1] == 'U' && c.name[2] == 'L' && c.name[3] == 'L');
            for (int b = 4; b < 16; ++b) CHECK(c.name[b] == 0);   // strncpy pads with NULs up to 15 bytes; byte 0x13 is stored 0
            break;
        }}
        case 'q': {{
@Q            const auto& q = *static_cast<const {S}Sentinel*>(row.obj);
@Q            const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(&q);
@Q            CHECK(q.q00 == base + 0x28 && q.q08 == base + 0x28 && q.q20 == base + 0x28 && q.q10 == base + 0xa8);
@Q            CHECK(q.q18 == 0xa5a5a5a5a5a5a5a5ull);   // never written
@Q            for (std::uint8_t b : q.area) CHECK(b == 0xa5);   // never written
            break;
        }}
{DCASE}{CCASE}{RC1CASE}{ZTCASE}{HTCASE}{LCASE}{QCASE}{RCASE}{RGCASE}{GCASE}{WCASE}{VCASE}{ACASE}{PCASE}{TCASE}{JCASE}        case 'h': {{
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
{HOLDCHK}
            break;
        }}
        default: {{
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == ((row.kind == 'g' || row.kind == 'J') ? static_cast<std::uint32_t>(row.var) : k{S}ItemFlags) && it.registered == 0);
            CHECK(same_view(it.label, k{S}MenuStrings[row.s0], k{S}MenuStringLengths[row.s0]));
            if (row.kind == 'x') {{
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, k{S}MenuStrings[row.s1], k{S}MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, k{S}MenuStrings[row.var], k{S}MenuStringLengths[row.var]));   // +0x70: the console command name
                CHECK(it.range_min == 1.0f && it.range_max == 10.0f);
                break;
            }}
            if (row.kind == 'J') {{
                CHECK(it.kind == 0 && it.target == nullptr && it.flags == static_cast<std::uint32_t>(row.var));   // 0x26800 stores kind 3, then overwrites it with 0
                CHECK(it.text.size == 13 && it.text.data != nullptr && std::memcmp(it.text.data, "MenuSeparator", 13) == 0);   // built by 0xf1a0 (heap copy)
                CHECK(same_view(it.label, k{S}MenuStrings[row.s0], k{S}MenuStringLengths[row.s0]));
                CHECK(it.range_min == 1.0f && it.range_max == 10.0f);
                break;
            }}
            if (row.kind == 'K') {{
                CHECK(it.kind == kMenuKindSeparator && it.target == nullptr);
                CHECK(it.text.size == 13 && it.text.data != nullptr && std::memcmp(it.text.data, "MenuSeparator", 13) == 0);   // built by 0xf1a0 (heap copy)
                break;
            }}
            if (row.kind == 'g') {{
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, k{S}MenuStrings[row.s1], k{S}MenuStringLengths[row.s1]));
                break;
            }}
@X             if (row.target < 0) {{
@X                 CHECK(row.kind == 'S' && row.ext != nullptr && it.kind == 2 && it.target == row.ext);   // external target: identity pointer only
@X                 CHECK(same_view(it.text, k{S}MenuStrings[row.s1], k{S}MenuStringLengths[row.s1]));
@X{SLIDCHK}
@X                 break;
@X             }}
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = (tg.kind == 'b' || tg.kind == 'y') ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {{
                CHECK(same_view(it.text, k{S}MenuStrings[row.s1], k{S}MenuStringLengths[row.s1]));
            }} else {{
                CHECK(it.text.data == tb.name.data && it.text.size == tb.name.size && it.text.ctl == nullptr);   // the variable's own name view
            }}
            if (row.kind == 's' || row.kind == 'S') {{
{SLIDCHK}
            }} else if (is_float) {{
                CHECK(it.range_min == 0.1f && it.range_max == 0.5f);   // float class slot3 = 1, slot6 = 3: the one narrowing case
            }} else {{
                CHECK(it.range_min == 1.0f && it.range_max == 10.0f);
            }}
            break;
        }}
    }}
}}

void section_alone() {{
    ConCmdRegistry creg;
@M     DebugMenuRegistry menu;
    {S}Objects o;
    poison(o);
{EXTSET}{RECSET}    const {S}Result r = init_slot{n}_{rva:x}(o, creg{MENUA});
    CHECK(r.ok && r.rows_done == k{S}RowCount);
    Row rows[k{S}RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < k{S}RowCount; ++i) {{
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'y' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'N' || kd == 'P' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u' || kd == 'd') ++vars;
        else if (kd == 'c' || kd == 'o' || kd == 'a' || kd == 'G' || kd == 'H') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'v' && kd != 'p' && kd != 'T' && kd != 'k' && kd != 'R' && kd != 'G' && kd != 'C' && kd != 'E' && kd != 'L' && kd != 'Q' && kd != 'Z' && kd != 'H') ++items;
    }}
    CHECK(vars == k{S}BoolCount{BXVARS}{DWVARS} + k{S}IntCount + k{S}LocalIntCount + k{S}UIntCount + k{S}CmdCount + k{S}StrVarCount + k{S}FloatCount + k{S}UFloatCount{ENUMCNT});
@M     CHECK(menu.root.count == items && items == {nitems} && menu.root.link.next == &o.r{firstitem:02d}.link);
@N     CHECK(items == 0);
    {PENDCHK}
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
@M     CHECK(cells == k{S}CellCount && creg.atexit_log.size() == k{S}RowCount - cells + 1{ATX});   // name cells record nothing
@N     CHECK(cells == k{S}CellCount && creg.atexit_log.size() == k{S}RowCount - cells{ATX});   // name cells record nothing; no menu root
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
{REC1DECL}    for (std::size_t i = 0; i < k{S}RowCount; ++i) {{
        if (rows[i].kind == 'c' || rows[i].kind == 'o' || rows[i].kind == 'a' || rows[i].kind == 'G' || rows[i].kind == 'H') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = (rows[i].kind == 'b' || rows[i].kind == 'y') ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : (rows[i].kind == 'N' || rows[i].kind == 'P') ? {NDEXPR} : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? {HDEXPR} : rows[i].kind == 'q' ? {SDEXPR} : rows[i].kind == 'r' ? {RDEXPR} : rows[i].kind == 'w' ? {WDEXPR} : rows[i].kind == 'v' ? {VDEXPR} : rows[i].kind == 'p' ? {ZPEXPR} : rows[i].kind == 'Z' ? {ZTEXPR} : rows[i].kind == 'j' ? {JDEXPR} : rows[i].kind == 'T' ? {TDEXPR} : rows[i].kind == 'k' ? {KDEXPR} : rows[i].kind == 'R' ? {GDEXPR} : rows[i].kind == 'd' ? {DWEXPR} : rows[i].kind == 'C' ? {CREXPR} : rows[i].kind == 'E' ? {RC1EXPR} : rows[i].kind == 'L' ? {LEXPR} : rows[i].kind == 'Q' ? {QEXPR} : 0x272d0;
{RLOOP}{REC1LOOP}
@M         if (dtor == 0x272d0 && !root_seen) {{
@M             check_atexit(creg, at, 0x27280, &menu.root);
@M             root_seen = true;
@M             check_atexit(creg, at + 1, dtor, rows[i].obj);
@M         }} else {{
@M             check_atexit(creg, at, dtor, rows[i].kind == 'Z' ? nullptr : rows[i].obj);
@M         }}
@N         check_atexit(creg, at, dtor, rows[i].kind == 'Z' ? nullptr : rows[i].obj);
    }}
{REC1POST}}}

void section_failures() {{
    {{
        {FAILBLK}
    }}
@M     {{
@M         // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
@M         ConCmdRegistry creg;
@M         DebugMenuRegistry menu;
@M         int token = 0;
@M         menu.parent.parent = &token;
@M         {S}Objects o;
@@RECSETM@@@M         const {S}Result r = init_slot{n}_{rva:x}(o, creg, menu);
@M         CHECK(!r.ok && r.rows_done == {firstitem});
@M     }}
{EXTNULL}{RECFAIL}{TREEFAIL}{PREFAIL}}}

}}  // namespace

int main() {{
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_{n}");
    return 0;
}}
''')
    CELLRESET=''.join(f' o.cell{i:02d} = {rows[i]["cell_image"]:#x};' for i in twb)   # twin B: the external value dword is an INPUT; poison() would turn it into 0xa5a5a5a5
    ENTRYTEST=(f"""// Entries with the registry object present (the later startup state): each entry is registered under its name (strcasecmp order) with the entry itself as the mapped value; without an allocator
// the first registration is refused visibly. The globals 0x2ef63f8 / 0x2ef6400 are process state and are reset afterwards.
struct EntryAllocLog {{
    std::vector<void*> blocks;
}};
EntryAllocLog g_entry_log;
void* entry_alloc(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) {{
    void* p = std::malloc(size);
    g_entry_log.blocks.push_back(p);
    return p;
}}
bool entry_free(void* p, std::uint32_t) {{
    std::free(p);
    for (void*& b : g_entry_log.blocks) if (b == p) b = nullptr;
    return true;
}}
void entry_log_release() {{
    for (void* p : g_entry_log.blocks) std::free(p);
    g_entry_log.blocks.clear();
}}

void section_entries() {{
    {{
        ConCmdRegistry creg;
@M         DebugMenuRegistry menu;
        {S}Objects o;
        poison(o);
        RbMap map{{}};
        o.entry_hooks = RbHooks{{}};
        o.entry_hooks.alloc.allocate = &entry_alloc;
        o.entry_hooks.alloc.deallocate = &entry_free;
        CHECK(rb_map_init_empty(map, o.entry_hooks));
        ctor_98a90_self_cell_2ef63f8() = &map;
        ctor_98a90_list_head_cell_2ef6400() = nullptr;
{EXTSET}        const {S}Result r = init_slot{n}_{rva:x}(o, creg{MENUA});
        CHECK(r.ok && r.rows_done == k{S}RowCount);
        CHECK(map.size == k{S}EntryCount && rb_check_invariants(map).ok);
        Row rows[k{S}RowCount];
        (void)rows_of(o, rows);
        for (std::size_t i = 0; i < k{S}RowCount; ++i) {{
            if (rows[i].kind != 'Q') continue;
            const std::size_t v = static_cast<std::size_t>(rows[i].var);
            const RbIndexResult found = rb_index_3c0840(map, family_view(k{S}EntryNames[v], k{S}EntryNameLengths[v]), o.entry_hooks);
            CHECK(found.ok && !found.created && found.slot != nullptr && *found.slot == rows[i].obj);
        }}
        CHECK(map.size == k{S}EntryCount);
        ctor_98a90_self_cell_2ef63f8() = nullptr;
        ctor_98a90_list_head_cell_2ef6400() = nullptr;
        entry_log_release();
    }}
    {{
        ConCmdRegistry creg;
@M         DebugMenuRegistry menu;
        {S}Objects o;
        poison(o);
        RbMap map{{}};
        RbHooks init_hooks{{}};
        init_hooks.alloc.allocate = &entry_alloc;
        init_hooks.alloc.deallocate = &entry_free;
        CHECK(rb_map_init_empty(map, init_hooks));
        o.entry_hooks = RbHooks{{}};   // no allocator: the first registration cannot create its node
        ctor_98a90_self_cell_2ef63f8() = &map;
        ctor_98a90_list_head_cell_2ef6400() = nullptr;
{EXTSET}        const {S}Result r = init_slot{n}_{rva:x}(o, creg{MENUA});
        CHECK(!r.ok && r.rows_done == {ents[0] if nent else 0});
        CHECK(map.size == 0);
        ctor_98a90_self_cell_2ef63f8() = nullptr;
        ctor_98a90_list_head_cell_2ef6400() = nullptr;
        entry_log_release();
    }}
}}

""" if nent else '')
    _tj=''.join(T)
    if nent:
        _tj=_tj.replace('#include "l4_test_util.hpp"\n','#include "l4_test_util.hpp"\n\n#include <cstdlib>\n#include <vector>\n',1)
        _mk='}  // namespace\n\nint main() {'
        assert _tj.count(_mk)==1, 'test main marker'
        _tj=_tj.replace(_mk,ENTRYTEST+_mk,1).replace('    section_failures();\n    std::puts','    section_failures();\n    section_entries();\n    std::puts',1)
    test=cond(_tj.replace('poison(o);','poison(o);'+CELLRESET).replace('@@RECSET@@',RECSET).replace('@@RECSETM@@',''.join('@M '+l+chr(10) for l in RECSET.splitlines())).replace('@@TREESET@@','        o.tree_alloc.allocate_17070 = &test_tree_alloc;\n' if ntree else ''),bool(bools),bool(floats),bool(ufl),bool(ints),not menu_less,bool(lints),bool(uints),bool(cmds),bool([i for i in sliders if rows[i]['kind']=='slider50']),bool(strv),hq=bool(nsent),hx=bool(exts),hy=bool(bx))
    open(ROOT+f'native/core/{stem}.hpp','w').write(header)
    open(ROOT+f'native/core/{stem}.cpp','w').write(cpp)
    open(ROOT+f'native/tests/test_ctor_slot_{n}.cpp','w').write(test)
    # ---------------- checker
    C_={f'k{S}Rva':rva,f'k{S}Size':size,f'k{S}BoolFnTag':btag,f'k{S}FloatFnTag':ftag,f'k{S}ItemFlags':0x30,f'k{S}VarFlags':1,f'k{S}BoolCount':len(bools),f'k{S}FloatCount':len(floats),f'k{S}UFloatFnTag':utag,f'k{S}UFloatCount':len(ufl),f'k{S}IntFnTag':itag,f'k{S}IntCount':len(ints),f'k{S}LocalIntCount':len(lints),f'k{S}UIntFnTag':wtag,f'k{S}UIntCount':len(uints),f'k{S}CmdCount':len(cmds),f'k{S}StrVarCount':len(strv),
        f'k{S}RowCount':R,f'k{S}StringCount':len(strs),f'k{S}SliderCount':len(sliders),f'k{S}Object0Rva':obj0,f'k{S}CellCount':ncells,f'k{S}HolderCount':nhold,f'k{S}SentinelCount':nsent}
    if nsent: C_[f'k{S}SentinelDtorRva']=SENT_DTOR
    if bx: C_[f'k{S}BoolXCount']=len(bx)
    if dwrs: C_[f'k{S}DwrCount']=len(dwrs)
    if crits: C_.update({f'k{S}CritCount':len(crits),f'k{S}CritStoreCount':sum(len(rows[i]['stores']) for i in crits)})
    if nrecs: C_.update({f'k{S}RecsCount':nrecs,f'k{S}RecordCount':NREC,f'k{S}RecsDtorRva':RECS['dtor']})
    if srcs: C_[f'k{S}UFloatSourceCount']=len(srcs)
    if ngt: C_[f'k{S}MemberCount']=GT['n']
    if nring: C_.update({f'k{S}RingCount':nring,f'k{S}RingDtorRva':0x354470,f'k{S}RingHeaderRva':0x1f220})
    if nzc: C_.update({f'k{S}ZeroCount':nzc,f'k{S}ZeroDtorRva':0x1c5090})
    if nrec1: C_.update({f'k{S}Rec1Count':nrec1,f'k{S}Rec1DtorRva':0xa81950})
    if nhst: C_.update({f'k{S}HashStoreCount':HST['n'],f'k{S}HashTableSize':HST['size'],f'k{S}HashTableRva':HST['obj'],f'k{S}HashStringCount':len(HST['strs']),f'k{S}HashFirstCallRva':HST['stores'][0]['call'],f'k{S}HashLastStoreRva':HST['stores'][-1]['addr']})
    if nstab: C_.update({f'k{S}StoreCount':len(STAB['stores']),f'k{S}StoreTableSize':STAB['size'],f'k{S}StoreTableRva':STAB['obj'],f'k{S}StoreDtorRva':STAB['dtor'],f'k{S}StoreStringCount':len(STAB['strs']),f'k{S}StoreFirstInsnRva':STAB['stores'][0]['addr'],f'k{S}StoreLastInsnRva':STAB['stores'][-1]['addr']})
    if nent: C_.update({f'k{S}EntryCount':nent,f'k{S}EntryDtorRva':0x3c15e0,f'k{S}EntrySize':0x90,f'k{S}EntryListHeadCellRva':0x2ef6400,f'k{S}EntrySelfCellRva':0x2ef63f8})
    if nzcell: C_.update({f'k{S}ZeroCellCount':nzcell,f'k{S}ZeroCellDtorRva':zcdtor,f'k{S}ZeroCellWords':zcwords})
    if nzq: C_.update({f'k{S}ZeroPtrCount':nzq,f'k{S}ZeroPtrDtorRva':ZQ_DTOR})
    if ienums: C_.update({f'k{S}EnumCount':len(ienums),f'k{S}EnumMidVptr':emid,f'k{S}EnumFinalVptr':efinal,f'k{S}EnumFnTag':etag,f'k{S}EnumDtorRva':edtor,f'k{S}EnumBounded':int(ebounded),f'k{S}EnumSlot3Result':1,f'k{S}EnumSlot6Result':1})
    if ntree: C_.update({f'k{S}TreeCount':ntree,f'k{S}TreeNodeSize':TREE_NODE,f'k{S}TreeDtorRva':TREE_DTOR})
    if nnref: C_.update({f'k{S}NodeRefCount':nnref,f'k{S}NodeRefDtorRva':0x560540})
    for hi,(hvp,hdt) in enumerate(hcl): C_.update({f'k{S}HolderVptr{HSFX[hi]}':hvp,f'k{S}HolderDtorRva{HSFX[hi]}':hdt})
    htoks=[lit(rows[i]['name']) for i in bools+bx+floats+ufl+ienums+lints+uints+cmds+strv+dwrs]+[lit(s) for s in strs]
    htoks+=[l.split('inline constexpr ',1)[1].rstrip(';') for l in tline]+['IDENTITY TAGS']
    # order tokens: textual order in cpp
    src_tokens=[]
    s50=[i for i in sliders if rows[i]['kind']=='slider50']; s27120=[i for i in sliders if rows[i]['kind']=='slider']
    if s27120: src_tokens+=[f'init_debug_menu_item_27120(item, view, var.base, k{S}ItemFlags, lo, hi)','register_debug_menu_item_268b0(item, menu, concmd.atexit_log)','release_inert_view_f590(view)','concmd.atexit_log.record(kDebugMenuItemDtorRva, &item)']
    if s50: src_tokens+=['init_debug_menu_item_27050(item, label_view, text_view, target, k'+S+'ItemFlags, lo, hi)','release_inert_view_f590(text_view) || !release_inert_view_f590(label_view)']
    if uints: src_tokens.append('helper_init_unbounded_int_var_site(var, spec, concmd)')
    if strv: src_tokens.append('init_string_var_257a0(var, name, init, concmd)')
    if cmds: src_tokens.append('init_fconcmd_object(command, spec, concmd)')
    if floats: src_tokens.append('init_float_var_static(var, spec, concmd)')
    if srcs: src_tokens.append('source_bits[src]')
    if ufl: src_tokens.append('helper_init_unbounded_float_var_site(var, spec, concmd)')
    if ints: src_tokens.append('helper_init_ranged_int_var_site(var, spec, concmd)')
    if lints: src_tokens.append('helper_init_local_int_var_site(var, k'+S+'LocalIntSites[n], view, concmd)')
    if bools: src_tokens.append('lane_m6::init_bool_var_literal(var, ')
    if bx: src_tokens.append('helper_init_bool_var_274730_site(var, k'+S+'BoolXSites[n], concmd)')
    if ienums: src_tokens.append('init_inline_enum_var(var, spec, concmd)')
    if nreal: src_tokens.append('cell.hash = fnv_cstr_52a990(name)')
    if nnc: src_tokens.append('store_null_name_cell_247f47(cell)')
    if nhold: src_tokens+=['holder.vptr = vptr','concmd.atexit_log.record(dtor, &holder)']
    if nsent: src_tokens+=['c.q20 = base + 0x28','c.q10 = base + 0xa8','concmd.atexit_log.record(dtor, &c)']
    if ngt: src_tokens+=['member_hash_getter_498(k'+S+'MemberLoRva[k], lo)','b.dw[3 * k + 2] = k'+S+'MemberSize[k]']
    if nring: src_tokens+=['ring_header_init_1f220(&c)','concmd.atexit_log.record(k'+S+'RingDtorRva, &c)']
    if nrecs: src_tokens+=['register_named_record_a81700(*cell, recs.rec[n], view, k'+S+'RecKind[n], k'+S+'RecFlag[n], alloc)','concmd.atexit_log.record(kRecordRegistryDtorRva, &cell->registry)','concmd.atexit_log.record(k'+S+'RecsDtorRva, &recs)']
    if nrec1: src_tokens+=['register_named_record_a81700(*cell, rec, view, k'+S+'Rec1Kind[n], k'+S+'Rec1Flag[n], alloc)','concmd.atexit_log.record(kRecordRegistryDtorRva, &cell->registry)','concmd.atexit_log.record(k'+S+'Rec1DtorRva, &rec)']
    if nzc: src_tokens+=['c.q08 = 0','concmd.atexit_log.record(dtor, &c)']
    if nhst: src_tokens+=['replay_hash_stores(t.bytes, sizeof t.bytes, spec)']
    if nstab: src_tokens+=['replay_store_table(t.bytes, sizeof t.bytes, spec)','concmd.atexit_log.record(k'+S+'StoreDtorRva, nullptr)']
    if nent: src_tokens+=['put(0x00, self88)','put(0x48, k'+S+'EntryFns[3 * n + 2])','head_cell = e.b','rb_register_entry_3be2d0(*static_cast<RbMap*>(registry), e.b, hooks)','concmd.atexit_log.record(k'+S+'EntryDtorRva, &e)']
    if nzcell: src_tokens+=['c.q00 = 0','concmd.atexit_log.record(k'+S+'ZeroCellDtorRva, &c)']
    if dwrs: src_tokens.append('helper_init_ranged_dword_var_site(var, spec, concmd)')
    if crits: src_tokens+=['critsec_init_at(c.b) != 0','concmd.atexit_log.record(k'+S+'CritDtorRva[n], &c)']
    if nzq: src_tokens+=['slot = 0','concmd.atexit_log.record(dtor, &slot)']
    src_tokens+= [(f'build_empty_tree(objects.r{i:02d}' if rowinfo[i]['kind']=='tree' else f'build_node_ref_row(objects.r{i:02d}' if rowinfo[i]['kind']=='nref' else f'objects.r{i:02d}') for i in range(R)]
    ttoks=[f'k{S}Rva == {rva:#x} && k{S}Size == {size}','section_constants()','section_alone()','section_failures()']+([f'CHECK(menu.root.count == items && items == {nitems}',f'creg.atexit_log.size() == k{S}RowCount - cells + 1{ATX});',f'r.rows_done == {firstitem}'] if not menu_less else ['CHECK(items == 0);',f'creg.atexit_log.size() == k{S}RowCount - cells{ATX});',(f'r.rows_done == {allvars[0]}' if allvars else 'CHECK(r.ok && creg.pending.size() == 0);')])
    native={'header':f'native/core/{stem}.hpp','source':f'native/core/{stem}.cpp','test':f'native/tests/test_ctor_slot_{n}.cpp','consts':C_,'header_tokens':htoks,'source_order':src_tokens,'test_tokens':ttoks}
    return dict(strv=strv,wtag=wtag,uints=uints,cmds=cmds,n=n,rva=rva,size=size,stem=stem,S=S,rows=rows,atexit=atexit,btag=btag,ftag=ftag,utag=utag,itag=itag,ints=ints,lints=lints,lsites=LSITES,menu_less=menu_less,native=native,bools=bools,floats=floats,sliders=sliders,
                kinds={k:sum(1 for r in rows if r['kind']==k) for k in SZ},bxtags=bxtags,nitems=nitems,firstitem=firstitem)
