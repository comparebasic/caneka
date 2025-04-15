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
        r |= Test(g != NULL, "Retrieved item is not null _i", i);
        if(r & ERROR){
            return r;
        }
        r |= Test(g->val.value == i, 
            "Retrieve an object with the same value as the index _i8 vs _i8", i, g->val.value);
    }

    max = SPAN_STRIDE+1;
    for(i64 i = 0; i < max; i++){
        Single *g = I64_Wrapped(m, i);
        Span_Set(p, i, (Abstract *)g);
    }

    for(i64 i = 0; i < max; i++){
        Single *g = Span_Get(p, i);
        r |= Test(g != NULL, "Retrieved item is not null _i", i);
        if(r & ERROR){
            return r;
        }
        r |= Test(g->val.value == i, 
            "Retrieve an object with the same value as the index _i8 vs _i8", i, g->val.value);
    }

    /* set and retrieve strings */
    p = Span_Make(m);
    r |= Test(p->type.of == TYPE_SPAN, "Span has type span '_c' found '_c'", 
        Type_ToChars(TYPE_SPAN), Type_ToChars(p->type.of));

    Str *s1 = Str_CstrRef(m, "Hello there");
    Span_Add(p, (Abstract *)s1);
    Str *s7 = Str_CstrRef(m, "Seven");
    Span_Set(p, 6, (Abstract *)s7);
    Str *s3 = Str_CstrRef(m, "Three");
    Span_Set(p, 2, (Abstract *)s3);
    Str *s8 = Str_CstrRef(m, "Eight");
    Span_Add(p, (Abstract *)s8);

    s = (Str *)Span_Get(p, 0);
    r |= Test(s->type.of == s1->type.of, "Span item 0 has type string _c found _c", 
        Type_ToChars(s1->type.of), Type_ToChars(s->type.of));
    r |= Test(Equals((Abstract *)s1, (Abstract *)s), "String 1 equals _c found _c", 
        s1->bytes, s->bytes);
        
    s = (Str *)Span_Get(p, 2);
    r |= Test(s->type.of == s3->type.of, "Span item 2 has type string _c found _c", 
        Type_ToChars(s3->type.of), Type_ToChars(s->type.of));
    r |= Test(Equals((Abstract *)s3, (Abstract *)s), "String 3 equals '_t' found '_t'", 
        s3, s);

    s = (Str *)Span_Get(p, 6);
    r |= Test(s->type.of == s7->type.of, "Span item 6 has type string '_c' found '_c'", 
        Type_ToChars(s7->type.of), Type_ToChars(s->type.of));
    r |= Test(Equals((Abstract *)s7, (Abstract *)s), "String 7 equals '_t' found '_t'", 
        s7, s);

    s = (Str *)Span_Get(p, 7);
    r |= Test(s->type.of == s8->type.of, "Span item 7 has type string _c found _c", 
        Type_ToChars(s8->type.of), Type_ToChars(s->type.of));
    r |= Test(Equals((Abstract *)s8, (Abstract *)s), "String 8 equals '_t' found '_t'", 
        s8, s);

    /* expand tests */
    Str *s4079 = Str_CstrRef(m, "Four Hundred Seventy Nine");
    Span_Set(p, 4078, (Abstract *)s4079);

    Str *s512 = Str_CstrRef(m, "Five Twelve");
    Span_Set(p, 511, (Abstract *)s512);

    Str *s513 = Str_CstrRef(m, "Five Thirteen");
    Span_Set(p, 512, (Abstract *)s513);

    s = (Str *)Span_Get(p, 0);
    r |= Test(s != 0, "After expand, Span item 0 is not null");
    r |= Test(s->type.of == s1->type.of, "After expand, Span item 0 has type string _c found _c", 
        Type_ToChars(s1->type.of), Type_ToChars(s->type.of));
    r |= Test(Equals((Abstract *)s1, (Abstract *)s), "String 1 equals '_t' found '_t'", 
        s1, s);
        
    s = (Str *)Span_Get(p, 2);
    r |= Test(s->type.of == s3->type.of, "After expand,Span item 2 has type string _c found _c", 
        Type_ToChars(s3->type.of), Type_ToChars(s->type.of));
    r |= Test(Equals((Abstract *)s3, (Abstract *)s), "String 3 equals '_t' found '_t'", 
        s3, s);

    s = (Str *)Span_Get(p, 6);
    r |= Test(s->type.of == s7->type.of, "After expand,Span item 6 has type string _c found _c", 
        Type_ToChars(s7->type.of), Type_ToChars(s->type.of));
    r |= Test(Equals((Abstract *)s7, (Abstract *)s), "String 7 equals '_t' found '_t'", 
        s7, s);

    s = (Str *)Span_Get(p, 7);
    r |= Test(s->type.of == s8->type.of, "After expand, Span item 7 has type string _c found _c", 
        Type_ToChars(s8->type.of), Type_ToChars(s->type.of));
    r |= Test(Equals((Abstract *)s8, (Abstract *)s), "String 8 equals '_t' found '_t'", 
        s8, s);

    s = (Str *)Span_Get(p, 511);
    r |= Test(s->type.of == s512->type.of, "Span item 512 has type string _c found _c", 
        Type_ToChars(s512->type.of), Type_ToChars(s->type.of));
    r |= Test(Equals((Abstract *)s512, (Abstract *)s), "String 512 equals '_t' found '_t'", 
        s512, s);

    s = (Str *)Span_Get(p, 4078);
    r |= Test(s->type.of == s4079->type.of, "Span item 4079 has type string _c found _c", 
        Type_ToChars(s4079->type.of), Type_ToChars(s->type.of));
    r |= Test(Equals((Abstract *)s4079, (Abstract *)s), "String 4079 equals '_t' found '_t'", 
        s4079, s);

    s = (Str *)Span_Get(p, 512);
    r |= Test(s->type.of == s513->type.of, "Span item 513 has type string _c found _c", 
        Type_ToChars(s513->type.of), Type_ToChars(s->type.of));
    r |= Test(Equals((Abstract *)s513, (Abstract *)s), "String 513 equals '_t' found '_t'", 
        s513, s);

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
