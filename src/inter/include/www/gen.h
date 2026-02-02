struct gen;

extern struct span *GenMimeTable;
extern struct lookup *MimeLookup;

enum route_flags {
    GEN_STATIC = 1 << 8,
    GEN_DYNAMIC = 1 << 9,
    GEN_FMT = 1 << 10,
    GEN_INDEX = 1 << 13,
    GEN_BINSEG = 1 << 11,
    GEN_ASSET = 1 << 12,
    GEN_ACTION = 1 << 14,
    GEN_FORBIDDEN = 1 << 15,
};

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
void Gen_Init(MemCh *m);
