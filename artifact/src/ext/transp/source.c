#include <external.h>
#include <caneka.h>

status Source_Reset(MemCtx *m, Source *source, String *dir, String *fname, String *ext){

    String *path = String_Init(m, STRING_EXTEND);
    String_Add(m, path, dir);
    String_AddBytes(m, path, bytes("/"), 1);
    String_Add(m, path, fname);

    source->path = path;
    source->dir = dir;
    String_Trunc(fname, -(ext->length));
    source->fname = fname;
    source->ext = ext;

    return SUCCESS;
}

Source *Source_Make(MemCtx *m){
    Source *s = MemCtx_Alloc(m, sizeof(Source)); 
    s->type.of = TYPE_SOURCE;
    return s;
}
