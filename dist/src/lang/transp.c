#include <external.h>
#include <caneka.h>

static char *srcDir = "src";
static char *distDir = "dist";

static status Transp_onInput(MemCtx *m, String *s, Abstract *_fmt){
    FmtCtx *fmt = asIfc(_fmt, TYPE_FMT_CTX);
    Roebling_Add(fmt->rbl, s);
    while((Roebling_RunCycle(fmt->rbl) & (SUCCESS|END|ERROR|BREAK)) == 0);
    return fmt->type.state;
}

static status Transp_transpile(Transp *p, FmtCtx *fmt){
#ifdef DEBUG_STACK
    DebugStack_Push(bytes("Transp_transpile"), NULL);
#endif
    if(File_CmpUpdated(p->m, p->current.source, p->current.dest, NULL)){
        Debug_Print((void *)p->current.source,0,  "Transpiling ",  COLOR_YELLOW, FALSE);
        Debug_Print((void *)p->current.dest,0,  " -> ",  COLOR_YELLOW, FALSE);
        printf("\n");
        printf("%s\n", p->current.sourceFile.abs->bytes);

        status r = File_Stream(p->m, &p->current.sourceFile,
            NULL, Transp_onInput, (Abstract *)fmt);

        Transp_PrintTree(p);

        printf("done!\n");

#ifdef DEBUG_STACK
        DebugStack_Pop();
#endif
        return r;
    }
#ifdef DEBUG_STACK
    DebugStack_Pop();
#endif
    return NOOP;
}

static status Transp_copy(Transp *p){
    if(File_CmpUpdated(p->m, p->current.source, p->current.dest, NULL)){
        Debug_Print((void *)p->current.dest,0,  "Updating :",  COLOR_YELLOW, FALSE);
        printf("\n");
        return File_Copy(p->m, p->current.source, p->current.dest, NULL);
    }
    return NOOP;
}

static status Transp_transDir(MemCtx *m, String *path, Abstract *source){
    Transp *p = asIfc(source, TYPE_TRANSP);
    String *new = String_Init(m, STRING_EXTEND);
    String_Add(m, new, p->dist);
    String_AddBytes(m, new, bytes("/"), 1);
    String_Add(m, new, path);

    DIR* dir = opendir(String_ToChars(m, new));
    if(dir){
        closedir(dir);
    }else if(ENOENT == errno){
        Debug_Print((void *)path, 0, "Making Dir:", COLOR_YELLOW, TRUE);
        printf("\n");
        return Dir_CheckCreate(m, new);
    }
    return TRUE;
}

static status Transp_transFile(MemCtx *m, String *dir, String *file, Abstract *source){
    Transp *p = asIfc(source, TYPE_TRANSP);
    p->dir = dir;
    p->file = file;

    p->current.source = String_Init(m, STRING_EXTEND);
    String_Add(m, p->current.source, dir);
    String_AddBytes(m, p->current.source, bytes("/"), 1);
    String_Add(m, p->current.source, file);

    p->current.dest = String_Init(m, STRING_EXTEND);
    String_Add(m, p->current.dest, p->dist);
    String_AddBytes(m, p->current.dest, bytes("/"), 1);
    String_Add(m, p->current.dest, dir);
    String_AddBytes(m, p->current.dest, bytes("/"), 1);
    String_Add(m, p->current.dest, file);

    Iter it;
    Iter_Init(&it, p->formats);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            String *abbrev = (String *)h->item;
            if(String_PosEqualsBytes(p->current.source, abbrev->bytes, abbrev->length, STRING_POS_END)){
                String_Trunc(p->current.dest, -1);

                Spool_Init(&p->current.sourceFile, p->current.source, NULL, NULL);
                Spool_Init(&p->current.destFile, p->current.dest, NULL, NULL);
                return Transp_transpile(p, (FmtCtx *)h->value);
            }
        }
    }

    return Transp_copy(p);

    return TRUE;
}

void Transp_PrintFmtTree(Transp *p, FmtCtx *fmt, FmtItem *item, int indent){
    if(item->value != NULL){
        Iter it;
        Iter_Init(&it, (Span *)item->value);
        while((Iter_Next(&it) & END) == 0){
            Abstract *a = Iter_Get(&it);
            if(a != NULL){
                if(Ifc_Match(a->type.of, TYPE_FMT_ITEM)){
                    FmtItem *item = (FmtItem *)a;
                    int i = indent;
                    while(i--){
                        printf("  ");
                    }
                    printf("Found: %s\n", fmt->rangeToChars(it.idx));
                    Transp_PrintFmtTree(p, fmt, item, indent+1);
                }else{
                    int i = indent;
                    while(i--){
                        printf("  ");
                    }
                    Debug_Print((void *)a, 0, "Item: ", 0, FALSE);
                    printf("\n");
                }
            }
        }
    }
}

void Transp_PrintTree(Transp *p){
    Iter it;
    Iter_Init(&it, p->formats);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            FmtCtx *fmt = (FmtCtx *)h->value;
            printf("\x1b[%dmExporting package:%s module:%s\x1b[0m\n", COLOR_PURPLE, p->dir->bytes, p->file->bytes);
            Transp_PrintFmtTree(p, fmt, fmt->root, 0);
        }
    }
}

status Transp_Trans(Transp *p){
    return Dir_Climb(p->m, p->src, Transp_transDir, Transp_transFile, (Abstract *)p);
}

Transp *Transp_Make(MemCtx *m){
    Transp *t = MemCtx_Alloc(m, sizeof(Transp));
    t->type.of = TYPE_TRANSP;
    t->m = m;
    t->formats = Span_Make(m, TYPE_TABLE); 
    return t;
}
