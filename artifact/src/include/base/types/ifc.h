void Type_SetFlag(Abstract *a, word flags);
cls Ifc_Get(cls inst);
boolean Ifc_Match(cls inst, cls ifc);

#ifndef __IFC_HEADER_FUNCS
#define __IFC_HEADER_FUNCS
static inline Abstract *as(Abstract *x, cls type){
    if(x == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Cast from NULL", NULL);
    }else {
        if(x->type.of != type){
            Abstract *args[] = {
                (Abstract *)Str_CstrRef(_debugM, Type_ToChars(type)),
                (Abstract *)Str_CstrRef(_debugM, Type_ToChars(x->type.of)),
                NULL
            };
            Fatal(FUNCNAME, FILENAME, LINENUMBER, "Cast from Abstract mismatched type expecting '$', have '$'", args);
            return NULL;
        }
    }
    return x;
}

static inline Abstract *asIfc(Abstract *x, cls type){
    if(x == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Cast from NULL", NULL);
    }else{
        if(!Ifc_Match(x->type.of, type)){
            Abstract *args[] = {
                (Abstract *)Str_CstrRef(_debugM, Type_ToChars(type)),
                (Abstract *)Str_CstrRef(_debugM, Type_ToChars(x->type.of)),
                NULL
            };
            Fatal(FUNCNAME, FILENAME, LINENUMBER, "Cast from Abstract mismatched interface, expecting '$', have '$'", args);
            return NULL;
        }
    }
    return x;
}

static inline Abstract *asIfcOffset(Abstract *x, cls type, word offset){
    if(x == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Cast from NULL", NULL);
    }else{
        if(!Ifc_Match(x->type.of-HTYPE_LOCAL, type)){
            Abstract *args[] = {
                (Abstract *)Str_CstrRef(_debugM, Type_ToChars(type)),
                (Abstract *)Str_CstrRef(_debugM, Type_ToChars(x->type.of)),
                NULL
            };
            Fatal(FUNCNAME, FILENAME, LINENUMBER, "Cast from Abstract mismatched interface with offset expecting '$', have '$'", args);
            return NULL;
        }
    }
    return x;
}
#endif
