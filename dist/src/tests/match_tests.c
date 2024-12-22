#include <external.h>
#include <caneka.h>

status Match_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    Span *p;
    status r = READY;

    Match mt;
    Match ko;
    String *s;

    s = String_Make(m, bytes("poo"));
    Match_SetString(m, &mt, s);

    s = String_Make(m, bytes("no"));
    for(int i = 0; i < s->length; i++){
        Match_Feed(&mt, s->bytes[i]);
    }

    r |= Test(mt.type.state != COMPLETE, "Non match has unsuccessful state found %s", State_ToChars(mt.type.state)); 
    s = String_Make(m, bytes("poo"));

    mt.type.state = READY;
    for(int i = 0; i < s->length; i++){
        Match_Feed(&mt, s->bytes[i]);
    }
    r |= Test(mt.type.state == COMPLETE, "Matching string has successful state found %s", State_ToChars(mt.type.state)); 

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

    word pat[] = { PAT_INVERT_CAPTURE|PAT_TERM, '<', '<', PAT_MANY, 'a', 'z', PAT_END, 0, 0};
    Match mt;
    Match_SetPattern(&mt, (PatCharDef *)pat);
    int i = 0;
    Match_Feed(&mt, s->bytes[i]);
    i++;
    r |= Test(HasFlag(mt.type.state, PROCESSING), "Has PROCESSING status %s", State_ToChars(mt.type.state));
    r |= Test(mt.pat.curDef == (mt.pat.startDef+1) , "On second pos, position is %d", (util)(mt.pat.curDef - mt.pat.startDef) / sizeof(PatCharDef));

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

    r |= Test(i ==  4, "Tag -Stopped on the fourth character");
    r |= Test(count == 3, "Tag -Found three chars");
    r |= Test(HasFlag(mt.type.state, SUCCESS), "Tag- Found SUCCESS have %s", State_ToChars(mt.type.state));
    PatCharDef *def = mt.pat.curDef;
    r |= Test((def->flags == PAT_END), "Tag -At end");

    word att[] = { PAT_INVERT_CAPTURE|PAT_TERM, ' ', ' ', PAT_KO, '=', '=', PAT_MANY|PAT_TERM, 'a', 'z', PAT_END, 0, 0};
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
    r |= Test(mt.count == 4, "Att - Found four chars, count is %d", mt.count);
    

    MemCtx_Free(m);
    return r;
}

status MatchKo_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    Span *p;
    status r = READY;

    Match mt;
    Match ko;
    String *s;
    int i = 0;

    word def[] = {
        PAT_KO|PAT_INVERT, '\\', '\\', PAT_KO, '"', '"', patText,
        PAT_END, 0, 0
    };

    s = String_Make(m, bytes("Hi there this is a string"));
    Match_SetPattern(&mt, (PatCharDef *)def);
    i = 0;
    while(1){
        Match_Feed(&mt, s->bytes[i]);
        if(HasFlag(mt.type.state, PROCESSING)){
            i++;
        }else{
            break;
        }
    }
    r |= Test(i == s->length, "Length matches for string that has no escape or closing quote %d", i);

    s = String_Make(m, bytes("Hi there this is a \\\"quoted string\\\""));
    Match_SetPattern(&mt, (PatCharDef *)def);
    i = 0;
    while(1){
        Match_Feed(&mt, s->bytes[i]);
        if(HasFlag(mt.type.state, PROCESSING)){
            i++;
        }else{
            break;
        }
    }
    r |= Test(i == s->length-2, "Length matches for string minus escape cahrs that has two escapes in it %d", i);

    s = String_Make(m, bytes("Hi there this is a string ending \" here"));
    Match_SetPattern(&mt, (PatCharDef *)def);
    i = 0;
    while(1){
        Match_Feed(&mt, s->bytes[i]);
        if(HasFlag(mt.type.state, PROCESSING)){
            i++;
        }else{
            break;
        }
    }
    r |= Test(i == s->length-5, "Length matches for string that has a terminator quote in it %d", i);



    word multiKoDef[] = {
        PAT_KO, 'e', 'e', PAT_KO, 'n', 'n', PAT_KO, 'd', 'd', patText,
        PAT_END, 0, 0
    };
    s = String_Make(m, bytes("it all end. did it end?"));
    Match_SetPattern(&mt, (PatCharDef *)multiKoDef);
    i = 0;
    while(1){
        Match_Feed(&mt, s->bytes[i]);
        if(HasFlag(mt.type.state, PROCESSING)){
            i++;
        }else{
            break;
        }
    }
    
    r |= Test(i == 10, "It took 10 counts to get to the end, have %d", i);
    r |= Test(mt.count == 7, "terminator 'end' is omited from the count have, %d", mt.count);

    MemCtx_Free(m);
    return r;
}
