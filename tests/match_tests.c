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


    /* test pat match */
    word line[] = { PAT_KO, '\n', '\n', patText, PAT_END, 0, 0};  

    s = String_Make(m, bytes("A good line :)\n"));
    Match_SetPattern(&mt, (PatCharDef *)line);
    for(int i = 0; i < s->length; i++){
        Match_Feed(&mt, s->bytes[i]);
        if(!HasFlag(mt.type.state, PROCESSING)){
            break;
        }
    }

    r |= Test(mt.count == s->length-1, "Matched length of string, less termMatching, expected %d have %d",
        s->length-1, mt.count);

    MemCtx_Free(m);
    return r;
}

status MatchElastic_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    Span *p;
    status r = READY;

    String *s = String_Make(m, bytes("<tag atts=\"poo\">hi</tab>"));

    word pat[] = { PAT_IGNORE|PAT_TERM, '<', '<', PAT_MANY, 'a', 'z', PAT_END, 0, 0};
    Match mt;
    Match_SetPattern(&mt, (PatCharDef *)pat);
    int i = 0;
    Match_Feed(&mt, s->bytes[i]);
    i++;
    r |= Test(HasFlag(mt.type.state, PROCESSING), "Has PROCESSING status %s", State_ToString(mt.type.state));
    r |= Test(HasFlag(mt.position, 1), "On second pos, position is %d", mt.position);

    int count = 0;
    while(1){
        Match_Feed(&mt, s->bytes[i]);
        if(HasFlag(mt.type.state, PROCESSING)){
            count++;
            i++;
        }else{
            break;
        }
    }
    r |= Test(i ==  4, "Stopped on the fourth character");
    r |= Test(count == 3, "Found three chars");
    r |= Test(HasFlag(mt.type.state, SUCCESS), "Found SUCCESS have %s", State_ToString(mt.type.state));
    PatCharDef *def = Match_GetDef(&mt);
    r |= Test((def->flags == PAT_END), "At end");

    word att[] = { PAT_IGNORE|PAT_TERM, ' ', ' ', PAT_KO, '=', '=', PAT_MANY|PAT_TERM, 'a', 'z', PAT_END, 0, 0};
    Match_SetPattern(&mt, (PatCharDef *)att);
    count = 0;
    while(1){
        Match_Feed(&mt, s->bytes[i]);
        if(HasFlag(mt.type.state, PROCESSING)){
            count++;
            i++;
        }else{
            break;
        }
    }
    r |= Test(mt.count == 4, "Found four chars, count is %d", mt.count);
    

    MemCtx_Free(m);
    return r;
}
