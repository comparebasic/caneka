BinSegCtx *FileD_Create(MemCh *m, StrVec *path);
BinSegCtx *FileD_Open(MemCh *m, StrVec *path);
Table *FileD_ToTbl(MemCh *ctx, StrVec *path, Table *keys);
status FileD_Add(BinSegCtx *ctx, i16 id, Abstract *a);
