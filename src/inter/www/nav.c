#include <external.h>
#include <caneka.h>

Object *Nav_TableFromPath(MemCh *m, Route *pages, StrVec *path){
    NodeObj *navObj = Config_FromPath(m, StrVec_Str(m, path));
    if(navObj == NULL){
        return NULL;
    }

    Object *obj = Object_Make(m, ZERO);

    Iter it;
    NodeObj *atts = Object_GetPropByIdx(navObj, NODEOBJ_PROPIDX_ATTS);


    Iter_Init(&it, atts->tbl); 
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            StrVec *key = StrVec_StrVec(m, h->key);

            Path_DotAnnotate(m, key);

            StrVec *value = (StrVec *)as(h->value, TYPE_STRVEC);
            IoUtil_Annotate(m, value);
            Route *rt = Object_ByPath(pages, value, NULL, SPAN_OP_GET);

            if(rt != NULL){
                Object *entry = Object_Make(m, ZERO);
                Object_Set(entry, Str_FromCstr(m, "key", STRING_COPY), key);
                Object_Set(entry, Str_FromCstr(m, "route", STRING_COPY), rt);
                Object_ByPath(obj, key, entry, SPAN_OP_SET);
            }
        }
    }

    return obj;
}
