#include <external.h>
#include <caneka.h>

static Str **genLabels = NULL;

static status Gen_Print(Buff *bf, void *a, cls type, word flags){
    Gen *gen = (Gen *)a;
    void *args[] = {
        Type_StateVec(bf->m, TYPE_GEN, gen->type.state),
        Lookup_Get(MimeLookup, gen->objType.of),
        gen->path,
        gen->action,
        NULL
    };
    return Fmt(bf, "Gen<@ @ @ @>", args);
}

void Www_Init(MemCh *m){

    if(genLabels == NULL){
        genLabels = (Str **)Arr_Make(m, 17);
        genLabels[9] = Str_CstrRef(m, "STATIC");
        genLabels[10] = Str_CstrRef(m, "DYNAMIC");
        genLabels[11] = Str_CstrRef(m, "FMT");
        genLabels[12] = Str_CstrRef(m, "INDEX");
        genLabels[13] = Str_CstrRef(m, "BINSEG");
        genLabels[14] = Str_CstrRef(m, "ASSET");
        genLabels[15] = Str_CstrRef(m, "ACTION");
        genLabels[16] = Str_CstrRef(m, "FORBIDDEN");
        Lookup_Add(m, ToSFlagLookup, TYPE_GEN, (void *)genLabels);
    }

    Lookup_Add(m, ToStreamLookup, TYPE_GEN, (void *)Gen_Print);
}
