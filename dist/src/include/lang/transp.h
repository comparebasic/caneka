/* Setup func for the @Transp class, triggered when it finds a source file with
 * any extension
 */
typedef status (*TranspSetup)(MemCtx *m, struct transp *tp);
/*
 * Transpilation main class
 * 
 * .setfuncs is a Table whose key is the extension and a pointer to a 
 * setup function.
 *
 * .targets is a span of @Target objects
 * There may be several of these for a single file that is transpiled, for
 * example, a Caneka lang file will generate a C source, and C header file, so
 * it would have two targets.
 */
typedef struct transp {
    Type type;   
    MemCtx *m;
    String *src;
    String *dist;
    Source *source;
    Span *targets;
    Span *setfuncs; 
} Transp;

Transp *Transp_Make(MemCtx*m);
status Transp_Trans(Transp *p);
status Transp_Out(Transp *p, String *s);
status Transp_OutHeader(Transp *p, String *s);
