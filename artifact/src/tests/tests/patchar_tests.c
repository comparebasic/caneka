#include <external.h>
#include <caneka.h>

static status comparePat(Str *label, PatCharDef *pat, PatCharDef *expected){
    status r = READY;
    boolean matches = TRUE;
    PatCharDef *p = pat;
    PatCharDef *e = expected;
    while(p->flags != PAT_END){
        if((p->flags != e->flags ||
            p->to != e->to ||
            p->from != e->from)){
                matches = FALSE;
                break;
        }

        p++;
        e++;
    }

    void *args[] = {label, pat, (void *)((i64)TYPE_PATCHARDEF), expected, (void *)((i64)TYPE_PATCHARDEF), NULL};
    r |= Test(matches, "PatChar _t matches _T vs _T", args);
    if(e->flags != PAT_END){
        void *args[] = {label, NULL};
        r |= Test((e->flags == PAT_END),
            "PatChar _t end reached", args);
    }
    return r;
}

status PatChar_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();
    status r = READY;

    PatCharDef *pat = PatChar_FromStr(m, Str_CstrRef(m, "GET"));
    PatCharDef *ko = PatChar_KoFromStr(m, Str_CstrRef(m, "\r\n"));
    StrVec *v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "One"));
    StrVec_Add(v, Str_CstrRef(m, "And"));
    StrVec_Add(v, Str_CstrRef(m, "Two"));
    PatCharDef *longPat = PatChar_FromStrVec(m, v);

    PatCharDef patExpected[] = {
        {PAT_TERM, 'G', 'G'},
        {PAT_TERM, 'E', 'E'},
        {PAT_TERM, 'T', 'T'},
        {PAT_END, 0, 0}
    };
    r |= comparePat(Str_CstrRef(m, "basic pattern"), pat, patExpected);

    PatCharDef koExpected[] = {
        {PAT_KO|PAT_KO_TERM, '\r', '\r'},
        {PAT_KO|PAT_KO_TERM, '\n', '\n'},
        {PAT_END, 0, 0}
    };
    r |= comparePat(Str_CstrRef(m, "ko pattern"), ko, koExpected);

    PatCharDef longExpected[] = {
        {PAT_TERM, 'O', 'O'},
        {PAT_TERM, 'n', 'n'},
        {PAT_TERM, 'e', 'e'},
        {PAT_TERM, 'A', 'A'},
        {PAT_TERM, 'n', 'n'},
        {PAT_TERM, 'd', 'd'},
        {PAT_TERM, 'T', 'T'},
        {PAT_TERM, 'w', 'w'},
        {PAT_TERM, 'o', 'o'},
        {PAT_END, 0, 0}
    };
    r |= comparePat(Str_CstrRef(m, "long vec pattern"), longPat, longExpected);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
