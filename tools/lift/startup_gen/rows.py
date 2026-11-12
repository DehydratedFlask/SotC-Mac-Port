import sys, re, subprocess, struct, json
import pathlib as _pl
_HERE=str(_pl.Path(__file__).resolve().parent)
_LIFT=str(_pl.Path(__file__).resolve().parents[1])
_ROOT=str(_pl.Path(__file__).resolve().parents[3])+'/'
sys.path.insert(0,_LIFT)
import debug_page_batch_gen as g
from slot_pin_util import Elf
e=Elf()
def disasm(rva,size):
    p=subprocess.run(['/usr/bin/objdump','-d','--no-show-raw-insn',f'--start-address={rva:#x}',f'--stop-address={rva+size:#x}',str(g.ELF_PATH)],capture_output=True,text=True)
    out=[]
    for line in p.stdout.splitlines():
        m=re.match(r'^\s*([0-9a-f]+):\s+(.*\S)\s*$',line)
        if m and rva<=int(m.group(1),16)<rva+size:
            out.append({'addr':int(m.group(1),16),'text':m.group(2)})
        elif out and line.strip().startswith('#') :
            out[-1]['text']+=' '+line.strip()
    for i,ins in enumerate(out):
        parts=ins['text'].split(None,1)
        ins['mnem']=parts[0]; ins['ops']=parts[1] if len(parts)>1 else ''
        m=g._RIPCOMMENT.search(ins['ops'])
        ins['rip']=int(m.group(1),16) if ('(%rip)' in ins['ops'] and m) else None
        nxt=out[i+1]['addr'] if i+1<len(out) else rva+size
        ins['len']=nxt-ins['addr']
        ins['bytes']=e.raw(ins['addr'],ins['len'])
    return out
def fde(rva):
    return g.fde_of(e,rva)[0]
if __name__=='__main__':
    idx=int(sys.argv[1])
    rva=e.u64(g.TABLE_TOP-8*idx); size=fde(rva)
    print(idx,hex(rva),size)
    ins=disasm(rva,size)
    for i in ins:
        if i['mnem'].startswith('call') or i['mnem'].startswith('j'):
            print(hex(i['addr']),i['mnem'],i['ops'].split('#')[0].strip())
