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

status TestShow(boolean condition, char *fmtSuccess, char *fmtError, void *args[]){
    if(!condition){
        Out("^r.- FAIL ", NULL);
    }else{
        Out("^g.- PASS ", NULL);
    }
    if(!condition){
        Fmt(OutStream, fmtError, args);
        Buff_AddBytes(OutStream, (byte *)"\n", 1);
    }else{
        Fmt(OutStream, fmtSuccess, args);
        Buff_AddBytes(OutStream, (byte *)"\n", 1);
    }
    Out("^0", NULL);
    return condition ? SUCCESS : ERROR;
}

status Test(boolean condition, char *fmt, void *args[]){
    if(!condition){
        Out("^r.- FAIL ", NULL);
    }else{
        Out("^g.- PASS ", NULL);
    }
    Fmt(OutStream, fmt, args);
    if(!condition){
        Buff_AddBytes(OutStream, (byte *)"\n", 1);
    }else{
        Buff_AddBytes(OutStream, (byte *)"\n", 1);
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

    void *args[8];

    word baseStackLevel = m->level;
    StrVec *v = StrVec_Make(m);

    MemBookStats st;
    MemBook_GetStats(gm, &st);
    i32 baseMem = st.total;
    i32 rollingBaseMem = baseMem;

    args[0] = Str_CstrRef(gm, suiteName);
    args[1] = NULL;
    Out("= Test Suite: _c\n", args);

    args[0] = Str_MemCount(m, st.total * PAGE_SIZE);
    args[1] = I64_Wrapped(gm, st.total);
    args[2] = I64_Wrapped(gm, st.pageIdx);
    args[3] = I64_Wrapped(gm, PAGE_SIZE);
    args[4] = NULL;

    Out("\nStarting Mem at $ total/maxIdx=^D.$/$^d. page-size=$b\n", args);

    while(set->name != NULL){
        args[0] = Str_CstrRef(gm, set->name);
        if(set->status == SECTION_LABEL){
            Out("^E.== $  ^e.\n", args);
            set++;
            continue;
        }

        Out("=== Testing: $\n\n", args);

        status r = READY;
        DebugStack_SetRef(set->name, TYPE_CSTR);
        if(set->func != NULL){
            m->level++;
            r = set->func(m);
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

            args[0] = color,
            args[1] = Str_MemCount(m, st.total * PAGE_SIZE),
            args[2] = I64_Wrapped(gm, st.total),
            args[3] = I64_Wrapped(gm, st.pageIdx),
            args[4] = I64_Wrapped(gm, PAGE_SIZE),
            args[5] = NULL;
            Out("$\nMem $ total/maxIdx=^D.$/$^d. page-size=$b^0\n\n", args);

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

    args[0] = Str_CstrRef(gm, suiteName),
    args[1] = I32_Wrapped(gm, pass),
    args[2] = I32_Wrapped(gm, fail),
    args[3] = NULL;
    if(!fail){
        Out("^g", NULL);
    }else{
        Out("^r", NULL);
    }
    Out("Suite $ pass:$ fail:$^0\n", args);

    DebugStack_Pop();
    return !fail ? SUCCESS : ERROR;
}
