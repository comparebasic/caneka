#include <external.h>
#include <caneka.h>
#include <test_module.h>
#include "tests.h"

status Relation_Tests(MemCh *m){
    status r = READY;

    Str *s = Str_CstrRef(m, "Hi There\n");
    Str *s2 = Str_CstrRef(m, "Number Two\n");

    Relation *rel = NULL;
    rel = Relation_Make(m, 0, NULL);

    Relation_AddValue(rel, Str_CstrRef(m, "ColumnA"));
    Relation_AddValue(rel, Str_CstrRef(m, "ColumnB"));
    Relation_AddValue(rel, Str_CstrRef(m, "ColumnC"));
    Relation_HeadFromValues(rel);


    Relation_SetValue(rel, 0, 0, I32_Wrapped(m, 1));
    Relation_SetValue(rel, 0, 1, I32_Wrapped(m, 2));
    Relation_SetValue(rel, 0, 2, I32_Wrapped(m, 3));

    Relation_SetValue(rel, 1, 0, I32_Wrapped(m, 11));
    Relation_SetValue(rel, 1, 1, I32_Wrapped(m, 12));
    Relation_SetValue(rel, 1, 2, I32_Wrapped(m, 13));

    Relation_SetValue(rel, 2, 0, I32_Wrapped(m, 21));
    Relation_SetValue(rel, 2, 1, I32_Wrapped(m, 22));
    Relation_SetValue(rel, 2, 2, I32_Wrapped(m, 23));

    Relation_SetValue(rel, 3, 0, I32_Wrapped(m, 31));
    Relation_SetValue(rel, 3, 1, I32_Wrapped(m, 32));
    Relation_SetValue(rel, 3, 2, I32_Wrapped(m, 33));

    Buff *bf = Buff_Make(m, ZERO);
    i64 total = ToS(bf, rel, 0, MORE|DEBUG);

    s = Str_CstrRef(m, "Rel<3x4 \x1b[1m\"ColumnA\"\x1b[22m,"
        "\x1b[1m\"ColumnB\"\x1b[22m,\x1b[1m\"ColumnC\"\x1b[22m [\n"
        "  Wi32<\x1b[1m1\x1b[22m>,Wi32<\x1b[1m2\x1b[22m>,Wi32<\x1b[1m3\x1b[22m>,\n"
        "  Wi32<\x1b[1m11\x1b[22m>,Wi32<\x1b[1m12\x1b[22m>,Wi32<\x1b[1m13\x1b[22m>,\n"
        "  Wi32<\x1b[1m21\x1b[22m>,Wi32<\x1b[1m22\x1b[22m>,Wi32<\x1b[1m23\x1b[22m>,\n"
        "  Wi32<\x1b[1m31\x1b[22m>,Wi32<\x1b[1m32\x1b[22m>,Wi32<\x1b[1m33\x1b[22m>,\n"
        "]>"
    );

    void *args[] = {
        bf->v,
        NULL
    };

    s->type.state |= DEBUG;
    r |= Test(Equals(s, bf->v) == TRUE, "Expect relation ToS output to match, have @", args);

    return r;
}
