#include <external.h>
#include <caneka.h>

status Match_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    Span *p;
    status r = READY;

    Match mt;
    Match ko;
    String *s;
    String *backlog = String_Init(m, STRING_EXTEND);
    backlog->type.state |= FLAG_STRING_CONTIGUOUS;

    s = String_Make(m, bytes("poo"));
    Match_SetString(m, &mt, s, backlog);

    s = String_Make(m, bytes("no"));
    for(int i = 0; i < s->length; i++){
        Match_Feed(m, &mt, s->bytes[i]);
    }

    r |= Test(mt.type.state != SUCCESS, "Non match has unsuccessful state found %s", State_ToChars(mt.type.state)); 
    s = String_Make(m, bytes("poo"));

    mt.type.state = READY;
    for(int i = 0; i < s->length; i++){
        Match_Feed(m, &mt, s->bytes[i]);
    }
    r |= Test(mt.type.state == SUCCESS, "Matching string has successful state found %s", State_ToChars(mt.type.state)); 

    word line[] = { PAT_KO, '\n', '\n', patText, PAT_END, 0, 0};  

    s = String_Make(m, bytes("A good line :)\n"));
    String_Reset(backlog);
    Match_SetPattern(&mt, (PatCharDef *)line, backlog);
    for(int i = 0; i < s->length; i++){
        Match_Feed(m, &mt, s->bytes[i]);
        if((mt.type.state & PROCESSING) == 0){
            break;
        }
    }

    r |= Test(Match_Total(&mt) == s->length-1, "Matched length of string, less termMatching, expected %d have %d",
        s->length-1, Match_Total(&mt));

    MemCtx_Free(m);
    return r;
}

