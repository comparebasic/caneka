#define FILE_READ_LENGTH 1023

typedef struct file {
    Type type;
    String *path;
    String *abs;
    Access *access;
    String *data;
} File;

enum file_status {
    FILE_TRACKED = 1 << 12,
    FILE_LOADED = 1 << 13,
    FILE_UPDATED = 1 << 14,
    FILE_SPOOL = 1 << 15,
};

status File_Persist(MemCtx *m, File *file);
status File_SetAbs(MemCtx *m, File *file, IoCtx *ctx);
File *File_Make(MemCtx *m, String *path, Access *access, IoCtx *ctx);
File * File_Clone(MemCtx *m, File *o);
File *File_Init(File *file, String *path, Access *access, IoCtx *ctx);
status File_Delete(File *file);
status File_Load(MemCtx *m, File *file, Access *access);
status File_Stream(MemCtx *m, File *file, Access *access, OutFunc out, Abstract *source);
status File_AbsFromCtx(MemCtx *m, File *file, Access *access, IoCtx *ctx);
status File_Read(MemCtx *m, File *file, Access *access, int pos, int length);
status File_Copy(MemCtx *m, String *a, String *b, Access *ac);
status File_StreamWithOpen(MemCtx *m, FILE *f, File *file, Access *access, OutFunc out, Abstract *source);
FILE *File_GetFILE(MemCtx *m, File *file, Access *access);
