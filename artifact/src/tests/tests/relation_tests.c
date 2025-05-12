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
    Debug("Relation: @\n m:@", args);
    Span *p = Span_Make(m);
    Debug("Relation: @\n m:@", args);

    MemCh_Free(m);
    return r;
}
