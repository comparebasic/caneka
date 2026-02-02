struct gen;

extern struct span *GenMimeTable;
extern struct lookup *MimeLookup;

typedef void (*GenFunc)(struct gen *gen, Buff *bf, Inst *data);

typedef struct gen {
    Type type;
    Type objType; /* mime */
    StrVec *path;
    GenFunc func;
    Abstract *action;
    Abstract *source;
} Gen;

Gen *Gen_Make(MemCh *m);
Gen *Gen_Copy(MemCh *m, Gen *gen);
Gen *Gen_FromPath(MemCh *m, StrVec *path, NodeObj *config);
void Gen_Setup(MemCh *m, Gen *gen, NodeObj *config);
