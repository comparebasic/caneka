#include <external.h>
#include <caneka.h>

status Stash_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    Abstract *args[5];
    MemCh *m = MemCh_Make();
    Span *p;
    status r = READY;

    StrVec *dir = IoUtil_AbsVec(m,
        StrVec_From(m, Str_FromCstr(m, "./dist/test/", ZERO)));
    Dir_CheckCreate(m, StrVec_Str(m, dir));

    Str *s = Str_CstrRef(m, "Hi is this real");
    void *orig = (void *)s;
    cls typeOf = s->type.of;

    i32 slIdx = m->it.p->max_idx;
    Abstract *arr[2] = {
        (Abstract *)Span_Get(m->it.p, m->it.p->max_idx)
    };

    Stash_PackAddr(s->type.of, slIdx, (void *)&s);
    StashCoord *coord = (StashCoord *)&s;

    r |= Test((void *)s != orig, "Pointer has been mutated and packed", NULL);
    r |= Test(coord->typeOf == typeOf, "Coord has type of object", NULL);
    r |= Test(coord->idx == slIdx, "Coord has idx sent to PackAddr", NULL);

    Stash_UnpackAddr(m, coord, arr);
    args[0] = (Abstract *)Str_Ref(m,
        (byte *)&orig,sizeof(void *), sizeof(void *), STRING_BINARY);
    args[1] = (Abstract *)Str_Ref(m,
        (byte *)&s, sizeof(void *), sizeof(void *), STRING_BINARY);
    args[2] = NULL;
    r |= Test((void *)s == orig, "Coord has been restored expected &, have &", args);

    MemCh *pst = MemCh_Make();
    Str *one = Str_FromCstr(pst, "One", STRING_COPY);
    Str *two = Str_FromCstr(pst, "Two", STRING_COPY);
    Str *three = Str_FromCstr(pst, "Three", STRING_COPY);

    StrVec *vec = StrVec_From(pst, Str_FromCstr(pst, "Halo", STRING_COPY));

    p = Span_Make(pst);
    Span_Add(p, (Abstract *)one);
    Span_Add(p, (Abstract *)two);
    Span_Add(p, (Abstract *)three);
    Span_Add(p, (Abstract *)vec);
    pst->owner = (Abstract *)p;

    Str *path = IoUtil_GetCwdPath(m, Str_CstrRef(m, "dist/test/persist.mem"));

    File_Unlink(m, path);
    Buff *bf = Buff_Make(m, ZERO);
    File_Open(bf, path, O_WRONLY|O_CREAT|O_TRUNC);

    bf->type.state |= DEBUG;
    status re = Stash_FlushFree(bf, pst);

    Buff_Stat(bf);
    args[0] = (Abstract *)I64_Wrapped(m, PAGE_SIZE+sizeof(StashHeader));
    args[1] = (Abstract *)I64_Wrapped(m, bf->st.st_size);
    args[2] = (Abstract *)bf;
    args[3] = NULL;
    r |= Test(bf->st.st_size == PAGE_SIZE+sizeof(StashHeader),
        "Bytes written equals 1 page + StashHeader, expected $, have $ - @", args);

    File_Close(bf);

    MemCh *takeSpace = MemCh_Make();
    for(util i = 0; i < 1000; i++){
        Util_Wrapped(takeSpace, i);
    }

    bf = Buff_Make(m, ZERO);
    File_Open(bf, path, O_RDONLY);
    MemCh *loaded = Stash_FromStream(bf);
    File_Close(bf);

    MemCh_Free(takeSpace);

    r |= Test(loaded != NULL, 
        "Stash From Stream returns non-null", NULL);

    s = Span_Get((Span *)loaded->owner, 0);
    Str *expected = Str_CstrRef(m, "One");
    r |= Test(pst != loaded,
        "loaded is not the original", NULL);
    r |= Test(s != one,
        "Str is not the original", NULL);

    args[0] = (Abstract *)expected;
    args[1] = (Abstract *)s;
    r |= Test(Equals((Abstract *)s, (Abstract *)expected),
        "Str has equivilent value, expected &, have &", args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
