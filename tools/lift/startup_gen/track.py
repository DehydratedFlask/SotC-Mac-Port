import re, struct, sys
import pathlib as _pl
_HERE=str(_pl.Path(__file__).resolve().parent)
sys.path.insert(0,_HERE)
from rows import *
M64=(1<<64)-1
def issp(p): return isinstance(p,tuple) and p[0]=='sp'
def cstr_z(a):
    b=bytearray()
    while True:
        c=e.raw(a+len(b),1)[0]
        if c==0: return bytes(b)
        b.append(c)
REG64=['rax','rbx','rcx','rdx','rsi','rdi','rbp','rsp','r8','r9','r10','r11','r12','r13','r14','r15']
def regname(r):
    r=r.lstrip('%')
    m={'eax':'rax','ebx':'rbx','ecx':'rcx','edx':'rdx','esi':'rsi','edi':'rdi','r8d':'r8','r9d':'r9','r10d':'r10','r11d':'r11','r12d':'r12','r13d':'r13','r14d':'r14','r15d':'r15'}
    if r in REG64: return r
    return m.get(r)
class T:
    def __init__(s):
        s.r={}; s.x={}; s.mem=[]  # mem: list of (space,off,size,val) latest last; space 'sp' or 'abs'
        s.sp=0; s.events=[]; s.started=False
    def val(s,op,ins):
        op=op.strip()
        if op.startswith('$'): return int(op[1:],16) if not op.startswith('$-') else (int(op[2:],16)*-1)&M64
        rn=regname(op) if op.startswith('%') else None
        if rn: return s.r.get(rn)
        return None
    def addr(s,op,ins):
        # returns (space, off) for memory operand or None
        op=op.split('#')[0].strip()
        if '(%rip)' in op: return ('abs', ins['rip']) if ins['rip'] is not None else None
        m=re.match(r'^(-?0x[0-9a-f]+|-?\d+)?\((%\w+)\)$',op)
        if not m: return None
        d=int(m.group(1),16) if m.group(1) else 0
        base=s.r.get(regname(m.group(2)))
        if base is None: return None
        if isinstance(base,tuple): return ('sp', base[1]+d) if base[0]=='sp' else None
        return ('abs', (base+d)&M64)
    def write(s,a,size,v):
        if a is None: return
        s.mem.append((a[0],a[1],size,v))
    def read(s,a,size):
        if a is None: return None
        sp,off=a
        for (sp2,o2,sz2,v) in reversed(s.mem):
            if sp2!=sp: continue
            if o2==off and sz2==size: return v
            if o2<=off and off+size<=o2+sz2:
                if v==0: return 0
                if sz2>size and isinstance(v,int) and o2==off: return v&((1<<(8*size))-1)
                if isinstance(v,tuple): return None
                if v is None: return None   # a covering store of an unknown value shadows every older store (never fall through to a stale one)
        return None
    def view(s,p):
        # SharedStr on stack: ptr at +0, len at +8
        if issp(p):
            a=('sp',p[1])
            return (s.read(a,8), s.read(('sp',p[1]+8),8))
        return None
