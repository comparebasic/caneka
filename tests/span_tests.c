#include <external.h>
#include <filestore.h>
#include <testsuite.h>

status Span_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    Span *p;
    status r = READY;
    String *s;

    p = Span_Make(m);
    r |= Test(p->type.of == TYPE_SPAN, "Span has type span %s found %s", 
        Class_ToString(TYPE_SPAN), Class_ToString(p->type.of));

    String *s1 = String_From(m, bytes("Hello there"));
    Span_Add(m, p, (Unit *)s1);
    String *s7 = String_From(m, bytes("Seven"));
    Span_Set(m, p, 6, (Unit *)s7);
    String *s3 = String_From(m, bytes("Three"));
    Span_Set(m, p, 2, (Unit *)s3);
    String *s8 = String_From(m, bytes("Eight"));
    Span_Add(m, p, (Unit *)s8);

    s = (String *)Span_Get(m, p, 0);
    r |= Test(s->type.of == s1->type.of, "Span item 0 has type string %s found %s", 
        Class_ToString(s1->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s1, s), "String 1 equals %s found %s", 
        s1->bytes, s->bytes);
        
    s = (String *)Span_Get(m, p, 2);
    r |= Test(s->type.of == s3->type.of, "Span item 2 has type string %s found %s", 
        Class_ToString(s3->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s3, s), "String 3 equals %s found %s", 
        s3->bytes, s->bytes);

    s = (String *)Span_Get(m, p, 6);
    r |= Test(s->type.of == s7->type.of, "Span item 6 has type string %s found %s", 
        Class_ToString(s7->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s7, s), "String 7 equals %s found %s", 
        s7->bytes, s->bytes);

    s = (String *)Span_Get(m, p, 7);
    r |= Test(s->type.of == s8->type.of, "Span item 7 has type string %s found %s", 
        Class_ToString(s8->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s8, s), "String 8 equals %s found %s", 
        s8->bytes, s->bytes);

    String *s4079 = String_From(m, bytes("Four Hundred Seventy Nine"));
    Span_Set(m, p, 4078, (Unit *)s4079);

    String *s512 = String_From(m, bytes("Five Twelve"));
    Span_Set(m, p, 511, (Unit *)s512);

    String *s513 = String_From(m, bytes("Five Thirteen"));
    Span_Set(m, p, 512, (Unit *)s513);

    Debug_Print((void *)p, TYPE_SPAN, "Span after first insert(s) ", COLOR_CYAN, TRUE);

    /*
    s = (String *)Span_Get(m, p, 0);
    r |= Test(s->type.of == s1->type.of, "After expand, Span item 0 has type string %s found %s", 
        Class_ToString(s1->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s1, s), "String 1 equals %s found %s", 
        s1->bytes, s->bytes);
        
    s = (String *)Span_Get(m, p, 2);
    r |= Test(s->type.of == s3->type.of, "After expand,Span item 2 has type string %s found %s", 
        Class_ToString(s3->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s3, s), "String 3 equals %s found %s", 
        s3->bytes, s->bytes);

    s = (String *)Span_Get(m, p, 6);
    r |= Test(s->type.of == s7->type.of, "After expand,Span item 6 has type string %s found %s", 
        Class_ToString(s7->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s7, s), "String 7 equals %s found %s", 
        s7->bytes, s->bytes);

    s = (String *)Span_Get(m, p, 7);
    r |= Test(s->type.of == s8->type.of, "After expand, Span item 7 has type string %s found %s", 
        Class_ToString(s8->type.of), Class_ToString(s->type.of));
    r |= Test(String_Equals(s8, s), "String 8 equals %s found %s", 
        s8->bytes, s->bytes);
        */


    MemCtx_Free(m);
    return r;
}
