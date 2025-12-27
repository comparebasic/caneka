#include <external.h>
#include <caneka.h>

static addLinesToTable(MemCh *m, Curs *curs, Table *tbl){
    status flags = LAST;
    Str *shelf = Str_Make(m, STR_DEFAULT);
    while((Cursor_PrevByte(curs) & END) == 0){
        byte b = *(curs->ptr);
        if(b == '\n'){

        }else if(b == ':'){
            flags &= ~LAST;
        }else{
            if(flags & LAST){
                
            }
        }
    }
}

status SysQuery(MemCh *m, NodeObj *dest, word flags){
    Span *cmd = Span_Make(m);
    Iter it;
    Iter_Init(&it, cmd);
    Iter_Add(&it, S(m, "sysctl"));
    Iter_Add(&it, S(m, "dev.cpu.0.freq"));
    Iter_Add(&it, S(m, "hw.machine"));
    Iter_Add(&it, S(m, "hw.model"));
    Iter_Add(&it, S(m, "hw.ncpu"));
    Iter_Add(&it, S(m, "hw.byteorder"));
    Iter_Add(&it, S(m, "hw.physmem"));
    Iter_Add(&it, S(m, "hw.usermem"));
    Iter_Add(&it, S(m, "hw.pagesize"));
    Iter_Add(&it, S(m, "hw.floatingpoint"));
    Iter_Add(&it, S(m, "hw.machine_arch"));
    Iter_Add(&it, S(m, "vm.stats.vm.v_page_count"));
    Iter_Add(&it, S(m, "vm.stats.vm.v_page_count"));
    Iter_Add(&it, S(m, "vm.stats.vm.v_free_count"));
    Iter_Add(&it, S(m, "vm.stats.vm.v_inactive_count"));
    Iter_Add(&it, S(m, "vm.stats.vm.v_active_count"));

    Buff *out = Buff_Make(m, ZERO);
    Buff *err = Buff_Make(m, ZERO);
    SubProcToBuff(m, cmd, out, err);

    double load[3];
    i32 ir = getloadavg(load, 3);
    printf("load response %d %lf\n", ir, load[0]);
    fflush(stdout);

    void *args[] = {
        out->v,
        err->v,
        I64_Wrapped(m, (i64)(load[0]*100)),
        I64_Wrapped(m, (i64)(load[1]*100)),
        I64_Wrapped(m, (i64)(load[2]*100)),
        NULL
    };
    Out("^y.System Stats @/^r.@\n^c.$,$,$^0\n", args);

    return NOOP;
}