def run(idx):
    rva=e.u64(g.TABLE_TOP-8*idx); size=fde(rva)
    ins=disasm(rva,size)
    t=T()
    t.r['rsp']=('sp',0)
    ev=[]
    # frames without a realigning `andq $-N,%rsp` (the rbp-frame bodies of the lower slots) start at the first instruction
    if not any(x['mnem']=='andq' and x['ops'].rstrip().endswith('%rsp') for x in ins): t.started=True
    for i in ins:
        m=i['mnem']; ops=i['ops'].split('#')[0].strip()
        parts=[p.strip() for p in re.split(r',(?![^()]*\))',ops)] if ops else []
        def setr(name,v):
            n=regname(name); 
            if n: t.r[n]=v
        if m=='andq' and parts[-1]=='%rsp': t.r['rsp']=('sp',0); t.started=True; continue
        if not t.started: continue
        if m in('subq','addq') and len(parts)==2:
            rn=regname(parts[1]); v=t.val(parts[0],i)
            if rn:
                cur=t.r.get(rn)
                if rn=='rsp': 
                    d=v if v is not None else 0
                    t.r['rsp']=('sp',cur[1]+(-d if m=='subq' else d)); continue
                if v is None or cur is None: t.r[rn]=None
                elif issp(cur): t.r[rn]=('sp',cur[1]+(v if m=='addq' else -v))
                elif isinstance(cur,tuple) or isinstance(v,tuple): t.r[rn]=None   # a symbolic (non-stack) value plus a constant is not tracked: unresolved, never guessed
                else: t.r[rn]=(cur+v if m=='addq' else cur-v)&M64
            continue
        if m=='pushq':
            cur=t.r['rsp']; t.r['rsp']=('sp',cur[1]-8); t.write(('sp',cur[1]-8),8,t.val(parts[0],i)); continue
        if m.startswith('lea') and parts[1]=='%rsp': break
        if m.startswith('lea'):
            rn=regname(parts[1]); 
            a=t.addr(parts[0],i)
            if '(%rip)' in parts[0]: t.r[rn]=i['rip']
            elif a is None: t.r[rn]=None
            else: t.r[rn]=('sp',a[1]) if a[0]=='sp' else a[1]
            continue
        if m in('xorl','xorq') and len(parts)==2 and parts[0]==parts[1] and parts[0].startswith('%') and regname(parts[0]):
            t.r[regname(parts[0])]=0; continue   # zeroing idiom: a known constant
        if m in('movq','movl','movb','movw') and len(parts)==2:
            sz={'movq':8,'movl':4,'movb':1,'movw':2}[m]
            src,dst=parts
            if dst.startswith('%'):
                rn=regname(dst)
                if src.startswith('%') or src.startswith('$'):
                    v=t.val(src,i)
                    if m=='movl' and isinstance(v,int): v&=0xffffffff
                    t.r[rn]=v
                else:
                    a=t.addr(src,i); t.r[rn]=t.read(a,sz) if a else None
                    if t.r[rn] is None and a is not None and a[0]=='abs' and sz==8: t.r[rn]=('ld',a[1])   # symbolic load [abs] (register-once slot provenance)
                    elif t.r[rn] is None and a is not None and a[0]=='abs' and sz==4 and m=='movl': t.r[rn]=('mut',a[1],4)   # 4-byte load of a global the body never wrote: its run-time value (kept symbolic, never read from the image here)
                    if '(%rip)' in src: t.r[rn]=None
            else:
                a=t.addr(dst,i); t.write(a,sz,t.val(src,i))
            continue
        if m in('vxorps','xorps','pxor','vpxor') and len(parts)==3:
            t.x[parts[2].lstrip('%')]=('zero',); continue
        if m in('vmovups','vmovaps','movups','movaps','vmovdqu','vmovdqa','movdqu','movdqa') and len(parts)==2:
            src,dst=parts
            if dst.startswith('%'):
                t.x[dst.lstrip('%')]=None
                if dst.startswith('%xmm') and not src.startswith('%'):
                    a_=t.addr(src,i)   # reload of a spilled vmovq-loaded register: low qword a global's 8 bytes, upper qword zero
                    if a_ is not None:
                        v0=t.read(a_,8); v1=t.read((a_[0],a_[1]+8),8)
                        if isinstance(v0,tuple) and v0[0]=='vq' and v1==0: t.x[dst.lstrip('%')]=v0
                continue
            xr=src.lstrip('%'); sz=32 if xr.startswith('ymm') else 16
            a=t.addr(dst,i)
            if t.x.get(xr)==('zero',) : t.write(a,sz,0)
            elif sz==16 and a is not None and isinstance(t.x.get(xr),tuple) and t.x[xr][0]=='vq': t.write(a,sz,None); t.write(a,8,t.x[xr]); t.write((a[0],a[1]+8),8,0)
            else: t.write(a,sz,None)
            continue
        if m=='vmovq' and len(parts)==2 and parts[1].startswith('%xmm') and '(%rip)' in parts[0]:
            t.x[parts[1].lstrip('%')]=('vq',i['rip']); continue   # low qword = the 8 bytes at a global, upper qword zero (VEX vmovq zero-extends)
        if m in('vmovss','movss') and len(parts)==2:
            src,dst=parts
            if dst.startswith('%'): t.x[dst.lstrip('%')]=('f',i['rip']) if '(%rip)' in src else None
            continue
        if m.startswith('call'):
            if ops.startswith('*'):
                prov=None
                mm=re.match(r'^\*(-?0x[0-9a-f]+)?\((%\w+)\)$',ops)
                if mm:
                    base=t.r.get(regname(mm.group(2))); d=int(mm.group(1),16) if mm.group(1) else 0
                    if isinstance(base,int): prov=('ld',base+d)
                    elif isinstance(base,tuple) and base[0]=='sp': prov=('stk',base[1]+d)
                elif ops.startswith('*%'):
                    prov=t.r.get(regname(ops[1:]))
                ev.append(('ind',i['addr'],prov,ops))
            else:
                tgt=int(ops.split()[0],16)
                R=lambda n:t.r.get(n)
                snap={n:R(n) for n in ['rdi','rsi','rdx','rcx','r8','r9']}
                e_={'t':tgt,'addr':i['addr'],'regs':snap,'sp':t.r['rsp']}
                for n in('rdi','rsi','rdx','rcx','r8','r9'):
                    e_['view_'+n]=t.view(snap[n])
                if tgt in (0x257a0,0xa81700):
                    # string variable ctor: name view (rsi) and initial view (rdx) as full 4-qword stack views (ptr, size, aux, ctl)
                    for n_ in (('rsi','rdx') if tgt==0x257a0 else ('rsi',)):
                        p_=snap[n_]
                        e_['v4_'+n_]=tuple(t.read(('sp',p_[1]+8*q),8) for q in range(4)) if issp(p_) else None
                if tgt==0x1580a0 and issp(snap['rsi']): e_['fov']=t.read(('sp',snap['rsi'][1]),4)   # the fov cell the builder reads through rsi
                e_['xmm2']=t.x.get('xmm2')
                if tgt==0x52a990 and isinstance(snap['rdi'],int):
                    e_['cell']=[m_[1] for m_ in t.mem if m_[0]=='abs' and m_[2]==8 and m_[3]==snap['rdi']][-1:]
                # stack args
                sp=t.r['rsp'][1]
                e_['stk0']=t.read(('sp',sp),8); e_['stk1']=t.read(('sp',sp+8),8)
                e_['xmm0']=t.x.get('xmm0'); e_['xmm1']=t.x.get('xmm1')
                # also floats on stack at rcx/r8 pointers
                for n in('rcx','r8','rdx'):
                    p=snap[n]
                    if issp(p): e_['f_'+n]=t.read(('sp',p[1]),4)
                # local-int wrapper (0x620e10 family): 7th argument = argument block, its functor tag sits at +0x40
                if issp(e_['stk0']): e_['lint_tag']=t.read(('sp',e_['stk0'][1]+0x40),8)
                if issp(e_['stk1']): e_['lint_desc']=t.view(e_['stk1'])   # local-int wrapper: second stack argument = description view
                if tgt==0x274730 and issp(snap['rdx']) and issp(snap['rcx']):
                    # out-of-line bool variable ctor: initial byte, the 4-qword block and the functor object (payload +0x30, class +0x40, aux +0x48), read AT the call
                    b_=snap['rcx'][1]
                    e_['bx']=dict(init=t.read(('sp',snap['rdx'][1]),1),q=[t.read(('sp',b_+8*k_),8) for k_ in range(4)],pay=t.read(('sp',b_+0x30),8),cls=t.read(('sp',b_+0x40),8),aux=t.read(('sp',b_+0x48),8))
                # functor tag at args struct + 0x40 (for 0xc300)
                if tgt in (0x6aa00,0x3a980,0x140b00,0x25ab0) and issp(snap['r8']):
                    e_['tag']=t.read(('sp',snap['r8'][1]+0x40),8)
                if tgt in (0xc300,0xc580) and issp(e_['stk0']):
                    e_['tag']=t.read(('sp',e_['stk0'][1]+0x40),8)
                    e_['descview']=t.view(e_['stk1'])
                ev.append(e_)
            if tgt==0xf2f0 and issp(snap['rdi']) and isinstance(snap['rdx'],int):
                # 0xf2f0(&out, 0, cstr) is the borrowed name view (hash-pinned 40 B body, strlen import tied): out = {cstr, strlen(cstr), 0, 0}
                o_=snap['rdi'][1]
                t.write(('sp',o_),8,snap['rdx']); t.write(('sp',o_+8),8,len(cstr_z(snap['rdx']))); t.write(('sp',o_+16),8,0); t.write(('sp',o_+24),8,0)
            for n in('rax','rcx','rdx','rsi','rdi','r8','r9','r10','r11'): t.r[n]=None
            for k in list(t.x): t.x[k]=None
            continue
        # unhandled: clobber dest register if obvious
        if parts and parts[-1].startswith('%') and regname(parts[-1]) and m not in('cmpq','testq','cmpl','testl','jmp'):
            t.r[regname(parts[-1])]=None
    return t,ev
if __name__=='__main__':
    t,ev=run(int(sys.argv[1]))
    for x in ev:
        if x[0]=='ind' if isinstance(x,tuple) else False: print('IND',hex(x[1])); continue
        print(hex(x['t']),hex(x['addr']),{k:(hex(v) if isinstance(v,int) else v) for k,v in x['regs'].items() if v is not None}, [ (k,x[k]) for k in x if k.startswith('view_') and x[k] and x[k][0] is not None][:3])
