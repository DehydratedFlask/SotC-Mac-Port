#!/usr/bin/env python3
"""Independent mathematical oracle for a transform + camera-facing-light harness.

No imports from the GFX7 decoder or emitter, no instruction stream/register model.
Equations are row-major affine transforms, decoded normal components, supplied
interpolation coefficients and normalized view-direction lighting. The explicit
harness profile rounds each operation to binary32 and flushes binary32 subnormals;
final color components use binary16 round-toward-zero. This is not a retail GPU
execution oracle, live descriptor ABI, geometry decoder or G3 proof.
"""
import argparse
import hashlib
import json
import math
from pathlib import Path
import random
import struct

FIELDS = ('position','encoded_normal','object_row0','object_row1','object_row2',
          'clip_row0','clip_row1','clip_row2','clip_row3','camera','material',
          'param0_p0','param0_p10','param0_p20','param1_p0','param1_p10','param1_p20','barycentric')
WORDS = len(FIELDS)*4


def bits(x):
    return struct.unpack('<I',struct.pack('<f',x))[0]


def from_bits(n):
    return struct.unpack('<f',struct.pack('<I',n))[0]


def f32(x):
    try: return struct.unpack('<f',struct.pack('<f',x))[0]
    except OverflowError: return math.copysign(math.inf,x)


def clean(x):
    b=bits(f32(x))
    return from_bits(b & 0x80000000) if b & 0x7f800000 == 0 else from_bits(b)


def product(a,b):
    return clean(f32(clean(a)*clean(b)))


def plus(a,b):
    return clean(f32(clean(a)+clean(b)))


def affine(row,p):
    # Preserve the documented operation order rather than fuse a dot product.
    return plus(product(row[0],p[0]),plus(product(row[1],p[1]),plus(product(row[2],p[2]),row[3])))


def linear(row,p):
    return plus(product(row[0],p[0]),plus(product(row[1],p[1]),product(row[2],p[2])))


def half_rtz_bits(x):
    # Independent numeric formulation: use host nearest conversion then choose
    # the adjacent representable magnitude toward zero. No bit algorithm shared
    # with the MSL emitter.
    x=f32(x)
    sign=0x8000 if math.copysign(1.0,x)<0 else 0
    if math.isnan(x): return sign|0x7e00
    if math.isinf(x): return sign|0x7c00
    if abs(x)>65504.0: return sign|0x7bff
    h=struct.unpack('<H',struct.pack('<e',x))[0]
    q=struct.unpack('<e',struct.pack('<H',h))[0]
    if abs(q)>abs(x): h-=1
    return h


def half_rtz(x):
    return struct.unpack('<e',struct.pack('<H',half_rtz_bits(x)))[0]


def evaluate(c):
    world=[affine(c['object_row'+str(i)],c['position']) for i in range(3)]
    clip=[affine(c['clip_row'+str(i)],world) for i in range(4)]
    normal=[plus(product(2.0,x),-1.0) for x in c['encoded_normal'][:3]]
    param0=[*world,normal[0]]
    param1=[normal[1],normal[2],0.0,0.0]
    interp=[]
    for attr in range(2):
        a=c[f'param{attr}_p0']; b=c[f'param{attr}_p10']; d=c[f'param{attr}_p20']
        interp.append([plus(product(d[j],c['barycentric'][1]),plus(product(b[j],c['barycentric'][0]),a[j])) for j in range(4)])
    p,n=interp
    view=[plus(c['camera'][j],-p[j]) for j in range(3)]
    surface=[p[3],n[0],n[1]]
    surface=[linear(c['object_row'+str(i)],surface) for i in range(3)]
    norm2=plus(product(view[2],view[2]),plus(product(view[1],view[1]),product(view[0],view[0])))
    dot=plus(product(surface[2],view[2]),plus(product(surface[1],view[1]),product(surface[0],view[0])))
    if math.isnan(norm2) or norm2<0: inverse=math.nan
    elif norm2==0: inverse=math.copysign(math.inf,norm2)
    else: inverse=clean(f32(1.0/math.sqrt(clean(norm2))))
    lighting=product(inverse,dot)
    lighting=0.0 if not lighting>0 else min(lighting,1.0)
    color=[half_rtz(product(c['material'][j],lighting)) for j in range(3)] + [half_rtz(c['material'][3])]
    return clip+param0+param1+color


def base_case():
    c={name:[0.0]*4 for name in FIELDS}
    c.update(position=[0,0,0.25,0],encoded_normal=[0.5,0.5,1,0],camera=[0,0,4,0],material=[0.8,0.4,0.2,1],
             param0_p0=[0,0,0.25,0],param1_p0=[0,1,0,0])
    for i in range(3): c['object_row'+str(i)][i]=1.0
    for i in range(4): c['clip_row'+str(i)][i]=1.0
    return c


