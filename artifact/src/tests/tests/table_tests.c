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
    "Hotel", "Happy Go Lucky",
    "India", "Idio-syncratic",
    "Juliet", "Jockey Rider",
    /*
    "Kilo", "Kangaroo",
    "Lima", "Lefties",
    */
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

status Table_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    Span *tbl = Span_Make(m, TYPE_TABLE);
    status r = SUCCESS;
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
        if(DEBUG_TABLE){
            Debug_Print((void *)s, 0, "Setting: ", DEBUG_TABLE, TRUE);
            printf("\n");
            Debug_Print((void *)tbl, 0, "In Table: ", DEBUG_TABLE, TRUE);
            printf("\n");
        }
    }


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

status TableResize_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    Span *tbl = Span_Make(m, TYPE_TABLE);
    status r = SUCCESS;
    String *s;
    String *value;
    String *found;

    for(int i = 0; ; i+= 2){
        if(valuesResize[i] == NULL){
            break;
        }
        s = String_Make(m, bytes(valuesResize[i]));
        value = String_Make(m, bytes(valuesResize[i+1]));
        Table_Set(tbl, (Abstract *)s, (Abstract *)value);
        if(DEBUG_TABLE){
            Debug_Print((void *)s, 0, "Setting: ", DEBUG_TABLE, TRUE);
            printf("\n");
            Debug_Print((void *)tbl, 0, "In Table: ", DEBUG_TABLE, TRUE);
            printf("\n");
        }
    }


    for(int i = 0; ; i+= 2){
        if(valuesResize[i] == NULL){
            break;
        }
        s = String_Make(m, bytes(valuesResize[i]));
        value = String_Make(m, bytes(valuesResize[i+1]));
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

status TablePreKey_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    Span *tbl = Span_Make(m, TYPE_TABLE);
    status r = SUCCESS;
    String *s;
    String *value;
    String *found;

    s = String_Make(m, bytes("PreKey"));
    value = String_Make(m, bytes("After Value"));
    Table_SetKey(tbl, (Abstract *)s);
    Table_SetValue(tbl, (Abstract *)value);
    found = (String *)Table_Get(tbl, (Abstract *)s);
    r |= Test(found != NULL, 
        "Expect SetKey and SetValue to effect the same entry: strings to not be NULL from key:'%s'", (char *)(s->bytes));
    r |= Test(String_Equals(value, found), 
        "Expect SetKey and SetValue to effect the same entry: strings to equal '%s' from key:'%s' found '%s'", (char *)(value->bytes), (char *)(s->bytes),
        found != NULL ? (char *)(found->bytes) : "NULL");


    tbl = Span_Make(m, TYPE_TABLE);
    for(int i = 0; ; i+= 2){
        if(values2[i] == NULL){
            break;
        }
        s = String_Make(m, bytes(values2[i]));
        value = String_Make(m, bytes(values2[i+1]));
        Table_Set(tbl, (Abstract *)s, (Abstract *)value);
    }

    MemCtx_Free(m);
    return r;
}
