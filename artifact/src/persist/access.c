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
    Access_SetFl(access, (fl|ACCESS_GRANT));
    if(GetAccess(access, String_Make(m, bytes("grant"))) == NULL){
        return ERROR;
    }

    Hashed *h = Table_GetHashed(_perms, (Abstract *)grantee->owner); 
    Span *userPerms = (Span *)h->value;

    MemCtx_SetToBase(m);
    String *userKey = String_Clone(m, grantee->owner);
    if(userPerms == NULL){
        userPerms = Span_Make(m, TYPE_TABLE);    
        h = Table_SetHashed(_perms, (Abstract *)userKey, (Abstract *)userPerms); 
    }
    h->type.state |= fl;

    h = Table_SetHashed(userPerms, (Abstract *)key, (Abstract *)value); 
    h->type.state |= fl;

    MemCtx_SetFromBase(m);
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
    DPrint((Abstract *)_perms, COLOR_PURPLE, "perms: ");
    Hashed *h = Table_GetHashed(_perms, (Abstract *)access->owner);
    if(h == NULL){
        DPrint((Abstract *)access->owner, COLOR_PURPLE, "h is null for: ");
        DPrint((Abstract *)Table_Get(_perms, (Abstract *)access->owner), COLOR_PURPLE, "h is null for direct?: ");
        DebugStack_Pop();
        return NULL;
    }
    word fl = (access->type.state & UPPER_FLAGS);
    if((fl & h->type.state) != fl){
        DebugStack_Pop();
        printf("flag mismatch\n");
        return NULL;
    }
    
    Span *tbl = (Span *)as(h->value, TYPE_TABLE);
    DPrint((Abstract *)tbl, COLOR_PURPLE, "usertbl: ");
    h = Table_GetHashed(_perms, (Abstract *)s);
    if((fl & h->type.state) != fl){
        DebugStack_Pop();
        printf("flag II  mismatch\n");
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
