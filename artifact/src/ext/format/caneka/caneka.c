#include <external.h>
#include <caneka.h>

static Lookup *funcs = NULL;
static Span *modules = NULL;

char * CnkLang_RangeToChars(word range){
    if(range == 0){
        return "ZERO";
    }else if(range == CNK_LANG_START){
        return "CNK_LANG_START";
    }else if(range == CNK_LANG_BLANK_LINE){
        return "CNK_LANG_BLANK_LINE";
    }else if(range == CNK_LANG_LINE){
        return "CNK_LANG_LINE";
    }else if(range == CNK_LANG_INDENT){
        return "CNK_LANG_INDENT";
    }else if(range == CNK_LANG_LINE_END){
        return "CNK_LANG_LINE_END";
    }else if(range == CNK_LANG_STRUCT){
        return "CNK_LANG_STRUCT";
    }else if(range == CNK_LANG_REQUIRE){
        return "CNK_LANG_REQUIRE";
    }else if(range == CNK_LANG_PACKAGE){
        return "CNK_LANG_PACKAGE";
    }else if(range == CNK_LANG_TYPE){
        return "CNK_LANG_TYPE";
    }else if(range == CNK_LANG_C){
        return "CNK_LANG_C";
    }else if(range == CNK_LANG_END_C){
        return "CNK_LANG_END_C";
    }else if(range == CNK_LANG_TOKEN){
        return "CNK_LANG_TOKEN";
    }else if(range == CNK_LANG_TOKEN_NULLABLE){
        return "CNK_LANG_TOKEN_NULLABLE";
    }else if(range == CNK_LANG_INVOKE){
        return "CNK_LANG_INVOKE";
    }else if(range == CNK_LANG_TOKEN_DOT){
        return "CNK_LANG_TOKEN_DOT";
    }else if(range == CNK_LANG_POST_TOKEN){
        return "CNK_LANG_POST_TOKEN";
    }else if(range == CNK_LANG_OP){
        return "CNK_LANG_OP";
    }else if(range == CNK_LANG_VALUE){
        return "CNK_LANG_VALUE";
    }else if(range == CNK_LANG_FUNC_PTR){
        return "CNK_LANG_FUNC_PTR";
    }else if(range == CNK_LANG_LINE_END){
        return "CNK_LANG_LINE_END";
    }else if(range == CNK_LANG_ARG_LIST){
        return "CNK_LANG_ARG_LIST";
    }else if(range == CNK_LANG_CURLY_OPEN){
        return "CNK_LANG_CURLY_OPEN";
    }else if(range == CNK_LANG_CURLY_CLOSE){
        return "CNK_LANG_CURLY_CLOSE";
    }else if(range == CNK_LANG_LIST_CLOSE){
        return "CNK_LANG_LIST_CLOSE";
    }else if(range == CNK_LANG_ROEBLING){
        return "CNK_LANG_ROEBLING";
    }else if(range == CNK_LANG_RBL_START){
        return "CNK_LANG_RBL_START";
    }else if(range == CNK_LANG_RBL_OPEN_CLOSE){
        return "CNK_LANG_RBL_OPEN_CLOSE";
    }else if(range == CNK_LANG_RBL_TEXT){
        return "CNK_LANG_RBL_TEXT";
    }else if(range == CNK_LANG_RBL_PAT_KEY){
        return "CNK_LANG_RBL_PAT_KEY";
    }else if(range == CNK_LANG_RBL_PAT_KEY_CLOSE){
        return "CNK_LANG_RBL_PAT_KEY_CLOSE";
    }else if(range == CNK_LANG_RBL_PAT){
        return "CNK_LANG_RBL_PAT";
    }else if(range == CNK_LANG_RBL_INVERT){
        return "CNK_LANG_RBL_INVERT";
    }else if(range == CNK_LANG_RBL_ESCAPE){
        return "CNK_LANG_RBL_ESCAPE";
    }else if(range == CNK_LANG_RBL_MANY){
        return "CNK_LANG_RBL_MANY";
    }else if(range == CNK_LANG_RBL_ANY){
        return "CNK_LANG_RBL_ANY";
    }else if(range == CNK_LANG_RBL_INVERT_CAPTURE){
        return "CNK_LANG_RBL_INVERT_CAPTURE";
    }else if(range == CNK_LANG_RBL_SUPER){
        return "CNK_LANG_RBL_SUPER";
    }else if(range == CNK_LANG_RBL_SUPER_CLOSE){
        return "CNK_LANG_RBL_SUPER_CLOSE";
    }else if(range == CNK_LANG_RBL_KO){
        return "CNK_LANG_RBL_KO";
    }else if(range == CNK_LANG_RBL_KO_CLOSE){
        return "CNK_LANG_RBL_KO_CLOSE";
    }else if(range == CNK_LANG_RBL_WS){
        return "CNK_LANG_RBL_WS";
    }else if(range == CNK_LANG_RBL_COMMENT){
        return "CNK_LANG_RBL_COMMENT";
    }else if(range == CNK_LANG_RBL_JUMP){
        return "CNK_LANG_RBL_JUMP";
    }else if(range == CNK_LANG_RBL_JUMP_TOKEN){
        return "CNK_LANG_RBL_JUMP_TOKEN";
    }else if(range == CNK_LANG_RBL_JUMP_TOKEN_PAT){
        return "CNK_LANG_RBL_JUMP_TOKEN_PAT";
    }else if(range == CNK_LANG_RBL_SEP){
        return "CNK_LANG_RBL_SEP";
    }else if(range == CNK_LANG_RBL_KEY_SEP){
        return "CNK_LANG_RBL_KEY_SEP";
    }else{
        return "unknown";
    }
}

