#include <external.h>
#include <caneka.h>

status Stash_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    Abstract *args[5];
    MemCh *m = MemCh_Make();
    Span *p;
    status r = READY;

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

    i32 fd = open(Str_Cstr(m, path), O_WRONLY|O_CREAT, 00644);
    Stream *sm = Stream_MakeToFd(m, fd, StrVec_Make(m), ZERO);
    status re = Stash_FlushFree(sm, pst);
    close(fd);

    MemCh *takeSpace = MemCh_Make();
    for(util i = 0; i < 100; i++){
        Util_Wrapped(takeSpace, i);
    }

    fd = open(Str_Cstr(m, path), O_RDONLY);
    sm = Stream_MakeFromFd(m, fd, ZERO);
    MemCh *loaded = Stash_FromStream(sm);
    close(fd);

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
