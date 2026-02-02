#include <external.h>
#include <caneka.h>

Span *GenMimeTable = NULL;
Lookup *MimeLookup = NULL;

static void routeFuncStatic(Gen *gen, Buff *bf, Table *_data){
    MemCh *m = bf->m;
    Str *pathS = StrVec_Str(bf->m, (StrVec *)as(gen->action, TYPE_STRVEC));
    bf->type.state |= BUFF_UNBUFFERED;
    File_Open(bf, pathS, O_RDONLY);
}

static void routeFuncTempl(Gen *gen, Buff *bf, Table *data){
    MemCh *m = bf->m;
    Templ *templ = (Templ *)as(gen->action, TYPE_TEMPL);

    Templ_Reset(templ);
    templ->type.state |= bf->type.state;
    Templ_ToS(templ, bf, data, NULL);
}

static void routeFuncFmt(Gen *gen, Buff *bf, Table *_data){
    MemCh *m = bf->m;
    Fmt_ToHtml(bf, (Mess *)as(gen->action, TYPE_MESS));
}

static void routeFuncFileDb(Gen *gen, Buff *bf, Table *data){
    MemCh *m = bf->m;
    BinSegCtx *bsCtx = (BinSegCtx *)as(gen->action, TYPE_BINSEG_CTX);
    status r = READY;
    /*
    if(action == NULL){
        gen->type.state |= ERROR;
        return;
    }

    if(Equals(action, K(m, "add")) && bf != NULL){
        BinSegCtx_Send(bsCtx, bf);
    }else if(Equals(action, K(m, "modify"))){
    }else if(Equals(action, K(m, "read"))){
    }else{
        gen->type.state |= ERROR;
        return;
    }
    */

    /*
    StrVec *acceptHeader = Table_Get(ctx->headersIt.p, K(m, "Accept"));
    if(acceptHeader != NULL && Equals(acceptHeader, K(m, "text/html"))){
        Table_Set(data, K(m, "form"), ctx->body);
        return routeFuncTempl(bf, rt, data, ctx);
    }else{
        / * handle json response here * /;
    }
    */
}

void Gen_Setup(MemCh *m, Gen *gen, NodeObj *config){
    printf("Gen Setup\n");
    fflush(stdout);
    if(gen->type.state & GEN_FORBIDDEN){
        gen->type.state |= (ERROR|NOOP);
    }else if(gen->type.state & GEN_STATIC){
        /* no action */
    }else if(gen->type.state & GEN_FMT){
        StrVec *content = File_ToVec(m, StrVec_Str(m, gen->path));
        Cursor *curs = Cursor_Make(m, content); 
        Roebling *rbl = FormatFmt_Make(m, curs, NULL);
        Roebling_Run(rbl);
        if(rbl->type.state & ERROR){
            void *args[2];
            args[0] = gen->path;
            args[1] = NULL;
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Error preparing template for $", args);
            gen->type.state |= ERROR;
            DebugStack_Pop();
            return;
        }

        gen->action = (Abstract *)rbl->dest;
        gen->type.state |= PROCESSING;
    }else if(gen->type.state & GEN_DYNAMIC){
        StrVec *content = File_ToVec(m, StrVec_Str(m, gen->path));

        if(content == NULL || content->total == 0){
            void *args[3];
            args[0] = gen->path;
            args[1] = content;
            args[2] = NULL;
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Error path for Templ has no content for: $ -> @", args);
            gen->type.state |= ERROR;
            return;
        }

        Buff *bf = Buff_Make(m, ZERO);

        Cursor *curs = Cursor_Make(m, content);
        TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);

        Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
        if((Templ_Prepare(templ) & PROCESSING) == 0){
            void *args[2];
            args[0] = gen->path;
            args[1] = NULL;
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Error preparing template for $", args);
            gen->type.state |= ERROR;
            Templ_Reset(templ);
            return;
        }

        gen->action = (Abstract *)templ;
        gen->type.state |= PROCESSING;
    }else if(gen->type.state & GEN_BINSEG){
        word flags = BINSEG_REVERSED;
        
        Table *seel = NULL;
        if(config != NULL){
            NodeObj *rbsConfig =
                Inst_ByPath(config, Sv(m, "binseg"), NULL, SPAN_OP_GET, NULL);
            StrVec *actionV = Inst_Att(rbsConfig, K(m, "action"));
            Span *actions = Config_Sequence(m, actionV);
            Iter it;
            Iter_Init(&it, actions);
            while((Iter_Next(&it) & END) == 0){
                Abstract *a = Iter_Get(&it);
                flags |= BinSeg_ActionByStr(a);
            }

            StrVec *seelName = Inst_Att(rbsConfig, K(m, "seel"));
            if(seelName != NULL){
                seel = Table_Get(SeelByName, seelName);
            }
        }

        BinSegCtx *ctx = BinSegCtx_Make(m, flags);
        BinSegCtx_Open(ctx, StrVec_Str(m, gen->path));
        ctx->seel = seel;
        gen->action = (Abstract *)ctx;
        gen->type.state |= PROCESSING;
    }
}

