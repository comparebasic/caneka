#include <external.h> 
#include <caneka.h> 

static TableChain *SaltyKeyChain = NULL;

status Enc_Init(MemCtx *m){
    if(SaltyKeyChain == NULL){
        Span *keys = Span_Make(m, TYPE_TABLE);
        SaltyKeyChain = TableChain_Make(m, keys);
        return SUCCESS;
    }
    return NOOP;
}

String *EncPair_GetKey(String *key, Access *access){
    DebugStack_Push(key, key->type.of);
    Access_SetFl(access, ACCESS_KEY);
    if(HasAccess(access, (Abstract *)key)){
        String *s = TableChain_Get(SaltyKeyChain, (Abstract *)key);
        DebugStack_Pop();
        return s;
    }
    access->type.state |= ERROR;
    DebugStack_Pop();
    return NULL;
}

status EncPair_AddKeyTable(MemCtx *m, Span *tbl, Access *access){
    Access_SetFl(access, (ACCESS_KEY|ACCESS_CREATE));
    if(HasAccess(access, (Abstract *)Cont(m, bytes("system")))){
        return TableChain_Extend(m, SaltyKeyChain, tbl);
    }else{
        Fatal("Trying to extend key table as non system user", TYPE_ACCESS);
        return ERROR;
    }
}

status EncPair_Conceal(MemCtx *m, EncPair *p){
    p->dec = NULL;
    return SUCCESS;
}

static boolean isFilled(String *s){
    return (s != NULL && s->type.of != TYPE_BLANK);
}

status EncPair_Fill(MemCtx *m, EncPair *p, Access *access){
    if(isFilled(p->enc) && isFilled(p->dec) && isFilled(p->keyId)){
        return (NOOP|SUCCESS);
    }

    if(p->keyId != NULL){
        String *key = (String *)TableChain_Get(SaltyKeyChain, (Abstract *)p->keyId); 
        if(!isFilled(p->enc) && isFilled(p->dec)){
            p->enc = Salty_Enc(m, key, p->dec); 
            return SUCCESS;
        }
        if(isFilled(p->enc) && !isFilled(p->dec)){
            p->dec = Salty_Dec(m, key, p->enc); 
            return SUCCESS;
        }
    }

    return ERROR;
}

EncPair *EncPair_Make(MemCtx *m, String *keyId, String *enc, String *dec, Access *access, int length){
    EncPair *p = (EncPair *)MemCtx_Alloc(m, sizeof(EncPair));
    p->type.of = TYPE_ENC_PAIR;

    p->enc = enc;
    p->length = length;
    p->dec = dec;
    p->keyId = keyId;

    return p;
}
