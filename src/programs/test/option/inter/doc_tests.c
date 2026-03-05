#include <external.h>
#include <caneka.h>
#include <test_module.h>

static Span *gather(MemCh *m){
    DirSel *sel = DirSel_Make(m, NULL, NULL, DIR_SELECTOR_NODIRS);
    sel->source = Span_Make(m);
    sel->ext = S(m, "c");
    StrVec *path = IoAbsPath(m, "fixtures/doc/mock-src/");

    StrVec *mod = StrVec_Clone(m, path);
    IoUtil_AddStr(m, mod, S(m, "mod"));
    Span_Add(sel->source, mod);

    StrVec *program = StrVec_Clone(m, path);
    IoUtil_AddStr(m, program, S(m, "program"));
    Span_Add(sel->source, program);

    sel->type.state |= DEBUG;
    Dir_GatherFilterDir(m, (Str *)Ifc(m, path, TYPE_STR), sel);

    Iter it;
    Iter_Init(&it, sel->dest);
    while((Iter_Next(&it) & END) == 0){
        StrVec *file = Iter_Get(&it);
        IoUtil_Annotate(m, file);
        Str *div =  Span_Get(file->p, path->p->max_idx);
        div->type.state |= NOOP;
    }

    return sel->dest;
}

status Doc_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;

    DebugStack_Push(NULL, 0);
    void *args[5];

    Span *files = gather(m);

    void *ar[] = {files, NULL};
    Out("^p.Files @^0\n", ar);

    DebugStack_Pop();
    return r;
}
