struct ifc_map;
void Type_SetFlag(void *a, word flags);
extern struct lookup *IfcLookup;
typedef void *(*IfcMapFunc)(struct mem_ctx *, void *a, struct ifc_map *imap);

typedef struct ifc_map {
    Type type;
    cls typeOf;
    i16 size;
    struct {
        i16 start;
        i16 end;
    } offset;
    IfcMapFunc func;
    struct ifc_map *next;
} IfcMap;

void Type_SetFlag(void *_a, word flags);
IfcMap *Ifc_Get(Abstract *a, cls typeOf);
cls Ifc_GetRoot(cls typeOf);
IfcMap *IfcMap_Make(struct mem_ctx *m,
        cls typeOf, i16 start, i16 end, i16 size, IfcMapFunc func);
void *Ifc(struct mem_ctx *m, void *_a, cls typeOf);
void Ifc_Init(struct mem_ctx *m);
