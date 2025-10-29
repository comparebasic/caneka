#include <external.h>
#include <caneka.h>

static i64 ErrorMsg_Print(Buff *bf, Abstract *a, cls type, word flags){
    if(a == NULL){
        Str *s = Str_CstrRef(bf->m, "Error");
        return Buff_Bytes(bf, s->bytes, s->length);
    }
    i64 total = 0;
    ErrorMsg *msg = (ErrorMsg *)as(a, TYPE_ERROR_MSG);
    if(flags & (MORE|DEBUG)){
        total += Fmt(bf, "Error $:$:$ - ", msg->lineInfo);
    }
    total += Fmt(bf, (char *)msg->fmt->bytes, msg->args);
    return total;
}

StrVec *Type_StateVec(MemCh *m, cls typeOf, word flags){
    Str **labels = Lookup_Get(ToSFlagLookup, typeOf);
    StrVec *v = StrVec_Make(m);
    Str *lbl = NULL;
    if(flags == 0){
        if(labels != NULL && labels[0] != NULL){
            lbl = labels[0];
        }else{
            lbl = stateLabels[0];
        }
        StrVec_Add(v, lbl);
    }else{
        for(i32 i = 0; i < 16; i++){ 
           if((flags & (1 << i)) != 0){
                if(labels != NULL && labels[i+1] != NULL){
                    lbl = labels[i+1];
                }else{
                    lbl = stateLabels[i+1];
                }
                StrVec_Add(v, lbl);
           }
        }
    }
    return v;
}


status Types_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_ERROR_MSG, (void *)ErrorMsg_Print);
    return r;
}
