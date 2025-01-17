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
            return SUCCESS;
        }
    }
}

status Test_Runner(MemCtx *m, char *suiteName, TestSet *tests){
    DebugStack_Push("Test_Runner"); 
    int i = 0;
    TestSet *set = tests;
    char *name = NULL;
    int pass = 0;
    int fail = 0;

    int baseStackLevel = MemCtx_TempLevel;
    String *s = String_Init(m, STRING_EXTEND);
    String_AddBytes(m, s, bytes("BaseMem: at "), strlen("mem: at "));
    String_AddInt(m, s, baseStackLevel);
    String_AddBytes(m, s, bytes(" "), strlen(" "));
    i64 baseMem = MemCount();
    i64 rollingBaseMem = baseMem;
    String_AddMemCount(m, s, baseMem);
    printf("%s\n", s->bytes);

    if((GLOBAL_flags & HTML_OUTPUT) != 0){
        printf("<div class=\"suite\">\n    <span class=\"label\">Suite %s</span>\n", suiteName);
    }
    while(set->func != NULL){
        if((GLOBAL_flags & HTML_OUTPUT) != 0){
            printf("<div class=\"set\">\n    <span class=\"set-label\">%s</span>\n", set->name);
            printf("    <span class=\"status\" data-status=\"%d\">%s</span>\n    <p>%s</p>\n    <ol class=\"tests\">\n",
                set->status, statusCstr(set->status), set->description);
        }else{
            printf("[Testing %s]\n", set->name);
        }

        DebugStack_Push("Test->func");
        m->type.state |= MEMCTX_TEMP;
        status r = set->func(m);
        MemCtx_FreeTemp(m, MemCtx_TempLevel);
        MemCtx_FreeTemp(DebugM, MemCtx_TempLevel);
        DebugStack_Pop();

        m->type.state &= ~MEMCTX_TEMP;
        if((GLOBAL_flags & HTML_OUTPUT) != 0){
            printf("    </ol>\n</div>\n");
        }else{
            String *s = String_Init(m, STRING_EXTEND);
            i64 memUsed = MemCount()-baseMem;
            i64 overRollingUsed = MemCount()-rollingBaseMem;

            int stackLevel = MemCtx_TempLevel-baseStackLevel;
            if(overRollingUsed > 0 || stackLevel > 0){
                String_AddBytes(m, s, bytes("\x1b[31m"), strlen("\x1b[31m"));
            }else if(memUsed > 0){
                String_AddBytes(m, s, bytes("\x1b[34m"), strlen("\x1b[34m"));
            }
            String_AddBytes(m, s, bytes("mem: at "), strlen("mem: at "));
            String_AddInt(m, s, stackLevel);
            String_AddBytes(m, s, bytes(" "), strlen(" "));
            String_AddMemCount(m, s, memUsed);
            if(memUsed > 0 || stackLevel > 0){
                String_AddBytes(m, s, bytes("\x1b[0m"), strlen("\x1b[0m"));
            }
            printf("%s\n", String_ToChars(m, s));
            rollingBaseMem = MemCount();
        }
        if((r & ERROR) != 0 || (r & SUCCESS) == 0){
            fail++;
            break;
        }
        pass++;
        i+= 2;
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
