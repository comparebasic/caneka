#include <external.h>
#include <caneka.h>

static MediTree_addNode(MemCh *m, i32 idx, NGram *ng, NRec *nr){
    i64 pos = Buff_GetPos(ng->buff);
    Buff_PosEnd(ng->bf);
}

NGram *MediTree_Make(MemCh *m, Buff *bf){
    return NGram_Make(m, bf, 3, NULL);
}

void MediTree_Add(MemCh *m, Ngram *ng, i32 value){
    ng->type.state = NGRAM_AMMEND;
    return MediTree_Query(m, ng, value);
}

i32 MediTree_Find(MemCh *m, Ngram *ng, i32 value){
    ng->type.state &= ~NGRAM_AMMEND;
    MediTree_Query(m, ng, value);
    if(ng->type.state & SUCCESS){
        return ng->value;
    }
}

void MediTree_Query(MemCh *m, Ngram *ng, i32 value){
    ng->type.state &= ~(SUCCESS|NOOP|NGRAM_FOUND);
    ng->s->length = 0;
        status r = READY;
        while((r & (SUCCESS|ERROR)) == 0){
            ReadToStr(ng->bf, ng->s);
            if(ng->s->length != ng->s->alloc){
                ng->type.state |= ERROR;
                Error(m, FUNCNAME, FILENAME, LINENUMBER,
                    "Incomplete length for NGram record", NULL);
                break;
            }

            NRec *fl = ((NRec *)ng->s->bytes)+MEDI_FLOOR;
            NRec *med = ((NRec *)ng->s->bytes)+MEDI_MEDIAN;
            NRec *cl = ((NRec *)ng->s->bytes)+MEDI_CEILING;

            if(value == fl->value){
                if(fl->type.state & NGRAM_VALUE){
                    ng->value = fl->value;
                    ng->type.state |= SUCCESS;
                }else{
                    ng->type.state |= NGRAM_FOUND;
                    Buff_PosAbs(ng->bf, fl->value);
                }
            }else if(value < fl->value){
                if(ng->type.state & NGRAM_AMMEND){
                    /* add node here */
                }else{
                    ng->type.state |= NOOP;
                }
            }else if(value < med->value){
                Buff_PosAbs(ng->bf, fl->value);
            }else if(value < cl->value){
                Buff_PosAbs(ng->bf, med->value);
            }else{
                if(ng->type.state & NGRAM_AMMEND){
                    /* add node here */
                }else{
                    ng->type.state |= NOOP;
                }
            }
        }
    }
}
