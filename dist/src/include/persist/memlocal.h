typedef struct localsetter {
    Type type;
    void **dptr;
} LocalSetter;

typedef struct localptr {
    i32 slabIdx;
    i32 offset;
} LocalPtr;

typedef struct memlocal_item {
    Type type;
    word typeOf;
    i64 idx;
    Abstract *a;
}MemLocalItem;

#define INDEX_PATHNAME "index"

extern Chain *MemLocalToChain;
extern Chain *MemLocalFromChain;
extern Lookup *ExemptLocal;

Span *MemLocal_Make(cls typeOf);
status MemLocal_Init(MemCtx *m);

status MemLocal_SetLocal(MemCtx *m, Abstract **addr);
status MemLocal_UnSetLocal(MemCtx *m, Abstract **addr);

status MemLocal_Persist(MemCtx *m, Span *tbl, String *path, Access *access);
Span *MemLocal_Load(MemCtx *m, String *path, Access *access);
status MemLocal_Destroy(MemCtx *m, String *path, Access *access);
status MemLocal_To(MemCtx *m, Abstract *a);
status MemLocal_From(MemCtx *m, Abstract *a);
