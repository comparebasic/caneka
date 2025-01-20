/*
 * Transpilation target output file details 
 */
typedef struct target {
    Type type;   
    String *dir;
    String *fname;
    String *ext;
    File destFile;
} Target;
