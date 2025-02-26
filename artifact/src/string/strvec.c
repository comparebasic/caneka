#include <external.h>
#include <caneka.h>

status StrVec_ToFd(StrVec *vh, int fd){
    SvIter it;
    SvIter_Init(&it, vh);
    while((SvIter_Next(&it) & END) == 0){
        StrVecEntry *ve = SvIter_Get(&it);
        write(fd, ve->content, ve->length);
    }
    return SUCCESS;
}

String *StrVec_ToStr(MemCtx *m, StrVec *vh){
    String *s = String_Init(m, STRING_EXTEND);
    SvIter it;
    SvIter_Init(&it, vh);
    StrVecEntry *ve = NULL;
    while((SvIter_Next(&it) & END) == 0){
        ve = SvIter_Get(&it);
        String_AddBytes(m, s, ve->content, ve->length);
    }

    return s;
}

status StrVecEntry_Set(StrVecEntry *ve, byte *ptr, int length){
    ve->content = ptr;
    if(length == 0 && ptr != NULL){
       length = strlen((char *)ptr); 
    }
    ve->length = length;
    return SUCCESS;
}

status StrVec_Add(MemCtx *m, StrVec *vh, byte *ptr, int length){
    StrVecEntry *ve = MemCtx_Alloc(m, sizeof(StrVecEntry));
    ve->type.of = TYPE_STRVEC_ENTRY;
    /* an empty next header implies a new sve as the next entry 
     * only broken sequences need a next address 
     * */
    if((vh->last+1) != ve){
        vh->last->jump = ve;
    };
    vh->last = ve;
    vh->count++;

    ve->content = ptr;
    if(length == 0 && ptr != NULL){
        length = strlen((char *)ptr);
    }
    ve->length = length;
    vh->total += length;

    return SUCCESS;
}

StrVec *StrVec_Make(MemCtx *m, byte *ptr, int length){
    StrVec *vh = MemCtx_Alloc(m, sizeof(StrVec)+sizeof(StrVecEntry));
    StrVecEntry *ve = (StrVecEntry *)vh+1;
    vh->type.of = TYPE_STRVEC;
    ve->type.of = TYPE_STRVEC_ENTRY;

    if(length == 0 && ptr != NULL){
        length = strlen((char *)ptr);
    }

    vh->last = ve;
    vh->count = 1;
    ve->length = vh->total = length;
    ve->content = ptr;

    return vh;
}
