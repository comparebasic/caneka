void Type_SetFlag(void *a, word flags);
cls Ifc_Get(cls inst);
boolean Ifc_Match(cls inst, cls ifc);
status Ifc_Init(struct mem_ctx *m);
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

IfcMap *IfcMap_Make(MemCh *m, cls typeOf, i16 start, i16 end, i64 size);
