#include <external.h>
#include "base_module.h"

status Inst_Print(Buff *bf, void *a, cls type, word flags){
    static i32 _objIndent = 0;
    Inst *obj = NULL;
    if(((Abstract *)a)->type.of & TYPE_INSTANCE){
        obj = (Inst *)a;
    }else{
        Ifc(bf->m, a, TYPE_INSTANCE);
    }
    Table *seel = Lookup_Get(SeelLookup, obj->type.of);
    Str *name = Lookup_Get(SeelNameLookup, obj->type.of);
    if(flags & DEBUG){
        void *args[] = {
            name,
            Type_StateVec(bf->m, obj->type.of, obj->type.state),
            NULL,
        };
        Fmt(bf, "$<@ ", args);
        Iter it;
        Iter_Init(&it, seel);
        while((Iter_Next(&it) & END) == 0){
            Hashed *h = Iter_Get(&it);;
            if(h != NULL){
                Buff_AddBytes(bf, (byte *)"#", 1);
                ToS(bf, h->key, ZERO, flags); 
                Buff_AddBytes(bf, (byte *)": ", 2);
                Abstract *value = Span_Get(obj, h->orderIdx);
                ToS(bf, value, ZERO, flags); 
                if((it.type.state & LAST) == 0){
                    Buff_AddBytes(bf, (byte *)", ", 2);
                }
            }
        }
        Fmt(bf, ">", NULL);
        return SUCCESS;
    }else if(flags & MORE){
        void *args[] = {
            name,
            Type_StateVec(bf->m, obj->type.of, obj->type.state),
            NULL,
        };
        Fmt(bf, "$<@ ", args);
        Iter it;
        Iter_Init(&it, seel);
        while((Iter_Next(&it) & END) == 0){
            Hashed *h = Iter_Get(&it);;
            if(h != NULL){
                Abstract *value = Span_Get(obj, h->orderIdx);
                if(Empty(value)){
                    continue;
                }
                Buff_AddBytes(bf, (byte *)"#", 1);
                ToS(bf, h->key, ZERO, flags); 
                Buff_AddBytes(bf, (byte *)": ", 2);
                ToS(bf, value, ZERO, flags); 
                if((it.type.state & LAST) == 0){
                    Buff_AddBytes(bf, (byte *)", ", 2);
                }
            }
        }
        Fmt(bf, ">", NULL);
        return SUCCESS;
    }else{
        Iter it;

        Buff_AddBytes(bf, (byte *)"<", 1);
        ToS(bf, name, name->type.of, ZERO); 

        Table *atts = Span_Get(obj, INST_PROPIDX_ATTS);
        if(atts != NULL){
            Iter_Init(&it, atts);
            while((Iter_Next(&it) & END) == 0){
                Hashed *h = Iter_Get(&it);
                ToS(bf, h->key, ZERO, ZERO);
                Buff_AddBytes(bf, (byte *)"=", 1);
                ToS(bf, h->value, ZERO, ZERO);
                if((it.type.state & LAST) == 0){
                    Buff_AddBytes(bf, (byte *)" ", 1);
                }
            }
        }

        Buff_AddBytes(bf, (byte *)" ", 2);

        Iter_Init(&it, seel);
        while((Iter_Next(&it) & END) == 0){
            Hashed *h = Iter_Get(&it);
            if(h != NULL){
                Abstract *value = Span_Get(obj, h->orderIdx);
                if(h->orderIdx == INST_PROPIDX_CHILDREN || Empty(value)){
                    continue;
                }
                if(it.idx > 0){
                    Buff_AddBytes(bf, (byte *)", ", 2);
                }
                Buff_AddBytes(bf, (byte *)"#", 1);
                ToS(bf, h->key, ZERO, ZERO); 
                Buff_AddBytes(bf, (byte *)"=", 2);
                ToS(bf, value, ZERO, ZERO); 
            }
        }

        Abstract *children = Span_Get(obj, INST_PROPIDX_CHILDREN);
        if(!Empty(children)){
            ToS(bf, children, ZERO, ZERO); 
        }

        Fmt(bf, ">", NULL);
        return SUCCESS;
    }
}
