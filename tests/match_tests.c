#include <external.h>
#include <caneka.h>
#include <testsuite.h>

status Match_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    Span *p;
    status r = READY;

    Match mt;
    Match ko;
    String *s;

    s = String_Make(m, bytes("poo"));
    Match_SetString(&mt, s);

    s = String_Make(m, bytes("no"));
    for(int i = 0; i < s->length; i++){
        Match_Feed(&mt, s->bytes[i]);
    }

    r |= Test(mt.type.state != COMPLETE, "Non match has unsuccessful state found %s", State_ToString(mt.type.state)); 
    s = String_Make(m, bytes("poo"));
    for(int i = 0; i < s->length; i++){
        Match_Feed(&mt, s->bytes[i]);
    }
    r |= Test(mt.type.state == COMPLETE, "Matching string has successful state found %s", State_ToString(mt.type.state)); 

    word line[] = { patText, PAT_END, 0, 0};  
    word lineko[] = { PAT_TERM, '\n', '\n', PAT_END, 0, 0};  
    Match_SetPattern(&mt, (PatCharDef *)line);

    s = String_Make(m, bytes("incomplete"));
    Match_SetPattern(&mt, (PatCharDef *)line);
    for(int i = 0; i < s->length; i++){
        Match_Feed(&mt, s->bytes[i]);
    }

    s = String_Make(m, bytes("\x02 Bad Malicious\n"));
    Match_SetPattern(&mt, (PatCharDef *)line);
    for(int i = 0; i < s->length; i++){
        Match_Feed(&mt, s->bytes[i]);
    }

    s = String_Make(m, bytes("A good line :)\n"));
    Match_SetPattern(&mt, (PatCharDef *)line);
    Match_SetPattern(&ko, (PatCharDef *)lineko);
    for(int i = 0; i < s->length; i++){
        Match_Feed(&ko, s->bytes[i]);
        if(HasFlag(ko.type.state, SUCCESS)){
            break;
        }
        Match_Feed(&mt, s->bytes[i]);
    }

    String *s2 = String_ToEscaped(m, s);
    r |= Test(HasFlag(ko.type.state, COMPLETE), "Matching string has successful for line '%s' state found %s",
        s2->bytes, State_ToString(ko.type.state)); 

    r |= Test(mt.count == s->length-1, "Matched length of string, less termMatching, expected %d have %d",
        s->length-1, mt.count);

    MemCtx_Free(m);
    return r;
}
