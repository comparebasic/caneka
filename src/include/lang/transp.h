typedef struct transp {
    Type type;   
    MemCtx *m;
    /* formatter */
    word spaceIdx;
    FmtDef *def;
    FmtItem *item;
    word offset;
    Roebling *rbl;
    Chain *byId;
    TableChain *byName;
    TableChain *byAlias;
    /* Transp */
    String *src;
    String *dist;
    struct {
        String *source;
        String *dest;
        File sourceFile;
        File destFile;
    } current;
    OutFunc out;
    /* End Transp */
} Transp;

Transp *Transp_Make(MemCtx*m);
status Transp_Trans(Transp *p);
