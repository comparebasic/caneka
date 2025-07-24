#define FILE_READ_LENGTH 1023

#define FILE_READ_MAX 16777216

typedef struct file {
    Type type;
    i16 guard;
    i16 _;
    MemCh *m;
    Str *path;
    Access *access;
    Stream *sm;
} File;

enum file_status {
    FILE_TRACKED = 1 << 12,
    FILE_LOADED = 1 << 13,
    FILE_UPDATED = 1 << 14,
    FILE_SPOOL = 1 << 15,
};

File *File_Make(MemCh *m, Str *path, Access *access);
File *File_Init(File *file, Str *path, Access *access);
status File_Write(File *f, i64 max);
status File_Read(File *f, i64 max);
status File_Open(File *f, word flags);
StrVec *File_GetVec(File *f);
Cursor *File_GetCurs(File *f);
status File_Close(File *f);

/*
status File_Persist(MemCh *m, File *file);
status File_SetAbs(MemCh *m, File *file, IoCtx *ctx);
File *File_Make(MemCh *m, Str *path, Access *access, IoCtx *ctx);
File * File_Clone(MemCh *m, File *o);
File *File_Init(File *file, Str *path, Access *access, IoCtx *ctx);
status File_Delete(File *file);
status File_Load(MemCh *m, File *file, Access *access);
status File_Stream(Stream *sm, File *file, Access *access, Abstract *source);
status File_AbsFromCtx(MemCh *m, File *file, Access *access, IoCtx *ctx);
status File_Read(MemCh *m, File *file, Access *access, int pos, int length);
status File_Copy(MemCh *m, Str *a, Str *b, Access *ac);
status File_StreamWithOpen(Stream *sm, FILE *f, File *file, Access *access, Abstract *source);
FILE *File_GetFILE(MemCh *m, File *file, Access *access);
*/
