#include <external.h>
#include <caneka.h>
#include <test.h>
#include "tests.h"

status Str_Tests(MemCh *m){
    Str *s;
    s = Str_CstrRef(m, "Hi");
    status r = READY;
    cls type = TYPE_STR;
    void *args1[] = {
        Type_ToStr(m, type),
        s,
        NULL
    };
    r |= Test(s->type.of == TYPE_STR, "Expect string to have fixed type $ found @", args1);
    i32 two = 2;
    void *args2[] = {
        I32_Wrapped(m, two),
        I32_Wrapped(m, s->length),
        NULL
    };
    r |= Test(s->length == 2, "Expect string length of @ found @", args2);
    void *args3[] = {
        Str_CstrRef(m, "Hi"),
        s,
        NULL
    };
    r |= Test(strncmp((char *)s->bytes, "Hi\0", 3) == 0,
        "Expect string match of @ found @", args3);

    i64 value = 35072;
    s = Str_FromI64(m, value);
    Str *expected_is = Str_CstrRef(m, "35072");
    void *args6[] = {
        I32_Wrapped(m, value), 
        I16_Wrapped(m, expected_is->length), 
        I16_Wrapped(m, s->length),
        NULL
    };
    r |= Test(s->length == expected_is->length, 
        "Expect for i32 value $ length of $ found $", args6);
    void *args7[] = {
        I32_Wrapped(m, value), 
        NULL
    };
    r |= Test(Equals(s, expected_is) == TRUE, 
        "Expect string match of i32 of $ to string", args7);

    char *cstr = "GET /path.html HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0\r\n"
    "Connection: keep-alive\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/png,image/svg+xml,*/*;q=0.8\r\n"
    "\r\n";

    s = Str_CstrRef(m, cstr);
    i64 len = strlen(cstr);
    void *args8[] = {
        I64_Wrapped(m, len),
        I16_Wrapped(m, s->length),
        NULL
    };
    r |= Test(s->length == strlen(cstr), "Expect length $, have $", args8);

    return r;
}

status Str_EndMatchTests(MemCh *m){
    status r = READY;
    Str *s;
    Str *s2;
    char *match;

    match = ".c";
    s = Str_CstrRef(m, "file1.c");
    void *args1[] = {
        s,
        NULL
    };
    r |= Test(Str_EndMatch(s, Str_CstrRef(m, match)), 
        "file ending in '.c' matches successfully, had @", args1);

    match = ".cnk";
    s = Str_CstrRef(m, "file1.cnk");
    void *args2[] = {
        s,
        NULL
    };
    r |= Test(Str_EndMatch(s, Str_CstrRef(m, match)), 
        "file ending in '.cnk' matches successfully, had @", args2);

    match = ".c";
    s = Str_Clone(m, s);
    Str_Trunc(s, -2);
    void *args3[] = {
        s,
        NULL
    };
    r |= Test(Str_EndMatch(s, Str_CstrRef(m, match)), "file ending in '.cnk' matches \".c\" after String_Trunc successfully, had @", args3);

    return r;
}
