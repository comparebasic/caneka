#include <external.h>
#include <caneka.h>

status Relation_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();

    Relation *rel = NULL;
    rel = Relation_Make(m, 0, NULL);

    Relation_AddValue(rel, (Abstract *)Str_CstrRef(m, "ColumnA"));
    Relation_AddValue(rel, (Abstract *)Str_CstrRef(m, "ColumnB"));
    Relation_AddValue(rel, (Abstract *)Str_CstrRef(m, "ColumnC"));
    Relation_HeadFromValues(rel);

    Relation_SetValue(rel, 0, 0, (Abstract *)I32_Wrapped(m, 1));
    Relation_SetValue(rel, 0, 1, (Abstract *)I32_Wrapped(m, 2));
    Relation_SetValue(rel, 0, 2, (Abstract *)I32_Wrapped(m, 3));

    Relation_SetValue(rel, 1, 0, (Abstract *)I32_Wrapped(m, 11));
    Relation_SetValue(rel, 1, 1, (Abstract *)I32_Wrapped(m, 12));
    Relation_SetValue(rel, 1, 2, (Abstract *)I32_Wrapped(m, 13));

    Relation_SetValue(rel, 2, 0, (Abstract *)I32_Wrapped(m, 11));
    Relation_SetValue(rel, 2, 1, (Abstract *)I32_Wrapped(m, 12));
    Relation_SetValue(rel, 2, 2, (Abstract *)I32_Wrapped(m, 13));

    Abstract *args[] = {
        (Abstract *)rel,
        NULL
    };
    Debug("Relation: @\n", args);

    MemCh_Free(m);
    return r;
}
