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
    OutStream->m->level++;
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
    OutStream->m->level--;
    MemCh_FreeTemp(OutStream->m);
    return condition ? SUCCESS : ERROR;
}

status Test(boolean condition, char *fmt, void *args[]){
    OutStream->m->level++;
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
    OutStream->m->level--;
    MemCh_FreeTemp(OutStream->m);
    return condition ? SUCCESS : ERROR;
}

status Test_Runner(MemCh *m, char *suiteName, TestSet *tests){
    DebugStack_Push(suiteName, TYPE_CSTR); 
    m->level++;
    TestSet *set = tests;
    char *name = NULL;
    i32 pass = 0;
    i32 fail = 0;

    void *args[8];

    word baseStackLevel = m->level;
    StrVec *v = StrVec_Make(m);

    MemBookStats st;
    MemBook_GetStats(m, &st);
    i32 baseMem = st.total;
    i32 threashold = 2;
    i32 maxCeiling = TEST_MEM_MAX_CEILING;

    args[0] = Str_CstrRef(m, suiteName);
    args[1] = NULL;
    Out("= Test Suite: _c\n", args);

    args[0] = Str_MemCount(m, st.total * PAGE_SIZE);
    args[1] = I64_Wrapped(m, st.total);
    args[2] = I64_Wrapped(m, st.pageIdx);
    args[3] = I64_Wrapped(m, PAGE_SIZE);
    args[4] = NULL;

    Out("\nStarting Mem at $ total/maxIdx=^D.$/$^d. page-size=$b\n", args);

    while(set->name != NULL){
        args[0] = Str_CstrRef(m, set->name);
        if(set->status == SECTION_LABEL){
            Out("^E.== $  ^e.\n", args);
            set++;
            continue;
        }

        Out("=== Testing: $\n", args);

        args[0] = K(m, set->description);
        args[2] = NULL;
        Out("\n$\n\n", args);

        status r = READY;
        DebugStack_SetRef(set->name, TYPE_CSTR);
        if(set->func != NULL){
            m->level++;
            MemCh *tm = MemCh_Make();
            r = set->func(tm);

            MemBook_GetStats(m, &st);
            i64 memUsed = st.total - baseMem;

            if(tm->metrics.totalCeiling > maxCeiling){
                args[0] = I64_Wrapped(m, maxCeiling);
                args[1] = I64_Wrapped(m, tm->metrics.totalCeiling);
                args[2] = NULL;
                Out("\n^r.Ceiling for test memory too high max=$ ceiling=$^0\n", args);
                r |= ERROR;
            }

            args[0] = Str_MemCount(m, tm->metrics.totalCeiling * PAGE_SIZE),
            args[1] = I32_Wrapped(m, tm->metrics.totalCeiling);
            args[2] = I64_Wrapped(m, st.total),
            args[3] = I64_Wrapped(m, st.pageIdx),
            args[4] = I64_Wrapped(m, PAGE_SIZE),
            args[5] = NULL;
            Out("\n^c.Mem ceiling='$' ceiling/global/max=^D.$/$/$^d. page-size=$b^0\n\n",
                args);

            MemCh_Free(tm);

            MemBook_GetStats(m, &st);
            if(st.total > (baseMem+threashold)){
                args[0] = I64_Wrapped(m, st.total);
                args[1] = I64_Wrapped(m, baseMem);
                Out("\n^r.MemLeak total-after-free=$ baseMem=$^0\n", args);
                r |= ERROR;
            }
            baseMem = st.total;

            m->level--;
            MemCh_FreeTemp(m);
            Buff_Flush(OutStream);
            if((r & ERROR) != 0 || (r & SUCCESS) == 0){
                fail++;
                break;
            }else{
                pass++;
            }
        }

        set++;
    }

    args[0] = Str_CstrRef(m, suiteName),
    args[1] = I32_Wrapped(m, pass),
    args[2] = I32_Wrapped(m, fail),
    args[3] = NULL;
    if(!fail){
        Out("^g", NULL);
    }else{
        Out("^r", NULL);
    }
    Out("Suite $ pass($) fail($)^0\n", args);

    m->level--;
    MemCh_FreeTemp(m);
    DebugStack_Pop();
    return !fail ? SUCCESS : ERROR;
}
