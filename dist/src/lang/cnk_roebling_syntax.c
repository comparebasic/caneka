#include <external.h>
#include <caneka.h>

word openCloseDef[] = {
    PAT_TERM, '/', '/',
    PAT_INVERT|PAT_INVERT_CAPTURE, '/', '/',
    PAT_END, 0, 0,
};

word patDef[] = {
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, '\\', '\\', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, '^', '^', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, '+', '+', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, '*', '*', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, '_', '_', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, '{', '{', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, '}', '}', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, '[', '[', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, ']', ']', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, '/', '/', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, ')', ')', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, ' ', ' ', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, ',', ',', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, '\t', '\t', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE, '\r', '\r', 
    PAT_KO|PAT_MANY|PAT_INVERT_CAPTURE|PAT_KO_TERM, '\n', '\n', 
    patText,
    PAT_END, 0, 0,
};

word escapeDef[] = {
    PAT_TERM, '\\', '\\',
    PAT_TERM|PAT_INVERT, 0, 32,/*includes space in invert */
    PAT_END, 0, 0,
};

word patKeyDef[] = {
    PAT_ANY|PAT_INVERT_CAPTURE, ' ', ' ',
    PAT_SINGLE|PAT_OPTIONAL, 'a', 'z',
    PAT_SINGLE|PAT_OPTIONAL, 'A', 'Z',
    PAT_SINGLE|PAT_OPTIONAL, '_', '_',
    PAT_SINGLE|PAT_OPTIONAL|PAT_TERM, '0', '9',
    PAT_ANY|PAT_INVERT_CAPTURE, ' ', ' ',
    PAT_ANY, 'a', 'z',
    PAT_ANY, 'A', 'Z',
    PAT_ANY, '_', '_',
    PAT_ANY, '-', '-',
    PAT_ANY|PAT_TERM, '0', '9',
    PAT_TERM|PAT_INVERT_CAPTURE|PAT_CONSUME, '(', '(',
    PAT_END, 0, 0,
};

word patTokenDef[] = {
    PAT_SINGLE, 'a', 'z',
    PAT_SINGLE, 'A', 'Z',
    PAT_SINGLE, '_', '_',
    PAT_SINGLE|PAT_TERM, '0', '9',
    PAT_MANY, 'a', 'z',
    PAT_MANY, 'A', 'Z',
    PAT_MANY, '_', '_',
    PAT_MANY, '-', '-',
    PAT_MANY|PAT_TERM, '0', '9',
    PAT_END, 0, 0,
};

word keyCloseDef[] = {
    PAT_TERM|PAT_INVERT_CAPTURE|PAT_CONSUME, ')', ')',
    PAT_END, 0, 0,
};

word invertDef[] = {
    PAT_TERM, '^', '^',
    PAT_END, 0, 0,
};

word manyDef[] = {
    PAT_TERM, '+', '+',
    PAT_END, 0, 0,
};

word anyDef[] = {
    PAT_TERM, '*', '*',
    PAT_END, 0, 0,
};

word invertCaptureDef[] = {
    PAT_TERM, '_', '_',
    PAT_END, 0, 0,
};

word superDef[] = {
    PAT_TERM, '{', '{',
    PAT_END, 0, 0,
};

word superCloseDef[] = {
    PAT_TERM, '}', '}',
    PAT_END, 0, 0,
};

word koDef[] = {
    PAT_TERM, '[', '[',
    PAT_END, 0, 0,
};

word koEndDef[] = {
    PAT_TERM, ']', ']',
    PAT_END, 0, 0,
};

word wsDef[] = {
    PAT_MANY,' ',' ',PAT_MANY,'\t','\t',PAT_MANY,'\r','\r',PAT_MANY|PAT_TERM,'\n','\n',
    PAT_END, 0, 0,
};

word commentDef[] = {
    PAT_TERM,'/','/',PAT_TERM,'/','/',PAT_KO|PAT_KO_TERM,'\n','\n',patText,
    PAT_END, 0, 0,
};

word jumpDef[] = {
    PAT_ANY|PAT_INVERT_CAPTURE,' ',' ',
    PAT_ANY|PAT_INVERT_CAPTURE,'\t','\t',
    PAT_ANY|PAT_INVERT_CAPTURE,'\r','\r',
    PAT_ANY|PAT_INVERT_CAPTURE,'\n','\n',
    PAT_TERM,'-','-',
    PAT_TERM,'>','>',
    PAT_END, 0, 0,
};

