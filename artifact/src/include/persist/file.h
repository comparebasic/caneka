#define FILE_READ_LENGTH 1023

typedef struct file {
    Type type;
    Str *path;
    Str *abs;
    Access *access;
    Stream *sm;
} File;

enum file_status {
    FILE_TRACKED = 1 << 12,
    FILE_LOADED = 1 << 13,
    FILE_UPDATED = 1 << 14,
    FILE_SPOOL = 1 << 15,
};

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
