typedef struct transp {
    Type type;   
    String *src;
    String *dist;
} Transp;

Transp *Transp_Make(MemCtx*m);
