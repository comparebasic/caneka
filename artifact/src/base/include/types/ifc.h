void Type_SetFlag(Abstract *a, word flags);
cls Ifc_Get(cls inst);
boolean Ifc_Match(cls inst, cls ifc);
status Ifc_Init(MemCh *m);
extern Lookup *SizeLookup;

#ifndef __IFC_HEADER_FUNCS
#define __IFC_HEADER_FUNCS
static inline Abstract *_as(char *func, char *file, i32 line, Abstract *x, cls type){
    if(x == NULL){
        Fatal(func, file, line, "Cast from NULL", NULL);
    }else {
        if(x->type.of != type){
            Abstract *args[] = {
                (Abstract *)Str_CstrRef(_debugM, Type_ToChars(type)),
                (Abstract *)Str_CstrRef(_debugM, Type_ToChars(x->type.of)),
                NULL
            };
            Fatal(func, file, line, "Cast from Abstract mismatched type expecting '$', have '$'", args);
            return NULL;
        }
    }
    return x;
}

static inline Abstract *_asIfc(char *func, char *file, i32 line, Abstract *x, cls type){
    if(x == NULL){
        Fatal(func, file, line, "Cast from NULL", NULL);
    }else{
        if(!Ifc_Match(x->type.of, type)){
            Abstract *args[] = {
                (Abstract *)Str_CstrRef(_debugM, Type_ToChars(type)),
                (Abstract *)Str_CstrRef(_debugM, Type_ToChars(x->type.of)),
                NULL
            };
            Fatal(func, file, line, "Cast from Abstract mismatched interface, expecting '$', have '$'", args);
            return NULL;
        }
    }
    return x;
}

#define as(x, t) _as(FUNCNAME, FILENAME, LINENUMBER, (x), (t))
#define asIfc(x, t) _asIfc(FUNCNAME, FILENAME, LINENUMBER, (x), (t))
#endif
