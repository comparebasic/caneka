#include <external.h>
#include <caneka.h>

static char *statusCstr(word status){
    if(status == NOT_STARTED){
        return "not started";
    } else if(status == PREVIOUSLY_WORKING){
        return "previously working";
    } else if(status == PARTIAL_FEATURE){
        return "partial feature";
    } else if(status == FEATURE_COMPLETE){
        return "feature complete";
    } else if(status == PRE_PRODUCTION){
        return "pre production";
    } else if(status == PRODUCTION){
        return "production";
    } else if(status == PERMANCE_TESTED){
        return "performance tuned";
    }else{
        return "unknown";
    }
}

status Test(boolean condition, char *fmt, Abstract *args[]){
    if(!condition){
        Out("^r.Fail: ", NULL);
    }else{
        Out("^g.Pass: ", NULL);
    }
    Fmt(OutStream, fmt, args);
    if(!condition){
        Stream_Bytes(OutStream, (byte *)"\n", 1);
    }else{
        /*
        Stream_Bytes(OutStream, (byte *)"\x1b[2K\r", 5);
        */
        Stream_Bytes(OutStream, (byte *)"\n", 1);
    }
    return condition ? SUCCESS : ERROR;
}

status Test_Runner(MemCh *gm, char *suiteName, TestSet *tests){
    MemCh *m = MemCh_Make();
    DebugStack_Push(suiteName, TYPE_CSTR); 
    TestSet *set = tests;
    char *name = NULL;
    i32 pass = 0;
    i32 fail = 0;

    word baseStackLevel = m->type.range;
    StrVec *v = StrVec_Make(m);
    i64 baseMem = MemCount(0);
    i64 rollingBaseMem = baseMem;

    i64 chapters = MemChapterCount();
    Abstract *args1[] = { (Abstract *)Str_CstrRef(gm, suiteName), NULL };
    Out("= Test Suite: _c\n", args1);
    i64 chap = chapters*PAGE_SIZE;
    Abstract *args2[] = {
        (Abstract *)Str_MemCount(m, baseMem),
        (Abstract *)I64_Wrapped(gm, chap),
        (Abstract *)I64_Wrapped(gm, chapters),
        NULL
    };
    Out("Starting Mem at $ ($ chapters/$ total+stack)\n", args2);

    while(set->name != NULL){
        Abstract *args3[] = {
            (Abstract *)Str_CstrRef(gm, set->name),
            NULL
        };
        if(set->status == SECTION_LABEL){
            Out("== $\n", args3);
            set++;
            continue;
        }

        Out("=== Testing: $\n", args3);

        status r = READY;
        DebugStack_SetRef(set->name, TYPE_CSTR);
        if(set->func != NULL){
            m->type.range++;
            _debugM->type.range++;
            r = set->func(m);
            MemCh_Free(m);
            MemCh_Free(_debugM);
            _debugM->type.range--;
            m->type.range--;

            word stackLevel = m->type.range-baseStackLevel;
            m->type.range++;
            i64 memUsed = MemCount(0)-baseMem;
            i64 overRollingUsed = MemCount(0)-rollingBaseMem;
            rollingBaseMem = MemCount(0);

            char *htmlCls = "";
            Str *color = Str_CstrRef(m, "");
            if(overRollingUsed > 0 || stackLevel > 0){
                color = Str_AnsiCstr(m, "b.");
            }else if(memUsed > 0){
                color = Str_AnsiCstr(m, "c.");
            }else{
                color = Str_AnsiCstr(m, "c.");
            }

            i64 chapters = MemChapterCount();
            i64 availableCh = MemAvailableChapterCount();
            Abstract *args4[] = {
                (Abstract *)color,
                (Abstract *)Str_MemCount(gm, overRollingUsed),
                (Abstract *)I64_Wrapped(gm, chapters),
                (Abstract *)I64_Wrapped(gm, availableCh),
                (Abstract *)Str_MemCount(m, chapters*PAGE_SIZE),
                NULL
            }; 
            Out("$Mem: $ ($ chapters/$ available/$ total+stack)^0\n", args4);

            MemCh_Free(m);
            m->type.range--;
            if((r & ERROR) != 0 || (r & SUCCESS) == 0){
                fail++;
                break;
            }else{
                pass++;
            }
        }

        set++;
    }
    Abstract *args5[] = {
        (Abstract *)Str_CstrRef(gm, suiteName),
        (Abstract *)I32_Wrapped(gm, pass),
        (Abstract *)I32_Wrapped(gm, fail),
        NULL,
    };
    if(!fail){
        Out("^g", NULL);
    }else{
        Out("^r", NULL);
    }
    Out("Suite $ pass:$ fail:$^0\n", args5);

    DebugStack_Pop();
    return !fail ? SUCCESS : ERROR;
}
