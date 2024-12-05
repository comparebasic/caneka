#include <external.h>
#include <caneka.h>

Abstract *String_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    String *s = (String *)asIfc(a, TYPE_STRING);
    return (Abstract *)s;
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

Abstract *FilePath_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    String *s = (String *)asIfc(a, TYPE_STRING);

    File *file = MemCtx_Alloc(m, sizeof(File));
    file->type.of = TYPE_FILE;
    file->path = s;
    file->type.state = s->type.state;

    return (Abstract *)file;
}
