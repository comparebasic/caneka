#include <external.h>
#include <caneka.h>

Span *Nav_TableFromPath(MemCh *m, Route *pages, StrVec *path){
    NodeObj *navObj = Config_FromPath(m, StrVec_Str(m, path));
    if(navObj == NULL){
        return NULL;
    }

    Table *tbl = Table_Make(m);

    Iter it;
    NodeObj *atts = Span_Get(navObj, NODEOBJ_PROPIDX_ATTS);
    Span *p = Table_Ordered(m, atts);

    Iter_Init(&it, p); 
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            StrVec *key = StrVec_StrVec(m, h->key);

            Path_DotAnnotate(m, key);

            StrVec *value = (StrVec *)as(h->value, TYPE_STRVEC);
            IoUtil_Annotate(m, value);
            Route *rt = Inst_ByPath(pages, value, NULL, SPAN_OP_GET);

            if(rt != NULL){
                Table *entry = Table_Make(m);
                Table_Set(entry, Str_FromCstr(m, "key", STRING_COPY), key);
                Table_Set(entry, Str_FromCstr(m, "route", STRING_COPY), rt);
                Table_ByPath(tbl, key, entry, SPAN_OP_SET);
            }else{
                void *args[] = {value, NULL};
                Error(m, FUNCNAME, FILENAME, LINENUMBER, 
                    "Route not found @\n", args);
            }
        }
    }

    return Table_Ordered(m, tbl);
}
