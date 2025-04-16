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
        Out("        <li class=\"test result result-_c\">_+</li>", condition ? "pass" : "fail");
        return condition ? SUCCESS : ERROR;
    }else{
        if(!condition){
            Out("^r.Fail: ");
        }else{
            Out("^g.Pass: ");
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
    char *name = NULL;
    int pass = 0;
    int fail = 0;

    word baseStackLevel = m->type.range;
    StrVec *v = StrVec_Make(m);
    i64 baseMem = MemCount(0);
    i64 rollingBaseMem = baseMem;

    i64 chapters = MemChapterCount();
    Out("= Test Suite: _c\n", suiteName);
    Out("Starting Mem at _t (_i8 chapters/_i8 total+stack)\n", 
        Str_MemCount(m, baseMem), chapters, chapters *PAGE_SIZE);

    while(set->name != NULL){

        if(set->status == SECTION_LABEL){
            Out("== _c\n", set->name);
            set++;
            continue;
        }

        Out("=== Testing: _c\n", set->name);

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
            Out("_tMem: _t (_i8 chapters/_i8 available/_t total+stack)^0\x1b[2K\r", 
                color, Str_MemCount(m, overRollingUsed), 
                chapters, availableCh, Str_MemCount(m, chapters*PAGE_SIZE));

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
    if((GLOBAL_flags & HTML_OUTPUT) != 0){
        Out("\n    <span class=\"result\">Suite _c pass:_i4 fail:_i4</span>\n", suiteName, pass, fail);
    }else if((GLOBAL_flags & NO_COLOR) != 0){
        Out("Suite _c pass:_i4 fail:_i4\n", suiteName, pass, fail);
    }else{
        if(!fail){
            Out("^g");
        }else{
            Out("^r");
        }
        Out("Suite _c pass:_i4 fail:_i4^0\n", suiteName, pass, fail);

    }
    if((GLOBAL_flags & HTML_OUTPUT) != 0){
        Out("</div>\n");
    }
    DebugStack_Pop();
    return !fail ? SUCCESS : ERROR;
}
