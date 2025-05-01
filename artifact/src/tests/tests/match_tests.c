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

    Match *mt = NULL;
    Str *s = NULL;

    i32 i = 0;
    PatCharDef def[] = {
        {PAT_MANY|PAT_KO|PAT_INVERT, '\\', '\\'}, {PAT_MANY|PAT_KO, '"', '"'}, patText,
        {PAT_END, 0, 0}
    };

    s = Str_CstrRef(m, "Hi there this is a string");
    mt = Match_Make(m, def, NULL);
    i = 0;
    while(1){
        Match_Feed(m, mt, s->bytes[i]);
        if((mt->type.state & PROCESSING) != 0){
            i++;
        }else{
            break;
        }
    }
    void *args1[] = {&i, NULL};
    r |= Test(i == s->length, "Length matches for string that has no escape or closing quote _i8", args1);

    s = Str_CstrRef(m, "Hi there this is a \\\"quoted string\\\"");
    mt = Match_Make(m, def, NULL);
    i = 0;
    while(1){
        Match_Feed(m, mt, s->bytes[i]);
        if((mt->type.state & PROCESSING) != 0){
            i++;
        }else{
            break;
        }
    }
    i16 subLen = s->length-2;
    void *args2[] = {&subLen, &i, NULL};
    r |= Test(i == s->length, "Length matches for string minus escape cahrs that has two escapes in it, expecting _i2, have _i4", args2);

    s = Str_CstrRef(m, "Hi there this is a string ending \" here");
    mt = Match_Make(m, def, NULL);
    i = 0;
    while(1){
        Match_Feed(m, mt, s->bytes[i]);
        if((mt->type.state & PROCESSING) != 0){
            i++;
        }else{
            break;
        }
    }
    void *args3[] = {&i, NULL};
    r |= Test(i == s->length-6, "Length matches for string that has a terminator quote in it, have _i4", args3);

    s = Str_CstrRef(m, "Hi there this is a string ending \\\" here");
    mt = Match_Make(m, def, NULL);
    i = 0;
    while(1){
        Match_Feed(m, mt, s->bytes[i]);
        if((mt->type.state & PROCESSING) != 0){
            i++;
        }else{
            break;
        }
    }
    void *args4[] = {&i, NULL};
    r |= Test(i == s->length, "Length matches for string that has a escaped terminator quote in it, have _i4", args4);

    PatCharDef multiKoDef[] = {
        {PAT_KO|PAT_KO_TERM, 'e', 'e'},
        {PAT_KO|PAT_KO_TERM, 'n', 'n'},
        {PAT_KO|PAT_KO_TERM, 'd', 'd'},
        patText,
        {PAT_END, 0, 0}
    };

    s = Str_CstrRef(m, "it all end. did it end?");
    mt = Match_Make(m, multiKoDef, NULL);
    i = 0;
    while(1){
        Match_Feed(m, mt, s->bytes[i]);
        if((mt->type.state & PROCESSING) != 0){
            i++;
        }else{
            break;
        }
    }
    
    void *args5[] = {&i, NULL};
    r |= Test(i == 9, "It took 10 counts to get to the end, have _i4", args5);
    i64 total = SnipSpan_Total(mt->backlog, SNIP_CONTENT);
    void *args6[] = {&total, NULL};
    r |= Test(total == 7, "Terminator 'end' is omited from the count expecting 7, have %d", args6);

    s = Str_CstrRef(m, "it's not all engaging until the end!");
    mt = Match_Make(m, multiKoDef, NULL);
    i = 0;
    while(1){
        Match_Feed(m, mt, s->bytes[i]);
        if((mt->type.state & PROCESSING) != 0){
            i++;
        }else{
            break;
        }
    }
    subLen = s->length-4;
    total = SnipSpan_Total(mt->backlog, SNIP_CONTENT);
    void *args7[] = {&subLen, &total, NULL};
    r |= Test(total == s->length-4, "terminator 'end' is omited and last punctuation as well, from the count, expecting _i2, have _i4", args7);

    PatCharDef eqDef[] = {
        {PAT_KO, '!', '!'},
        {PAT_KO, '=', '='},
        {PAT_KO, '<', '<'},
        {PAT_KO|PAT_KO_TERM, '>', '>'},
        {PAT_INVERT_CAPTURE|PAT_MANY,' ', ' '}, 
        patText,
        {PAT_END, 0, 0}
    };

    s = Str_CstrRef(m, "9 != sessionMax");
    mt = Match_Make(m, eqDef, NULL);
    i = 0;
    while(1){
        Match_Feed(m, mt, s->bytes[i]);
        if((mt->type.state & PROCESSING) != 0){
            i++;
        }else{
            break;
        }
    }

    total = SnipSpan_Total(mt->backlog, SNIP_CONTENT);
    void *args8[] = {&total, NULL};
    r |= Test(total == 1, "counted first letter only, have %d", args8);

    MemCh_Free(m);
    return r;
}
