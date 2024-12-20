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
        String *destHeader;
        String *ext;
        File sourceFile;
        File destFile;
        File destHeaderFile;
    } current;
    Span *formats; /* FmtCtx */
    /* End Transp */
} Transp;

Transp *Transp_Make(MemCtx*m);
status Transp_Trans(Transp *p);
status Transp_Out(Transp *p, String *s);
status Transp_OutHeader(Transp *p, String *s);