word sepDef[] = {
    PAT_ANY|PAT_INVERT_CAPTURE,' ',' ',
    PAT_ANY|PAT_INVERT_CAPTURE,'\t','\t',
    PAT_ANY|PAT_INVERT_CAPTURE,'\r','\r',
    PAT_ANY|PAT_INVERT_CAPTURE,'\n','\n',
    PAT_TERM,',',',',
    PAT_ANY|PAT_INVERT_CAPTURE,' ',' ',
    PAT_ANY|PAT_INVERT_CAPTURE,'\t','\t',
    PAT_ANY|PAT_INVERT_CAPTURE,'\r','\r',
    PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM,'\n','\n',
    PAT_END, 0, 0,
};

static status start(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    
    r |= Roebling_SetPattern(rbl, (PatCharDef *)openCloseDef, 
        CNK_LANG_RBL_OPEN_CLOSE,  
        CNK_LANG_RBL_SEP);

    return r;
}

static status pat(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    
    r |= Roebling_SetPattern(rbl, (PatCharDef *)jumpDef, 
        CNK_LANG_RBL_JUMP,  
        CNK_LANG_RBL_JUMP_TOKEN_PAT);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)keyCloseDef, 
        CNK_LANG_RBL_PAT_KEY_CLOSE,
        CNK_LANG_RBL_SEP);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)patDef, 
        CNK_LANG_RBL_PAT,
        CNK_LANG_RBL_PAT);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)invertDef, 
        CNK_LANG_RBL_INVERT,
        CNK_LANG_RBL_PAT);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)escapeDef, 
        CNK_LANG_RBL_ESCAPE,
        CNK_LANG_RBL_PAT);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)manyDef, 
        CNK_LANG_RBL_MANY,
        CNK_LANG_RBL_PAT);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)anyDef, 
        CNK_LANG_RBL_ANY,
        CNK_LANG_RBL_PAT);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)invertCaptureDef, 
        CNK_LANG_RBL_INVERT_CAPTURE,
        CNK_LANG_RBL_PAT);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)superDef, 
        CNK_LANG_RBL_SUPER,
        CNK_LANG_RBL_PAT);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)superCloseDef, 
        CNK_LANG_RBL_SUPER_CLOSE,
        CNK_LANG_RBL_PAT);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)koDef,
        CNK_LANG_RBL_KO,
        CNK_LANG_RBL_PAT);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)koEndDef,
        CNK_LANG_RBL_KO_CLOSE,
        CNK_LANG_RBL_PAT);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)wsDef,
        CNK_LANG_RBL_WS,
        CNK_LANG_RBL_PAT);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)commentDef,
        CNK_LANG_RBL_COMMENT,
        CNK_LANG_RBL_PAT);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)openCloseDef,
        CNK_LANG_RBL_OPEN_CLOSE,
        CNK_LANG_RBL_END);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)patKeyDef, 
        CNK_LANG_RBL_PAT_KEY,
        CNK_LANG_RBL_PAT);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)sepDef, 
        CNK_LANG_RBL_SEP,
        CNK_LANG_RBL_PAT);

    return r;
}

static status sep(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl, (PatCharDef *)jumpDef, 
        CNK_LANG_RBL_JUMP,  
        CNK_LANG_RBL_JUMP_TOKEN);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)sepDef, 
        CNK_LANG_RBL_KEY_SEP,
        CNK_LANG_RBL_SEP);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)patKeyDef, 
        CNK_LANG_RBL_PAT_KEY,
        CNK_LANG_RBL_PAT);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)openCloseDef, 
        CNK_LANG_RBL_OPEN_CLOSE,  
        CNK_LANG_RBL_SEP);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)wsDef,
        CNK_LANG_RBL_WS,
        CNK_LANG_RBL_SEP);

    return r;
}

static status jumpToken(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl, (PatCharDef *)patTokenDef, 
        CNK_LANG_RBL_JUMP_TOKEN,
        CNK_LANG_RBL_SEP);

    return r;
}

static status jumpTokenPat(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl, (PatCharDef *)patTokenDef, 
        CNK_LANG_RBL_JUMP_TOKEN,
        CNK_LANG_RBL_PAT);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)wsDef,
        CNK_LANG_RBL_WS,
        CNK_LANG_RBL_JUMP_TOKEN_PAT);
    r |= Roebling_SetPattern(rbl, (PatCharDef *)sepDef, 
        CNK_LANG_RBL_SEP,
        CNK_LANG_RBL_PAT);

    return r;
}


