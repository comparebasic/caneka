#include <external.h>
#include <caneka.h>

NGram *MediTree_Make(MemCh *m, Buff *bf){
    return NGram_Make(m, bf, 4, NULL);
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
    NRec toWrite = {0, 0};
    status r = READY;


    if(Buff_IsEmpty(ng->bf)){
        toWrite.value = value;
        toWrite.addr = 0;
        Buff_Add(ng->bf, &toWrite);
        toWrite.value = 0;
        Buff_Add(ng->bf, &toWrite);
        Buff_Add(ng->bf, &toWrite);
        Buff_Add(ng->bf, &toWrite);
        ng->type.state |= SUCCESS;
        return;
    }
    
    while((r & (SUCCESS|ERROR)) == 0){
        ReadToStr(ng->bf, ng->s);
        if(ng->s->length != ng->s->alloc){
            ng->type.state |= ERROR;
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Incomplete length for NGram record", NULL);
            break;
        }

        NRec *val = ((NRec *)ng->s->bytes)+MEDI_VALUE;
        NRec *fl = ((NRec *)ng->s->bytes)+MEDI_FLOOR;
        NRec *med = ((NRec *)ng->s->bytes)+MEDI_MEDIAN;
        NRec *cl = ((NRec *)ng->s->bytes)+MEDI_CEILING;

        if(value == val->value){
            ng->addr = val->addr;
            ng->type.state |= SUCCESS;
        }else if(value < fl->value){
            if(ng->type.state & NGRAM_AMMEND){
                i64 pos = Buff_GetPos(ng->buff);
                Buff_PosEnd(ng->bf);
                Buff_Add(ng->bf, ng->s);
                i64 npos = Buff_GetPos(ng->buff);
                Buff_PosAbs(ng->bf, pos);
                toWrite.value = value;
                toWrite.addr = 0;
                Buff_Add(ng->bf, &toWrite);
                toWrite.value = fl->value;
                toWrite.addr = npos;
                Buff_Add(ng->bf, &toWrite);
            }else{
                ng->type.state |= NOOP;
            }
        }else if(med->value == 0){
            if(ng->type.state & NGRAM_AMMEND){
                i64 pos = Buff_GetPos(ng->buff);
                toWrite.value = value;
                toWrite.addr = 0;
                Buff_PosEnd(ng->bf);
                Buff_Add(ng->bf, &toWrite);
                toWrite.value = 0;
                toWrite.addr = 0;
                Buff_Add(ng->bf, &toWrite);
                Buff_Add(ng->bf, &toWrite);
                Buff_Add(ng->bf, &toWrite);
                Buff_PosAbs(ng->bf, pos+(sizeof(NRec)*MEDI_MEDIAN));
                toWrite.value = value;
                toWrite.addr = (i32)pos;
                Buff_Add(ng->bf, &toWrite);
                Buff_Add(ng->bf, &toWrite);
                ng->type.state |= SUCCESS;
            }else{
                ng->type.state |= NOOP;
            }
        }else if(value < med->value){
            Buff_PosAbs(ng->bf, fl->addr);
        }else if(value < cl->value){
            Buff_PosAbs(ng->bf, med->addr);
        }else{
            if(ng->type.state & NGRAM_AMMEND){
                i64 pos = Buff_GetPos(ng->buff);
                Buff_PosEnd(ng->bf);
                toWrite.value = value;
                toWrite.addr = 0;
                Buff_Add(ng->bf, &toWrite);
                i64 npos = Buff_GetPos(ng->buff);
                Buff_PosAbs(ng->bf, pos+sizeof(NRec)*MEDI_CEILING);
                toWrite.value = value;
                toWrite.addr = npos;
                Buff_Add(ng->bf, &toWrite);
                ng->type.state |= SUCCESS;
            }else{
                ng->type.state |= NOOP;
            }
        }
    }
}
