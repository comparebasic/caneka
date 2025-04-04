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

    StrVec *v = StrVec_Make(_debugM);
    StrVec_FmtHandle(_debugM, v, fmt, args, -1);

    if((GLOBAL_flags & HTML_OUTPUT) != 0){
        Out(_debugM, "        <li class=\"test result result-_c\">_+</li>", condition ? "pass" : "fail", v);
        return condition ? SUCCESS : ERROR;
    }else{
        if(!condition){
            Out(_debugM, "^r.Fail: ");
        }else{
            Out(_debugM, "^g.Pass: ");
        }
        Out(_debugM, "_+^0\n",  v);
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
    Out(m, "= Test Suite: _c\n", suiteName);
    Out(m, "Starting Mem at _t (_i8 chapters/_i8 total+stack)\n", 
        Str_MemCount(m, baseMem), chapters, chapters *PAGE_SIZE);

    while(set->name != NULL){

        if(set->status == SECTION_LABEL){
            Out(m, "== _c\n", set->name);
            set++;
            continue;
        }

        Out(m, "=== Testing: _c\n", set->name);

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
            Out(m, "_tMem: _t (_i8 chapters/_i8 available/_t total+stack)^0\n", 
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
        Out(m, "\n    <span class=\"result\">Suite _c pass:_i4 fail:_i4</span>\n", suiteName, pass, fail);
    }else if((GLOBAL_flags & NO_COLOR) != 0){
        Out(m, "Suite _c pass:_i4 fail:_i4\n", suiteName, pass, fail);
    }else{
        if(!fail){
            Out(m, "^g");
        }else{
            Out(m, "^r");
        }
        Out(m, "Suite _c pass:_i4 fail:_i4^0\n", suiteName, pass, fail);

    }
    if((GLOBAL_flags & HTML_OUTPUT) != 0){
        Out(m, "</div>\n");
    }
    DebugStack_Pop();
    return !fail ? SUCCESS : ERROR;
}
