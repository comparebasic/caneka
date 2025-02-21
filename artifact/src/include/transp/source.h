/*
 * Transpilation source file details 
 */
typedef struct source {
    Type type;   
    String *path;
    String *dir;
    String *fname;
    String *ext;
    String *source;
    File sourceFile;
} Source;

Source *Source_Make(MemCtx *m);
status Source_Reset(MemCtx *m, Source *source, String *dir, String *fname, String *ext);
