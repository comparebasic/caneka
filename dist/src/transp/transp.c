#include <external.h>
#include <caneka.h>

static status Transp_onInput(MemCtx *m, String *s, Abstract *_fmt){
    DebugStack_Push(_fmt, _fmt->type.of);
    FmtCtx *fmt = asIfc(_fmt, TYPE_FMT_CTX);
    if(DEBUG_LANG_TRANSP){
        /*
        DPrint((Abstract *)s, DEBUG_LANG_TRANSP, "Transp_onInput s:");
        */
    }
    Roebling_Add(fmt->rbl, s);
    while((Roebling_RunCycle(fmt->rbl) & (SUCCESS|END|ERROR)) == 0);
    DebugStack_Pop();
    return fmt->type.state;
}

static status Transp_transpile(Transp *p, FmtCtx *fmt){
    DebugStack_Push(p->source->path, p->source->path->type.of);
    if(fmt->Setup != NULL){
        fmt->Setup(fmt, p);
    }
    Target *t = Span_GetSelected(p->targets->values);
    if(File_CmpUpdated(p->m, p->source->path, t->path, NULL)){
        Roebling_Reset(fmt->rbl->m, fmt->rbl, NULL);
    
        status r = File_Stream(p->m, &(p->source->sourceFile),
            NULL, Transp_onInput, (Abstract *)fmt);

        fprintf(stderr, "\n\x1b[%dmFinished parsing\x1b[0m\n", COLOR_BLUE);

        DebugStack_Pop();
        return r;
    }
    DebugStack_Pop();
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
    DebugStack_Push(fname, fname->type.of); 
    Transp *p = asIfc(source, TYPE_TRANSP);
    
    Match mt;
    String *backlog = String_Init(m, STRING_EXTEND);
    backlog->type.state |= FLAG_STRING_CONTIGUOUS;
    ExtMatch_Init(&mt, backlog);

    String *ext = String_SubMatch(m, fname, &mt);
    Source_Reset(m, p->source, dir, fname, ext);

    Abstract *a = Table_Get(p->fmts, (Abstract *)ext);
    if(a != NULL && Ifc_Match(a->type.of, TYPE_WRAPPED_DO)){
        Single *sg = (Single *)a;
        sg->val.dof(m, (Abstract *)p);
        DebugStack_Pop();
        return SUCCESS;
    }else if(a != NULL){
        FmtCtx *fmt = (FmtCtx *)a;
        p->fmts->metrics.selected = p->fmts->metrics.get;
        DPrint((Abstract *)p->source->path, COLOR_PURPLE, "Transpiling: ");
        Spool_Init(&p->source->sourceFile, p->source->path, NULL, NULL);
        status r = Transp_transpile(p, fmt);
        DebugStack_Pop();

        exit(1);

        return r;
    }else{
        DPrint((Abstract *)p->source->path, COLOR_PURPLE, "Not Transpiling");
        DebugStack_Pop();
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
    DebugStack_Push(p, p->type.of); 
    status r =  Dir_Climb(p->m, p->src, NULL, Transp_transFile, (Abstract *)p);
    DebugStack_Pop();
    return r;
}

Transp *Transp_Make(MemCtx *m){
    Transp *t = MemCtx_Alloc(m, sizeof(Transp));
    t->type.of = TYPE_TRANSP;
    t->m = m;
    t->source = Source_Make(m);
    t->fmts = Span_Make(m, TYPE_TABLE);
    t->targets = Lookup_Make(m, _APP_BOUNDRY_START, NULL, NULL);
    return t;
}
