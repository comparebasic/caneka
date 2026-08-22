#include <external.h>
#include <caneka.h>

NGram *BytesTree_Make(MemCh *m, Buff *bf){
    NGram *ng = NGram_Make(m, bf, 26, NULL);
    ng->source = I32_Wrapped(m, 61);
    ng->s = Str_Make(m, sizeof(util) + (sizeof(i32) * ng->objRange.range));
    return ng;
}

void BytesTree_Add(MemCh *m, NGram *ng, Str *s, util meaning){
    ng->type.state |= NGRAM_AMMEND;
    return BytesTree_Query(m, ng, s, meaning);
}

util BytesTree_Find(MemCh *m, NGram *ng, Str *s){
    ng->type.state &= ~NGRAM_AMMEND;
    BytesTree_Query(m, ng, s, 0);
    if(ng->type.state & SUCCESS){
        return ng->meaning;
    }
    return -1;
}

void BytesTree_Query(MemCh *m, NGram *ng, Str *s, util meaning){
    if(ng->type.state & DEBUG){
        void *ar[] = {
            s,
            I32_Wrapped(m, meaning),
            NULL
        };
        Out("BytesTree_Query @/$^0\n", ar);
    }

    i64 pos = 0;
    i32 npos = 0;
    i32 idx = 0;
    util *current = NULL;

    ng->type.state &= ~(SUCCESS|NOOP|NGRAM_FOUND);
    Buff_PosAbs(ng->bf, 0);

    if(Buff_IsEmpty(ng->bf)){
        goto rec;
    }

    Single *sg = (Single *)ng->source;
    byte offset = sg->val.b;

    byte *b = s->bytes;
    byte *end = s->bytes+s->length-1;
    while(b <= end){
        byte idx = *b-offset;
        if(idx > ng->objRange.range){
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Byte out of range of NGram range", NULL);
            ng->type.state |= ERROR;
            return;
        }

        pos = Buff_GetPos(ng->bf);
        ng->s->length = 0;
        Buff_ReadToStr(ng->bf, ng->s);

        if(ng->s->length != ng->s->alloc){
            ng->type.state |= ERROR;
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Incomplete length for NGram record", NULL);
            break;
        }

        current = (util *)ng->s->bytes;
        i32 *bv = (i32 *)(ng->s->bytes+sizeof(util));
        bv += idx;
        i32 addr = *bv;
        
        if(addr == 0){
            if((ng->type.state & NGRAM_AMMEND) == 0){
                ng->type.state |= NOOP;
                return;
            }
rec:
            Buff_PosEnd(ng->bf);
            npos = (i32)Buff_GetPos(ng->bf);

            ng->s->length = 0;
            Str_Add(ng->s, (byte *)&meaning, sizeof(util));
            if(idx > 0){
                memset(ng->s+ng->s->length, 0, sizeof(i32)*idx-1);
            }
            Str_Add(ng->s, (byte *)&npos, sizeof(i32));
            if(idx < ng->objRange.range-1){
                memset(ng->s+ng->s->length, 0, ng->s->alloc-ng->s->length);
            }
            
            Buff_Add(ng->bf, ng->s);

            Buff_PosAbs(ng->bf, pos+sizeof(util)+(sizeof(i32)*idx));
            Buff_AddBytes(ng->bf, (byte *)&npos, sizeof(i32));

            ng->type.state |= SUCCESS;
            return;
        }else{
            if(b == end){
                ng->meaning = *current;
                ng->type.state |= SUCCESS;
                return;
            }
            Buff_PosAbs(ng->bf, addr);
            continue;
        }
    }
}
