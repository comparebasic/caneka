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

Str *StrVec_ToStr(MemCtx *m, StrVec *vh){
    Str *ret = Str_Make(m, NULL, 0);
    Str *s = ret;
    i64 remaining = vh->total;
    i64 segment = min(remaining, MEM_SLAB_SIZE);
    byte *ptr = s->bytes = MemCtx_Alloc(m, (size_t)segment); 

    SvIter it;
    SvIter_Init(&it, vh);
    int length = 0;
    int remainder = 0;
    int used = 0;
    StrVecEntry *ve = NULL;
    byte *vePtr = NULL;
    while((SvIter_Next(&it) & END) == 0){
        ve = SvIter_Get(&it);
        vePtr = ve->content;
        do {
            int length = ve->length;
            if(remainder > 0){
                length = remainder;
            }
            if(s->length + length > STR_MAX){
                int use = MEM_SLAB_SIZE - s->length;
                remainder = length - use;
                length = use;
                printf("over max! using %d remaining %d\n", length, remainder);
            }else{
                remainder = 0;
            }
            printf("length: %d of %d\n", length, ve->length); 
            memcpy(ptr, vePtr, length);
            s->length += length;
            remaining -= length;
            vePtr += length;
            if(remainder > 0){
                segment = min(remaining, MEM_SLAB_SIZE);
                printf("making string %ld\n", segment);
                ptr = MemCtx_Alloc(m, (size_t)segment);
                s->type.state |= CONTINUED;
                s->next = Str_Make(m, ptr, 0);
                s = s->next;
            }else{
                ptr += length;
            }
        } while(remainder > 0);
    }

    return ret;
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
    if(length == 0){
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

    if(length == 0){
        length = strlen((char *)ptr);
    }

    vh->last = ve;
    vh->count = 1;
    ve->length = vh->total = length;
    ve->content = ptr;

    return vh;
}
