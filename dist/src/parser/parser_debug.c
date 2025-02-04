#include <external.h>
#include <caneka.h>

status PrintMatchAddr(MemCtx *m, Abstract *a){
    if(a == NULL){
        printf("NULL ");
    }else{
        printf("0x%lx ", (util)(a));
    }
    return SUCCESS;
}

void Roebling_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
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

