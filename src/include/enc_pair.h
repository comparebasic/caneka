typedef struct enc_pair {
    Type type;
    String *keyId;
    String *enc;
    String *dec;
} EncPair;

status Enc_Init(MemCtx *m);
EncPair *EncPair_Make(MemCtx *m, String *keyId, String *enc, String *dec, struct access *access);
String *EncPair_GetKey(String *key, struct access *access);
status EncPair_AddKeyTable(MemCtx *m, struct span *tbl, struct access *access);
