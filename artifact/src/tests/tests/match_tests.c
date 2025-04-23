#include <external.h>
#include <caneka.h>

status Match_Tests(MemCh *gm){
    MemCh *m = MemCh_Make();
    Span *p;
    status r = READY;

    Match *mt = NULL;
    Str *s;
    Span *backlog = NULL;

    s = Str_CstrRef(m, "pox");
    backlog = Span_Make(m);
    mt = Match_Make(m, PatChar_FromStr(m, s), backlog);

    s = Str_CstrRef(m, "no");

    for(int i = 0; i < s->length; i++){
        Match_Feed(m, mt, s->bytes[i]);
    }

    void *args1[] = {State_ToStr(m, mt->type.state), NULL};
    r |= Test(mt->type.state != SUCCESS, "Non match has unsuccessful state found _t", args1); 

    s = Str_CstrRef(m, "pox");

    mt->type.state = READY;
    for(int i = 0; i < s->length; i++){
        Match_Feed(m, mt, s->bytes[i]);
    }
    void *args2[] = {State_ToStr(m, mt->type.state), NULL};
    r |= Test(mt->type.state == SUCCESS, "Matching string has successful state found _t", args2); 


    PatCharDef line[] = {{PAT_KO, '\n', '\n'}, patText, {PAT_END, 0, 0}};  

    s = Str_CstrRef(m, "A good line :)\n");
    backlog = Span_Make(m);
    mt = Match_Make(m, line, backlog); 
    for(int i = 0; i < s->length; i++){
        Match_Feed(m, mt, s->bytes[i]);
        if((mt->type.state & PROCESSING) == 0){
            break;
        }
    }

    word len = s->length-1;
    i64 total = SnipSpan_Total(mt->backlog, SNIP_CONTENT);
    void *args3[] = {&len, &total, NULL};
    r |= Test(total == s->length-1, "Matched length of string, less termMatching, expected _i2 have _i8", args3);

    void *args4[] = {s, mt->backlog, (void *)((i64)TYPE_SNIPSPAN), NULL};

    MemCh_Free(m);
    return r;
}

status MatchElastic_Tests(MemCh *gm){
    MemCh *m = MemCh_Make();
    status r = READY;

    Match *mt = NULL;
    Str *s;
    s = Str_CstrRef(m, "<tag atts=\"poo\">hi</tab>");

    PatCharDef pat[] = {
        {PAT_INVERT_CAPTURE|PAT_TERM, '<', '<'},
        {PAT_MANY|PAT_TERM, 'a', 'z'},
        {PAT_END, 0, 0}
    };
    mt = Match_Make(m, pat, NULL);

    i32 i = 0;
    Match_Feed(m, mt, s->bytes[i]);
    i++;
    void *args1[] = {State_ToStr(m, mt->type.state), NULL};
    r |= Test((mt->type.state & PROCESSING) != 0, "Has PROCESSING status _t", args1);
    i64 delta = mt->pat.curDef - mt->pat.startDef;
    void *args2[] = {&delta, NULL};
    r |= Test(mt->pat.curDef == (mt->pat.startDef+1) , "On second pos, position is _i8", args2);

    int count = 0;
    while(1){
        Match_Feed(m, mt, s->bytes[i]);
        if((mt->type.state & PROCESSING) != 0){
            count++;
            i++;
        }else{
            break;
        }
    }

    r |= Test(i ==  4, "Tag -Stopped on the fourth character", NULL);
    r |= Test(count == 3, "Tag -Found three chars", NULL);
    void *args3[] = {State_ToStr(m, mt->type.state), NULL};
    r |= Test((mt->type.state & SUCCESS) != 0, "Tag- Found SUCCESS have _t", args3);
    PatCharDef *def = mt->pat.curDef;
    r |= Test((def->flags == PAT_END), "Tag -At end", NULL);

    PatCharDef att[] = {{PAT_INVERT_CAPTURE|PAT_TERM, ' ', ' '}, {PAT_KO, '=', '='}, {PAT_MANY|PAT_TERM, 'a', 'z'}, {PAT_END, 0, 0}};
    mt = Match_Make(m, att, NULL);
    count = 0;
    while(1){
        Match_Feed(m, mt, s->bytes[i]);
        if((mt->type.state & PROCESSING) != 0){
            count++;
            i++;
        }else{
            break;
        }
    }

    i64 total = SnipSpan_Total(mt->backlog, SNIP_CONTENT);
    void *args4[] = {&total, NULL};
    r |= Test(total == 4, "Att - Found 4 chars, count is _i8", args4);

    MemCh_Free(m);
    return r;
}

status MatchKo_Tests(MemCh *gm){
    MemCh *m = MemCh_Make();
    Span *p;
    status r = READY;
    /*

    Match mt;
    Match ko;
    Str *s;
    Str *backlog = String_Init(m, STRING_EXTEND);
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
    r |= Test(Match_Total(&mt) == 7, "Terminator 'end' is omited from the count expecting 7, have %d", Match_Total(&mt));

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
    */

    MemCh_Free(m);
    return r;
}
