#include <external.h>
#include <caneka.h>

static char *srcDir = "src";
static char *distDir = "dist";

static status Trans_transDir(MemCtx *m, String *path, Abstract *source){
    Transp *p = as(source, TYPE_TRANSP);
    String *new = String_Init(m, STRING_EXTEND);
    String_Add(m, p->dist);
    String_AddBytes(m, new, bytes("/"), 1);
    String_Add(m, new, path);

    DIR* dir = opendir(path->bytes);
    if(dir){
        closedir(dir);
    }else if(ENOENT == errno){
        return Dir_CheckCreate(m, path);
    }
    return TRUE;
}

static static int Transp_transFile(Cstr *source, Cstr *dest){
    printf("\x1b[1;34m%s -> %s\x1b[0m\n", source->bytes, dest->bytes);
    return TRUE;
}

static int copy(Cstr *source, Cstr *dest){
    /*
    printf("\x1b[33m%s -> %s\x1b[0m\n", source->bytes, dest->bytes);
    */
    return TRUE;
}

int transFile(MemCtx *m, String *dir, String *file, Abstract *source){
    Transp *p = as(source, TYPE_TRANSP);
    char *C = ".c";
    char *Cnk = ".cnk";
    char *H = ".h";

    String *source = String_Init(m, STRING_EXTEND);
    String_Add(m, source, dir);
    String_AddBytes(m, source, bytes("/"), 1);
    String_Add(m, source, file);

    String *dest = String_Init(m, STRING_EXTEND);
    String_Add(m, source, p->dist);
    String_AddBytes(m, source, bytes("/"), 1);
    String_Add(m, source, dir);
    String_AddBytes(m, source, bytes("/"), 1);
    String_Add(m, source, file);

    if(String_PosEqualsBytes(source, bytes(C), strlen(H), STRING_POS_END) || 
            String_PosEqualsBytes(source, bytes(H), strlen(H), STRING_POS_END)){
        copy(&source, &dest);
    }else if(String_PosEqualsBytes(source, bytes(Cnk), strlen(Cnk), STRING_POS_END)){
        int l = strlen(dest.content);
        dest.content[l-2] = '\0';
        transpile(&source, &dest);
    }else{
        copy(dir, file);
    }

    return TRUE;
}

void Trans(){
    Cstr dir_cstr;
    Cstr_Init(&dir_cstr, srcDir);
    FolderClimb(&dir_cstr, transDir, transFile);
}

Transp *Transp_Make(MemCtx *m){
    Transp *t = MemCtx_Alloc(m, sizeof(Transp));
    t->type.of = TYPE_TRANSP;
    return t;
}
