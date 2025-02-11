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
        DPrint((Abstract *)SaltyKeyChain, COLOR_PURPLE, "SaltyKeyChain:");
        return TableChain_Get(SaltyKeyChain, (Abstract *)key);
    }
    access->type.state |= ERROR;
    return NULL;
}

status EncPair_AddKeyTable(MemCtx *m, Span *tbl, Access *access){
    if(HasAccess(Cont(m, bytes("system")), access)){
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
            p->enc = String_Clone(m, p->dec);
            p->type.state |= Salty_Enc(m, key, p->enc); 
            p->enc->type.of |= FLAG_STRING_BINARY;
            return SUCCESS;
        }
        if(isFilled(p->enc) && !isFilled(p->dec)){
            p->dec = String_Clone(m, p->enc);
            p->type.state |= Salty_Dec(m, key, p->dec); 
            p->dec->length = p->length;

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
