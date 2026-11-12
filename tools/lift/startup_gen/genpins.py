import sys, re, hashlib, struct
import pathlib as _pl
_HERE=str(_pl.Path(__file__).resolve().parent)
_LIFT=str(_pl.Path(__file__).resolve().parents[1])
_ROOT=str(_pl.Path(__file__).resolve().parents[3])+'/'
sys.path.insert(0,_LIFT)
import debug_page_batch_gen as g
from slot_pin_util import Elf, TABLE_START
g.DISC=_HERE+"/objdump_noraw.sh"
e=Elf()
CALL=re.compile(r'^(callq|jmp|jmpq)\s+0x([0-9a-f]+)\b')
def gen(n, out=print, at=None):
    if at: rva,size=at
    else:
        rva=e.u64(TABLE_START-8*n); size,_,_=g.fde_of(e,rva)
    ins=g.disassemble(rva,size)
    body=e.raw(rva,size)
    sha=hashlib.sha256(body).hexdigest()
    padn=(-(rva+size))%16
    pad=e.raw(rva+size,padn).hex()
    pins=[]; strings={}; lengths=[]
    leas=[]  # (addr, strtarget)
    for it in ins:
        a=it['addr']; b=it['bytes']; m=it['mnem']; ops=it['ops']
        c=CALL.match(it['text'])
        if c and b[0] in (0xe8,0xe9):
            tgt=int(c.group(2),16)
            pins.append(("call" if b[0]==0xe8 else "jmp",a,tgt)); continue
        if it['rip'] is not None:
            tgt=it['rip']; L=it['len']
            disp=struct.pack('<i',tgt-(a+L))
            off=b.rfind(disp)
            assert off>=0,(hex(a),it['text'])
            pins.append(("rip",a,L,off,tgt))
            if m.startswith('lea') and tgt<0x1500000: leas.append((a,tgt))
            if off+4!=L or ('$' in ops):
                pins.append(("imm",a,b.hex()))
            continue
        if m in('callq','jmpq') and '*' in ops:
            pins.append(("imm",a,b.hex())); continue
        if '$' in ops and not re.match(r'^\$(-?0x[0-9a-f]+) *,\s*%rsp$',ops) :
            if m.startswith(('add','sub')) and ops.endswith('%rsp'): continue
            pins.append(("imm",a,b.hex()))
            mm=re.match(r'^movq\s+\$(0x[0-9a-f]+), (-?0x[0-9a-f]+)\((?:%rbp|%rsp)\)$',it['text'])
            if mm and int(mm.group(1),16)>0: lengths.append((a,int(mm.group(1),16),b.hex()))
    for a,t in leas:
        strings[t]=g.string_at(e,t)
    # name lengths: pair each length-store with the closest preceding string of that length
    lens=[]
    for a,L,hx in lengths:
        cands=[(sa,t) for sa,t in leas if sa<a and len(strings[t])==L]
        if cands:
            lens.append((strings[cands[-1][1]].decode(),a,hx))
    out(f"# slot {n} rva {rva:#x} size {size}")
    return dict(slot=n,base=rva,size=size,sha=sha,pad=pad,pins=pins,strings=strings,lens=lens,ins=ins)
def fmt_pin(p):
    if p[0] in('call','jmp'): return f'        ("{p[0]}", {p[1]:#x}, {p[2]:#x}),'
    if p[0]=='rip': return f'        ("rip", {p[1]:#x}, {p[2]}, {p[3]}, {p[4]:#x}),'
    return f'        ("imm", {p[1]:#x}, "{p[2]}"),'
if __name__=='__main__':
    d=gen(int(sys.argv[1]))
    print(d['base'],d['size'],d['sha'],d['pad'])
    for p in d['pins']: print(fmt_pin(p))
    for k,v in sorted(d['strings'].items()): print(hex(k),v)
    print(d['lens'])