status CnkRoebling_Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    FmtCtx *ctx = (FmtCtx *)asIfc(source, TYPE_LANG_CNK_RBL);

    if(DEBUG_LANG_CNK_RBL){
        printf("\n\n\x1b[%dmCnkRbl Capture %s:", DEBUG_LANG_CNK_RBL, CnkLang_RangeToChars(captureKey));
        Debug_Print((void *)s, 0, " ", DEBUG_LANG_CNK_RBL, TRUE);
        printf("\n");
        Debug_Print((void *)ctx->item->def, 0, "item->def: ", DEBUG_LANG_CNK_RBL, TRUE);
        printf("\x1b[%dm(%s)\x1b[0m\n", DEBUG_LANG_CNK_RBL, CnkLang_RangeToChars(ctx->item->def->id));
    }

    FmtDef *def = Chain_Get(ctx->byId, captureKey);
    if(def != NULL && def->to){
        if((def->flags & FMT_DEF_INDENT) != 0){
            FmtDef_PushItem(ctx, captureKey, s, def);
        }
        if(def->to != NULL && (def->type.state & FMT_DEF_TO_ON_CLOSE) == 0){
            def->to(ctx->rbl->m, def, ctx, s, source);
        }
        if((def->flags & FMT_DEF_OUTDENT) != 0){
            ctx->item = ctx->item->parent;
            if((def->to != NULL) && (def->type.state & FMT_DEF_TO_ON_CLOSE) != 0){
                def->to(ctx->rbl->m, def, ctx, s, source);
            }
        }
    }

    return NOOP;
}

status CnkRoebling_AddParsers(MemCtx *m, Span *parsers, Lookup *marks){
    Span *additions = Span_Make(m, TYPE_SPAN);
    Span_Add(additions, (Abstract *)Int_Wrapped(m, CNK_LANG_RBL_START));
    Span_Add(additions, (Abstract *)Do_Wrapped(m, (DoFunc)start));
    Span_Add(additions, (Abstract *)Int_Wrapped(m, CNK_LANG_RBL_PAT));
    Span_Add(additions, (Abstract *)Do_Wrapped(m, (DoFunc)pat));
    Span_Add(additions, (Abstract *)Int_Wrapped(m, CNK_LANG_RBL_SEP));
    Span_Add(additions, (Abstract *)Do_Wrapped(m, (DoFunc)sep));
    Span_Add(additions, (Abstract *)Int_Wrapped(m, CNK_LANG_RBL_JUMP_TOKEN));
    Span_Add(additions, (Abstract *)Do_Wrapped(m, (DoFunc)jumpToken));
    Span_Add(additions, (Abstract *)Int_Wrapped(m, CNK_LANG_RBL_JUMP_TOKEN_PAT));
    Span_Add(additions, (Abstract *)Do_Wrapped(m, (DoFunc)jumpTokenPat));
    Span_Add(additions, (Abstract *)Int_Wrapped(m, CNK_LANG_RBL_END));

    LookupConfig config[] = {
        {CNK_LANG_RBL_START, (Abstract *)String_Make(m, bytes("RBL_START"))},
        {CNK_LANG_RBL_PAT, (Abstract *)String_Make(m, bytes("RBL_PAT"))},
        {CNK_LANG_RBL_SEP, (Abstract *)String_Make(m, bytes("RBL_SEP"))},
        {CNK_LANG_RBL_END, (Abstract *)String_Make(m, bytes("RBL_END"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);

    return Roebling_AddParsers(m, parsers, marks, additions,  desc);
}

Roebling *CnkRoeblingCtx_RblMake(MemCtx *m, FmtCtx *ctx){
    Span *parsers = Span_Make(m, TYPE_SPAN);
    Lookup *desc = Lookup_Make(m, CNK_LANG_RBL_START, NULL, NULL);

    String *s = String_Init(m, STRING_EXTEND);

    CnkRoebling_AddParsers(m, parsers, desc);

    return Roebling_Make(m, TYPE_ROEBLING,
        parsers,
        desc,
        s,
        CnkRoebling_Capture,
        (Abstract *)ctx
    ); 
}

FmtCtx *CnkRoeblingCtx_Make(MemCtx *m, Abstract *source){
    FmtCtx *ctx = MemCtx_Alloc(m, sizeof(FmtCtx));
    ctx->type.of = TYPE_LANG_CNK_RBL;
    ctx->m = m;
    ctx->rbl = CnkRoeblingCtx_RblMake(m, ctx);
    ctx->out = ToStdOut;

    ctx->rangeToChars = CnkLang_RangeToChars;
    CnkRblLang_AddDefs(ctx);
    ctx->root = ctx->item = FmtItem_Make(ctx->m, ctx);
    ctx->root->value = (Abstract *)Span_Make(m, TYPE_TABLE);
    ctx->item->spaceIdx = CNK_LANG_RBL_START;
    FmtDef *def = Chain_Get(ctx->byId, ctx->item->spaceIdx);
    ctx->item->def = def;
    ctx->source = source;

    return ctx;
}
