/*
 * Transpilation source file details 
 */
typedef struct source {
    Type type;   
    String *dir;
    String *fname;
    String *ext;
    String *source;
    File sourceFile;
} Source;
