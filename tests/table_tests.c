#include <external.h>
#include <caneka.h>
#include <testsuite.h>

char *values[] = {
    "Alpha", "Apples",
    "Bravo", "Bandits",
    "Charlie", "Carrots",
    "Delta", "DeadMan",
    "Echo", "Elevator",
    "Foxtrot", "Five Minutes",
    "Golf", "Gophers Are Cool",
    "Hotel", "Happy Go Lucky",
    "India", "Idio-syncratic",
    "Juliet", "Jockey Rider",
    "Kilo", "Kangaroo",
    "Lima", "Lefties",
    NULL,
    NULL,
};

status Table_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    Span *tbl = Span_Make(m);
    status r = READY;
    String *s;
    String *value;
    String *found;

    for(int i = 0; ; i+= 2){
        if(values[i] == NULL){
            break;
        }
        s = String_Make(m, bytes(values[i]));
        value = String_Make(m, bytes(values[i+1]));
        Table_Set(tbl, (Abstract *)s, (Abstract *)value);
    }

    Debug_Print((void *)tbl, 0, "TAble: ", COLOR_BLUE, TRUE);

    for(int i = 0; ; i+= 2){
        if(values[i] == NULL){
            break;
        }
        s = String_Make(m, bytes(values[i]));
        value = String_Make(m, bytes(values[i+1]));
        found = (String *)Table_Get(tbl, (Abstract *)s);
        r |= Test(found != NULL, 
            "Expect strings to not be NULL from key:%s", (char *)(s->bytes));
        r |= Test(String_Equals(value, found), 
            "Expect strings to equal %s from key:%s found %s", (char *)(value->bytes), (char *)(s->bytes),
            found != NULL ? (char *)(found->bytes) : "NULL");
    }

    MemCtx_Free(m);
    return r;
}
