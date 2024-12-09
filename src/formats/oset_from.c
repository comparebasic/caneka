#include <external.h>
#include <caneka.h>

Abstract *String_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    String *s = (String *)asIfc(a, TYPE_STRING);
    return (Abstract *)s;
}

Abstract *Blank_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    String *s = (String *)asIfc(a, TYPE_STRING);
    Blank *b = Blank_Make(m);
    b->length = s->length;
    return (Abstract *)b;
}

Abstract *I64_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    String *s = (String *)asIfc(a, TYPE_STRING);
    return (Abstract *)I64_Wrapped(m, I64_FromString((String *)a));
}

Abstract *Iter_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    return (Abstract *)o->item->value;
}

Abstract *Table_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    return (Abstract *)o->item->value;
}

Abstract *Span_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    return (Abstract *)o->item->value;
}

Abstract *Auth_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    return (Abstract *)o->item->value;
}

Abstract *EncPair_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    return (Abstract *)o->item->value;
}

Abstract *FilePath_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    String *s = (String *)asIfc(a, TYPE_STRING);

    File *file = MemCtx_Alloc(m, sizeof(File));
    File_Init(file, s, NULL, NULL);

    Debug_Print((void *)file->path, 0, "File From Oset: ", COLOR_PURPLE, TRUE); 
    printf("\n");

    file->type.state = s->type.state;

    return (Abstract *)file;
}
