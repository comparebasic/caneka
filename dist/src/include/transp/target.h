/*
 * Transpilation target output file details 
 */
typedef struct target {
    Type type;   
    String *path;
    String *dir;
    String *fname;
    String *ext;
    File destFile;
    FmtResolver *resolver;
} Target;

Target *Target_Make(MemCtx *m, String *dir, String *fname, String *ext);
