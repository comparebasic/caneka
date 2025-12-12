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
    }else{
        return "unknown";
    }
}

Str *Test_GetStr512(MemCh *m){
    return S(m, 
        "0102030405060708091011121314151617181920212223242526272829303132"
        "3334353637383940414243444546474849505152535455565758596061626364"
        "6566676869707172737475767778798081828384858687888990919293949596"
        "979899a1a2a3a4a5a6a7a8a9a0b1b1b2b3b4b5b6b7b8b9b0c1c2c3c4c5c6c7c8"
        "c9c0d1d2d3d4d5d6d7d8d9d0e1e2e3e4e5e6e7e8e9e0f1f2f3f4f5f6f7f8f9f0"
        "g1g2g3g4g5g6g7g8g9g0h1h2h3h4h5h6h7h8h9h0i1i2i3i4i5i6i7i8i9i0j1j2"
        "j3j4j5j6j7j8j9j0k1k2k3k4k5k6k7k8k9k0l1l2l3l4l5l6l7l8l9l0m1m2m3m4"
        "m5m6m7m8m9m0n1n2n3n4n5n6n7n8n9n0o1o2o3o4o5o6o7o8o9o0p1p2p3p4p5p6"
    );
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

status Test_Runner(MemCh *m, TestSuite *suite){
    DebugStack_Push(suite->name, suite->name->type.of); 
    m->level++;
    TestSet *set = suite->set;
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

    args[0] = suite->name;
    args[1] = NULL;
    Out("= Test Suite: $\n", args);

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

        if(set->status & SKIP_TEST){
            Out("^y.=== Skipping $  ^0.\n", args);
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
                suite->fail++;
                break;
            }else{
                suite->pass++;
            }
        }

        set++;
    }

    args[0] = suite->name,
    args[1] = I32_Wrapped(m, suite->pass),
    args[2] = I32_Wrapped(m, suite->fail),
    args[3] = NULL;
    if(!suite->fail){
        Out("^g", NULL);
    }else{
        Out("^r", NULL);
    }
    Out("Suite $ pass($) fail($)^0\n", args);

    m->level--;
    MemCh_FreeTemp(m);
    DebugStack_Pop();
    return !suite->fail ? SUCCESS : ERROR;
}

TestSuite *TestSuite_Make(MemCh *m, Str *name, TestSet *set){
    TestSuite *suite = MemCh_AllocOf(m, sizeof(TestSuite), TYPE_TEST_SUITE);
    suite->type.of = TYPE_TEST_SUITE;
    suite->name = name;
    suite->set = set;
    return suite;
}
