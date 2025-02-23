#include <external.h>
#include <caneka.h>

static Span *_perms = NULL;

status Access_Init(MemCtx *m){
    if(_perms == NULL){
        MemCtx_SetToBase(m);

        Span *tbl = Span_Make(m, TYPE_TABLE);
        String *key = String_Make(m, bytes("grant"));
        Hashed *h = Table_SetHashed(tbl, (Abstract *)key, (Abstract *)Range_Wrapped(m, SUCCESS));
        h->type.state |= UPPER_FLAGS;

        key = String_Make(m, bytes("system"));
        _perms = Span_Make(m, TYPE_TABLE);
        h = Table_SetHashed(_perms, (Abstract *)key, (Abstract *)tbl);
        h->type.state |= UPPER_FLAGS;


        MemCtx_SetFromBase(m);

        return SUCCESS;
    }
    return NOOP;
}

status Access_Grant(MemCtx *m, Access *grantee, word fl, String *key, Abstract *value, Access *access){
    DebugStack_Push(grantee->owner, grantee->owner->type.of);
    Access_SetFl(access, (fl|ACCESS_GRANT));
    if(GetAccess(access, String_Make(m, bytes("grant"))) == NULL){
        DebugStack_Pop();
        return ERROR;
    }

    Span *userPerms = NULL;
    Hashed *h = Table_GetHashed(_perms, (Abstract *)grantee->owner); 

    MemCtx_SetToBase(m);
    String *userKey = String_Clone(m, grantee->owner);
    if(h == NULL){
        userPerms = Span_Make(m, TYPE_TABLE);    
        h = Table_SetHashed(_perms, (Abstract *)userKey, (Abstract *)userPerms); 
    }else{
        userPerms = (Span *)h->value;
    }
    h->type.state |= fl;

    h = Table_SetHashed(userPerms, (Abstract *)key, (Abstract *)value); 
    h->type.state |= fl;

    MemCtx_SetFromBase(m);
    DebugStack_Pop();
    return SUCCESS;
}

String *GetGroupAccess(Access *access, String *s){
    Iter it;
    Iter_Init(&it, access->groups);
    String *v = NULL;
    while((Iter_Next(&it) & END) == 0){
        Access *ac = (Access *)Iter_Get(&it);
        if(ac != NULL && (v = GetAccess(ac, s)) != NULL){
            break;
        }
    }
    return v;
}

String *GetAccess(Access *access, String *s){
    DebugStack_Push(access->owner, access->owner->type.of);
    Hashed *h = Table_GetHashed(_perms, (Abstract *)access->owner);
    if(h == NULL){
        printf("eI\n");
        DebugStack_Pop();
        return NULL;
    }
    word fl = (access->type.state & UPPER_FLAGS);
    if((fl & h->type.state) != fl){
        printf("eII\n");
        DebugStack_Pop();
        return NULL;
    }
    
    Span *tbl = (Span *)as(h->value, TYPE_TABLE);
    h = Table_GetHashed(tbl, (Abstract *)s);
    if(h == NULL || (fl & h->type.state) != fl){
        DebugStack_Pop();
        return NULL;
    }

    DebugStack_Pop();
    return (String *)h->value;
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
