#include <external.h>
#include <caneka.h>

Target *Target_Make(MemCtx *m, String *dir, String *fname, String *ext){
    Target *t = MemCtx_Alloc(m, sizeof(Target)); 
    t->type.of = TYPE_TARGET;
    t->dir = dir;
    t->fname = fname;
    t->ext = ext;

    String *path = String_Init(m, STRING_EXTEND);
    String_Add(m, path, dir);
    String_AddBytes(m, path, bytes("/"), 1);
    String_Add(m, path, fname);
    String_Add(m, path, ext);

    t->path = path;

    return t;
}
