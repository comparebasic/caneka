typedef struct transp {
    Type type;   
    MemCtx *m;
    String *src;
    String *dist;
    struct {
        String *dir;
        String *fname;
        String *source;
        String *dest;
        String *ext;
        File sourceFile;
        File destFile;
    } current;
    OutFunc out;
    Span *formats; /* FmtCtx */
    /* End Transp */
} Transp;

Transp *Transp_Make(MemCtx*m);
status Transp_Trans(Transp *p);
void Transp_PrintFmtTree(Transp *p, FmtCtx *fmt, FmtItem *item, int indent);
void Transp_PrintTree(Transp *p);
