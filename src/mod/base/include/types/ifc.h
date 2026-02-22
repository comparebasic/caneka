void Type_SetFlag(void *a, word flags);
cls Ifc_Get(cls inst);
boolean Ifc_Match(cls inst, cls ifc);
status Ifc_Init(struct mem_ctx *m);
extern struct lookup *IfcLookup;

void *_as(char *func, char *file, i32 line, void *x, cls type);
void *_asIfc(char *func, char *file, i32 line, void *x, cls type);
void *_asError(char *func, char *file, i32 line, void *x, cls type);

#ifndef __IFC_HEADER_FUNCS
#define __IFC_HEADER_FUNCS
#define as(x, t) _as(FUNCNAME, FILENAME, LINENUMBER, (x), (t))
#define asIfc(x, t) _asIfc(FUNCNAME, FILENAME, LINENUMBER, (x), (t))
#endif

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
