typedef struct blank {
    Type type;
    int length;
} Blank;

Blank *Blank_Make(MemCtx *m){
