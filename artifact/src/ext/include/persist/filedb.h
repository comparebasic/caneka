typedef struct filed {
    Type type;
    MemCh *m;
    File *f;
    BinSegCtx *ctx;
} FileDB;

status FileDB_Open(FileDB *fdb);
status FileDB_Close(FileDB *fdb);
status FileDB_Add(FileDB *fdb, i16 id, Abstract *a);
Table *FileDB_ToTbl(FileDB *fdb, Table *keys);
FileDB *FileDB_Make(MemCh *m, Str *path);
