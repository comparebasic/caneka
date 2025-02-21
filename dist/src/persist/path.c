#include <external.h>
#include <caneka.h>

static status markPath(MemCtx *m, String *path, String *backlog){
    Match mt;
    String *pat = PatChar_FromString(m, String_Make(m, bytes("/")));
    status r =  Match_SetPattern(&mt, (PatCharDef *)pat->bytes, backlog); 
    mt.type.state |= MATCH_SEARCH;
    String *s = path;
    while(s != NULL){
        for(int i = 0; i < s->length; i++){
            byte b = s->bytes[i];
            Match_Feed(m, &mt, (word)b);
            if((mt.type.state & SUCCESS) != 0){
                mt.type.state &= ~SUCCESS;
                int pos = mt.snip.start+mt.snip.length;
                Match_SetPattern(&mt, (PatCharDef *)pat->bytes, backlog);
                mt.type.state |= MATCH_SEARCH;
                mt.snip.start = pos;
            }
        }
        s = String_Next(s); 
    }

    if(mt.snip.length > 0){
        String_AddBytes(m, backlog, bytes(&mt.snip), sizeof(StrSnip));
    }

    return mt.type.state;
}

String *Path_Base(MemCtx *m, String *path){
    String *s = String_Init(m, STRING_EXTEND);
    Path_Pair(m, path, s, NULL);
    return s;
}

String *Path_File(MemCtx *m, String *path){
    String *s = String_Init(m, STRING_EXTEND);
    Path_Pair(m, path, NULL, s);
    return s;
}

status Path_Pair(MemCtx *m, String *path, String *base, String *fname){
    String *backlog = String_Init(m, STRING_EXTEND);
    backlog->type.state |= (FLAG_STRING_CONTIGUOUS|FLAG_STRING_BINARY);
    markPath(m, path, backlog);

    i64 endsep = -1;
    IterStr it;
    IterStr_Init(&it, backlog, sizeof(StrSnip), NULL);
    while((IterStr_Next(&it) & END) == 0){
        StrSnip *sn = (StrSnip*)IterStr_Get(&it); 
        if(it.idx <= (it.count-2)){
            if(sn->type.state & STRSNIP_CONTENT){
                endsep = sn->start+sn->length;
            }
        }
    }

    if(endsep == -1){
        return ERROR;
    }

    if(base != NULL){
        String_Add(m, base, String_Sub(m, path, 0, endsep));
    }

    if(fname != NULL){
        String_Add(m, fname, String_Sub(m, path, endsep, String_Length(path)-endsep));
    }

    return SUCCESS;
}
