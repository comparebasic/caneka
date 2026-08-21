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

    NVal nv = {0,0};
    Str s = {
        .type = {TYPE_STR, STRING_CONST|STRING_BINARY},
        .length = 0,
        .alloc = (word)sizeof(NVal),
        .bytes = (byte *)&nv
    };

    i64 pos = 0;
    i32 npos = 0;
    i32 idx = 0;
    util *current = NULL;
    NVal *val = NULL;
    NVal *fl = NULL;
    NVal *cl = NULL;

    ng->type.state &= ~(SUCCESS|NOOP|NGRAM_FOUND);
    Buff_PosAbs(ng->bf, 0);

    if(Buff_IsEmpty(ng->bf)){
        goto rec;
    }
    
    while((ng->type.state & (SUCCESS|ERROR|NOOP)) == 0){

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
            idx = 1;
            goto swap;
        }else if(cl->addr == 0 && fl->addr == 0){
            idx = ng->objRange.range-1;
            goto rec;
        }else if(value > cl->value){
            /* TODO: set previous idx so former cl can be found here */
            NVal *prev = cl-1;
            if(prev->value == 0 && prev->addr == 0){
                Buff_PosAbs(ng->bf, NVal_Coord(pos, ng->objRange.range-2));
                Buff_AddBytes(ng->bf, (byte *)cl, sizeof(NVal));
            }else{
                while(1){
                    Buff_PosAbs(ng->bf, NVal_Coord(prev->addr, ng->objRange.range-1));
                    s.length = 0;
                    Buff_ReadToStr(ng->bf, &s);
                    if(nv.value == 0 && nv.addr == 0){
                        Buff_Pos(ng->bf, - sizeof(NVal));
                        Buff_AddBytes(ng->bf, (byte *)cl, sizeof(NVal));
                        break;
                    }else{
                        prev = &nv;
                    }
                }
            }
            idx = ng->objRange.range-1;
            goto rec;
        }else{
            for(i32 i = ng->objRange.range-1; i > 0; i--){
                NVal *rec = val+i;
                if(rec->value == 0){
                    idx = i;
                    goto rec;
                }else if(value == rec->value){
                    if(pos == rec->addr){
                        Error(m, FUNCNAME, FILENAME, LINENUMBER, 
                            "Circular link", NULL);
                        ng->type.state |= ERROR;
                        break;
                    }
                    Buff_PosAbs(ng->bf, rec->addr);
                    break;
                }else if(value > rec->value){
                    if(rec->addr == 0){
                        idx = i;
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
                }else if(i == 1){
                    idx = i;
                    goto swap;
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
    Buff_AddBytes(ng->bf, (byte *)&val->value, sizeof(i32));
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

    Buff_PosEnd(ng->bf);
    npos = Buff_GetPos(ng->bf);
    Buff_AddBytes(ng->bf, (byte *)&meaning, sizeof(util));
    Buff_AddBytes(ng->bf, (byte *)&value, sizeof(i32));
    i32 z = (i32)0;
    Buff_AddBytes(ng->bf, (byte *)&z, sizeof(i32));
    for(i32 i = 0; i < ng->objRange.range-1; i++){
        NVal n = {0,0};
        Buff_AddBytes(ng->bf, (byte *)&n, sizeof(NVal));
    }

    if(idx > 0){
        Buff_PosAbs(ng->bf, NVal_Coord(pos, idx));
        Buff_AddBytes(ng->bf, (byte *)&value, sizeof(i32));
        Buff_AddBytes(ng->bf, (byte *)&npos, sizeof(i32));
    }

    ng->type.state |= SUCCESS;
    return;
}
