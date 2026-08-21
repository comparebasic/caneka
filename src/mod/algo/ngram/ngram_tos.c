#include <external.h>
#include <caneka.h>

status NGram_PrintFromStart(Buff *bf, NGram *ng){
    MemCh *m = ng->bf->m;
    i64 pos = Buff_GetPos(ng->bf);
    Buff_PosAbs(ng->bf, 0);
    void *args[6];

    args[0] = ng->bf;
    args[1] = I64_Wrapped(m, pos);
    args[2] = NULL;
    Fmt(bf, "^p.NGram<@ \\@$\n", args);

    while((ng->bf->type.state & (NOOP|END)) == 0){
        i64 current = Buff_GetPos(ng->bf);
        ng->s->length = 0;
        Buff_ReadToStr(ng->bf, ng->s);
        i32 i = 0;
        if(ng->s->length == ng->s->alloc){

            util *meaning = (util *)ng->s->bytes;
            NVal *val = (NVal *)(ng->s->bytes+sizeof(util));

            args[0] = I64_Wrapped(m, current);
            args[1] = Util_Wrapped(m, *meaning);
            Fmt(bf, "  \\@$ $ -> ", args);

            for(i32 i = 0; i < ng->objRange.range; i++){
                if(val->addr == 0 && val->value == 0){
                    Buff_AddBytes(bf, (byte *)" ", 1);
                    val++;
                    continue;
                }

                args[0] = I32_Wrapped(m, i);
                args[1] = I32_Wrapped(m, val->value);
                args[2] = I32_Wrapped(m, val->addr);
                args[3] = NULL;
                Fmt(bf, " nth$=$\\@$", args);

                val++;
            }
        }else{
            break;
        }
        Buff_AddBytes(bf, (byte *)"\n", 1);
    }

    Buff_PosAbs(ng->bf, pos);
    return Fmt(bf, ">\n^0", NULL);
}

status NGram_TosInit(MemCh *m){
    status r = READY;
    Lookup *lk = ToStreamLookup;
    return r;
}

