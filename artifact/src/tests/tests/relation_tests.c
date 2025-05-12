#include <external.h>
#include <caneka.h>

status Relation_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();

    Relation *rel = NULL;
    rel = Relation_Make(m, 0, NULL);

    Abstract *args[] = {
        (Abstract *)rel,
        (Abstract *)m,
        NULL
    };
    Debug("Relation: @\n m:@\n", args);
    Span *p = Span_Make(m);

    Str_CstrRef(m, "Hidy Hody there\n");
    Debug("Relation: @\n ^p.m:@^0\n", args);

    Single *n = I64_Wrapped(m, 13);
    Abstract *args1[] = {
        (Abstract *)n,
        NULL
    };
    Debug("^yDB.n:@^0\n", args1);

    MemCh_Free(m);
    r |= SUCCESS;
    return r;
}
