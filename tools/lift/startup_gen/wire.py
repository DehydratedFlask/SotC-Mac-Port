import sys,re
import pathlib
R=str(pathlib.Path(__file__).resolve().parents[3]/'native')
def wire(n,stem,rva,after_stem='ctor_slot_337_7e81d0',after_n=337,cmake_after=None,test=True,fn=None,menu=False,pre=''):
    S=f'Slot{n}'
    fn=fn or f'init_slot{n}_{rva:x}'
    hp=R+'/core/startup_ctors.hpp'; h=open(hp).read()
    inc=f'#include "{after_stem}.hpp"\n'
    assert inc in h and f'{stem}.hpp' not in h
    h=h.replace(inc,inc+f'#include "{stem}.hpp"\n',1)
    mem=f'    Slot{after_n}Objects slot{after_n}{{}}; Slot{after_n}Result slot{after_n}_result{{}};\n'
    assert mem in h
    h=h.replace(mem,mem+f'    {S}Objects slot{n}{{}}; {S}Result slot{n}_result{{}};\n',1)
    open(hp,'w').write(h)
    cp=R+'/core/startup_ctors.cpp'; c=open(cp).read()
    rows=[(int(m.group(2)),m.start()) for m in re.finditer(r'\n(        \{0x[0-9a-f]+, (\d+), \[)',c)]
    later=[(i,p) for i,p in rows if i>n]
    if later: nxt=min(later)
    else:
        lastp=max((i,p) for i,p in rows if i<n)[1]   # no later entry: append after the highest lower one (end of its line)
        nxt=(n,c.index('\n',lastp+1))
    row=f'\n        {{{rva:#x}, {n}, [](StartupState& s) noexcept {{ {pre}s.slot{n}_result = {fn}(s.slot{n}, s.concmd{', s.debug_menu' if menu else ''}); if (!s.slot{n}_result.ok) s.init_failed = true; }}}},'
    c=c[:nxt[1]]+row+c[nxt[1]:]
    open(cp,'w').write(c)
    mp=R+'/CMakeLists.txt'   # append-only (shared file, concurrent workers): target_sources + test, as the helper blocks do
    with open(mp,'a') as f:
        f.write(f'target_sources(sotc_core PRIVATE core/{stem}.cpp)\nadd_executable(test_ctor_slot_{n} tests/test_ctor_slot_{n}.cpp)\ntarget_link_libraries(test_ctor_slot_{n} sotc_core)\nadd_test(NAME ctor_slot_{n} COMMAND test_ctor_slot_{n})\n')
if __name__=='__main__':
    wire(int(sys.argv[1]),sys.argv[2],int(sys.argv[3],16))
