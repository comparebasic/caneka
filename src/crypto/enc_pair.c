#include <external.h> 
#include <caneka.h> 

static TableChain *SaltyKeyChain;

status Enc_Init(MemCtx *m){
    if(SaltyKeyChain == NULL){
        Span *keys = Span_Make(m, TYPE_TABLE);
        Table_Set(keys, Cont(m, bytes("system"), (Abstract *)CB_Phrase(m));
        SaltyKeyChain = TableChain_Make(m, keys);
        return SUCCESS;
    }
    return NOOP;
}

EncPair *EncPair_Make(MemCtx *m, String *keyId, String *enc, String *dec, Access *access){
    EncPair *p = (EndPair)MemCtx_Alloc(m, sizeof(EncPair));
    p->type = TYPE_ENC_PAIR;

    p->enc = enc;
    p->dec = dec;
    p->keyId = keyId;
    if(enc != NULL && dec != NULL && keyId != NULL){
        return p;
    }

    if(keyId != NULL){
        String *key = (String *)TableChain_Get(SaltyKeyChain, keyId); 
        if(!key){
            p->type.state |= ERROR;
        }else{
            if(enc == NULL && dec != NULL){
               p->enc = Salty_Enc(m, key, p->dec); 
            }
            if(enc != NULL && dec == NULL){
               p->dec = Salty_Dec(m, key, p->enc); 
            }
        }
    }

    return p;
}
