typedef struct file {
    Type type;
    String *path;
    String *abs;
    Access *access;
    String *data;
} File;

status File_Persist(File *file);
status File_Make(MemCtx *m);
