#include <external.h>
#include <caneka.h>

status Nested_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();
    Nested *nd = Nested_Make(m);

    StrVec *v = StrVec_From(m, Str_CstrRef(m, "one.two.three.keyFour"));
    StrVec_Split(v, (Abstract *)I8_Wrapped(m, '.'));
    Span *path = StrVec_ToSpan(m, v);

    Str *value = Str_CstrRef(m, "TheValue");
    nd->type.state |= DEBUG;
    Nested_AddByPath(nd, path, (Abstract *)value);

    Abstract *args[] = {
        (Abstract *)path,
        (Abstract *)value,
        (Abstract *)nd,
        NULL
    };
    Out("^p.Path: &\nValue: &\nNested: &\n", args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