status MatchElastic_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    status r = READY;
    String *backlog = String_Init(m, STRING_EXTEND);
    backlog->type.state |= FLAG_STRING_CONTIGUOUS;

    String *s = String_Make(m, bytes("<tag atts=\"poo\">hi</tab>"));

    word pat[] = { PAT_INVERT_CAPTURE|PAT_TERM, '<', '<', PAT_MANY, 'a', 'z', PAT_END, 0, 0};
    Match mt;
    Match_SetPattern(&mt, (PatCharDef *)pat, backlog);
    int i = 0;
    Match_Feed(m, &mt, s->bytes[i]);
    i++;
    r |= Test((mt.type.state & PROCESSING) != 0, "Has PROCESSING status %s", State_ToChars(mt.type.state));
    r |= Test(mt.pat.curDef == (mt.pat.startDef+1) , "On second pos, position is %d", (util)(mt.pat.curDef - mt.pat.startDef) / sizeof(PatCharDef));

    int count = 0;
    while(1){
        Match_Feed(m, &mt, s->bytes[i]);
        if((mt.type.state & PROCESSING) != 0){
            count++;
            i++;
        }else{
            break;
        }
    }

    r |= Test(i ==  4, "Tag -Stopped on the fourth character");
    r |= Test(count == 3, "Tag -Found three chars");
    r |= Test((mt.type.state & SUCCESS) != 0, "Tag- Found SUCCESS have %s", State_ToChars(mt.type.state));
    PatCharDef *def = mt.pat.curDef;
    r |= Test((def->flags == PAT_END), "Tag -At end");

    word att[] = { PAT_INVERT_CAPTURE|PAT_TERM, ' ', ' ', PAT_KO, '=', '=', PAT_MANY|PAT_TERM, 'a', 'z', PAT_END, 0, 0};
    String_Reset(backlog);
    Match_SetPattern(&mt, (PatCharDef *)att, backlog);
    count = 0;
    while(1){
        Match_Feed(m, &mt, s->bytes[i]);
        if((mt.type.state & PROCESSING) != 0){
            count++;
            i++;
        }else{
            break;
        }
    }
    r |= Test(Match_Total(&mt) == 4, "Att - Found 4 chars, count is %d", Match_Total(&mt));

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
    String *backlog = String_Init(m, STRING_EXTEND);
    backlog->type.state |= FLAG_STRING_CONTIGUOUS;
    int i = 0;

    word def[] = {
        PAT_MANY|PAT_KO|PAT_INVERT, '\\', '\\', PAT_MANY|PAT_KO, '"', '"', patText,
        PAT_END, 0, 0
    };

    s = String_Make(m, bytes("Hi there this is a string"));
    Match_SetPattern(&mt, (PatCharDef *)def, backlog);
    i = 0;
    while(1){
        Match_Feed(m, &mt, s->bytes[i]);
        if((mt.type.state & PROCESSING) != 0){
            i++;
        }else{
            break;
        }
    }
    r |= Test(i == s->length, "Length matches for string that has no escape or closing quote %d", i);

    s = String_Make(m, bytes("Hi there this is a \\\"quoted string\\\""));
    String_Reset(backlog);
    Match_SetPattern(&mt, (PatCharDef *)def, backlog);
    i = 0;
    while(1){
        Match_Feed(m, &mt, s->bytes[i]);
        if((mt.type.state & PROCESSING) != 0){
            i++;
        }else{
            break;
        }
    }
    r |= Test(i == s->length, "Length matches for string minus escape cahrs that has two escapes in it, expecting %d, have %d", s->length-2, i);

    s = String_Make(m, bytes("Hi there this is a string ending \" here"));
    String_Reset(backlog);
    Match_SetPattern(&mt, (PatCharDef *)def, backlog);
    i = 0;
    while(1){
        Match_Feed(m, &mt, s->bytes[i]);
        if((mt.type.state & PROCESSING) != 0){
            i++;
        }else{
            break;
        }
    }
    r |= Test(i == s->length-6, "Length matches for string that has a terminator quote in it, have %d", i);

    s = String_Make(m, bytes("Hi there this is a string ending \\\" here"));
    String_Reset(backlog);
    Match_SetPattern(&mt, (PatCharDef *)def, backlog);
    i = 0;
    while(1){
        Match_Feed(m, &mt, s->bytes[i]);
        if((mt.type.state & PROCESSING) != 0){
            i++;
        }else{
            break;
        }
    }
    r |= Test(i == s->length, "Length matches for string that has a escaped terminator quote in it, have %d", i);

    word multiKoDef[] = {
        PAT_KO|PAT_KO_TERM, 'e', 'e', PAT_KO|PAT_KO_TERM, 'n', 'n', PAT_KO|PAT_KO_TERM, 'd', 'd', patText,
        PAT_END, 0, 0
    };

    s = String_Make(m, bytes("it all end. did it end?"));
    String_Reset(backlog);
    Match_SetPattern(&mt, (PatCharDef *)multiKoDef, backlog);
    i = 0;
    while(1){
        Match_Feed(m, &mt, s->bytes[i]);
        if((mt.type.state & PROCESSING) != 0){
            i++;
        }else{
            break;
        }
    }
    
    r |= Test(i == 9, "It took 10 counts to get to the end, have %d", i);
    r |= Test(Match_Total(&mt) == 7, "terminator 'end' is omited from the count expecting 7, have %d", Match_Total(&mt));

    s = String_Make(m, bytes("it's not all engaging until the end!"));

    String_Reset(backlog);
    Match_SetPattern(&mt, (PatCharDef *)multiKoDef, backlog);
    i = 0;
    while(1){
        Match_Feed(m, &mt, s->bytes[i]);
        if((mt.type.state & PROCESSING) != 0){
            i++;
        }else{
            break;
        }
    }
    r |= Test(Match_Total(&mt) == s->length-4, "terminator 'end' is omited and last punctuation as well, from the count, expecting %d, have %d", s->length-4,Match_Total(&mt));

    word eqDef[] = {
        PAT_KO, '!', '!',PAT_KO, '=', '=',PAT_KO, '<', '<',PAT_KO|PAT_KO_TERM, '>', '>',PAT_INVERT_CAPTURE|PAT_MANY,' ', ' ', patText,
        PAT_END, 0, 0
    };

    s = String_Make(m, bytes("9 != sessionMax"));
    String_Reset(backlog);
    Match_SetPattern(&mt, (PatCharDef *)eqDef, backlog);
    i = 0;
    while(1){
        Match_Feed(m, &mt, s->bytes[i]);
        if((mt.type.state & PROCESSING) != 0){
            i++;
        }else{
            break;
        }
    }
    r |= Test(Match_Total(&mt) == 1, "counted first letter only, have %d", Match_Total(&mt));

    MemCtx_Free(m);
    return r;
}
