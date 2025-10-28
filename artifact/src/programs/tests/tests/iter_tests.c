#include <external.h>
#include <caneka.h>

static status makeAndIterItems(MemCh *m, i64 max){
    Abstract *args[2];
    status r = READY;
    m->level++;
    Span *p = Span_Make(m);
    for(i64 i = 0; i < max; i++){
        Span_Set(p, i, (Abstract *)I64_Wrapped(m, i));
    }

    Iter it;
    Iter_Init(&it, p);
    i32 i = 0;
    while((Iter_Next(&it) & END) == 0){
        Single *sg = (Single *)Iter_Get(&it);
        if(sg->val.i != i){
            r |= ERROR;
            break;
        }
        i++;
    }

    args[0] = (Abstract *)I64_Wrapped(m, max);
    args[1] = NULL;
    r |= Test((r & ERROR) == 0, "Adding and Iter_Next over $ items", args);
    MemCh_Free(m);
    m->level--;
    return r; 
}

static status makeAndIterPrevRemoveItems(MemCh *m, i64 max){
    Abstract *args[2];
    status r = READY;
    m->level++;
    Span *p = Span_Make(m);
    for(i64 i = 0; i < max; i++){
        Span_Set(p, i, (Abstract *)I64_Wrapped(m, i));
    }

    Iter it;
    Iter_Init(&it, p);
    i32 maxIdx = p->max_idx;
    while((Iter_PrevRemove(&it) & END) == 0){
        Single *sg = (Single *)Iter_Current(&it);
        if(sg->val.i != maxIdx || p->nvalues != (maxIdx+1)){
            r |= ERROR;
            break;
        }
        maxIdx--;
    }

    args[0] = (Abstract *)I64_Wrapped(m, max);
    args[1] = NULL;
    r |= Test((r & ERROR) == 0, "Adding and Iter_Next over $ items", args);
    MemCh_Free(m);
    m->level--;
    return r; 
}


status Iter_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();
    Span *p;
    Str *s;
    status r = READY;
    Iter it;

    m->level++;

    p = Span_Make(m);

    Abstract *arr[6] = {
        (Abstract *)Str_CstrRef(m, "FancyOne"),
        (Abstract *)Str_CstrRef(m, "FancyTwoA"),
        (Abstract *)Str_CstrRef(m, "FancyThreeA"),
        (Abstract *)Str_CstrRef(m, "FancyFour"),
        NULL
    };

    Abstract *arr2[6] = {
        (Abstract *)Str_CstrRef(m, "FancyOne"),
        (Abstract *)Str_CstrRef(m, "FancyTwoA"),
        (Abstract *)Str_CstrRef(m, "FancyThreeA"),
        (Abstract *)Str_CstrRef(m, "FancyThreeB"),
        (Abstract *)Str_CstrRef(m, "FancyFour"),
        NULL
    };

    Abstract *arr3[7] = {
        (Abstract *)Str_CstrRef(m, "FancyOne"),
        (Abstract *)Str_CstrRef(m, "FancyTwoA"),
        (Abstract *)Str_CstrRef(m, "FancyTwoB"),
        (Abstract *)Str_CstrRef(m, "FancyThreeA"),
        (Abstract *)Str_CstrRef(m, "FancyThreeB"),
        (Abstract *)Str_CstrRef(m, "FancyFour"),
        NULL
    };

    Iter_Init(&it, p);
    Iter_Add(&it, arr[0]);
    Iter_Add(&it, arr[1]);
    Iter_Add(&it, arr[2]);
    Iter_Add(&it, arr[3]);

    i32 i = 0;
    Iter_Reset(&it);
    while((Iter_Next(&it) & END) == 0){
        Abstract *args[] = {
            (Abstract *)arr[i],
            it.value,
            NULL
        };
        r |= Test(Equals(arr[i], it.value), 
            "Iter_Next first set of values gives expected, expected @, have @", args);
        i++;
    }

    MemCh_Free(m);

    Abstract *arr6[20] = {
        (Abstract *)Str_CstrRef(m, "FancyOne"),
        (Abstract *)Str_CstrRef(m, "FancyTwoA"),
        (Abstract *)Str_CstrRef(m, "FancyThreeA"),
        (Abstract *)Str_CstrRef(m, "FancyFour"),
        (Abstract *)Str_CstrRef(m, "FancyFive"),
        (Abstract *)Str_CstrRef(m, "FancySix"),
        (Abstract *)Str_CstrRef(m, "FancySixTwo"),
        (Abstract *)Str_CstrRef(m, "FancySeven"),
        (Abstract *)Str_CstrRef(m, "FancyEight"),
        (Abstract *)Str_CstrRef(m, "FancyNine"),
        (Abstract *)Str_CstrRef(m, "FancyTen"),
        (Abstract *)Str_CstrRef(m, "FancyEleven"),
        (Abstract *)Str_CstrRef(m, "FancyTwelve"),
        (Abstract *)Str_CstrRef(m, "FancyThirteen"),
        (Abstract *)Str_CstrRef(m, "FancyFourteen"),
        (Abstract *)Str_CstrRef(m, "FancyFifteen"),
        (Abstract *)Str_CstrRef(m, "FancySixteen"),
        (Abstract *)Str_CstrRef(m, "FancySeventeen"),
        (Abstract *)Str_CstrRef(m, "FancyEighteen"),
        NULL
    };

    Abstract **item = NULL;
    p = Span_Make(m);
    Iter_Init(&it, p);
    Abstract **ptr = arr6;
    Abstract *first = *ptr;
    while(*ptr != NULL){
        item = ptr;
        Iter_Add(&it, *item);
        ptr++;
    }

    Iter_Reset(&it);

    while((Iter_Prev(&it) & END) == 0){
        Abstract *found = Iter_Get(&it);
        Abstract *args[] = {
            (Abstract *)*item,
            (Abstract *)found,
            NULL
        };

        r |= Test(Equals(*item, found), 
            "Iter_Prev set of values gives expected, expected @, have @", args);
        if((it.type.state & LAST) == 0){
            item--;
        }
    }

    Abstract *args[] = {
        (Abstract *)*item,
        (Abstract *)first,
        NULL
    };

    r |= Test(Equals(*item, first), 
        "Iter_Prev ends at first value, expected @, have @", args);

    m->level--;
    MemCh_Free(m);
    DebugStack_Pop();

    return r;
}

