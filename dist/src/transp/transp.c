#include <external.h>
#include <caneka.h>

static status Transp_onInput(MemCtx *m, String *s, Abstract *_fmt){
    FmtCtx *fmt = asIfc(_fmt, TYPE_FMT_CTX);
    Roebling_Add(fmt->rbl, s);
    while((Roebling_RunCycle(fmt->rbl) & (SUCCESS|END|ERROR)) == 0);
    return fmt->type.state;
}

static status Transp_transpile(Transp *p, FmtCtx *fmt){
    Target *t =  Span_GetSelected(p->targets->values);
    if(File_CmpUpdated(p->m, p->source->path, t->path, NULL)){
        if(fmt->Setup != NULL){
            fmt->Setup(fmt, p->targets);
        }
        status r = File_Stream(p->m, &(p->source->sourceFile),
            NULL, Transp_onInput, (Abstract *)fmt);

        fprintf(stderr, "\n\x1b[%dmFinished parsing\x1b[0m\n", COLOR_BLUE);

        return r;
    }
    return NOOP;
}

static status Transp_transDir(MemCtx *m, String *path, Abstract *source){
    DIR* dir = opendir(String_ToChars(m, path));
    if(dir){
        closedir(dir);
    }else if(ENOENT == errno){
        Debug_Print((void *)path, 0, "Making Dir:", COLOR_YELLOW, TRUE);
        printf("\n");
        return Dir_CheckCreate(m, path);
    }

    return NOOP;
}

static status Transp_transFile(MemCtx *m, String *dir, String *fname, Abstract *source){
    Transp *p = asIfc(source, TYPE_TRANSP);
    
    Match mt;
    String *backlog = String_Init(m, STRING_EXTEND);
    backlog->type.state |= FLAG_STRING_CONTIGUOUS;
    ExtMatch_Init(&mt, backlog);
    String *ext = String_SubMatch(m, fname, &mt);
    Source_Reset(m, p->source, dir, fname, ext);

    Abstract *a = Table_Get(p->fmts, (Abstract *)ext);
    if(a != NULL && Ifc_Match(a->type.of, TYPE_FMT_CTX)){
        FmtCtx *fmt = (FmtCtx *)a;
        p->fmts->metrics.selected = p->fmts->metrics.get;
        DPrint((Abstract *)p->source->path, COLOR_PURPLE, "Transpiling");
        return SUCCESS;
        /*
        Spool_Init(&p->current.sourceFile, path, NULL, NULL);
        return Transp_transpile(p, fmt);
        */
    }else{
        DPrint((Abstract *)p->source->path, COLOR_PURPLE, "Not Transpiling");
        return SUCCESS;
        return NOOP;
    }
}

status Transp_Copy(Transp *p){
    Target *t = Span_GetSelected(p->targets->values);
    if(File_CmpUpdated(p->m, p->source->path, t->path, NULL)){
        return File_Copy(p->m, p->source->path, t->path, NULL);
    }
    return NOOP;
}

status Transp_Out(Transp *p, String *s, word targetId){
    Target *t = Lookup_Get(p->targets, targetId);
    return Spool_Add(p->m, s, (Abstract *)&t->destFile);
}

status Transp_Trans(Transp *p){
    return Dir_Climb(p->m, p->src, NULL, Transp_transFile, (Abstract *)p);
}

Transp *Transp_Make(MemCtx *m){
    Transp *t = MemCtx_Alloc(m, sizeof(Transp));
    t->type.of = TYPE_TRANSP;
    t->m = m;
    t->source = Source_Make(m);
    return t;
}
