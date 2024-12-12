typedef struct transp {
    /* Transp */
    Type type;   
    MemCtx *m;
    String *src;
    String *dist;
    struct {
        String *source;
        String *dest;
        File *sourceFile;
        File *destFile;
    } current;
    OutFunc out;
    Roebling *rbl;
    /* End Transp */
} Transp;

Transp *Transp_Make(MemCtx*m);
