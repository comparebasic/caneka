String *Salty_MakeKey(MemCtx *m, String *s);
status Salty_Enc(MemCtx *m, String *key, String *s);
status Salty_Dec(MemCtx *m, String *key, String *s);
