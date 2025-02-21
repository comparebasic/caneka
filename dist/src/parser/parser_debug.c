#include <external.h>
#include <caneka.h>

static word matchFlags[] = {
    PAT_END, /* E */
    PAT_TERM, /* X */
    PAT_OPTIONAL, /* P */
    PAT_MANY, /* M */
    PAT_ANY, /* N */
    PAT_INVERT, /* I */
    PAT_COUNT, /* C */
    PAT_INVERT_CAPTURE, /* G */
    PAT_KO, /* K */
    PAT_KO_TERM, /* O */
    PAT_SINGLE, /* S */
    PAT_LEAVE , /* L */
    PAT_CMD, /* D */
    PAT_GO_ON_FAIL, /* T */
    PAT_CONSUME, /* U */
};

static char *matchFlagChars = "EXPMNICGKOSLDTU";

static status patFlagStr(word flags, char str[]){
    int p = 0;
    int i = 0;
    /* 0 and & x != 0 dont mix well */
    if(flags == PAT_END){
        str[p++] = 'E';
    }
    int l = strlen(matchFlagChars);
    while(i < l){
       if((flags &matchFlags[i]) != 0){
            str[p++] = matchFlagChars[i];
       }
       i++;
    }
    str[p] = '\0';
    return SUCCESS;
}

static void patCharDef_PrintSingle(PatCharDef *def, cls type, char *msg, int color, boolean extended){
    char flag_cstr[12];
    patFlagStr(def->flags, flag_cstr);
    if((def->flags & PAT_COUNT) != 0){
        if(def->from == '\r' || def->from == '\n'){
            printf("%s=0x%hux0x%hu", flag_cstr, def->from, def->to);
        }else{
            printf("?");
            printf("%s=%cx%hu", flag_cstr, (char)def->from, def->to);
        }
    }else if(def->flags == PAT_END){
        printf("%s", flag_cstr);
    }else if(def->from == def->to){
        if(def->from == '\r' || def->from == '\n' || def->from == '\t'){
            printf("%s=#%hu", flag_cstr, def->from);
        }else{
            printf("%s=%c/%hu", flag_cstr, (char)def->from, (i8)def->from);
        }
    }else{
        if((def->from == '\r' || def->from == '\n') || (def->to == '\r' || def->to == '\n') || (def->to < 32 || def->to < 32) || (def->to > 128 || def->to > 127)){
            printf("%s=#%hu-#%hu", flag_cstr, def->from, def->to);
        }else{
            printf("%s=%c-%c", flag_cstr, (char)def->from, (char)def->to);
        }
    }
    if((def->flags & PAT_TERM) != 0){
        printf(".");
    }
}

static void PatCharDef_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    PatCharDef *def = (PatCharDef *)a;
    printf("\x1b[%dm%s", color, msg);
    if(extended){
        boolean first = TRUE;
        if(def->flags == PAT_END){
            patCharDef_PrintSingle(def, TYPE_PATCHARDEF, "", color, extended);
        }else{
            while(def->flags != PAT_END){
                if(first){
                    first = FALSE;
                }else{
                    printf(",");
                }
                patCharDef_PrintSingle(def, TYPE_PATCHARDEF, "", color, extended);
                def++;
            }
        }
    }else{
        patCharDef_PrintSingle(def, TYPE_PATCHARDEF, msg, color, extended);
    }
    printf("\x1b[0m");
}

static char *flagStrs[] = {
    "PAT_TERM",
    "PAT_OPTIONAL",
    "PAT_MANY",
    "PAT_ANY",
    "PAT_INVERT",
    "PAT_COUNT",
    "PAT_INVERT_CAPTURE",
    "PAT_KO",
    "PAT_KO_TERM",
    "PAT_SINGLE",
    "PAT_LEAVE ",
    "PAT_CMD",
    "PAT_GO_ON_FAIL",
    "PAT_CONSUME",
    NULL,
};

status PrintMatchAddr(MemCtx *m, Abstract *a){
    if(a == NULL){
        printf("NULL ");
    }else{
        printf("0x%lx ", (util)(a));
    }
    return SUCCESS;
}

