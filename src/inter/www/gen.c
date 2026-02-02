#include <external.h>
#include <caneka.h>

static Span *AllExtSpan = NULL;
Span *RouteFuncTable = NULL;
Span *RouteMimeTable = NULL;

static status routeFuncStatic(Buff *bf,
        Route *rt, Table *_data, HttpCtx *ctx){
    MemCh *m = bf->m;
    Str *pathS = StrVec_Str(bf->m,
        (StrVec *)as(Seel_Get(rt, K(m, "action")), TYPE_STRVEC));
    bf->type.state |= BUFF_UNBUFFERED;
    return File_Open(bf, pathS, O_RDONLY);
}

static status routeFuncTempl(Buff *bf,
        Route *rt, Table *data, HttpCtx *ctx){
    MemCh *m = bf->m;
    Templ *templ = (Templ *)as(Seel_Get(rt, K(m, "templ")), TYPE_TEMPL);

    Templ_Reset(templ);
    templ->type.state |= bf->type.state;
    Templ_ToS(templ, bf, data, NULL);
    return templ->type.state;
}

static status routeFuncFmt(Buff *bf,
        Route *rt, Table *_data, HttpCtx *ctx){
    MemCh *m = bf->m;
    return Fmt_ToHtml(bf,
        (Mess *)as(Seel_Get(rt, K(m, "action")), TYPE_MESS));
}

static status routeFuncFileDb(Buff *bf,
        Route *rt, Table *data, HttpCtx *ctx){
    MemCh *m = bf->m;
    BinSegCtx *bsCtx = (BinSegCtx *)as(Seel_Get(rt, K(m, "action")), TYPE_BINSEG_CTX);
    Abstract *action = Table_Get(ctx->queryIt.p, K(m, "action"));
    status r = READY;
    if(action == NULL){
        ctx->code = 403;
        ctx->type.state |= ERROR;
        return ctx->type.state;
    }
    if(Equals(action, K(m, "add")) && ctx->body != NULL &&
            ctx->body->type.of == TYPE_TABLE || 
            ctx->body->type.of == TYPE_SPAN || 
            (ctx->body->type.of & TYPE_INSTANCE) 
        ){
        BinSegCtx_Send(bsCtx, ctx->body);
    }else if(Equals(action, K(m, "modify"))){
        /* modify and existing binseg record */;
    }else if(Equals(action, K(m, "read"))){
        /* read from binseg */;
    }else{
        ctx->code = 403;
        ctx->type.state |= ERROR;
        return ctx->type.state;
    }

    StrVec *acceptHeader = Table_Get(ctx->headersIt.p, K(m, "Accept"));
    if(acceptHeader != NULL && Equals(acceptHeader, K(m, "text/html"))){
        Table_Set(data, K(m, "form"), ctx->body);
        return routeFuncTempl(bf, rt, data, ctx);
    }else{
        /* handle json response here */;
    }

    return NOOP;
}

static Templ *prepareTempl(Route *rt, StrVec *path){
    void *args[3];
    MemCh *m = rt->m;
    StrVec *content = File_ToVec(m, StrVec_Str(m, path));

    if(content == NULL || content->total == 0){
        args[0] = path;
        args[1] = content;
        args[2] = NULL;
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error path for Templ has no content for: $ -> @", args);
        rt->type.state |= ERROR;
        return NULL;
    }

    Buff *bf = Buff_Make(m, ZERO);

    Cursor *curs = Cursor_Make(m, content);
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);

    Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
    if((Templ_Prepare(templ) & PROCESSING) == 0){
        args[0] = path;
        args[1] = NULL;
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error preparing template for $", args);
        rt->type.state |= ERROR;
        Templ_Reset(templ);
        return NULL;
    }
    return templ;
}

Abstract *Gen_FromPath(MemCh *m, StrVec *path, NodeObj *config){
    StrVec *ext = IoUtil_GetExt(m, path);
    Str *mime = (Str *)Table_Get(RouteMimeTable, ext);
    Single *funcW = (Single *)Table_Get(RouteFuncTable, ext);
    Gen_Setup(gen, path, mime, funcW, config);
    return gen;
}

