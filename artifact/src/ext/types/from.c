#include <external.h>
#include <caneka.h>

Abstract *From_Key(Abstract *data, Abstract *key){
    if(data == NULL){
        return NULL;
    }

    if(data->type.of == TYPE_ORDTABLE){
        Hashed *h = OrdTable_Get((OrdTable *)data, key);
        if(h != NULL){
            return h->value;
        }
    }else if(data->type.of == TYPE_TABLE){
        return Table_Get((Table *)data, key);
    }
    return NULL;
}

Iter *From_GetIter(MemCh *m, Abstract *data, Abstract *key){
    if(data == NULL){
        return NULL;
    }

    if(key == NULL){
        Span *p = NULL;
        if(data->type.of == TYPE_ITER){
            return (Iter *)data;
        }else if(data->type.of == TYPE_SPAN){
            p = (Span *)data;
        }else if(data->type.of == TYPE_ORDTABLE){
            p = ((OrdTable *)data)->order;
        }

        if(p != NULL){
            return Iter_Make(m, p);
        }
    }else{
        if(data->type.of == TYPE_ORDTABLE){
            Hashed *h = OrdTable_Get((OrdTable *)data, key);
            if(h != NULL){
                return From_GetIter(m, h->value, NULL);
            }
        }else if(data->type.of == TYPE_TABLE){
            return From_GetIter(m, Table_Get((Table *)data, key), NULL);
        }
    }
    return NULL;
}

Hashed *From_KeyHashed(Abstract *data, Abstract *key){
    if(data == NULL){
        return NULL;
    }

    if(data->type.of == TYPE_ORDTABLE){
        return OrdTable_Get((OrdTable *)data, key);
    }else if(data->type.of == TYPE_TABLE){
        return Table_GetHashed((Table *)data, key);
    }
    return NULL;
}

Abstract *From_Value(Abstract *a){
    if(a == NULL){
        return NULL;
    }else if(a->type.of == TYPE_HASHED){
        return ((Hashed *)a)->value;
    }
    return NULL;
}

Abstract *From_Name(Abstract *a){
    if(a == NULL){
        return NULL;
    }else if(a->type.of == TYPE_HASHED){
        return ((Hashed *)a)->item;
    }
    return NULL;
}
