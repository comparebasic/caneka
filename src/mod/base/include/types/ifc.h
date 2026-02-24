void Type_SetFlag(void *a, word flags);
cls Ifc_Get(cls inst);
boolean Ifc_Match(cls inst, cls ifc);
extern struct lookup *IfcLookup;

typedef struct ifc_map {
    Type type;
    Type objType;
    struct {
        i16 start;
        i16 end;
    } offset;
    i64 size;
    SourceMakerFunc func;
} IfcMap;

void Type_SetFlag(void *_a, word flags);
IfcMap *IfcMap_Make(struct mem_ctx *m, 
    cls typeOf, i16 start, i16 end, i64 size);
void *Ifc(struct mem_ctx *m, void *_a, cls typeOf);
void Ifc_Init(struct mem_ctx *m);