void Gen_Setup(Inst *gen, StrVec *path, Str *mime, Single *funcW, NodeObj *config){

    Seel_Set(gen, K(m, "path"), path);
    Seel_Set(gen, K(m, "mime"), mime);
    Seel_Set(gen, K(m, "func"), funcW);
    void *action = NULL;

    if(funcW == NULL || (funcW->type.state & ROUTE_FORBIDDEN)){
        /* no action */
    }else if(funcW->type.state & ROUTE_STATIC){
        /* no action */
        action = path;
    }else if(funcW->type.state & ROUTE_FMT){
        StrVec *content = File_ToVec(m, path);
        Cursor *curs = Cursor_Make(m, content); 
        Roebling *rbl = FormatFmt_Make(m, curs, NULL);
        Roebling_Run(rbl);
        if(rbl->type.state & ERROR){
            args[0] = path;
            args[1] = NULL;
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Error preparing template for $", args);
            rt->type.state |= ERROR;
            DebugStack_Pop();
            return NULL;
        }

        action = rbl->dest;
    }else if(funcW->type.state & ROUTE_DYNAMIC){
        Templ *templ = prepareTempl(rt, path);
        if(templ != NULL){
            Span_Set(rt, ROUTE_PROPIDX_TEMPL, templ);
        }
        action = templ;
    }else if(funcW->type.state & ROUTE_BINSEG){
        word flags = BINSEG_REVERSED;
        
        Table *seel = NULL;
        if(config != NULL){
            NodeObj *rbsConfig = Inst_ByPath(config, Sv(m, "binseg"), NULL, SPAN_OP_GET);
            StrVec *actionV = NodeObj_Att(rbsConfig, K(m, "action"));
            Span *actions = Config_Sequence(m, actionV);
            Iter it;
            Iter_Init(&it, actions);
            while((Iter_Next(&it) & END) == 0){
                Abstract *a = Iter_Get(&it);
                flags |= BinSeg_ActionByStr(a);
            }

            StrVec *seelName = NodeObj_Att(rbsConfig, K(m, "seel"));
            if(seelName != NULL){
                seel = Table_Get(SeelByName, seelName);
            }

            StrVec *incPath = StrVec_Copy(m, path);
            IoUtil_SwapExt(m, incPath, S(m, "tinc"));

            if(File_PathExists(m, StrVec_Str(m, incPath))){
                Templ *templ = prepareTempl(rt, incPath);
                if(templ != NULL){
                    Span_Set(rt, ROUTE_PROPIDX_TEMPL, templ);
                }
            }
        }

        BinSegCtx *ctx = BinSegCtx_Make(m, flags);
        BinSegCtx_Open(ctx, StrVec_Str(m, path));
        ctx->seel = seel;
        action = ctx;
    }

    if(action != NULL){
        Seel_Set(gen, K(m, "action"), action);
    }
}

status Gen_Init(MemCh *m){

    Table *seel = Table_Make(m);
    Table_Set(seel, S(m, "path"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(seel, S(m, "atts"), I16_Wrapped(m, TYPE_TABLE));
    Table_Set(seel, S(m, "children"), I16_Wrapped(m, TYPE_TABLE));
    Table_Set(seel, S(m, "mime"), I16_Wrapped(m, TYPE_STR));
    Table_Set(seel, S(m, "func"), I16_Wrapped(m, TYPE_SINGLE));
    Table_Set(seel, S(m, "action"), I16_Wrapped(m, TYPE_ABSTRACT));
    Seel_Seel(m, seel, S(m, "Gen"), TYPE_WWW_GEN);

    if(RouteFuncTable == NULL){
        RouteFuncTable = Table_Make(m);
        RouteMimeTable = Table_Make(m);
        AllExtSpan = Span_Make(m);

        Str *key = S(m, "html");
        Span_Add(AllExtSpan, key);
        Single *funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_STATIC);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "text/html"));

        key = S(m, "png");
        Span_Add(AllExtSpan, key);
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_ASSET|ROUTE_STATIC);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "image/png"));

        key = S(m, "jpg");
        Span_Add(AllExtSpan, key);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "image/jpeg"));

        key = S(m, "js");
        Span_Add(AllExtSpan, key);
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_STATIC|ROUTE_ASSET);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "text/javascript"));

        key = S(m, "css");
        Span_Add(AllExtSpan, key);
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_STATIC|ROUTE_ASSET);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "text/css"));

        key = S(m, "body");
        Span_Add(AllExtSpan, key);
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_STATIC);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "text/html"));

        key = S(m, "txt");
        Span_Add(AllExtSpan, key);
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_STATIC|ROUTE_ASSET);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "text/plain"));

        key = S(m, "fmt");
        Span_Add(AllExtSpan, key);
        funcW = Func_Wrapped(m, routeFuncFmt);
        funcW->type.state |= ROUTE_FMT;
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "text/html"));

        key = S(m, "rbs");
        Span_Add(AllExtSpan, key);
        funcW = Func_Wrapped(m, routeFuncFileDb);
        funcW->type.state |= ROUTE_BINSEG;
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "*/*"));

        key = S(m, "templ");
        Span_Add(AllExtSpan, key);
        funcW = Func_Wrapped(m, routeFuncTempl);
        funcW->type.state |= ROUTE_DYNAMIC;
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "text/html"));
    }
}
