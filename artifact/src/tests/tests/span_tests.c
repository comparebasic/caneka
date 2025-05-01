#include <external.h>
#include <caneka.h>

status Span_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();
    Span *p;
    status r = READY;
    Str *s;

    /* set and retrieve numbers beyond stride */
    p = Span_Make(m);
    i64 max = 6;
    for(i64 i = 0; i < max; i++){
        Single *g = I64_Wrapped(m, i);
        Span_Set(p, i, (Abstract *)g);
    }

    for(i64 i = 0; i < max; i++){
        Single *g = Span_Get(p, i);
        Abstract *args1[] = {
            (Abstract *)I32_Wrapped(m, i),
            NULL
        };
        r |= Test(g != NULL, "Retrieved item is not null $", args1);
        if(r & ERROR){
            return r;
        }
        Abstract *args2[] = {
            (Abstract *)I32_Wrapped(m, i),
            (Abstract *)g, NULL
        };
        r |= Test(g->val.value == i, 
            "Retrieve an object with the same value as the index $ vs $", args2);
    }

    max = SPAN_STRIDE+1;
    for(i64 i = 0; i < max; i++){
        Single *g = I64_Wrapped(m, i);
        Span_Set(p, i, (Abstract *)g);
    }

    for(i64 i = 0; i < max; i++){
        Single *g = Span_Get(p, i);
        Abstract *args3[] = {
            (Abstract *)I32_Wrapped(m, i),
            NULL
        };
        r |= Test(g != NULL, "Retrieved item is not null $", args3);
        if(r & ERROR){
            return r;
        }
        Abstract *args4[] = {
            (Abstract *)I32_Wrapped(m, i),
            (Abstract *)g,
            NULL
        };
        r |= Test(g->val.value == i, 
            "Retrieve an object with the same value as the index $ vs $", args4);
    }

    /* set and retrieve strings */
    p = Span_Make(m);
    Abstract *args5[] = {
        (Abstract *)Type_ToStr(m, TYPE_SPAN),
        (Abstract *)Type_ToStr(m, p->type.of),
        NULL
    };
    r |= Test(p->type.of == TYPE_SPAN, "Span has type span '$' found '$'", args5);

    Str *s1 = Str_CstrRef(m, "Hello there");
    Span_Add(p, (Abstract *)s1);
    Str *s7 = Str_CstrRef(m, "Seven");
    Span_Set(p, 6, (Abstract *)s7);
    Str *s3 = Str_CstrRef(m, "Three");
    Span_Set(p, 2, (Abstract *)s3);
    Str *s8 = Str_CstrRef(m, "Eight");
    Span_Add(p, (Abstract *)s8);

    s = (Str *)Span_Get(p, 0);
    Abstract *args6[] = {
        (Abstract *)Type_ToStr(m, s1->type.of),
        (Abstract *)Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s1->type.of, "Span item 0 has type string $ found $", args6);
    Abstract *args7[] = {
        (Abstract *)s1,
        (Abstract *)s,
        NULL
    };
    r |= Test(Equals((Abstract *)s1, (Abstract *)s), "String 1 equals $ found $", args7);
        
    s = (Str *)Span_Get(p, 2);
    Abstract *args8[] = {
        (Abstract *)Type_ToStr(m, s3->type.of),
        (Abstract *)Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s3->type.of, "Span item 2 has type string $ found $", args8);
    Abstract *args9[] = {
        (Abstract *)s3,
        (Abstract *)s,
        NULL
    };
    r |= Test(Equals((Abstract *)s3, (Abstract *)s), "String 3 equals '$' found '$'", args9);

    s = (Str *)Span_Get(p, 6);
    Abstract *args10[] = {
        (Abstract *)Type_ToStr(m, s7->type.of),
        (Abstract *)Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s7->type.of, "Span item 6 has type string '$' found '$'", args10);
    Abstract *args11[] = {
        (Abstract *)s7,
        (Abstract *)s,
        NULL
    };
    r |= Test(Equals((Abstract *)s7, (Abstract *)s), "String 7 equals '$' found '$'", args11);

    s = (Str *)Span_Get(p, 7);
    Abstract *args12[] = {
        (Abstract *)Type_ToStr(m, s8->type.of),
        (Abstract *)Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s8->type.of, "Span item 7 has type string $ found $", args12);
    Abstract *args13[] = {
        (Abstract *)s8,
        (Abstract *)s,
        NULL
    };
    r |= Test(Equals((Abstract *)s8, (Abstract *)s), "String 8 equals '$' found '$'", args13);

    /* expand tests */
    Str *s4079 = Str_CstrRef(m, "Four Hundred Seventy Nine");
    Span_Set(p, 4078, (Abstract *)s4079);

    Str *s512 = Str_CstrRef(m, "Five Twelve");
    Span_Set(p, 511, (Abstract *)s512);

    Str *s513 = Str_CstrRef(m, "Five Thirteen");
    Span_Set(p, 512, (Abstract *)s513);

    s = (Str *)Span_Get(p, 0);
    r |= Test(s != 0, "After expand, Span item 0 is not null", NULL);
    Abstract *args14[] = {
        (Abstract *)Type_ToStr(m, s1->type.of),
        (Abstract *)Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s1->type.of, "After expand, Span item 0 has type string $ found $", args14);
    Abstract *args15[] = {
        (Abstract *)s1,
        (Abstract *)s,
        NULL
    };
    r |= Test(Equals((Abstract *)s1, (Abstract *)s), "String 1 equals '$' found '$'", args15);
    s = (Str *)Span_Get(p, 2);
    Abstract *args16[] = {
        (Abstract *)Type_ToStr(m, s3->type.of),
        (Abstract *)Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s3->type.of, "After expand,Span item 2 has type string @ found @", args16);
    Abstract *args17[] = {
        (Abstract *)s3,
        (Abstract *)s,
        NULL
    };
    r |= Test(Equals((Abstract *)s3, (Abstract *)s), "String 3 equals '$' found '$'", args17);

    s = (Str *)Span_Get(p, 6);
    Abstract *args18[] = {
        (Abstract *)Type_ToStr(m, s7->type.of),
        (Abstract *)Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s7->type.of, "After expand,Span item 6 has type string $ found $", args18);
    Abstract *args19[] = {
        (Abstract *)s7,
        (Abstract *)s,
        NULL
    };
    r |= Test(Equals((Abstract *)s7, (Abstract *)s), "String 7 equals '$' found '$'", args19);

    s = (Str *)Span_Get(p, 7);
    Abstract *args20[] = {
        (Abstract *)Type_ToStr(m, s8->type.of),
        (Abstract *)Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s8->type.of, "After expand, Span item 7 has type string $ found $", args20);
    Abstract *args21[] = {
        (Abstract *)s8,
        (Abstract *)s,
        NULL
    };
    r |= Test(Equals((Abstract *)s8, (Abstract *)s), "String 8 equals '$' found '$'", args12);

    s = (Str *)Span_Get(p, 511);
    Abstract *args22[] = {
        (Abstract *)Type_ToStr(m, s512->type.of),
        (Abstract *)Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s512->type.of, "Span item 512 has type string _c found _c", args22);
    Abstract *args23[] = {
        (Abstract *)s512,
        (Abstract *)s,
        NULL
    };
    r |= Test(Equals((Abstract *)s512, (Abstract *)s), "String 512 equals '_t' found '_t'", args23);

    s = (Str *)Span_Get(p, 4078);
    Abstract *args24[] = {
        (Abstract *)Type_ToStr(m, s4079->type.of),
        (Abstract *)Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s4079->type.of, "Span item 4079 has type string _c found _c", args24);
    Abstract *args25[] = {
        (Abstract *)s4079,
        (Abstract *)s,
        NULL
    };
    r |= Test(Equals((Abstract *)s4079, (Abstract *)s), "String 4079 equals '_t' found '_t'", args25);

    s = (Str *)Span_Get(p, 512);
    Abstract *args26[] = {
        (Abstract *)Type_ToStr(m, s513->type.of),
        (Abstract *)Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s513->type.of, "Span item 513 has type string _c found _c", args26);
    Abstract *args27[] = {
        (Abstract *)s513,
        (Abstract *)s,
        NULL
    };
    r |= Test(Equals((Abstract *)s513, (Abstract *)s), "String 513 equals '_t' found '_t'", args27);

    /* raw tests 
    p = Span_Make(m);
    Span_Set(p, 0, (Abstract *)Int_Wrapped(m, 0));
    Span_Set(p, 1, (Abstract *)Int_Wrapped(m, 1));
    Span_Set(p, 2, (Abstract *)Int_Wrapped(m, 2));
    Span_Set(p, 6, (Abstract *)Int_Wrapped(m, 6));
    Span_Set(p, 225, (Abstract *)Int_Wrapped(m, 225));
    */

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}

status SpanClone_Tests(MemCh *gm){
    MemCh *m = MemCh_Make();
    Span *p;
    status r = READY;

    p = Span_Make(m);

    Span_Set(p, 0, (Abstract *)Str_CstrRef(m, "Zero"));
    Span_Set(p, 3, (Abstract *)Str_CstrRef(m, "Three"));
    Span_Set(p, 4, (Abstract *)Str_CstrRef(m, "Four"));
    Span_Set(p, 5, (Abstract *)Str_CstrRef(m, "Five"));
    Span_Set(p, 17, (Abstract *)Str_CstrRef(m, "Seventeen"));

    MemCh_Free(m);
    return r;
}
