#include <external.h>
#include <caneka.h>

Object *Nav_TableFromPath(MemCh *m, Route *pages, StrVec *path){
    NodeObj *navObj = Config_FromPath(m, StrVec_Str(m, path));
    if(navObj == NULL){
        return NULL;
    }

    Object *obj = Object_Make(m, ZERO);

    Iter it;
    Table *atts = Object_GetPropByIdx(navObj, NODEOBJ_PROPIDX_ATTS);

    void *args[] = {
        atts,
        NULL,
    };
    Out("^c.Nav @^0\n", args);

    Iter_Init(&it, atts); 
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            StrVec *key = StrVec_StrVec(m, h->key);
            Path_DotAnnotate(m, key);

            StrVec *value = (StrVec *)as(h->value, TYPE_STRVEC);
            IoUtil_Annotate(m, value);
            Route *pg = Object_ByPath(pages, value, NULL, SPAN_OP_GET);
            if(pg != NULL){
                Object_ByPath(obj, key, pg, SPAN_OP_SET);
            }
        }
    }

    return obj;
}
