#include <external.h>
#include <caneka.h>

status Iter_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();
    Span *p;
    Str *s;
    status r = READY;
    Iter it;

    m->type.range++;

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

    Iter_Insert(&it, 3, arr2[3]);

    i = 0;
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        Abstract *args[] = {
            (Abstract *)arr2[i],
            it.value,
            NULL
        };
        r |= Test(Equals(arr2[i], it.value), 
            "Iter_Next second set of values gives expected, expected @, have @", args);
        i++;
    }

    Iter_Insert(&it, 2, arr3[2]);

    i = 0;
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        Abstract *args[] = {
            (Abstract *)arr3[i],
            it.value,
            NULL
        };
        r |= Test(Equals(arr3[i], it.value), 
            "Iter_Next third set of values gives expected, expected @, have @", args);
        i++;
    }

    Abstract *arr4[19] = {
        (Abstract *)Str_CstrRef(m, "FancyOne"),
        (Abstract *)Str_CstrRef(m, "FancyTwoA"),
        (Abstract *)Str_CstrRef(m, "FancyThreeA"),
        (Abstract *)Str_CstrRef(m, "FancyFour"),
        (Abstract *)Str_CstrRef(m, "FancyFive"),
        (Abstract *)Str_CstrRef(m, "FancySix"),
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

    Abstract *arr5[20] = {
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

    p = Span_Make(m);
    Iter_Init(&it, p);
    Abstract **ptr = arr4;
    while(*ptr != NULL){
        Iter_Add(&it, *ptr);
        ptr++;
    }

    Iter_Insert(&it, 6, arr5[6]);

    i = 0;
    Iter_Reset(&it);
    while((Iter_Next(&it) & END) == 0){
        Abstract *args[] = {
            (Abstract *)arr5[i],
            it.value,
            NULL
        };
        r |= Test(Equals(arr5[i], it.value), 
            "Iter_Next fourth set of values gives expected, expected @, have @", args);
        i++;
    }

    MemCh_Free(m);

    i64 max = 5128;
    p = Span_Make(m);
    p->type.state |= FLAG_SPAN_RAW;
    Iter_Init(&it, p);
    for(i64 i = 0; i < max; i++){
        Iter_Add(&it, (void *)i);
    }

    Iter_Insert(&it, 177, (void*)((i64)177));

    i64 exp = 0;
    Iter_Reset(&it);
    boolean second = FALSE;
    while((Iter_Next(&it) & END) == 0){
        if(exp != (i64)it.value){
            r |= ERROR;
            Abstract *args[] = {
                (Abstract *)I32_Wrapped(m, it.idx),
                (Abstract *)I64_Wrapped(m, exp),
                (Abstract *)I64_Wrapped(m, (i64)it.value),
                NULL
            };
            r |= Test(FALSE, "at $: expected $ found $", args);
        }else{
            /*
            printf("\x1b[32m%d: %ld vs %ld\x1b[0m,", it.idx, exp, (i64)it.value); 
            */
        }
        if(exp == 177){
            if(second){
                exp++;
            }else{
                second = TRUE;
            }
        }else{
            exp++;
        }
    }

    i32 endExpected = 5128;
    Abstract *args3[] = {
        (Abstract *)I64_Wrapped(m, endExpected),
        (Abstract *)I64_Wrapped(m, exp),
        NULL
    };
    r |= Test(exp == endExpected,
        "177 Was added in the middle of 5k values, ended contigous values expected $, have $", args3);

    m->type.range--;
    MemCh_Free(m);
    DebugStack_Pop();

    return r;
}
