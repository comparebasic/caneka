#include <external.h> 
#include <caneka.h> 

static Span *SaltyKeyTbl = NULL;

status Enc_Init(MemCh *m){
    if(SaltyKeyTbl == NULL){
        Span *keys = Span_Make(m);
        SaltyKeyTbl = Span_Make(m);
        return SUCCESS;
    }
    return NOOP;
}

Str *EncPair_GetKey(MemCh *m, Str *key, Access *access){
    DebugStack_Push(NULL, 0);
    Str *s = Str_Prefixed(m, key, Str_CstrRef(m, "_keys."));
    Access_SetFl(access, ACCESS_KEY);
    Str *ret = GetAccess(access, s);
    DebugStack_Pop();
    return ret;
}

status EncPair_Conceal(MemCh *m, EncPair *p){
    p->dec = NULL;
    return SUCCESS;
}

static boolean isFilled(void *v){
    return (v != NULL);
}

status EncPair_Fill(MemCh *m, EncPair *p, Access *access){
    DebugStack_Push(access->owner, access->owner->type.of);
    if(isFilled(p->enc) && isFilled(p->dec) && isFilled(p->keyId)){
        DebugStack_Pop();
        return (NOOP|SUCCESS);
    }

    if(p->keyId != NULL){
        Str *key = EncPair_GetKey(m, access->owner, access);
        if(!isFilled(p->enc) && isFilled(p->dec)){
            p->enc = Salty_Enc(m, key, p->dec); 
            DebugStack_Pop();
            return SUCCESS;
        }
        if(isFilled(p->enc) && !isFilled(p->dec)){
            p->dec = Salty_Dec(m, key, p->enc); 
            DebugStack_Pop();
            return SUCCESS;
        }
    }

    DebugStack_Pop();
    return ERROR;
}

EncPair *EncPair_Make(MemCh *m, Str *keyId, StrVec *enc, StrVec *dec, Access *access, int length){
    EncPair *p = (EncPair *)MemCh_Alloc(m, sizeof(EncPair));
    p->type.of = TYPE_ENC_PAIR;

    p->enc = enc;
    p->length = length;
    p->dec = dec;
    p->keyId = keyId;

    return p;
}
