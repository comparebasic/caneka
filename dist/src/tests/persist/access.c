#include <external.h>
#include <caneka.h>

boolean HasAccess(String *perm, Access *access){
    if(String_Equals(perm, access->owner) || 
            (access->groups != NULL && 
                Table_Get(access->groups, (Abstract *)perm) != NULL)){
        return TRUE;
    }else{
        return FALSE;
    }
}

Access *Access_Make(MemCtx *m, String *owner, Span *groups){
    Access *a = (Access *)MemCtx_Alloc(m, sizeof(Access));
    a->type.of = TYPE_ACCESS;
    a->owner = owner;
    if(groups != NULL && groups->def->typeOf != TYPE_TABLE){
        Fatal("Group span is expected to be a table", TYPE_ACCESS);
        return NULL;
    }
    a->groups = groups;
    return a;
}
