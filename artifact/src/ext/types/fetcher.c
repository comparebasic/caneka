#include <external.h>
#include <caneka.h>

Abstract *Fetch(MemCh *m, Fetcher *fch, Abstract *data, Abstract *source){
    if(fch->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)fch,
            (Abstract *)a,
            NULL,
        };
        Out("^y.Fetch & from @^0.\n", args);
    }
    Iter it;
    Iter_Init(&it, fch->targets);
    Abstract *value = data;
    while(a != NULL && (Iter_Next(&it) & END) == 0){
        FetchTarget *tg = (FetchTarget *)Iter_Get(&it);
        if(tg->type.state & FETCH_TARGET_RESOLVED){
            if(fch->target.type.state & FETCH_TARGET_ATT){
                value = Att_FromOffset(m, value, tg->val.offset, tg->objType.of);
            }else{
                value = fg->func(fg, value);
            }
        }else if(tg->type.state & FETCH_TARGET_SELF){
            continue;
        }else{
            ClassDef *cls = Lookup_Get(ClassLookup, data->type.of);
            if(cls != NULL){
                if(tg->type.state & FETCH_TARGET_ATT){
                    Single *sg = Table_Get(cls->attsTbl, fg->key);
                    if(sg != NULL){
                        tg->val.offset = sg->val.w;
                    }else{
                        value = Att_FromOffset(m, value, tg->offset, tg->objType.of);
                        break;
                    }
                }else if(tg->type.state & FETCH_TARGET_KEY){
                    tg->func = cls->byKey;
                }else if(tg->type.state & FETCH_TARGET_IDX){
                    tg->func = cls->byIdx;
                }else if(tg->type.state & FETCH_TARGET_ITER){
                    tg->func = cls->getIter;
                }else{
                    value = NULL;
                    break;
                }
                tg->type.state |= FETCH_TARGET_RESOLVED;
            }
        }
    }
    if(it->type.state & END){
        return value;
    }else{
        return NULL;
    }
}

status Fetcher_SetOp(Fetcher *fch, word op){
    fch->objType.state = op;
    return SUCCESS;
}

Fetcher *Fetcher_Make(MemCh *m){
    Fetcher *fch = (Fetcher *)MemCh_Alloc(m, sizeof(Fetcher));
    fch->type.of = TYPE_FETCHER;
    fch->targets = Span_Make(m);
    return fch;
}
