#include <external.h>
#include <caneka.h>
#include <tests.h>

status Str_Tests(MemCh *gm){
    MemCh *m = MemCh_Make();
    Str *s;
    s = Str_CstrRef(m, "Hi");
    status r = READY;
    r |= Test(s->type.of == TYPE_STR, "Expect string to have fixed type _O found _o", (i32)TYPE_STR, s);
    r |= Test(s->length == 2, "Expect string length of _i4 found _i4", 2, s->length);
    r |= Test(strncmp((char *)s->bytes, "Hi\0", 3) == 0, "Expect string match of '_c' found '_t'", "Hi", s);

    return r;

    s = Str_CstrRef(m, longCstr);
    i32 ls_l = strlen(longCstr);

    r |= Test(s->type.of == TYPE_STR, 
        "Expect string to have chain type _O found _o", (i32)TYPE_STR, s->type.of);
    r |= Test(s->length == ls_l, "Expect string length of _i4 found _i4", ls_l, s->length);

    i64 value = 35072;
    s = Str_FromI64(m, value);
    Str *expected_is = Str_CstrRef(m, "35072");
    r |= Test(s->length == expected_is->length, "Expect for i32 value _i4  length of _i4 found _i4", value, expected_is->length, s->length);
    r |= Test(Equals((Abstract *)s, (Abstract *)expected_is) == TRUE, "Expect string match of i32 of _i4 to string", value);


    char *cstr = "GET /path.html HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0\r\n"
    "Connection: keep-alive\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/png,image/svg+xml,*/*;q=0.8\r\n"
    "\r\n";

    s = Str_CstrRef(m, cstr);

    r |= Test(s->length == strlen(cstr), "Expect length _i8, have _i4", strlen(cstr), s->length);

    MemCh_Free(m);
    return r;
}

status Str_EndMatchTests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *s;
    Str *s2;
    char *match;

    match = ".c";
    s = Str_CstrRef(m, "file1.c");
    r |= Test(Str_EndMatch(s, Str_CstrRef(m, match)), "file ending in '.c' matches successfully, had '_t'", s);

    match = ".cnk";
    s = Str_CstrRef(m, "file1.cnk");
    r |= Test(Str_EndMatch(s, Str_CstrRef(m, match)), "file ending in '.cnk' matches successfully, had '_t'", s);

    match = ".c";
    s = Str_Clone(m, s, s->alloc);
    Str_Trunc(s, -2);
    r |= Test(Str_EndMatch(s, Str_CstrRef(m, match)), "file ending in '.cnk' matches \".c\" after String\\_Trunc successfully, had '_t'", s);
    /*

    match = "bork!";
    s = Str_CstrRef(m, "Super long sentance that spans more than a single chunk, but ends in a very special word and the word is so amazing it's like super duper, amazingly amazing, like the most amazing-ness waste of a long sentence that could have been short, but oh well, we have to test longs tuff sometimes so here it is: bork!");
r |= Test(Str_EndMatch(s, Str_CstrRef(m, match)), "file ending in 'bork!' matches successfully, had '_t'", s);
*/

    MemCh_Free(m);

    return r;
}
