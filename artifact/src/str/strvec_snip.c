#include <external.h>
#include <caneka.h>

StrVec *StrVec_Snip(MemCtx *m, Span *sns, Cursor *curs){
    DebugStack_Push(sns, TYPE_STRSNIP_STRING);
    StrVec *ret = StrVec_Make(m);

    Iter it;
    Iter_Init(&it, sns);
    i32 pos = 0;
    Iter_Reset(&curs->it); 
    while((Iter_Next(&it) & END) == 0){
        StrSnip sn = (StrSnip)((util)Iter_Get(&it));
        if(s.type.state & STRSNIP_CONTENT){
            StrVec_Copy(v, ret, pos, pos+sn.length);
        }
        pos += sn.length;
    }

    return ret;
}
