void Type_SetFlag(Abstract *a, word flags);
cls Ifc_Get(cls inst);
boolean Ifc_Match(cls inst, cls ifc);
status Ifc_Init(struct mem_ctx *m);
extern struct lookup *SizeLookup;

Abstract *_as(char *func, char *file, i32 line, Abstract *x, cls type);
Abstract *_asIfc(char *func, char *file, i32 line, Abstract *x, cls type);
Abstract *_asError(char *func, char *file, i32 line, Abstract *x, cls type);

#ifndef __IFC_HEADER_FUNCS
#define __IFC_HEADER_FUNCS
#define as(x, t) _as(FUNCNAME, FILENAME, LINENUMBER, (x), (t))
#define asLegal(x, t) (MemBook_Check(x) == NULL ? \
    _asError(FUNCNAME, FILENAME, LINENUMBER, (x), (t)) :\
    _as(FUNCNAME, FILENAME, LINENUMBER, (x), (t)))
#define asIfc(x, t) _asIfc(FUNCNAME, FILENAME, LINENUMBER, (x), (t))
#endif
