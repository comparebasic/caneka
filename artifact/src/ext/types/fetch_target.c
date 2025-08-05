#include <external.h>
#include <caneka.h>

FetchTarget *FetchTarget_Make(MemCh *m){
    FetchTarget *tg = (FetchTarget *)Memch_Alloc(m, sizeof(FetchTarget));
    tg->type.of = TYPE_FETCH_TARGET;
    return tg;
}

FetchTarget *FetchTarget_MakeKey(MemCh *m, Str *key){
    FetchTarget *tg = FetchTarget_Make(m);
    tg->type.state = FETCHER_TARGET_KEY;
    tg->val.key = key;
    return tg;
}

FetchTarget *FetchTarget_MakeAtt(MemCh *m, Str *att){
    FetchTarget *tg = FetchTarget_Make(m);
    tg->type.state = FETCHER_TARGET_ATT;
    tg->val.key = key;
    return tg;
}

FetchTarget *FetchTarget_MakeFunc(MemCh *m, Str *key){
    FetchTarget *tg = FetchTarget_Make(m);
    tg->type.state = FETCHER_TARGET_FUNC;
    tg->val.key = key;
    return tg;
}

FetchTarget *FetchTarget_MakeIdx(MemCh *m, i32 idx){
    FetchTarget *tg = FetchTarget_Make(m);
    tg->type.state = FETCHER_TARGET_IDX;
    tg->val.idx = idx
    return tg;
}
