void Type_SetFlag(Abstract *a, word flags);
cls Ifc_Get(cls inst);
boolean Ifc_Match(cls inst, cls ifc);

#ifndef __IFC_HEADER_FUNCS
#define __IFC_HEADER_FUNCS
static inline Abstract *as(Abstract *x, cls type){
    if(x == NULL){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Cast from NULL", NULL);
    }else {
        if(x->type.of != type){
            const char *typeCstr = Type_ToChars(type); 
            const char *typeOfCstr = Type_ToChars(x->type.of); 
            void *args[] = {(void *)typeCstr, (void *)typeOfCstr, NULL};
            Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Cast from Abstract mismatch expecting '_c', have '_c'", args);
            return NULL;
        }
    }
    return x;
}

static inline Abstract *asIfc(Abstract *x, cls type){
    if(x == NULL){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Cast from NULL", NULL);
    }else{
        if(!Ifc_Match(x->type.of, type)){
            const char *typeCstr = Type_ToChars(type); 
            const char *typeOfCstr = Type_ToChars(x->type.of); 
            void *args[] = {(void *)typeCstr, (void *)typeOfCstr, NULL};
            Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Cast from Abstract mismatch expecting '_c', have '_c'", args);
            return NULL;
        }
    }
    return x;
}

static inline Abstract *asIfcOffset(Abstract *x, cls type, word offset){
    if(x == NULL){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Cast from NULL", NULL);
    }else{
        if(!Ifc_Match(x->type.of-HTYPE_LOCAL, type)){
            const char *typeCstr = Type_ToChars(type); 
            const char *typeOfCstr = Type_ToChars(x->type.of); 
            void *args[] = {(void *)typeCstr, (void *)typeOfCstr, NULL};
            Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Cast from Abstract mismatch expecting '_c', have '_c'", args);
            return NULL;
        }
    }
    return x;
}
#endif
