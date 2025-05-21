#include <external.h>
#include <caneka.h>

status Iter_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();
    Span *p;
    Str *s;
    status r = READY;
    Iter it;


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

    Iter_Setup(&it, p, SPAN_OP_ADD, p->max_idx);
    it.value = arr[0];
    Iter_Query(&it);

    Iter_Setup(&it, p, SPAN_OP_ADD, p->max_idx);
    it.value = arr[1];
    Iter_Query(&it);

    Iter_Setup(&it, p, SPAN_OP_ADD, p->max_idx);
    it.value = arr[2];
    Iter_Query(&it);

    Iter_Setup(&it, p, SPAN_OP_ADD, p->max_idx);
    it.value = arr[3];
    Iter_Query(&it);

    i32 i = 0;
    Iter_Setup(&it, p, SPAN_OP_GET, 0);
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


    Iter_Setup(&it, p, SPAN_OP_ADD|CONTINUE, 3);
    it.value = arr2[3];
    Iter_Query(&it);

    Abstract *args[] = {
        (Abstract *)p,
        NULL
    };
    Debug("^p.@^0\n", args);

    i = 0;
    Iter_Setup(&it, p, SPAN_OP_GET, 0);
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

    Iter_Setup(&it, p, SPAN_OP_ADD|CONTINUE, 2);
    it.value = arr3[2];
    Iter_Query(&it);

    i = 0;
    Iter_Setup(&it, p, SPAN_OP_GET, 0);
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
    Abstract **ptr = arr4;
    while(*ptr != NULL){
        Iter_Setup(&it, p, SPAN_OP_ADD, p->max_idx);
        it.value = *ptr;
        Iter_Query(&it);
        ptr++;
    }

    /*
    Abstract *args1[] = {
        (Abstract *)p,
        NULL
    };
    Debug("^p.After populated 18: @^0\n", args1);
    */

    Iter_Setup(&it, p, SPAN_OP_ADD|CONTINUE, 6);
    it.value = arr5[6];
    Iter_Query(&it);

    Abstract *args2[] = {
        (Abstract *)p,
        NULL
    };
    Debug("^p.after multi-level insert: p.@^0\n", args2);

    i = 0;
    Iter_Setup(&it, p, SPAN_OP_GET, 0);
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
    DebugStack_Pop();
    return r;
}
