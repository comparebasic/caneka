#include <external.h>
#include <caneka.h>

static Span *_perms = NULL;

status Access_Init(MemCh *m){
    if(_perms == NULL){
        MemCh_SetToBase(m);

        Span *tbl = Span_Make(m);
        Str *key = Str_CstrRef(m, "grant");
        Hashed *h = Table_SetHashed(tbl, (Abstract *)key, (Abstract *)Range_Wrapped(m, SUCCESS));
        h->type.state |= UPPER_FLAGS;

        key = Str_CstrRef(m, "system");
        _perms = Span_Make(m);
        h = Table_SetHashed(_perms, (Abstract *)key, (Abstract *)tbl);
        h->type.state |= UPPER_FLAGS;


        MemCh_SetFromBase(m);

        return SUCCESS;
    }
    return NOOP;
}

status Access_Grant(MemCh *m, Access *grantee, word fl, Str *key, Abstract *value, Access *access){
    DebugStack_Push(grantee->owner, grantee->owner->type.of);
    Access_SetFl(access, (fl|ACCESS_GRANT));
    if(GetAccess(access, Str_CstrRef(m, "grant")) == NULL){
        DebugStack_Pop();
        return ERROR;
    }

    Span *userPerms = NULL;
    Hashed *h = Table_GetHashed(_perms, (Abstract *)grantee->owner); 

    MemCh_SetToBase(m);
    Str *userKey = Str_Clone(m, grantee->owner);
    if(h == NULL){
        userPerms = Span_Make(m);
        h = Table_SetHashed(_perms, (Abstract *)userKey, (Abstract *)userPerms); 
    }else{
        userPerms = (Span *)h->value;
    }
    h->type.state |= fl;

    h = Table_SetHashed(userPerms, (Abstract *)key, (Abstract *)value); 
    h->type.state |= fl;

    MemCh_SetFromBase(m);
    DebugStack_Pop();
    return SUCCESS;
}

Str *GetGroupAccess(Access *access, Str *s){
    Iter it;
    Iter_Init(&it, access->groups);
    Str *v = NULL;
    while((Iter_Next(&it) & END) == 0){
        Access *ac = (Access *)it.value;
        if(ac != NULL && (v = GetAccess(ac, s)) != NULL){
            break;
        }
    }
    return v;
}

Str *GetAccess(Access *access, Str *s){
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
    
    Table *tbl = (Table *)as(h->value, TYPE_TABLE);
    h = Table_GetHashed(tbl, (Abstract *)s);
    if(h == NULL || (fl & h->type.state) != fl){
        DebugStack_Pop();
        return NULL;
    }

    DebugStack_Pop();
    return (Str *)h->value;
}

Access *Access_Make(MemCh *m, Str *owner, Span *groups){
    Access *a = (Access *)MemCh_Alloc(m, sizeof(Access));
    a->type.of = TYPE_ACCESS;
    a->owner = owner;
    if(groups != NULL && groups->type.of != TYPE_TABLE){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Group span is expected to be a table", NULL);
        return NULL;
    }
    a->groups = groups;
    return a;
}
