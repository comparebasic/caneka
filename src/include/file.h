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
};

status File_Persist(MemCtx *m, File *file);
status File_SetAbs(MemCtx *m, File *file, IoCtx *ctx);
File *File_Make(MemCtx *m, String *path, Access *access, IoCtx *ctx);
status File_Delete(File *file);
