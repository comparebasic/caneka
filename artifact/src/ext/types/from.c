#include <external.h>
#include <caneka.h>

Abstract *FromKey(Abstract *data, Abstract *key){
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

Hashed *FromKeyHashed(Abstract *data, Abstract *key){
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

Abstract *Value(Abstract *a){
    if(a == NULL){
        return NULL;
    }else if(a->type.of == TYPE_HASHED){
        return ((Hashed *)a)->value;
    }
    return NULL;
}

Abstract *Name(Abstract *a){
    if(a == NULL){
        return NULL;
    }else if(a->type.of == TYPE_HASHED){
        return ((Hashed *)a)->item;
    }
    return NULL;
}