status IterMax_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();
    Abstract *args[5];

    Span *p = Span_Make(m);
    Span_Add(p, (Abstract *)Str_CstrRef(m, "one"));
    Span_Add(p, (Abstract *)Str_CstrRef(m, "two"));
    Span_Add(p, (Abstract *)Str_CstrRef(m, "three"));
    Span_Add(p, (Abstract *)Str_CstrRef(m, "four"));
    Span_Add(p, (Abstract *)Str_CstrRef(m, "five"));
    Span_Add(p, (Abstract *)Str_CstrRef(m, "six"));
    Span_Add(p, (Abstract *)Str_CstrRef(m, "seven"));
    Span_Add(p, (Abstract *)Str_CstrRef(m, "eight"));
    Span_Add(p, (Abstract *)Str_CstrRef(m, "nine"));
    Span_Add(p, (Abstract *)Str_CstrRef(m, "ten"));
    Span_Add(p, (Abstract *)Str_CstrRef(m, "eleven"));
    Span_Add(p, (Abstract *)Str_CstrRef(m, "twelve"));
    Span_Add(p, (Abstract *)Str_CstrRef(m, "thirteen"));
    Span_Add(p, (Abstract *)Str_CstrRef(m, "fourteen"));
    Span_Add(p, (Abstract *)Str_CstrRef(m, "fifteen"));
    Span_Add(p, (Abstract *)Str_CstrRef(m, "sixteen"));
    Span_Add(p, (Abstract *)Str_CstrRef(m, "seventeen"));

    Iter it;
    Iter_Init(&it, p);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);;
    args[1] = NULL;
    r |= Test(p->nvalues == 17, "Span has 17 values to start, have $", args);

    Iter_PrevRemove(&it);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);;
    args[1] = (Abstract *)Iter_Get(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 17 && Equals(Iter_Get(&it), (Abstract *)Str_CstrRef(m, "seventeen")), "Span has 17 values to start, have $, item: $", args);
    Iter_PrevRemove(&it);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);;
    args[1] = (Abstract *)Iter_Get(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 16 && Equals(Iter_Get(&it), (Abstract *)Str_CstrRef(m, "sixteen")), "Span has 16 values to start, have $, item: $", args);
    Iter_PrevRemove(&it);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);;
    args[1] = (Abstract *)Iter_Get(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 15 && Equals(Iter_Get(&it), (Abstract *)Str_CstrRef(m, "fifteen")), "Span has 15 values to start, have $, item: $", args);
    Iter_PrevRemove(&it);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);;
    args[1] = (Abstract *)Iter_Get(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 14 && Equals(Iter_Get(&it), (Abstract *)Str_CstrRef(m, "fourteen")), "Span has 15 values to start, have $, item: $", args);
    Iter_PrevRemove(&it);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);;
    args[1] = (Abstract *)Iter_Get(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 13 && Equals(Iter_Get(&it), (Abstract *)Str_CstrRef(m, "thirteen")), "Span has 13 values to start, have $, item: $", args);
    Iter_PrevRemove(&it);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);;
    args[1] = (Abstract *)Iter_Get(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 12 && Equals(Iter_Get(&it), (Abstract *)Str_CstrRef(m, "twelve")), "Span has 12 values to start, have $, item: $", args);
    Iter_PrevRemove(&it);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);;
    args[1] = (Abstract *)Iter_Get(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 11 && Equals(Iter_Get(&it), (Abstract *)Str_CstrRef(m, "eleven")), "Span has 11 values to start, have $, item: $", args);
    Iter_PrevRemove(&it);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);;
    args[1] = (Abstract *)Iter_Get(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 10 && Equals(Iter_Get(&it), (Abstract *)Str_CstrRef(m, "ten")), "Span has 10 values to start, have $, item: $", args);
    Iter_PrevRemove(&it);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);;
    args[1] = (Abstract *)Iter_Get(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 9 && Equals(Iter_Get(&it), (Abstract *)Str_CstrRef(m, "nine")), "Span has 9 values to start, have $, item: $", args);
    Iter_PrevRemove(&it);
    args[0] = (Abstract *)Iter_Get(&it);
    args[1] = NULL;
    r |= Test(p->nvalues == 8 && Equals(Iter_Get(&it), (Abstract *)Str_CstrRef(m, "eight")), "Span has 8 values to start, have $, item: $", args);
    Iter_PrevRemove(&it);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);;
    args[1] = (Abstract *)Iter_Get(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 7 && Equals(Iter_Get(&it), (Abstract *)Str_CstrRef(m, "seven")), "Span has 7 values to start, have $, item: $", args);
    Iter_PrevRemove(&it);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);;
    args[1] = (Abstract *)Iter_Get(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 6 && Equals(Iter_Get(&it), (Abstract *)Str_CstrRef(m, "six")), "Span has 6 values to start, have $, item: $", args);
    Iter_PrevRemove(&it);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);;
    args[1] = (Abstract *)Iter_Get(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 5 && Equals(Iter_Get(&it), (Abstract *)Str_CstrRef(m, "five")), "Span has 5 values to start, have $, item: $", args);
    Iter_PrevRemove(&it);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);;
    args[1] = (Abstract *)Iter_Get(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 4 && Equals(Iter_Get(&it), (Abstract *)Str_CstrRef(m, "four")), "Span has 4 values to start, have $, item: $", args);
    Iter_PrevRemove(&it);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);;
    args[1] = (Abstract *)Iter_Get(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 3 && Equals(Iter_Get(&it), (Abstract *)Str_CstrRef(m, "three")), "Span has 3 values to start, have $, item: $", args);
    Iter_PrevRemove(&it);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);;
    args[1] = (Abstract *)Iter_Get(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 2 && Equals(Iter_Get(&it), (Abstract *)Str_CstrRef(m, "two")), "Span has 2 values to start, have $, item: $", args);
    Iter_PrevRemove(&it);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);;
    args[1] = (Abstract *)Iter_Get(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 1 && Equals(Iter_Get(&it), (Abstract *)Str_CstrRef(m, "one")), "Span has 1 values to start, have $, item: $", args);
    Iter_PrevRemove(&it);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);;
    args[1] = (Abstract *)Iter_Get(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 0 && Iter_Get(&it) == NULL && (it.type.state & END) != 0, "Span is empty and has END flag, have $, item: &", args);

    p = Span_Make(m);
    Iter_Init(&it, p);

    it.type.state |= DEBUG;

    r |= Test(p->nvalues == 0, "Fresh Span has 0 nvalues", NULL);
    Iter_Add(&it, (Abstract *)Str_CstrRef(m, "one"));
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);
    args[1] = (Abstract *)Iter_Current(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 1 && 
        Equals(Iter_Get(&it), (Abstract *)Str_CstrRef(m, "one")), 
        "Span has 1 values to after add, have $, item: &", args);

    Iter_PrevRemove(&it);
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);
    args[1] = (Abstract *)Iter_Current(&it);
    args[2] = NULL;

    r |= Test(p->nvalues == 1 && 
        Equals(Iter_Current(&it), (Abstract *)Str_CstrRef(m, "one")), 
        "Span has 1 values to after first remove, have $, item: &", args);

    Iter_PrevRemove(&it);
    Iter_Add(&it, (Abstract *)Str_CstrRef(m, "two"));
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);
    args[1] = (Abstract *)Iter_Current(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 1 &&
        Equals(Iter_Current(&it), (Abstract *)Str_CstrRef(m, "two")), 
        "Span has 1 values to after PrevRemove/Add, have $, item: &", args);

    Iter_PrevRemove(&it);
    Iter_Add(&it, (Abstract *)Str_CstrRef(m, "three"));
    args[0] = (Abstract *)I32_Wrapped(m, p->nvalues);
    args[1] = (Abstract *)Iter_Current(&it);
    args[2] = NULL;
    r |= Test(p->nvalues == 1 &&
        Equals(Iter_Current(&it), (Abstract *)Str_CstrRef(m, "three")), 
        "Span has 1 values to after PrevRemove/Add, have $, item: &", args);

    /*
    r |= makeAndIterItems(m, 10);
    r |= makeAndIterItems(m, 300);
    r |= makeAndIterItems(m, 5000);
    r |= makeAndIterItems(m, 63000);
    */
    MemCh_Free(m);
    return r;
}

status IterPrevRemove_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();
    r |= makeAndIterPrevRemoveItems(m, 10);
    r |= makeAndIterPrevRemoveItems(m, 300);
    r |= makeAndIterPrevRemoveItems(m, 5000);
    r |= makeAndIterPrevRemoveItems(m, 63000);
    MemCh_Free(m);
    return r;
}
