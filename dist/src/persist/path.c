#include <external.h>
#include <caneka.h>

static status setMatch(MemCtx *m, Match *mt){
    String *pat = PatChar_FromString(m, String_Make(m, bytes("/"));
    String *backlog = String_Init(rbl->m, STRING_EXTEND);
    backlog->type.state |= (FLAG_STRING_CONTIGUOUS|FLAG_STRING_BINARY);
    status r =  Match_SetPattern(mt, (PatCharDef *)pat->bytes, backlog); 
    mt->type.state |= MATCH_SEARCH;
    return r;
}

static markPath(MemCtx *m, String *path){
    Match mt;
    setMatch(m, &mt);
    String *s = path;
    while(s != NULL){
        for(int i = 0; i < s->length; i++){
            byte b = s->bytes[i];
            Match_Feed(m, mt, (word)b);
            if((mt->type.state & SUCCESS) != 0){
                mt->type.state &= ~SUCCESS;
            }
        }
        s = String_Next(s); 
    }
    return mt->type.state;
}

String *Path_Base(MemCtx *m, String *path){
    markPath(m, path);
    Debug_Print((Abstract *)path, TYPE_STRSNIP_STRING, "path: ", COLOR_PURPLE, TRUE);
    Debug_Print((Abstract *)mt.backlog, TYPE_STRSNIP_STRING, "backlog: ", COLOR_PURPLE, TRUE);
    return NULL;
}

String *Path_File(){
    markPath(m, path);
    return NULL;
}
