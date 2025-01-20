#include <external.h>
#include <caneka.h>

static PatCharDef extDef[] = {
    {PAT_TERM, '.', '.'},
    {PAT_KO|PAT_KO_TERM, '.', '.'},
    patText,
    {PAT_END, 0, 0},
};

status ExtMatch_Init(Match *mt, String *backlog){
    Match_SetPattern(mt, extDef, backlog);
    mt->type.state |= MATCH_SEARCH;
    return SUCCESS;
}
