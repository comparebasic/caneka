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

status TestShow(boolean condition, char *fmtSuccess, char *fmtError, Abstract *args[]){
    if(!condition){
        Out("^r.Fail: ", NULL);
    }else{
        Out("^g.Pass: ", NULL);
    }
    if(!condition){
        Fmt(OutStream, fmtError, args);
        Stream_Bytes(OutStream, (byte *)"\n", 1);
    }else{
        /*
        Stream_Bytes(OutStream, (byte *)"\x1b[2K\r", 5);
        */
        Fmt(OutStream, fmtSuccess, args);
        Stream_Bytes(OutStream, (byte *)"\n", 1);
    }
    Out("^0", NULL);
    return condition ? SUCCESS : ERROR;
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
    Out("^0", NULL);
    return condition ? SUCCESS : ERROR;
}

status Test_Runner(MemCh *gm, char *suiteName, TestSet *tests){
    MemCh *m = MemCh_Make();
    DebugStack_Push(suiteName, TYPE_CSTR); 
    TestSet *set = tests;
    char *name = NULL;
    i32 pass = 0;
    i32 fail = 0;

    word baseStackLevel = m->level;
    StrVec *v = StrVec_Make(m);

    MemBookStats st;
    MemBook_GetStats(gm, &st);
    i32 baseMem = st.total;
    i32 rollingBaseMem = baseMem;

    Abstract *args1[] = { (Abstract *)Str_CstrRef(gm, suiteName), NULL };
    Out("= Test Suite: _c\n", args1);
    Abstract *args2[] = {
        (Abstract *)Str_MemCount(m, st.total * PAGE_SIZE),
        (Abstract *)I64_Wrapped(gm, st.total),
        (Abstract *)I64_Wrapped(gm, st.pageIdx),
        NULL
    };
    Out("Starting Mem at $ total/maxIdx=^D.$/$^d.\n", args2);

    while(set->name != NULL){
        Abstract *args3[] = {
            (Abstract *)Str_CstrRef(gm, set->name),
            NULL
        };
        if(set->status == SECTION_LABEL){
            Out("^E.== $  ^e.\n", args3);
            set++;
            continue;
        }

        Out("=== Testing: $\n", args3);

        status r = READY;
        DebugStack_SetRef(set->name, TYPE_CSTR);
        if(set->func != NULL){
            m->level++;
            _debugM->level++;
            r = set->func(m);
            MemCh_Free(m);
            MemCh_Free(_debugM);
            _debugM->level--;
            m->level--;

            word stackLevel = m->level-baseStackLevel;
            m->level++;

            MemBook_GetStats(gm, &st);
            i64 memUsed = st.total - baseMem;
            i64 overRollingUsed = st.total-rollingBaseMem;
            rollingBaseMem = st.total;

            char *htmlCls = "";
            Str *color = Str_CstrRef(m, "");
            if(overRollingUsed > 0 || stackLevel > 0){
                color = Str_AnsiCstr(m, "b.");
            }else if(memUsed > 0){
                color = Str_AnsiCstr(m, "c.");
            }else{
                color = Str_AnsiCstr(m, "c.");
            }

            Abstract *args2[] = {
                (Abstract *)color,
                (Abstract *)I64_Wrapped(m, st.total * PAGE_SIZE),
                (Abstract *)I64_Wrapped(gm, st.total),
                (Abstract *)I64_Wrapped(gm, st.pageIdx),
                NULL
            };
            Out("$Mem: $ total/maxIdx=^D.$/$^d.^0\n", args2);


            MemCh_Free(m);
            m->level--;
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
