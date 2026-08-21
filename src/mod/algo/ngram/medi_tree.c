#include <external.h>
#include <caneka.h>


NGram *MediTree_Make(MemCh *m, Buff *bf){
    return NGram_Make(m, bf, 4, NULL);
}

void MediTree_Add(MemCh *m, NGram *ng, i32 value, util meaning){
    ng->type.state |= NGRAM_AMMEND;
    return MediTree_Query(m, ng, value, meaning);
}

util MediTree_Find(MemCh *m, NGram *ng, i32 value){
    ng->type.state &= ~NGRAM_AMMEND;
    MediTree_Query(m, ng, value, 0);
    if(ng->type.state & SUCCESS){
        return ng->meaning;
    }
    return -1;
}

void MediTree_Query(MemCh *m, NGram *ng, i32 value, util meaning){
    if(ng->type.state & DEBUG){
        void *ar[] = {
            I32_Wrapped(m, value),
            I32_Wrapped(m, meaning),
            NULL
        };
        Out("MediTree_Query $/$^0\n", ar);
    }

    i64 pos = 0;
    i32 npos = 0;
    i32 idx = 0;
    i32 amount = 1;
    util *current = NULL;
    NVal *val = NULL;
    NVal *fl = NULL;
    NVal *cl = NULL;

    ng->type.state &= ~(SUCCESS|NOOP|NGRAM_FOUND);
    Buff_PosAbs(ng->bf, 0);

    if(Buff_IsEmpty(ng->bf)){
        idx = -1;
        goto rec;
    }
    
    while((ng->type.state & (SUCCESS|ERROR|NOOP)) == 0){

        amount = 1;
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
        val = (NVal *)(ng->s->bytes+sizeof(util));
        fl = val+1;
        cl = val+(ng->objRange.range-1);

        if(ng->type.state & DEBUG){
            void *ar[] = {
                I32_Wrapped(m, val->value),
                NULL
            };
            Out("    -> $^0\n", ar);
        }

        if(value == val->value){
            if(ng->type.state & DEBUG){
                void *ar[] = {
                    I32_Wrapped(m, val->value),
                    Util_Wrapped(m, *current),
                    NULL
                };
                Out("  <- $/$^0\n", ar);
            }

            ng->value = val->value;
            ng->meaning = *current;

            ng->type.state |= SUCCESS;
        }else if(value < val->value){
            printf("    lt\n");
            fflush(stdout);
            idx = 0;
            goto swap;
        }else if(cl->addr == 0 && fl->addr == 0){
            printf("    gt val %d blank %d\n", value, val->value);
            fflush(stdout);
            idx = ng->objRange.range-1;
            amount = ng->objRange.range-1;
            goto rec;
        }else if(value > cl->value){
            printf("    gt cl %d vs %d\n", value, cl->value);
            fflush(stdout);
            if(ng->objRange.range > 2){
                idx = ng->objRange.range-2;
                amount = 2;
            }else{
                idx = ng->objRange.range-1;
            }
            goto rec;
        }else{
            for(i32 i = ng->objRange.range-1; i > 0; i--){
                printf("    nth%d\n", i);
                fflush(stdout);
                NVal *rec = val+i;
                if(rec->value == 0){
                    idx = i;
                    goto rec;
                }else if(value < rec->value){
                    if(i == 1){
                        idx = i;
                        goto swap;
                    }else{
                        rec--;
                        if(rec->addr == 0){
                            idx = i-1;
                            goto rec;
                        }else{
                            if(pos == rec->addr){
                                Error(m, FUNCNAME, FILENAME, LINENUMBER, 
                                    "Circular link", NULL);
                                ng->type.state |= ERROR;
                                break;
                            }
                            Buff_PosAbs(ng->bf, rec->addr);
                            /* break here, continue outer loop */
                            break;
                        }
                    }
                }
                rec--;
            }
        }
    }

swap:
    if((ng->type.state & NGRAM_AMMEND) == 0){
        ng->type.state |= NOOP;
        return;
    }
    printf("  swap with %d\n", val->value);
    fflush(stdout);

    /* copy meaning and val records */
    Buff_PosEnd(ng->bf);
    npos = Buff_GetPos(ng->bf);
    Buff_AddBytes(ng->bf, (byte *)current, sizeof(util));
    Buff_AddBytes(ng->bf, (byte *)val, sizeof(NVal));
    for(i32 i = 1; i < ng->objRange.range; i++){
        NVal zeros = {0,0};
        Buff_AddBytes(ng->bf, (byte *)&zeros, sizeof(NVal));
    }

    /* update link */
    Buff_PosAbs(ng->bf, NVal_Coord(pos, idx));
    Buff_AddBytes(ng->bf, (byte *)&value, sizeof(i32));
    Buff_AddBytes(ng->bf, (byte *)&npos, sizeof(i32));

    /* update  meaning and value */
    Buff_PosAbs(ng->bf, pos);
    Buff_AddBytes(ng->bf, (byte *)&meaning, sizeof(util));
    Buff_AddBytes(ng->bf, (byte *)&value, sizeof(i32));
    i32 addr = 0;
    Buff_AddBytes(ng->bf, (byte *)&addr, sizeof(i32));

    ng->type.state |= SUCCESS;
    return;

rec:
    if((ng->type.state & NGRAM_AMMEND) == 0){
        ng->type.state |= NOOP;
        return;
    }
    printf("  rec %d\n", value);
    fflush(stdout);

    Buff_PosEnd(ng->bf);
    npos = Buff_GetPos(ng->bf);
    Buff_AddBytes(ng->bf, (byte *)&meaning, sizeof(util));
    Buff_AddBytes(ng->bf, (byte *)&value, sizeof(i32));
    i32 z = (i32)pos;
    Buff_AddBytes(ng->bf, (byte *)&pos, sizeof(i32));
    for(i32 i = 1; i < ng->objRange.range; i++){
        NVal zeros = {0,0};
        Buff_AddBytes(ng->bf, (byte *)&zeros, sizeof(NVal));
    }

    if(idx >= 0){
        printf("Linking @%ld nth%d value of %d x%d\n", pos, idx, value, amount);
        fflush(stdout);

        idx -= amount;
        if(idx < 0){
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Cannot set NVals of indexes below 0", NULL);
            ng->type.state |= ERROR;
        }

        Buff_PosAbs(ng->bf, NVal_Coord(pos, idx));
        while(amount-- > 0){
            Buff_AddBytes(ng->bf, (byte *)&value, sizeof(i32));
            Buff_AddBytes(ng->bf, (byte *)&npos, sizeof(i32));
        }
    }

    ng->type.state |= SUCCESS;
    return;
}
