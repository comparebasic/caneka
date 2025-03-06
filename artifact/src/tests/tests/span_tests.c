#include <external.h>
#include <caneka.h>

status Span_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    Span *p;
    status r = READY;
    String *s;

    p = Span_Make(m);
    r |= Test(p->type.of == TYPE_SPAN, "Span has type span %s found %s", 
        Class_ToString(TYPE_SPAN), Class_ToString(p->type.of));

    String *s1 = String_Make(m, bytes("Hello there"));
    Span_Add(p, (Abstract *)s1);
    String *s7 = String_Make(m, bytes("Seven"));
    Span_Set(p, 6, (Abstract *)s7);
    String *s3 = String_Make(m, bytes("Three"));
    Span_Set(p, 2, (Abstract *)s3);
    String *s8 = String_Make(m, bytes("Eight"));
    Span_Add(p, (Abstract *)s8);

    s = (String *)Span_Get(p, 0);
    r |= Test(s->type.of == s1->type.of, "Span item 0 has type string %s found %s", 
        Class_ToString(s1->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s1, s), "String 1 equals %s found %s", 
        s1->bytes, s->bytes);
        
    s = (String *)Span_Get(p, 2);
    r |= Test(s->type.of == s3->type.of, "Span item 2 has type string %s found %s", 
        Class_ToString(s3->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s3, s), "String 3 equals %s found %s", 
        s3->bytes, s->bytes);

    s = (String *)Span_Get(p, 6);
    r |= Test(s->type.of == s7->type.of, "Span item 6 has type string %s found %s", 
        Class_ToString(s7->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s7, s), "String 7 equals %s found %s", 
        s7->bytes, s->bytes);

    s = (String *)Span_Get(p, 7);
    r |= Test(s->type.of == s8->type.of, "Span item 7 has type string %s found %s", 
        Class_ToString(s8->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s8, s), "String 8 equals %s found %s", 
        s8->bytes, s->bytes);

    String *s4079 = String_Make(m, bytes("Four Hundred Seventy Nine"));
    Span_Set(p, 4078, (Abstract *)s4079);

    String *s512 = String_Make(m, bytes("Five Twelve"));
    Span_Set(p, 511, (Abstract *)s512);

    String *s513 = String_Make(m, bytes("Five Thirteen"));
    Span_Set(p, 512, (Abstract *)s513);

    s = (String *)Span_Get(p, 0);
    r |= Test(s->type.of == s1->type.of, "After expand, Span item 0 has type string %s found %s", 
        Class_ToString(s1->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s1, s), "String 1 equals %s found %s", 
        s1->bytes, s->bytes);
        
    s = (String *)Span_Get(p, 2);
    r |= Test(s->type.of == s3->type.of, "After expand,Span item 2 has type string %s found %s", 
        Class_ToString(s3->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s3, s), "String 3 equals %s found %s", 
        s3->bytes, s->bytes);

    s = (String *)Span_Get(p, 6);
    r |= Test(s->type.of == s7->type.of, "After expand,Span item 6 has type string %s found %s", 
        Class_ToString(s7->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s7, s), "String 7 equals %s found %s", 
        s7->bytes, s->bytes);

    s = (String *)Span_Get(p, 7);
    r |= Test(s->type.of == s8->type.of, "After expand, Span item 7 has type string %s found %s", 
        Class_ToString(s8->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s8, s), "String 8 equals %s found %s", 
        s8->bytes, s->bytes);

    s = (String *)Span_Get(p, 511);
    r |= Test(s->type.of == s512->type.of, "Span item 512 has type string %s found %s", 
        Class_ToString(s512->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s512, s), "String 512 equals %s found %s", 
        s512->bytes, s->bytes);

    s = (String *)Span_Get(p, 4078);
    r |= Test(s->type.of == s4079->type.of, "Span item 4079 has type string %s found %s", 
        Class_ToString(s4079->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s4079, s), "String 4079 equals %s found %s", 
        s4079->bytes, s->bytes);

    s = (String *)Span_Get(p, 512);
    r |= Test(s->type.of == s513->type.of, "Span item 513 has type string %s found %s", 
        Class_ToString(s513->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s513, s), "String 513 equals %s found %s", 
        s513->bytes, s->bytes);

    p = Span_Make(m);
    Span_Set(p, 0, (Abstract *)Int_Wrapped(m, 0));
    Span_Set(p, 1, (Abstract *)Int_Wrapped(m, 1));
    Span_Set(p, 2, (Abstract *)Int_Wrapped(m, 2));
    Span_Set(p, 6, (Abstract *)Int_Wrapped(m, 6));
    Span_Set(p, 225, (Abstract *)Int_Wrapped(m, 225));

    MemCtx_Free(m);
    return r;
}

status SpanClone_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    Span *p;
    status r = READY;

    p = Span_Make(m);
    String *s;

    Span_Set(p, 0, (Abstract *)String_Make(m, bytes("Zero")));
    Span_Set(p, 3, (Abstract *)String_Make(m, bytes("Three")));
    Span_Set(p, 4, (Abstract *)String_Make(m, bytes("Four")));
    Span_Set(p, 5, (Abstract *)String_Make(m, bytes("Five")));
    Span_Set(p, 17, (Abstract *)String_Make(m, bytes("Seventeen")));

    MemCtx_Free(m);
    return r;
}
