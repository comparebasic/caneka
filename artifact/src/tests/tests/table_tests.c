#include <external.h>
#include <caneka.h>

char *values[] = {
    "Alpha", "Apples",
    "Bravo", "Bandits",
    "Charlie", "Carrots",
    "Delta", "DeadMan",
    "Echo", "Elevator",
    "Foxtrot", "Five Minutes",
    "Golf", "Gophers Are Cool",
    NULL,
    NULL,
};

char *valuesResize[] = {
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
    "Mike", "Mangoes",
    "November", "Neighbourhood Villan",
    "Oscar", "Oasis of Gems",
    "Pappa", "Pinapple Ham",
    "Quebec", "Quarterly Report",
    "Romeo", "Rooty Tooty",
    "Sierra", "Stainless Steel",
    NULL,
    NULL,
};

char *values2[] = {
    "Apples", "Mmmm pie in the making",
    "Bananas", "Don't slip",
    "Carrots", "And peas and potatoes and corn.",
    "Dandilions", "Summer-summer-summer-Time!",
    NULL, NULL,
};

status Table_Tests(MemCh *gm){
    MemCh *m = MemCh_Make();
    Span *tbl = Span_Make(m);
    status r = SUCCESS;
    Str *s;
    Str *value;
    Str *found;

    for(int i = 0; ; i+= 2){
        if(values[i] == NULL){
            break;
        }
        s = Str_CstrRef(m, values[i]);
        value = Str_CstrRef(m, values[i+1]);
        Table_Set(tbl, (Abstract *)s, (Abstract *)value);
    }


    for(int i = 0; ; i+= 2){
        if(values[i] == NULL){
            break;
        }
        s = Str_CstrRef(m, values[i]);
        value = Str_CstrRef(m, values[i+1]);
        found = (Str *)Table_Get(tbl, (Abstract *)s);
        Abstract *args1[] = {
            (Abstract *)s, NULL
        };
        r |= Test(found != NULL, 
            "Expect strings to not be NULL from key:@", args1);
        Abstract *args2[] = {
            (Abstract *)value,
            (Abstract *)s, 
            (Abstract *)found,
            NULL
        };
        r |= Test(Equals((Abstract *)value, (Abstract *)found), 
            "Expect strings to equal @ from key:@ found @", args2);
    }

    MemCh_Free(m);
    return r;
}

status TableResize_Tests(MemCh *gm){
    MemCh *m = MemCh_Make();
    Span *tbl = Span_Make(m);
    status r = SUCCESS;
    Str *s;
    Str *value;
    Str *found;

    for(i32 i = 0; valuesResize[i] != NULL; i+= 2){
        s = Str_CstrRef(m, valuesResize[i]);
        value = Str_CstrRef(m, valuesResize[i+1]);
        Table_Set(tbl, (Abstract *)s, (Abstract *)value);
    }

    for(i32 i = 0; valuesResize[i] != NULL;i+= 2){
        s = Str_CstrRef(m, valuesResize[i]);
        value = Str_CstrRef(m, valuesResize[i+1]);
        found = (Str *)Table_Get(tbl, (Abstract *)s);
        Abstract *args1[] = {
            (Abstract *)s,
            NULL
        };
        r |= Test(found != NULL, "Expect strings to not be NULL from key:$", args1);
        Abstract *args2[] = {
            (Abstract *)value,
            (Abstract *)s,
            (Abstract *)found,
            NULL
        };
        r |= Test(Equals((Abstract *)value, (Abstract *)found), 
            "Expect strings to equal $ from key:@ found @", args2);
    }

    MemCh_Free(m);
    return r;
}

status TablePreKey_Tests(MemCh *gm){
    MemCh *m = MemCh_Make();
    Span *tbl = Span_Make(m);
    status r = READY;
    /*
    Str *s;
    Str *value;
    Str *found;

    s = Str_CstrRef(m, "PreKey");
    value = Str_CstrRef(m, "After Value");
    Table_SetKey(tbl, (Abstract *)s);
    Table_SetValue(tbl, (Abstract *)value);
    found = (Str *)Table_Get(tbl, (Abstract *)s);
    r |= Test(found != NULL, 
        "Expect SetKey and SetValue to effect the same entry: strings to not be NULL from key:'_t", s);
    r |= Test(Equals((Abstract *)value, (Abstract *)found), 
        "Expect SetKey and SetValue to effect the same entry: strings to equal '_t' from key:'_t' found '_t'", value, s, found);


    tbl = Span_Make(m);
    for(int i = 0; ; i+= 2){
        if(values2[i] == NULL){
            break;
        }
        s = Str_CstrRef(m, values2[i]);
        value = Str_CstrRef(m, values2[i+1]);
        Table_Set(tbl, (Abstract *)s, (Abstract *)value);
    }
    */

    MemCh_Free(m);
    return r;
}
