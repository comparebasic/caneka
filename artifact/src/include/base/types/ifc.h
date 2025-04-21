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
            printf("%s:%s %d vs %d\n", FILENAME, FUNCNAME, x->type.of, type);
            void *args[] = {(void *)Type_ToChars(type), (void *)Type_ToChars(x->type.of), NULL};
            Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Cast from Abstract mismatched type expecting '_c', have '_c'", args);
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
            void *args[] = {(void *)Type_ToChars(type), (void *)Type_ToChars(x->type.of), NULL};
            Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Cast from Abstract mismatched interface, expecting '_c', have '_c'", args);
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
            void *args[] = {(void *)Type_ToChars(type), (void *)Type_ToChars(x->type.of), NULL};
            Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Cast from Abstract mismatched interface with offset expecting '_c', have '_c'", args);
            return NULL;
        }
    }
    return x;
}
#endif
