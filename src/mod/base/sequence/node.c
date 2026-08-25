#include <external.h>
#include "base_module.h"

Span *Node_KvFromChild(Inst *node, void *key){
    MemCh *m = node->m;
    Abstract *a = Span_Get(node, INST_PROPIDX_CHILDREN);
    if(a == NULL){
        void *ar[] = {key, NULL};
        Error(m, FUNCNAME, FILENAME, LINENUMBER, "Key not found $", ar);
        return NULL;
    }

    if(a->type.of == TYPE_TABLE){
        Table *tbl = Table_Make(m); 
        Node *values = Table_Get((Table *)a, key);
        Iter it;
        Iter_Init(&it, Span_Get(values, INST_PROPIDX_CHILDREN));
        Iter it2;

        while((Iter_Next(&it) & END) == 0){
            Node *nd = Iter_Get(&it);
            if(nd != NULL && nd->type.of == TYPE_NODE){
                Iter_Init(&it2, Span_Get(nd, INST_PROPIDX_CHILDREN));
                while((Iter_Next(&it2) & END) == 0){
                    Hashed *h = Iter_Get(&it2);
                    if(h != NULL){
                        Table_Set(tbl, h->key, h->value);
                    }
                }
            }
        }

        return tbl;
    }

    return NULL;
}

status Node_ClsInit(MemCh *m){
    status r = READY;
    Table *tbl = Table_Make(m);
    Table_Set(tbl, S(m, "name"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, S(m, "atts"), I16_Wrapped(m, TYPE_TABLE));
    Table_SetHashed(tbl, S(m, "children"), I16_Wrapped(m, TYPE_SPAN));
    r |= Seel_Seel(m, tbl, S(m, "Node"), TYPE_NODE);
    return r;
}
