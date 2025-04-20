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

status Test(boolean condition, char *fmt, ...){
	va_list args;
    va_start(args, fmt);


    if((GLOBAL_flags & HTML_OUTPUT) != 0){
        char *cstr = condition ? "pass" : "fail";
        void *args[] = { cstr, NULL };
        Out("        <li class=\"test result result-_c\">_+</li>", args);
        return condition ? SUCCESS : ERROR;
    }else{
        if(!condition){
            Out("^r.Fail: ", NULL);
        }else{
            Out("^g.Pass: ", NULL);
        }
        StrVec_FmtHandle(OutStream, fmt, args);
        if(!condition){
            Stream_To(OutStream, (byte *)"\n", 1);
        }else{
            /*
            Stream_To(OutStream, (byte *)"\x1b[2K\r", 5);
            */
            Stream_To(OutStream, (byte *)"\n", 1);
        }
        return condition ? SUCCESS : ERROR;
    }
}

status Test_Runner(MemCh *gm, char *suiteName, TestSet *tests){
    MemCh *m = MemCh_Make();
    DebugStack_Push(suiteName, TYPE_CSTR); 
    TestSet *set = tests;
    void **args;
    char *name = NULL;
    int pass = 0;
    int fail = 0;

    word baseStackLevel = m->type.range;
    StrVec *v = StrVec_Make(m);
    i64 baseMem = MemCount(0);
    i64 rollingBaseMem = baseMem;

    i64 chapters = MemChapterCount();
    args = { suiteName, NULL };
    Out("= Test Suite: _c\n", args);
    i32 memCount = Str_MemCount(m, baseMem);
    i32 chap = chapters *PAGE_SIZE;
    args = { &memCount, &chap, NULL };
    Out("Starting Mem at _t (_i8 chapters/_i8 total+stack)\n", args);

    while(set->name != NULL){

        args = {set->name, NULL };
        if(set->status == SECTION_LABEL){
            Out("== _c\n", args);
            set++;
            continue;
        }

        Out("=== Testing: _c\n", args);

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
                color = Str_AnsiCstr(m, "^r.");
            }else if(memUsed > 0){
                color = Str_AnsiCstr(m, "^b.");
            }

            i64 chapters = MemChapterCount();
            i64 availableCh = MemAvailableChapterCount();

            i64 strMemCount = Str_MemCount(m, overRollingUsed);
            i64 strMemTotal = Str_MemCount(m, chapters*PAGE_SIZE);
            args = {
                &color, &strMemCount, &chapters, &availableCh,
                &strMemTotal, NULL
            }; 
            Out("_tMem: _t (_i8 chapters/_i8 available/_t total+stack)^0\x1b[2K\r", args);

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
    args = {suiteName, &pass, &fail};
    if((GLOBAL_flags & HTML_OUTPUT) != 0){
        Out("\n    <span class=\"result\">Suite _c pass:_i4 fail:_i4</span>\n", args);
    }else if((GLOBAL_flags & NO_COLOR) != 0){
        Out("Suite _c pass:_i4 fail:_i4\n", args);
    }else{
        if(!fail){
            Out("^g");
        }else{
            Out("^r");
        }
        Out("Suite _c pass:_i4 fail:_i4^0\n", args);

    }
    if((GLOBAL_flags & HTML_OUTPUT) != 0){
        Out("</div>\n");
    }
    DebugStack_Pop();
    return !fail ? SUCCESS : ERROR;
}
