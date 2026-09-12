#include <external.h>
#include <caneka.h>
#include <test_module.h>

static status hKeyExpected(MemCh *m, HKey *hk, i32 expected[]){
    status r = READY;
    void *args[5];
    for(i32 i = 0; i < 12; i++){
        Table_HKeyVal(hk);
        args[0] = I8_Wrapped(m, hk->dim);
        args[1] = I32_Wrapped(m, expected[i]);
        args[2] = I32_Wrapped(m, hk->idx);
        args[3] = hk;
        args[4] = NULL;
        r |= Test(hk->idx == expected[i],
            "Test idx mismatch for dim $ expected $, have $, of &",
            args);
    }

    return r;
}

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

status TableHKey_Tests(MemCh *m){
    status r = READY;
    void *args[5]; 

    Str *k = K(m, "small");
    util parity = Parity_From(k);

    HKey hk;
    Table_HKeyInit(&hk, 1, parity);
    i32 expected[] = {117,109,97,108,5,7,13,6,7,8,9,10};
    r |= hKeyExpected(m, &hk, expected);

    args[0] = k;
    args[1] = Str_Ref(m,
        (byte *)&parity, sizeof(util), sizeof(util), STRING_BINARY|MORE|DEBUG);
    args[2] = NULL;
    r |= Test((r & (SUCCESS|ERROR)) == SUCCESS, 
        "Small key  $, had expected hkeys, bits @^0", args);

    k = K(m, "tiny");
    parity = Parity_From(k);

    Table_HKeyInit(&hk, 1, parity);
    i32 expected2[] = {116,105,110,121,4,7,9,6,7,8,9,10};
    r |= hKeyExpected(m, &hk, expected2);

    args[0] = k;
    args[1] = Str_Ref(m,
        (byte *)&parity, sizeof(util), sizeof(util), STRING_BINARY|MORE|DEBUG);
    args[2] = NULL;
    r |= Test((r & (SUCCESS|ERROR)) == SUCCESS, 
        "Small key  $, had expected hkeys, bits @^0", args);

    k = K(m, "strange");
    parity = Parity_From(k);

    Table_HKeyInit(&hk, 1, parity);
    i32 expected3[] = {119,116,114,97,7,7,4,7,8,9,10,11};
    r |= hKeyExpected(m, &hk, expected3);

    args[0] = k;
    args[1] = Str_Ref(m,
        (byte *)&parity, sizeof(util), sizeof(util), STRING_BINARY|MORE|DEBUG);
    args[2] = NULL;
    r |= Test((r & (SUCCESS|ERROR)) == SUCCESS, 
        "Small key  $, had expected hkeys, bits @^0", args);

    return r;
}

status Table_Tests(MemCh *m){
    Span *tbl = Span_Make(m);
    status r = READY;
    Str *s;
    Str *value;
    Str *found;

    for(int i = 0; ; i+= 2){
        if(values[i] == NULL){
            break;
        }
        s = Str_CstrRef(m, values[i]);
        value = Str_CstrRef(m, values[i+1]);
        Table_Set(tbl, s, value);
    }


    for(int i = 0; ; i+= 2){
        if(values[i] == NULL){
            break;
        }
        s = Str_CstrRef(m, values[i]);
        value = Str_CstrRef(m, values[i+1]);
        found = (Str *)Table_Get(tbl, s);
        void *args1[] = {
            s, NULL
        };
        r |= Test(found != NULL, 
            "Expect strings to not be NULL from key:@", args1);
        void *args2[] = {
            value,
            s, 
            found,
            NULL
        };
        r |= Test(Equals(value, found), 
            "Expect strings to equal @ from key:@ found @", args2);
    }

    return r;
}

status TableResize_Tests(MemCh *m){
    Span *tbl = Span_Make(m);
    status r = READY;
    Str *s;
    Str *value;
    Str *found;

    for(i32 i = 0; valuesResize[i] != NULL; i+= 2){
        s = Str_CstrRef(m, valuesResize[i]);
        value = Str_CstrRef(m, valuesResize[i+1]);
        Table_Set(tbl, s, value);
    }

    for(i32 i = 0; valuesResize[i] != NULL; i+= 2){
        s = Str_CstrRef(m, valuesResize[i]);
        value = Str_CstrRef(m, valuesResize[i+1]);
        found = (Str *)Table_Get(tbl, s);
        void *args1[] = {
            s,
            NULL
        };
        r |= Test(found != NULL, "Expect strings to not be NULL from key:$", args1);
        void *args2[] = {
            value,
            s,
            found,
            NULL
        };
        r |= Test(Equals(value, found), 
            "Expect strings to equal $ from key:@ found @", args2);
    }

    return r;
}

status TableUtilKey_Tests(MemCh *m){
    status r = READY;
    void *args[5];
    Table *tbl = Table_Make(m);

    Single *v = NULL;
    v = Util_Wrapped(m, 34961084002);
    Table_Set(tbl, v, v);
    v = Util_Wrapped(m, 34961084034);
    Table_Set(tbl, v, v);
    v = Util_Wrapped(m, 34961084044);
    Table_Set(tbl, v, v);
    v = Util_Wrapped(m, 34961084060);
    Table_Set(tbl, v, v);
    v = Util_Wrapped(m, 34961084068);
    Table_Set(tbl, v, v);
    v = Util_Wrapped(m, 34961084084);
    Table_Set(tbl, v, v);
    v = Util_Wrapped(m, 34961084092);
    Table_Set(tbl, v, v);
    v = Util_Wrapped(m, 34961084108);
    Table_Set(tbl, v, v);
    v = Util_Wrapped(m, 34961084240);
    Table_Set(tbl, v, v);
    v = Util_Wrapped(m, 34961084272);
    Table_Set(tbl, v, v);

    args[0] = Util_Wrapped(m, tbl->nvalues);
    args[1] = tbl;
    args[2] = NULL;
    r |= TestShow(tbl->nvalues == 10, 
        "Expected 10 items to be added", 
        "Incorrect count, expected 10 have $ instead for @", 
        args);

    return r;
}