Gen *Gen_FromPath(MemCh *m, StrVec *path, NodeObj *config){
    StrVec *ext = IoUtil_GetExt(m, path);
    Gen *def = (Gen *)Table_Get(GenMimeTable, ext);
    if(def == NULL){
        void *ar[] = {GenMimeTable, path, NULL};
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Gen Def not found for ext @ of @", ar);
        return NULL;    

    }
    Gen *gen = Gen_Copy(m, def);
    gen->path = path;
    gen->source = (Abstract *)config;
    return gen;
}

Gen *Gen_Make(MemCh *m){
    Gen *gen = MemCh_AllocOf(m, sizeof(Gen), TYPE_GEN);
    gen->type.of = TYPE_GEN;
    return gen;
}

Gen *Gen_Copy(MemCh *m, Gen *_gen){
    Gen *gen = MemCh_AllocOf(m, sizeof(Gen), TYPE_GEN);
    gen->type.of = TYPE_GEN;
    gen->type.state = _gen->type.state;
    gen->objType.of = _gen->objType.of;
    gen->func = _gen->func;
    return gen;
}

void Gen_Init(MemCh *m){
    if(GenMimeTable == NULL){
        GenMimeTable = Table_Make(m);
        MimeLookup = Lookup_Make(m, _MIME_START);

        Gen *gen = Gen_Make(m);
        gen->path = Sv(m, "html");
        gen->type.state = GEN_STATIC;
        gen->objType.of = MIME_HTML;
        gen->func = routeFuncStatic;
        Table_Set(GenMimeTable, gen->path, gen);

        gen = Gen_Make(m);
        gen->path = Sv(m, "png");
        gen->type.state = (GEN_ASSET|GEN_STATIC);
        gen->objType.of = MIME_PNG;
        gen->func = routeFuncStatic;
        Table_Set(GenMimeTable, gen->path, gen);
        Lookup_Add(m, MimeLookup, MIME_HTML, S(m, "image/png"));

        gen = Gen_Make(m);
        gen->path = Sv(m, "jpg");
        gen->type.state = (GEN_ASSET|GEN_STATIC);
        gen->objType.of = MIME_JPEG;
        gen->func = routeFuncStatic;
        Table_Set(GenMimeTable, gen->path, gen);

        gen = Gen_Make(m);
        gen->path = Sv(m, "js");
        gen->type.state = (GEN_ASSET|GEN_STATIC);
        gen->objType.of = MIME_JAVASCRIPT;
        gen->func = routeFuncStatic;
        Table_Set(GenMimeTable, gen->path, gen);

        gen = Gen_Make(m);
        gen->path = Sv(m, "css");
        gen->type.state = (GEN_ASSET|GEN_STATIC);
        gen->objType.of = MIME_CSS;
        gen->func = routeFuncStatic;
        Table_Set(GenMimeTable, gen->path, gen);

        gen = Gen_Make(m);
        gen->path = Sv(m, "txt");
        gen->type.state = (GEN_ASSET|GEN_STATIC);
        gen->objType.of = MIME_TEXT;
        gen->func = routeFuncStatic;
        Table_Set(GenMimeTable, gen->path, gen);

        gen = Gen_Make(m);
        gen->path = Sv(m, "fmt");
        gen->type.state = GEN_FMT;
        gen->objType.of = MIME_HTML;
        gen->func = routeFuncFmt;
        Table_Set(GenMimeTable, gen->path, gen);

        gen = Gen_Make(m);
        gen->path = Sv(m, "rbs");
        gen->type.state = GEN_BINSEG;
        gen->objType.of = MIME_JSON;
        gen->func = routeFuncFileDb;
        Table_Set(GenMimeTable, gen->path, gen);

        gen = Gen_Make(m);
        gen->path = Sv(m, "templ");
        gen->type.state = GEN_DYNAMIC;
        gen->objType.of = MIME_HTML;
        gen->func = routeFuncTempl;
        Table_Set(GenMimeTable, gen->path, gen);

        Lookup_Add(m, MimeLookup, MIME_HTML, S(m, "text/html"));
        Lookup_Add(m, MimeLookup, MIME_CSS, S(m, "image/css"));
        Lookup_Add(m, MimeLookup, MIME_TEXT, S(m, "text/plain"));
        Lookup_Add(m, MimeLookup, MIME_PNG, S(m, "image/png"));
        Lookup_Add(m, MimeLookup, MIME_JPEG, S(m, "image/jpeg"));
        Lookup_Add(m, MimeLookup, MIME_JAVASCRIPT, S(m, "text/javascript"));
        Lookup_Add(m, MimeLookup, MIME_JSON, S(m, "application/json"));

    }
}
