#include <external.h>
#include <caneka.h>
#include <testsuite.h>

status Table_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    Span *tbl = Span_Make(m);
    tbl->eq = String_Eq;
    status r = READY;
    String *s;
    String *value;
    String *found;

    s = String_Make(m, bytes("Alpha"));
    value = String_Make(m, bytes("Apples"));
    Table_Set(tbl, (Abstract *)s, (Abstract *)value);

    s = String_Make(m, bytes("Bravo"));
    value = String_Make(m, bytes("Bandits"));
    Table_Set(tbl, (Abstract *)s, (Abstract *)value);

    s = String_Make(m, bytes("Charlie"));
    value = String_Make(m, bytes("Carrots"));
    Table_Set(tbl, (Abstract *)s, (Abstract *)value);

    s = String_Make(m, bytes("Delta"));
    value = String_Make(m, bytes("DeadMan"));
    Table_Set(tbl, (Abstract *)s, (Abstract *)value);

    s = String_Make(m, bytes("Echo"));
    value = String_Make(m, bytes("Elevator"));
    Table_Set(tbl, (Abstract *)s, (Abstract *)value);

    s = String_Make(m, bytes("Foxtrot"));
    value = String_Make(m, bytes("Five Minutes"));
    Table_Set(tbl, (Abstract *)s, (Abstract *)value);

    s = String_Make(m, bytes("Golf"));
    value = String_Make(m, bytes("Gophers Are Cool"));
    Table_Set(tbl, (Abstract *)s, (Abstract *)value);

    s = String_Make(m, bytes("Hotel"));
    value = String_Make(m, bytes("Happy Go Lucky"));
    Table_Set(tbl, (Abstract *)s, (Abstract *)value);

    s = String_Make(m, bytes("India"));
    value = String_Make(m, bytes("Idio-syncratic"));
    Table_Set(tbl, (Abstract *)s, (Abstract *)value);

    s = String_Make(m, bytes("Juliet"));
    value = String_Make(m, bytes("Jockey Rider"));
    Table_Set(tbl, (Abstract *)s, (Abstract *)value);

    s = String_Make(m, bytes("Kilo"));
    value = String_Make(m, bytes("Kangaroo"));
    Table_Set(tbl, (Abstract *)s, (Abstract *)value);

    s = String_Make(m, bytes("Lima"));
    value = String_Make(m, bytes("Lefties"));
    Table_Set(tbl, (Abstract *)s, (Abstract *)value);

    Debug_Print((void *)tbl, 0, "TAble: ", COLOR_BLUE, TRUE);

    s = String_Make(m, bytes("Alpha"));
    value = String_Make(m, bytes("Apples"));
    found = (String *)Table_Get(tbl, (Abstract *)s);
    Debug_Print((void *)found, 0, "found: ", COLOR_BLUE, TRUE);
    printf("\n");

    /*
    r |= Test(String_Equals(value, found), 
        "Expect strings to equal %s from key:%s found %s", (char *)(s->bytes), (char *)(value->bytes),
        found != NULL ? (char *)(found->bytes) : "NULL");
        */

    MemCtx_Free(m);
    return r;
}
