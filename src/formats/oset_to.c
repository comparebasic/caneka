#include <external.h>
#include <caneka.h>

String *String_ToOset(MemCtx *m, OsetDef *odef, Oset *o, String *key, Abstract *a){
    String *s = (String *s)asIfc(v, TYPE_STRING);
    String *os = String_Make(m, STRING_EXTEND);
    if(key != NULL){
        String_Add(m, os, key);
        String_AddBytes(m, os, bytes(":"), 1);
    }
    String_Add(m, os, odef->name);
    String_AddBytes(m, os, bytes("/"), 1);
    String_Add(m, os, String_FromInt(m, s->length));
    char eq = '=';
    char end = ';';
    if((o->type.state & LINE_SEPERATED) != 0){
        eq = ' ';
        end = '\n';
    }
    String_AddBytes(m, os, bytes(eq), 1);
    String_Add(m, os, s);
    String_AddBytes(m, os, bytes(end), 1);

    return os;
}

String *I64_ToOset(MemCtx *m, OsetDef *odef, Oset *o, String *key, Abstract *a){
    Single *sg = (Single *s)as(v, TYPE_WRAPPED_I64);
    String *os = String_Make(m, STRING_EXTEND);
    if(key != NULL){
        String_Add(m, os, key);
        String_AddBytes(m, os, bytes(":"), 1);
    }
    String_Add(m, os, odef->name);
    String_AddBytes(m, os, bytes("/"), 1);
    String *s = String_FromI64(m, (i64)(*v));
    String_Add(m, os, String_FromInt(m, s->length));
    char eq = '=';
    char end = ';';
    if((o->type.state & LINE_SEPERATED) != 0){
        eq = ' ';
        end = '\n';
    }
    String_AddBytes(m, os, bytes(eq), 1);
    String_Add(m, os, String_FromInt(m, s);
    String_AddBytes(m, os, bytes(end), 1);

   return os;
}

String *Iter_ToOset(MemCtx *m, OsetDef *odef, Oset *o, String *key, Abstract *a){
    Span *p = asIfc(a, TYPE_SPAN);

    String *os = String_Make(m, STRING_EXTEND);
    if(key != NULL){
        String_Add(m, os, key);
        String_AddBytes(m, os, bytes(":"), 1);
    }
    String_Add(m, os, odef->name);
    String_AddBytes(m, os, bytes("/"), 1);
    String *s = String_FromI64(m, (i64)(*v));
    String_Add(m, os, String_FromInt(m, p->nvalues));
    char eq = '=';
    char end = ';';
    if((o->type.state & LINE_SEPERATED) != 0){
        eq = ' ';
        end = '\n';
    }
    String_AddBytes(m, os, bytes(eq), 1);

    Iter it;
    Iter_Init(m, a);
    String *key = NULL;
    while((Iter_Next(it) & END) == 0){
        Abstract *item = Iter_Get(it);
        if(item != NULL){
            if(item->type.of == TYPE_HASHED){
                Hashed *h = (Hashed *)item;
                key = h->item;
                item = h->value;
            }else{
                key = NULL;
            }
            String_Add(m, os, Oset_To(m, key, item));
        }
    }
    String_AddBytes(m, os, bytes(end), 1);

    return os;
}