status Match_AddFlagsToStr(MemCtx *m, String *s, word flag){
    status r = READY;
    char *cstr = "";
    boolean first = TRUE;
    if(flag == 0){
        cstr = "PAT_END";
        String_AddBytes(m, s, bytes(cstr), strlen(cstr));
        r |= SUCCESS;
    }
    for(int i = 0; i <= 13; i++){
        if((flag & (1 << i)) != 0){
            if(!first){
                cstr = "|";
                String_AddBytes(m, s, bytes(cstr), strlen(cstr));
            }
            first = FALSE;
            cstr = flagStrs[i];
            String_AddBytes(m, s, bytes(cstr), strlen(cstr));
            r |= SUCCESS;
        }
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}


void Roebling_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    DebugStack_Push("Roebling_Print", TYPE_CSTR);
    Roebling *rbl = (Roebling *) as(a, TYPE_ROEBLING);
    String *sec = Roebling_GetMarkDebug(rbl, rbl->idx);
    printf("\x1b[%dm%sRbl<%s\x1b[1;%dm%s\x1b[0;%dm source=%u", color, msg, State_ToChars(rbl->type.state), color, String_ToChars(DebugM, sec), color, rbl->source != NULL ? rbl->source->type.of: 0);
    printf(":");
    if(extended){
        printf(" idx:%d jump:%d ", rbl->idx, rbl->jump);
        Debug_Print((void *)&(rbl->cursor), 0, "Cursor: ", color, extended);
        if(rbl->marks->values->nvalues > 0){
            printf("\n  \x1b[%dmmarks=\n    ", color);
            for(int i = 0; i <= rbl->marks->values->max_idx; i++){
                Debug_Print((void *)Span_Get(rbl->markLabels->values, i), 0, "", color, FALSE);
                printf("=");
                int *n = Span_Get(rbl->marks->values, i);
                if(n != NULL){
                    printf("%u", *n);
                }
                printf(" ");
            }
        }
        printf("\n  \x1b[%dmmatches=\n", color);
        Match *mt = NULL;
        for(int i = 0; i < rbl->matches->nvalues; i++){
            mt = Span_Get(rbl->matches, i);
            Debug_Print((void *)mt, 0, "    ", color, extended);
            if(i < rbl->matches->nvalues-1){
                printf(",\n");
            }
        }
        printf("\n  \x1b[%dmParsers(DoF)=\n    ", color);
        Span_Run(NULL, rbl->parsers_do, PrintMatchAddr);
        printf("\n");
    }else{
        printf(" idx:%d ", rbl->idx);
        Debug_Print((void *)&(rbl->cursor), 0, "", color, extended);
    }
    printf("\x1b[%dm>\x1b[0m", color);
    DebugStack_Pop();
    return;
}


void Match_PrintPat(Abstract *a, cls type, char *msg, int color, boolean extended){
    Match *mt = (Match *)as(a, TYPE_PATMATCH);
    if(extended){
        printf("\x1b[%dm%sMatch<%s:state=%s:jump=%d:remainig=%d:%d/%d", color, msg,
            State_ToChars(mt->type.state), State_ToChars(mt->type.state), mt->jump, mt->remaining, mt->snip.start, mt->snip.length);
        Debug_Print(mt->backlog, TYPE_STRSNIP_STRING, ":backlog=(", color, FALSE);
        printf(") \x1b[1;%dm[", color);
        Debug_Print((void *)mt->pat.curDef, TYPE_PATCHARDEF, "", color, FALSE);
        printf("\x1b[1;%dm] \x1b[0;%dm ", color, color);
        Debug_Print((void *)mt->pat.startDef, TYPE_PATCHARDEF, "", color, TRUE);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%sMatch<state=%s ", color, msg, State_ToChars(mt->type.state));
        Debug_Print((void *)mt->pat.startDef, TYPE_PATCHARDEF, "", color, TRUE);
        printf("\x1b[%dm>\x1b[0m", color);
    }
}



void Match_midDebug(char type, word c, PatCharDef *def, Match *mt, boolean matched, boolean extended){
    if(extended){
        printf("    \x1b[%dm%s ", DEBUG_PATMATCH, 
        State_ToChars(mt->type.state));
        Debug_Print((void *)def, TYPE_PATCHARDEF, "", DEBUG_PATMATCH, FALSE);
    }else{
        if(matched){
            printf("\x1b[1;%dmY\x1b[%dm/\x1b[0m", DEBUG_PATMATCH, DEBUG_PATMATCH);
        }else{
            printf("\x1b[%dmN/\x1b[0m", DEBUG_PATMATCH);
        }
        Debug_Print((void *)def, TYPE_PATCHARDEF, "", DEBUG_PATMATCH, FALSE);
        if(matched){
            printf("\x1b[%dm, \x1b[0m", DEBUG_PATMATCH);
        }else{
            printf("\x1b[%dm, \x1b[0m", DEBUG_PATMATCH);
        }
    }
}

