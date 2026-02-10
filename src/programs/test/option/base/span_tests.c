#include <external.h>
#include <caneka.h>
#include <test_module.h>

static status makeAndCompareItems(MemCh *m, i64 max){
    void *args[2];
    status r = READY;
    m->level++;
    Span *p = Span_Make(m);
    for(i64 i = 0; i < max; i++){
        Span_Set(p, i, I64_Wrapped(m, i));
    }

    for(i64 i = 0; i < max; i++){
        Single *sg = Span_Get(p, i);
        if(sg->val.i != i){
            r |= ERROR;
            break;
        }
    }

    args[0] = I64_Wrapped(m, max);
    args[1] = NULL;
    r |= Test((r & ERROR) == 0, "Adding and comparing $ items", args);
    MemCh_Free(m);
    m->level--;
    return r; 
}

static status testDims(MemCh *m, i32 idx, i8 expectedDim){
    i8 dimsNeeded = 0;
    while(_increments[dimsNeeded+1] <= idx){
        ++dimsNeeded;
    }
    void *args[] = {
        I32_Wrapped(m, idx),
        I8_Wrapped(m, expectedDim),
        I8_Wrapped(m, dimsNeeded),
        NULL
    };

    return Test(dimsNeeded == expectedDim, "0 Dims needed for value of $, expected $, have $", 
        args);
}

status Span_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    Span *p;
    status r = READY;
    Str *s;

    /* set and retrieve numbers beyond stride */
    p = Span_Make(m);
    i64 max = 6;
    for(i64 i = 0; i < max; i++){
        Single *g = I64_Wrapped(m, i);
        Span_Set(p, i, g);
    }

    for(i64 i = 0; i < max; i++){
        Single *g = Span_Get(p, i);
        void *args1[] = {
            I32_Wrapped(m, i),
            NULL
        };
        r |= Test(g != NULL, "Retrieved item is not null $", args1);
        if(r & ERROR){
            return r;
        }
        void *args2[] = {
            I32_Wrapped(m, i),
            g, NULL
        };
        r |= Test(g->val.value == i, 
            "Retrieve an object with the same value as the index $ vs $", args2);
    }

    max = SPAN_STRIDE+1;
    for(i64 i = 0; i < max; i++){
        Single *g = I64_Wrapped(m, i);
        Span_Set(p, i, g);
    }

    for(i64 i = 0; i < max; i++){
        Single *g = Span_Get(p, i);
        void *args3[] = {
            I32_Wrapped(m, i),
            NULL
        };
        r |= Test(g != NULL, "Retrieved item is not null $", args3);
        if(r & ERROR){
            return r;
        }
        void *args4[] = {
            I32_Wrapped(m, i),
            g,
            NULL
        };
        r |= Test(g->val.value == i, 
            "Retrieve an object with the same value as the index $ vs $", args4);
    }

    /* set and retrieve strings */
    p = Span_Make(m);
    void *args5[] = {
        Type_ToStr(m, TYPE_SPAN),
        Type_ToStr(m, p->type.of),
        NULL
    };
    r |= Test(p->type.of == TYPE_SPAN, "Span has type span '$' found '$'", args5);

    Str *s1 = Str_CstrRef(m, "Hello there");
    Span_Add(p, s1);
    Str *s7 = Str_CstrRef(m, "Seven");
    Span_Set(p, 6, s7);
    Str *s3 = Str_CstrRef(m, "Three");
    Span_Set(p, 2, s3);
    Str *s8 = Str_CstrRef(m, "Eight");
    Span_Add(p, s8);

    s = (Str *)Span_Get(p, 0);
    void *args6[] = {
        Type_ToStr(m, s1->type.of),
        Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s1->type.of, "Span item 0 has type string $ found $", args6);
    void *args7[] = {
        s1,
        s,
        NULL
    };
    r |= Test(Equals(s1, s), "String 1 equals $ found $", args7);
        
    s = (Str *)Span_Get(p, 2);
    void *args8[] = {
        Type_ToStr(m, s3->type.of),
        Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s3->type.of, "Span item 2 has type string $ found $", args8);
    void *args9[] = {
        s3,
        s,
        NULL
    };
    r |= Test(Equals(s3, s), "String 3 equals '$' found '$'", args9);

    s = (Str *)Span_Get(p, 6);
    void *args10[] = {
        Type_ToStr(m, s7->type.of),
        Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s7->type.of, "Span item 6 has type string '$' found '$'", args10);
    void *args11[] = {
        s7,
        s,
        NULL
    };
    r |= Test(Equals(s7, s), "String 7 equals '$' found '$'", args11);

    s = (Str *)Span_Get(p, 7);
    void *args12[] = {
        Type_ToStr(m, s8->type.of),
        Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s8->type.of, "Span item 7 has type string $ found $", args12);
    void *args13[] = {
        s8,
        s,
        NULL
    };
    r |= Test(Equals(s8, s), "String 8 equals '$' found '$'", args13);

    /* expand tests */
    Str *s4079 = Str_CstrRef(m, "Four Hundred Seventy Nine");
    Span_Set(p, 4078, s4079);

    Str *s512 = Str_CstrRef(m, "Five Twelve");
    Span_Set(p, 511, s512);

    Str *s513 = Str_CstrRef(m, "Five Thirteen");
    Span_Set(p, 512, s513);

    s = (Str *)Span_Get(p, 0);
    r |= Test(s != 0, "After expand, Span item 0 is not null", NULL);
    void *args14[] = {
        Type_ToStr(m, s1->type.of),
        Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s1->type.of, "After expand, Span item 0 has type string $ found $", args14);
    void *args15[] = {
        s1,
        s,
        NULL
    };
    r |= Test(Equals(s1, s), "String 1 equals '$' found '$'", args15);
    s = (Str *)Span_Get(p, 2);
    void *args16[] = {
        Type_ToStr(m, s3->type.of),
        Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s3->type.of, "After expand,Span item 2 has type string @ found @", args16);
    void *args17[] = {
        s3,
        s,
        NULL
    };
    r |= Test(Equals(s3, s), "String 3 equals '$' found '$'", args17);

    s = (Str *)Span_Get(p, 6);
    void *args18[] = {
        Type_ToStr(m, s7->type.of),
        Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s7->type.of, "After expand,Span item 6 has type string $ found $", args18);
    void *args19[] = {
        s7,
        s,
        NULL
    };
    r |= Test(Equals(s7, s), "String 7 equals '$' found '$'", args19);

    s = (Str *)Span_Get(p, 7);
    void *args20[] = {
        Type_ToStr(m, s8->type.of),
        Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s8->type.of, "After expand, Span item 7 has type string $ found $", args20);
    void *args21[] = {
        s8,
        s,
        NULL
    };
    r |= Test(Equals(s8, s), "String 8 equals '$' found '$'", args12);

    s = (Str *)Span_Get(p, 511);
    void *args22[] = {
        Type_ToStr(m, s512->type.of),
        Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s512->type.of, "Span item 512 has type string $ found $", args22);
    void *args23[] = {
        s512,
        s,
        NULL
    };
    r |= Test(Equals(s512, s), "String 512 equals @ found @", args23);

    s = (Str *)Span_Get(p, 4078);
    void *args24[] = {
        Type_ToStr(m, s4079->type.of),
        Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s4079->type.of, "Span item 4079 has type string $ found $", args24);
    void *args25[] = {
        s4079,
        s,
        NULL
    };
    r |= Test(Equals(s4079, s), "String 4079 equals @ found @", args25);

    s = (Str *)Span_Get(p, 512);
    void *args26[] = {
        Type_ToStr(m, s513->type.of),
        Type_ToStr(m, s->type.of),
        NULL
    };
    r |= Test(s->type.of == s513->type.of, "Span item 513 has type string $ found $", args26);
    void *args27[] = {
        s513,
        s,
        NULL
    };
    r |= Test(Equals(s513, s), "String 513 equals @ found @", args27);

    DebugStack_Pop();
    return r;
}

