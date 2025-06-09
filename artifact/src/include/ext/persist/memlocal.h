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

extern Lookup *MemLocalToLookup;
extern Lookup *MemLocalFromLookup;
extern Lookup *ExemptLocal;

Span *MemLocal_Make(cls typeOf);
status MemLocal_Init(MemCh *m);

status MemLocal_SetLocal(MemCh *m, Abstract **addr);
status MemLocal_UnSetLocal(MemCh *m, Abstract **addr);

status MemLocal_Persist(MemCh *m, Span *tbl, Str *path, Access *access);
Span *MemLocal_Load(MemCh *m, Str *path, Access *access);
status MemLocal_Destroy(MemCh *m, Str *path, Access *access);
status MemLocal_To(MemCh *m, Abstract *a);
status MemLocal_From(MemCh *m, Abstract *a);
