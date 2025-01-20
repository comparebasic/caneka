#include <external.h>
#include <caneka.h>

Target *Target_Make(MemCtx *m, String *dir, String *fname, String *ext){
    Target *t = MemCtx_Alloc(m, sizeof(Target)); 
    t->type.of = TYPE_TARGET;
    t->dir = dir;
    t->fname = fname;
    t->ext = ext;
    return t;
}
