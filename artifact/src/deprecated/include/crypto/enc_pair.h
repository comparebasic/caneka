typedef struct enc_pair {
    Type type;
    int length;
    Str *keyId;
    StrVec *enc;
    StrVec *dec;
} EncPair;

status Enc_Init(MemCh *m);
EncPair *EncPair_Make(MemCh *m, Str *keyId, StrVec *enc, StrVec *dec, struct access *access, int length);
Str *EncPair_GetKey(MemCh *m, Str *key, struct access *access);
status EncPair_AddKeyTable(MemCh *m, struct span *tbl, struct access *access);
status EncPair_Fill(MemCh *m, EncPair *p, struct access *access);
status EncPair_Conceal(MemCh *m, EncPair *p);
