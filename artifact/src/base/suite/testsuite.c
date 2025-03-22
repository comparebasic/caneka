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

status Test(boolean condition, StrVec *msg){
    if((GLOBAL_flags & HTML_OUTPUT) != 0){
        Out(_debugM, "        <li class=\"test result result-_c\">_+</li>", condition ? "pass" : "fail");
        return condition ? SUCCESS : ERROR;
    }else{
        if(!condition){
            Out(_debugM, "^rFail: ");
        }else{
            Out(_debugM, "^gPass: ");
        }
        Out(_debugM, "_c:_+", condition ? "pass" : "fail");
        return condition ? SUCCESS : ERROR;
    }
}

status Test_Runner(MemCtx *gm, char *suiteName, TestSet *tests){
    MemCtx *m = MemCtx_Make();
    DebugStack_Push(suiteName, TYPE_CSTR); 
    TestSet *set = tests;
    char *name = NULL;
    int pass = 0;
    int fail = 0;

    word baseStackLevel = m->type.range;
    StrVec *v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "Basemem: at "));
    StrVec_Add(v, Str_FromI64(m, baseStackLevel));
    StrVec_Add(v, Str_CstrRef(m, " "));
    i64 baseMem = MemCount(0);
    i64 rollingBaseMem = baseMem;
    StrVec_Add(v, Str_MemCount(m, baseMem));

    if((GLOBAL_flags & HTML_OUTPUT) != 0){
        Out(m, "        <span class=\"mem-label\">_+</span>\n", v);
    }else{
        Out(m, "_+", v);
    }

    if((GLOBAL_flags & HTML_OUTPUT) != 0){
        Out(m, "<div class=\"suite\">\n    <span class=\"label\">Suite _c</span>\n", suiteName);
    }

    while(set->name != NULL){

        if(set->status == SECTION_LABEL){
            if((GLOBAL_flags & HTML_OUTPUT) != 0){
                Out(m, "<h4>_c</h4>\n", set->name);
            }else{
                Out(m, "== _c ==\n", set->name);
            }
            set++;
            continue;
        }

        if((GLOBAL_flags & HTML_OUTPUT) != 0){
            Out(m, "<div class=\"set\">\n    <span class=\"set-label\">_c</span>\n", set->name);
            Out(m, "    <span class=\"status\" data-status=\"_i4\">_c</span>\n    <p>_c</p>\n    <ol class=\"tests\">\n",
                set->status, statusCstr(set->status), set->description);
        }else{
            Out(m, "[Testing _c]\n", set->name);
        }

        status r = READY;
        DebugStack_SetRef(set->name, TYPE_CSTR);
        if(set->func != NULL){
            m->type.range++;
            _debugM->type.range++;
            r = set->func(m);
            MemCtx_Free(m);
            MemCtx_Free(_debugM);
            _debugM->type.range--;
            m->type.range--;

            word stackLevel = m->type.range-baseStackLevel;
            m->type.range++;
            i64 memUsed = MemCount(0)-baseMem;
            i64 overRollingUsed = MemCount(0)-rollingBaseMem;
            rollingBaseMem = MemCount(0);

            char *htmlCls = "";
            if(overRollingUsed > 0 || stackLevel > 0){
                if((GLOBAL_flags & HTML_OUTPUT) != 0){
                    htmlCls = " text-red"; 
                }else{
                    StrVec_Add(v, Str_AnsiCstr(m, "^r"));
                }
            }else if(memUsed > 0){
                if((GLOBAL_flags & HTML_OUTPUT) != 0){
                    htmlCls = " text-blue"; 
                }else{
                    StrVec_Add(v, Str_AnsiCstr(m, "^b"));
                }
            }
            StrVec_Add(v, Str_CstrRef(m, "mem: at "));
            StrVec_Add(v, Str_FromI64(m, stackLevel));
            StrVec_Add(v, Str_MemCount(m, memUsed));
            if(memUsed > 0 || stackLevel > 0){
                if((GLOBAL_flags & HTML_OUTPUT) == 0){
                    StrVec_Add(v, Str_AnsiCstr(m, "^0"));
                }
            }
            if((GLOBAL_flags & HTML_OUTPUT) != 0){
                Out(m, "    <li><span class=\"mem-label_s\">_+</span></li>\n", htmlCls, v);
                Out(m, "    </ol>\n");
                Out(m, "</div>\n");
            }else{
                Out(m, "_+", v);
            }
            MemCtx_Free(m);
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
