typedef struct transp {
    /* Transp */
    Type type;   
    String *src;
    String *dist;
    struct {
        String *source;
        String *dest;
    } current;
    OutFunc out;
    Roebling *rbl;
    /* End Transp */
} Transp;

Transp *Transp_Make(MemCtx*m);
