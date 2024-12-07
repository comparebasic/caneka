#include <external.h> 
#include <caneka.h> 

static TableChain *SaltyKeyChain = NULL;

status Enc_Init(MemCtx *m){
    if(SaltyKeyChain == NULL){
        Span *keys = Span_Make(m, TYPE_TABLE);
        Table_Set(keys, (Abstract *)Cont(m, bytes("system")), (Abstract *)CB_Phrase(m));
        SaltyKeyChain = TableChain_Make(m, keys);
        return SUCCESS;
    }
    return NOOP;
}

String *EncPair_GetKey(String *key, Access *access){
    if(HasAccess(key, access)){
        return TableChain_Get(SaltyKeyChain, key);
    }
    access->type.state |= ERROR;
    return NULL;
}

status EncPair_AddKeyTable(MemCtx *m, Span *tbl, Access *access){
    if(HasAccess(Cont(m, bytes("system")), access)){
        printf("Extend Called!\n");
        return TableChain_Extend(m, SaltyKeyChain, tbl);
    }else{
        Fatal("Trying to extend key table as non system user", TYPE_ACCESS);
        return ERROR;
    }
}

static isFilled(Abstract *a){
    return (a != NULL && a->type.of != TYPE_BLANK);
}

status EncPair_Fill(MemCtx *m, String *keyId, EncPair *p, Access *access){
    if(isFilled(p->enc) && isFilled(p->dec) && isFilled(p->keyId)){
        return (NOOP|SUCCESS);
    }

    if(keyId != NULL){
        String *key = (String *)TableChain_Get(SaltyKeyChain, keyId); 
        if(!isFilled(p->enc) && isFilled(dec)){
            p->enc = String_Clone(m, p->dec);
            p->type.state |= Salty_Enc(m, key, p->enc); 
            return SUCCESS;
        }
        if(isFilled(enc) && !isFilled(dec)){
            p->dec = String_Clone(m, p->enc);
            p->type.state |= Salty_Dec(m, key, p->dec); 
            return SUCCESS;
        }
    }

    return ERROR;
}

EncPair *EncPair_Make(MemCtx *m, String *keyId, String *enc, String *dec, Access *access){
    EncPair *p = (EncPair *)MemCtx_Alloc(m, sizeof(EncPair));
    p->type.of = TYPE_ENC_PAIR;

    p->enc = enc;
    p->dec = dec;
    p->keyId = keyId;

    return p;
}