def fixtures(count=2048):
    rng=random.Random(190731)
    cases=[]
    for x,y in ((-.75,-.75),(.75,-.75),(0,.75)):
        c=base_case(); c['position'][:2]=[x,y]; cases.append(c)
    for _ in range(count):
        c=base_case()
        for name in FIELDS:
            if name.startswith(('object_row','clip_row')):
                c[name]=[f32(rng.uniform(-2,2)) for _ in range(4)]
            elif name not in ('encoded_normal','material'):
                c[name]=[f32(rng.uniform(-3,3)) for _ in range(4)]
        c['encoded_normal']=[f32(rng.random()) for _ in range(4)]
        c['material']=[f32(rng.uniform(-2,2)) for _ in range(3)]+[from_bits(rng.getrandbits(32))]
        cases.append(c)
    edges=[0x00000000,0x80000000,0x00000001,0x007fffff,0x00800000,0x80800000,
           0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0x3f800001,
           0x33800000,0x33000000,0x387fc000,0x38800000,0x477fe000,0x47800000]
    for b in edges:
        for field,component in (('material',3),('camera',0),('encoded_normal',0),('position',0),('param0_p0',3)):
            c=base_case(); c[field][component]=from_bits(b); cases.append(c)
    return cases


def write_fixture(directory,count):
    directory.mkdir(parents=True,exist_ok=True)
    cases=fixtures(count)
    raw=b''.join(struct.pack('<'+str(WORDS)+'f',*(x for name in FIELDS for x in c[name])) for c in cases)
    expected=b''.join(struct.pack('<16f',*evaluate(c)) for c in cases)
    (directory/'harness-inputs.bin').write_bytes(raw)
    (directory/'oracle-expected.bin').write_bytes(expected)
    meta=dict(cases=len(cases),random_cases=count,edge_cases=90,raster_cases=3,record_bytes=WORDS*4,
              input_sha256=hashlib.sha256(raw).hexdigest(),oracle_sha256=hashlib.sha256(expected).hexdigest(),
              profile='nearest_even_ftz32_rtz16',seed=190731)
    (directory/'oracle-fixture.json').write_text(json.dumps(meta,indent=2)+'\n')
    return meta


def compare(directory):
    raw=(directory/'harness-inputs.bin').read_bytes(); actual=(directory/'metal-output.bin').read_bytes()
    expected=(directory/'oracle-expected.bin').read_bytes(); cases=len(raw)//(WORDS*4)
    if len(actual)!=len(expected) or len(actual)!=cases*64: raise ValueError('output size mismatch')
    a=struct.unpack('<'+str(cases*16)+'f',actual); e=struct.unpack('<'+str(cases*16)+'f',expected)
    failures=[]; max_abs=0.0; compared=exact_alpha=0; max_scaled=0.0
    for k,(x,y) in enumerate(zip(a,e)):
        component=k%16; ok=False
        if math.isnan(y): ok=math.isnan(x)
        elif math.isinf(y): ok=x==y
        elif component==15: # Pure material-alpha move and RTZ conversion is exact.
            ok=bits(x)==bits(y); exact_alpha+=int(ok)
        elif math.isfinite(x):
            error=abs(x-y); max_abs=max(max_abs,error)
            if component<12:
                tolerance=2e-5*max(1.0,abs(y))
            else:
                # One representable half step for reciprocal-sqrt precision near
                # quantization boundaries; not a blanket relative error allowance.
                tolerance=2**(max(-14,math.floor(math.log2(abs(y))) if y else -14)-10)
            max_scaled=max(max_scaled,error/tolerance)
            ok=error<=tolerance
        compared+=1
        if not ok and len(failures)<30: failures.append(dict(case=k//16,component=component,actual=str(x),expected=str(y)))
    result=dict(status='PASSED' if not failures else 'FAILED',cases=cases,components=compared,
                finite_alpha_exact=exact_alpha,max_absolute_error=max_abs,max_fraction_of_tolerance=max_scaled,
                float32_tolerance='2e-5*max(1,abs(expected)) for transform paths',
                color_tolerance='one binary16 step for RGB; exact finite alpha; NaN class and Inf sign',failures=failures)
    (directory/'oracle-comparison.json').write_text(json.dumps(result,indent=2)+'\n')
    return result


def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('mode',choices=('fixture','compare'))
    p.add_argument('directory',type=Path); p.add_argument('--random-cases',type=int,default=2048)
    a=p.parse_args()
    try: result=write_fixture(a.directory,a.random_cases) if a.mode=='fixture' else compare(a.directory)
    except (OSError,ValueError,OverflowError) as e: print('FAILED:',e);return 1
    print(json.dumps(result,indent=2));return int(result.get('status')=='FAILED')


if __name__=='__main__':
    raise SystemExit(main())