static void CnkModule_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    CnkLangModule *mod = (CnkLangModule *)as(a, TYPE_LANG_CNK_MODULE);
    printf("\x1b[%dm%sCnkModule<%s", color, msg, State_ToChars(mod->type.state));
    Debug_Print((void *)mod->ref, 0, "", color, extended);
    printf("\n");
    Debug_Print((void *)mod->args, 0, "args:", color, TRUE);
    printf("\n");
    Debug_Print((void *)mod->states, 0, "states:", color, FALSE);
    printf("\n");
    Debug_Print((void *)mod->hfile, 0, "hfile:", color, FALSE);
    printf("\n");
    Debug_Print((void *)mod->cfile, 0, "cfile:", color, FALSE);
    printf("\n");
    Debug_Print((void *)mod->funcDefs, 0, "funcDefs:", color, FALSE);
    printf("\x1b[%dm>\x1b[0m\n", color);

}

static void CnkModuleRef_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    CnkLangModRef *ref = (CnkLangModRef *)as(a, TYPE_LANG_CNK_MOD_REF);
    printf("\x1b[%dm%sCnkRef<%s", color, msg, CnkLang_RangeToChars(ref->spaceIdx));
    Debug_Print((void *)ref->name, 0, " name:", color, FALSE);
    Debug_Print((void *)ref->typeName, 0, " typeName:", color, FALSE);
    printf("\x1b[%dm>\x1b[0m", color);

}

static void FmtItem_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    FmtItem *item = (FmtItem *)as(a, TYPE_FMT_ITEM);
    printf("\x1b[%dm%sFmtItem<\x1b[1;%dm%s\x1b[%dm/%s\x1b[0m", color, msg,
        color,
        CnkLang_RangeToChars(item->spaceIdx),
        color,
        State_ToChars(item->type.state));
    Debug_Print((void *)item->content, 0, "", color, extended);
    Debug_Print((void *)item->value, 0, "", color, extended);
    Debug_Print((void *)item->children, 0, "", color, extended);
    printf("\x1b[%dm>\x1b[0m\n", color);
}

static status populatePrint(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_LANG_CNK_MODULE, (void *)CnkModule_Print);
    r |= Lookup_Add(m, lk, TYPE_LANG_CNK_MOD_REF, (void *)CnkModuleRef_Print);

    /* overwriting native fmt item */
    r |= Lookup_Add(m, DebugPrintChain->funcs, TYPE_FMT_ITEM, (void *)FmtItem_Print);
    
    return r;
}

status CnkLang_Init(MemCtx *m){
    if(funcs == NULL){
        funcs = Lookup_Make(m, CNK_LANG_START, populatePrint, NULL);
        return Chain_Extend(m, DebugPrintChain, funcs);
    }
    return NOOP;
}

static status populateModules(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_LANG_CNK_MODULE, (void *)CnkModule_Print);
    r |= Lookup_Add(m, lk, TYPE_LANG_CNK_MOD_REF, (void *)CnkModuleRef_Print);

    /* overwriting native fmt item */
    r |= Lookup_Add(m, DebugPrintChain->funcs, TYPE_FMT_ITEM, (void *)FmtItem_Print);
    
    return r;
}

Span * CnkLang_GetModules(MemCtx *m){
    if(modules == NULL){
        modules = Span_Make(m);
        CnkLangModule *mod;
        mod = CnkLangModule_Make(m);
        mod->ref = CnkLangModRef_Make(m);
        mod->ref->name = String_Make(m, bytes("Getter"));
        mod->type.state |= CNK_LANG_MOD_FUNC_PTR;
        Table_Set(modules, (Abstract *)mod->ref->name, (Abstract *)mod);
    }
    return modules;
}
