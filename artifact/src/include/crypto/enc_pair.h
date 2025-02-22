typedef struct enc_pair {
    Type type;
    int length;
    String *keyId;
    String *enc;
    String *dec;
} EncPair;

status Enc_Init(MemCtx *m);
EncPair *EncPair_Make(MemCtx *m, String *keyId, String *enc, String *dec, struct access *access, int length);
String *EncPair_GetKey(MemCtx *m, String *key, struct access *access);
status EncPair_AddKeyTable(MemCtx *m, struct span *tbl, struct access *access);
status EncPair_Fill(MemCtx *m, EncPair *p, struct access *access);
status EncPair_Conceal(MemCtx *m, EncPair *p);