status SpanClone_Tests(MemCh *m){
    Span *p;
    status r = READY;

    p = Span_Make(m);

    Span_Set(p, 0, Str_CstrRef(m, "Zero"));
    Span_Set(p, 3, Str_CstrRef(m, "Three"));
    Span_Set(p, 4, Str_CstrRef(m, "Four"));
    Span_Set(p, 5, Str_CstrRef(m, "Five"));
    Span_Set(p, 17, Str_CstrRef(m, "Seventeen"));

    return r;
}

status SpanMax_Tests(MemCh *m){
    status r = READY;
    void *args[5];

    r |= testDims(m, 10, 0);
    r |= testDims(m, 16, 1);
    r |= testDims(m, 34, 1);
    r |= testDims(m, 244, 1);
    r |= testDims(m, 257, 2);
    r |= testDims(m, 3078, 2);
    r |= testDims(m, 5000, 3);
    r |= testDims(m, 64000, 3);
    r |= testDims(m, 66000, 4);

    r |= makeAndCompareItems(m, 18);
    r |= makeAndCompareItems(m, 72);
    r |= makeAndCompareItems(m, 300);
    r |= makeAndCompareItems(m, 5000);
    r |= makeAndCompareItems(m, 63000);
    r |= makeAndCompareItems(m, 100000);

    m->level = 0;
    args[0] = I32_Wrapped(m, m->it.p->nvalues);
    args[1] = I8_Wrapped(m, m->it.p->dims);
    args[2] = NULL;
    Out("^p.MemCh nvalues $/$dims^0\n", args);
    return r;
}
