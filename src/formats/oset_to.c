#include <external.h>
#include <caneka.h>

Abstract *String_ToOset(MemCtx *m, OsetDef *odef, Oset *o, String *key, Abstract *a){
    String *s = (String *)asIfc(a, TYPE_STRING);
    String *os = String_Init(m, STRING_EXTEND);
    if(key != NULL){
        String_Add(m, os, key);
        String_AddBytes(m, os, bytes(":"), 1);
    }
    String_Add(m, os, odef->name);
    String_AddBytes(m, os, bytes("/"), 1);
    String_Add(m, os, String_FromInt(m, s->length));
    char *eq = "=";
    char *end = ";";
    if((o->type.state & LINE_SEPERATED) != 0){
        eq = " ";
        end = "\n";
    }
    String_AddBytes(m, os, bytes(eq), 1);
    String_Add(m, os, s);
    String_AddBytes(m, os, bytes(end), 1);

    return (Abstract *)os;
}

Abstract *I64_ToOset(MemCtx *m, OsetDef *odef, Oset *o, String *key, Abstract *a){
    Single *sg = (Single *)as(a, TYPE_WRAPPED_I64);
    String *os = String_Init(m, STRING_EXTEND);
    if(key != NULL){
        String_Add(m, os, key);
        String_AddBytes(m, os, bytes(":"), 1);
    }
    String_Add(m, os, odef->name);
    String_AddBytes(m, os, bytes("/"), 1);
    String *s = String_FromI64(m, *((i64*)a));
    String_Add(m, os, String_FromInt(m, s->length));
    char *eq = "=";
    char *end = ";";
    if((o->type.state & LINE_SEPERATED) != 0){
        eq = " ";
        end = "\n";
    }
    String_AddBytes(m, os, bytes(eq), 1);
    String_Add(m, os, String_FromInt(m, s->length));
    String_AddBytes(m, os, bytes(end), 1);

   return (Abstract *)os;
}

Abstract *Iter_ToOset(MemCtx *m, OsetDef *odef, Oset *o, String *key, Abstract *a){
    Span *p = asIfc(a, TYPE_SPAN);

    String *os = String_Init(m, STRING_EXTEND);
    if(key != NULL){
        String_Add(m, os, key);
        String_AddBytes(m, os, bytes(":"), 1);
    }
    String_Add(m, os, odef->name);
    String_AddBytes(m, os, bytes("/"), 1);
    String_Add(m, os, String_FromInt(m, p->nvalues));
    String_AddBytes(m, os, bytes("="), 1);
    if(p->def->typeOf == TYPE_TABLE){
        String_AddBytes(m, os, bytes("{"), 1);
    }else if((p->type.state & FLAG_SPAN_HAS_GAPS) != 0){
        String_AddBytes(m, os, bytes("["), 1);
    }else{
        String_AddBytes(m, os, bytes("("), 1);
    }

    Iter it;
    Iter_Init(&it, p);
    String *itemKey = NULL;
    while((Iter_Next(&it) & END) == 0){
        Abstract *item = Iter_Get(&it);
        if(item != NULL){
            if(item->type.of == TYPE_HASHED){
                Hashed *h = (Hashed *)item;
                itemKey = (String *)h->item;
                item = h->value;
            }else if((p->type.state & FLAG_SPAN_HAS_GAPS) != 0){
                itemKey = String_FromInt(m, it.idx);
            }else {
                itemKey = NULL;
            }
            String_Add(m, os, Oset_To(m, itemKey, item));
        }
    }

    if(p->def->typeOf == TYPE_TABLE){
        String_AddBytes(m, os, bytes("}"), 1);
    }else if((p->type.state & FLAG_SPAN_HAS_GAPS) != 0){
        String_AddBytes(m, os, bytes("]"), 1);
    }else{
        String_AddBytes(m, os, bytes(")"), 1);
    }


    return (Abstract *)os;
}
