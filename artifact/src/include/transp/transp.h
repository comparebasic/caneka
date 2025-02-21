/*
 * Transpilation main class
 *
 * .fmts is a Table of @FmtCtx objects for each file extension 
 *
 * .targets is a lookup of @Target objects
 *  This is usually wiped by @FmtCtx.Setup
 */
typedef struct transp {
    Type type;   
    MemCtx *m;
    String *src;
    String *dist;
    Source *source;
    Span *fmts;
    Lookup *targets;
} Transp;

Transp *Transp_Make(MemCtx*m);
status Transp_Trans(Transp *p);
status Transp_Copy(Transp *p);
status Transp_Out(Transp *p, String *s, word targetId);
status ExtMatch_Init(Match *mt, String *backlog);
