#include <external.h>
#include <caneka.h>
#include <test_module.h>
#include "tests.h"

status Match_Tests(MemCh *m){
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

    void *args1[] = {
        State_ToStr(m, mt->type.state),
        NULL
    };
    r |= Test(mt->type.state != SUCCESS, "Non match has unsuccessful state found @", args1);

    s = Str_CstrRef(m, "pox");

    mt->type.state = READY;
    for(int i = 0; i < s->length; i++){
        Match_Feed(m, mt, s->bytes[i]);
    }
    void *args2[] = {
        Type_StateVec(m, mt->type.of, mt->type.state),
        NULL
    };
    r |= Test(mt->type.state == SUCCESS, "Matching string has successful state found @", args2); 


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
    void *args3[] = {
        I16_Wrapped(m, len), 
        I64_Wrapped(m, total), 
        NULL
    };
    r |= Test(total == s->length-1, "Matched length of string, less termMatching, expected $ have $", args3);

    void *args4[] = {s, mt->backlog, (void *)((i64)TYPE_SNIPSPAN), NULL};

    return r;
}

status MatchElastic_Tests(MemCh *m){
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
    void *args1[] = {
        State_ToStr(m, mt->type.state),
        NULL
    };
    r |= Test((mt->type.state & PROCESSING) != 0, "Has PROCESSING status $", args1);
    i64 delta = mt->pat.curDef - mt->pat.startDef;
    void *args2[] = {
         I64_Wrapped(m, delta),
        NULL
    };
    r |= Test(mt->pat.curDef == (mt->pat.startDef+1) , "On second pos, position is $", args2);

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
    void *args3[] = {
        State_ToStr(m, mt->type.state),
        NULL
    };
    r |= Test((mt->type.state & SUCCESS) != 0, "Tag- Found SUCCESS have @", args3);
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
    void *args4[] = {
        I64_Wrapped(m, total),
        NULL
    };
    r |= Test(total == 4, "Att - Found 4 chars, count is @", args4);

    return r;
}

status MatchKo_Tests(MemCh *m){
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
    void *args1[] = {
        I32_Wrapped(m, i),
        NULL
    };
    r |= Test(i == s->length, "Length matches for string that has no escape or closing quote @", args1);

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
    void *args2[] = {
        I16_Wrapped(m, subLen), 
        I32_Wrapped(m, i),
        NULL
    };
    r |= Test(i == s->length, "Length matches for string minus escape cahrs that has two escapes in it, expecting $, have $", args2);

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

    void *args3[] = {
        I32_Wrapped(m, i),
        NULL
    };
    r |= Test(i == s->length-6, "Length matches for string that has a terminator quote in it, have $", args3);

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
    void *args4[] = {
        I32_Wrapped(m, i),
        NULL
    };
    r |= Test(i == s->length, "Length matches for string that has a escaped terminator quote in it, have $", args4);

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
    
    void *args5[] = {
        I32_Wrapped(m, i),
        NULL
    };
    r |= Test(i == 9, "It took 10 counts to get to the end, have $", args5);
    i64 total = SnipSpan_Total(mt->backlog, SNIP_CONTENT);

    void *args6[] = {
        I32_Wrapped(m, total),
        NULL
    };
    r |= Test(total == 7, "Terminator 'end' is omited from the count expecting 7, have $", args6);

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
    void *args7[] = {
        I16_Wrapped(m, subLen),
        I64_Wrapped(m, total),
        NULL
    };
    r |= Test(total == s->length-4, "terminator 'end' is omited and last punctuation as well, from the count, expecting $, have $", args7);

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
    void *args8[] = {
        I64_Wrapped(m, total),
        NULL
    };
    r |= Test(total == 1, "counted first letter only, have $", args8);

    return r;
}

status MatchReplace_Tests(MemCh *m){
    Span *p;
    status r = READY;
    Match *mt = NULL;
    Str *s;

    Str *path = Str_CstrRef(m, "/happy/sad/fancy/things");
    Str *newPath = Str_CloneAlloc(m, path, STR_DEFAULT);
    Str *new = Str_CstrRef(m, "good");
    s = Str_CstrRef(m, "sad");
    Span *backlog = Span_Make(m);
    mt = Match_Make(m, PatChar_FromStr(m, s), backlog);
    mt->type.state |= MATCH_SEARCH;

    i32 pos;
    Match_StrReplace(m, newPath, new, mt, &pos);

    void *args[] = {
        mt,
        s,
        new,
        path,
        newPath,
        NULL
    };

    r |= Test(Equals(newPath,
        Str_CstrRef(m, "/happy/good/fancy/things")),
        "Match @: Replacing & with & in $ to make &\n", args);

    new = Str_CstrRef(m, "it");
    backlog = Span_Make(m);
    mt = Match_Make(m, PatChar_FromStr(m, s), backlog);
    mt->type.state |= MATCH_SEARCH;

    newPath = Str_CloneAlloc(m, path, STR_DEFAULT);
    Match_StrReplace(m, newPath, new, mt, &pos);

    void *args2[] = {
        mt,
        s,
        new,
        path,
        newPath,
        NULL
    };

    r |= Test(Equals(newPath, 
        Str_CstrRef(m, "/happy/it/fancy/things")),
        "Match @: Replacing @ with @ in $ to make &\n", args2);

    return r;
}
