#include <external.h>
#include <caneka.h>

status WwwNav_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[5];

    NodeObj *node = Inst_Make(m, TYPE_NODEOBJ);
    Table *coordTbl = Table_Make(m);

    StrVec *path = IoPath_From(m, S(m, "/docs/base/bytes/str"));
    Span *coords = Span_Make(m);
    WwwNav *item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    StrVec *name = Sv(m, "Str");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    path = IoPath_From(m, S(m, "/docs/base/bytes/fmt"));
    coords = Span_Make(m);
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    name = Sv(m, "Fmt");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    path = IoPath_From(m, S(m, "/docs/base/bytes/tos"));
    coords = Span_Make(m);
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    name = Sv(m, "ToS");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    path = IoPath_From(m, S(m, "/docs/base/mem/span"));
    coords = Span_Make(m);
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    name = Sv(m, "Span");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    path = IoPath_From(m, S(m, "/docs/base/mem/memch"));
    coords = Span_Make(m);
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    name = Sv(m, "MemCh");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    Str *s = S(m, "Fmt");
    Span *crd = Table_Get(coordTbl, s);
    Iter *it = Iter_Make(m, NULL);
    NestSel_Init(it, node, crd);
    it->type.state |= DEBUG;

    void *expected[] = {
        S(m, "docs"), I32_Wrapped(m, 0), I16_Wrapped(m, UFLAG_ITER_INDENT|UFLAG_ITER_OUTDENT),
        S(m, "base"), I32_Wrapped(m, 1), I16_Wrapped(m, UFLAG_ITER_INDENT|UFLAG_ITER_OUTDENT|UFLAG_ITER_SELECTED),
        S(m, "bytes"), I32_Wrapped(m, 2), I16_Wrapped(m, UFLAG_ITER_INDENT|UFLAG_ITER_SELECTED),
        S(m, "str"), I32_Wrapped(m, 3), I16_Wrapped(m, UFLAG_ITER_INDENT),
        S(m, "fmt"), I32_Wrapped(m, 3), I16_Wrapped(m, UFLAG_ITER_SELECTED),
        S(m, "tos"), I32_Wrapped(m, 3), I16_Wrapped(m, UFLAG_ITER_OUTDENT),
        S(m, "mem"), I32_Wrapped(m, 3), I16_Wrapped(m, UFLAG_ITER_OUTDENT),
        S(m, "span"), I32_Wrapped(m, 3), I16_Wrapped(m, UFLAG_ITER_OUTDENT),
        S(m, "memch"), I32_Wrapped(m, 3), I16_Wrapped(m, UFLAG_ITER_OUTDENT),
        NULL, NULL,
    };

    i32 idx = 0;
    status relevantFl = (UFLAG_ITER_OUTDENT|UFLAG_ITER_INDENT|UFLAG_ITER_SELECTED);
    while((NestSel_Next(it) & END) == 0){
        Hashed *h = NestSel_Get(it);
        if(h == NULL){
            args[0] = I32_Wrapped(m, it->idx);
            args[1] = NULL;
            r |= Test(FALSE, "Item found \\@$", args);
            break;
        }
        args[0] = expected[idx*3];
        args[1] = h->key;
        args[2] = NULL;
        r |= Test(Equals(args[0], h->key), "Key matches, expected @, found @", args);

        Single *flagSg = expected[idx*3+2];
        args[0] = expected[idx*3+1];
        args[1] = Type_StateVec(m, TYPE_ITER_UPPER, flagSg->val.w);
        args[2] = I32_Wrapped(m, it->idx);
        args[3] = Type_StateVec(m, TYPE_ITER_UPPER, it->objType.state & relevantFl);
        args[4] = NULL;

        r |= Test(it->idx == ((Single *)args[0])->val.i &&
            ((it->objType.state & relevantFl) == flagSg->val.w),
            "Idx matches, $/@, found $/@",
            args);
        idx++;
    }

    s = S(m, "MemCh");
    crd = Table_Get(coordTbl, s);
    it = Iter_Make(m, NULL);
    NestSel_Init(it, node, crd);

    void *memChExpected[] = {
        S(m, "docs"), I32_Wrapped(m, 0), I16_Wrapped(m, ZERO),
        S(m, "base"), I32_Wrapped(m, 1), I16_Wrapped(m, ZERO),
        S(m, "mem"), I32_Wrapped(m, 2), I16_Wrapped(m, ZERO),
        S(m, "span"), I32_Wrapped(m, 3), I16_Wrapped(m, ZERO),
        S(m, "memch"), I32_Wrapped(m, 3), I16_Wrapped(m, MORE),
        NULL, NULL,
    };

    idx = 0;
    while((NestSel_Next(it) & END) == 0){
        Hashed *h = NestSel_Get(it);
        if(h == NULL){
            r |= Test(FALSE, "Item found", NULL);
            break;
        }
        args[0] = memChExpected[idx*3];
        args[1] = h->key;
        args[2] = NULL;
        r |= Test(Equals(args[0], h->key), "Key matches, expected @, found @", args);
        args[0] = memChExpected[idx*3+1];
        args[1] = I32_Wrapped(m, it->idx);
        args[2] = Type_StateVec(m, TYPE_ITER, it->type.state & MORE);
        args[3] = NULL;
        Single *flagSg = expected[idx*3+2];
        r |= Test(((Single *)args[0])->val.i == ((Single *)args[1])->val.i && ((it->type.state & MORE) == flagSg->val.w), "Idx matches, @, found @ @", args);
        idx++;
    }

    DebugStack_Pop();
    return r;
}
