#include <external.h>
#include <caneka.h>

static TableChain *PermChain = NULL;

status Access_Init(MemCtx *m){
    if(PermChain == NULL){
        Span *keys = Span_Make(m, TYPE_TABLE);
        PermChain = TableChain_Make(m, keys);
        return SUCCESS;
    }
    return NOOP;
}

boolean HasAccess(Access *access, Abstract *a){
    Span *tbl = TableChain_Get(PermChain, (Abstract *)access->owner);
    if(tbl != NULL){
        Permission *p = (Permission *)Table_Get(tbl, a);
        if(p != NULL && ((p->type.state & a->type.state) & UPPER_FLAGS) != 0){
            return TRUE;
        }
    }
    return FALSE;
}

Permission *Access_MakePermission(MemCtx *m, word flags, Abstract *a){
    Permission *p = (Permission *)MemCtx_Alloc(m, sizeof(Permission));
    p->type.of = TYPE_PERMISSION;
    p->type.state |= flags;
    p->a = a;
    return p;
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
