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

status Test(int condition, char *msg, ...){
	va_list args;
    va_start(args, msg);
    if((GLOBAL_flags & HTML_OUTPUT) != 0){
        printf("        <li class=\"test result result-%s\">", condition ? "pass" : "fail");
        vprintf(msg, args);
        printf("</li>\n");
        fflush(stdout);
        return condition ? SUCCESS : ERROR;
    }else{
        if(!condition){
            if((GLOBAL_flags & NO_COLOR) == 0){
                printf("\x1b[31m");
            }
            printf("FAIL: ");
            vprintf(msg, args);
            if((GLOBAL_flags & NO_COLOR) == 0){
                printf("\x1b[0m");
            }
            printf("\n");
            fflush(stdout);
            return ERROR;
        }else{
            if((GLOBAL_flags & NO_COLOR) == 0){
                printf("\x1b[32m");
            }
            printf("PASS: ");
            vprintf(msg, args);
            if((GLOBAL_flags & NO_COLOR) == 0){
                printf("\x1b[0m");
            }
            printf("\n");
            fflush(stdout);
            return SUCCESS;
        }
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
    String *s = String_Init(m, STRING_EXTEND);
    String_AddBytes(m, s, bytes("BaseMem: at "), strlen("mem: at "));
    String_AddInt(m, s, baseStackLevel);
    String_AddBytes(m, s, bytes(" "), strlen(" "));
    i64 baseMem = MemCount(0);
    i64 rollingBaseMem = baseMem;
    String_AddMemCount(m, s, baseMem);

    if((GLOBAL_flags & HTML_OUTPUT) != 0){
        printf("        <span class=\"mem-label\">%s</span>\n", s->bytes);
        fflush(stdout);
    }else{
        printf("%s\n", s->bytes);
    }

    if((GLOBAL_flags & HTML_OUTPUT) != 0){
        printf("<div class=\"suite\">\n    <span class=\"label\">Suite %s</span>\n", suiteName);
        fflush(stdout);
    }
    while(set->name != NULL){

        if(set->status == SECTION_LABEL){
            if((GLOBAL_flags & HTML_OUTPUT) != 0){
                printf("<h4>%s</h4>\n", set->name);
                fflush(stdout);
            }else{
                printf("== %s ==\n", set->name);
            }
            set++;
            continue;
        }

        if((GLOBAL_flags & HTML_OUTPUT) != 0){
            printf("<div class=\"set\">\n    <span class=\"set-label\">%s</span>\n", set->name);
            printf("    <span class=\"status\" data-status=\"%d\">%s</span>\n    <p>%s</p>\n    <ol class=\"tests\">\n",
                set->status, statusCstr(set->status), set->description);
            fflush(stdout);
        }else{
            printf("[Testing %s]\n", set->name);
        }

        status r = READY;
        DebugStack_SetRef(set->name, TYPE_CSTR);
        if(set->func != NULL){
            m->type.range++;
            DebugM->type.range++;
            r = set->func(m);
            MemCtx_Free(m);
            MemCtx_Free(DebugM);
            DebugM->type.range--;
            m->type.range--;

            word stackLevel = m->type.range-baseStackLevel;
            m->type.range++;
            i64 memUsed = MemCount(0)-baseMem;
            i64 overRollingUsed = MemCount(0)-rollingBaseMem;
            rollingBaseMem = MemCount(0);
            String *s = String_Init(m, STRING_EXTEND);

            char *htmlCls = "";
            if(overRollingUsed > 0 || stackLevel > 0){
                if((GLOBAL_flags & HTML_OUTPUT) != 0){
                    htmlCls = " text-red"; 
                }else{
                    String_AddBytes(m, s, bytes("\x1b[31m"), strlen("\x1b[31m"));
                }
            }else if(memUsed > 0){
                if((GLOBAL_flags & HTML_OUTPUT) != 0){
                    htmlCls = " text-blue"; 
                }else{
                    String_AddBytes(m, s, bytes("\x1b[34m"), strlen("\x1b[34m"));
                }
            }
            String_AddBytes(m, s, bytes("mem: at "), strlen("mem: at "));
            String_AddInt(m, s, stackLevel);
            String_AddBytes(m, s, bytes(" "), strlen(" "));
            String_AddMemCount(m, s, memUsed);
            if(memUsed > 0 || stackLevel > 0){
                if((GLOBAL_flags & HTML_OUTPUT) == 0){
                    String_AddBytes(m, s, bytes("\x1b[0m"), strlen("\x1b[0m"));
                }
            }
            if((GLOBAL_flags & HTML_OUTPUT) != 0){
                printf("    <li><span class=\"mem-label%s\">%s</span></li>\n", htmlCls, String_ToChars(m, s));
                printf("    </ol>\n");
                printf("</div>\n");
                fflush(stdout);
            }else{
                printf("%s\n", String_ToChars(m, s));
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
        printf("\n    <span class=\"result\">Suite %s pass:%d fail:%d</span>\n", suiteName, pass, fail);
    }else if((GLOBAL_flags & NO_COLOR) != 0){
        printf("Suite %s pass:%d fail:%d\n", suiteName, pass, fail);
    }else{
        if(!fail){
            printf("\x1b[%dmSuite %s \x1b[1;%dmpass:%d\x1b[0;%dm fail:%d\x1b[0m\n", COLOR_GREEN, suiteName, COLOR_GREEN, pass, COLOR_GREEN, fail);
        }else{
            printf("\x1b[%dmSuite %s pass:%d \x1b[1;%dmfail:%d\x1b[0m\n", COLOR_RED, suiteName, pass, COLOR_RED,  fail);
        }
    }
    if((GLOBAL_flags & HTML_OUTPUT) != 0){
        printf("</div>\n");
    }
    DebugStack_Pop();
    return !fail ? SUCCESS : ERROR;
}
