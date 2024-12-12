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

static Transp_writeOut(MemCtx *m, String *s, Abstract *_tp){
    Transp *tp = as(_tp, TYPE_TRANSP);
    Spool_Add(m, s, (Abstract *)tp->dest);
    return tp->type.state;
}

static Transp_onInput(MemCtx *m, String *s, Abstract *_tp){
    Transp *tp = as(_tp, TYPE_TRANSP);
    Roebling_Add(tp->rbl, s);
    while((Roebling_RunCycle(tp->rbl) & (SUCCESS|END|ERROR)) == 0);
    return tp->type.state;
}

static status Transp_transpile(MemCtx *m, Transp *tp){
    File *in = Spool_Make(m, source, NULL, NULL);
    return File_Load(m, in, NULL, NULL, Transp_onInput);
}

static int Transp_copy(Cstr *source, Cstr *dest){
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
        Transp_copy(&source, &dest);
    }else if(String_PosEqualsBytes(source, bytes(Cnk), strlen(Cnk), STRING_POS_END)){
        int l = strlen(dest.content);
        dest.content[l-2] = '\0';

        Spool_Init(p->source, source, NULL, NULL);
        Spool_Init(p->dest, dest, NULL, NULL);
        Transp_transpile(m, p);
    }else{
        copy(dir, file);
    }

    return TRUE;
}

void Trans(MemCtx *m, String *src, String *dist){
    Transp *tp = Transp_Make(m);
    FolderClimb(m, transDir, transFile);
}

Transp *Transp_Make(MemCtx *m){
    Transp *t = MemCtx_Alloc(m, sizeof(Transp));
    t->type.of = TYPE_TRANSP;
    return t;
}
