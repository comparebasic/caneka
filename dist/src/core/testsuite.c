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
    Stack(bytes("Test"), (Abstract *)String_Make(DebugM, bytes(msg)));
	va_list args;
    va_start(args, msg);
    if(HasFlag(GLOBAL_flags, HTML_OUTPUT)){
        printf("        <li class=\"test result result-%s\">", condition ? "pass" : "fail");
        vprintf(msg, args);
        printf("</li>\n");
        Return condition ? SUCCESS : ERROR;
    }else{
        if(!condition){
            if(!HasFlag(GLOBAL_flags, NO_COLOR)){
                printf("\x1b[31m");
            }
            printf("FAIL: ");
            vprintf(msg, args);
            if(!HasFlag(GLOBAL_flags, NO_COLOR)){
                printf("\x1b[0m");
            }
            printf("\n");
            Return ERROR;
        }else{
            if(!HasFlag(GLOBAL_flags, NO_COLOR)){
                printf("\x1b[32m");
            }
            printf("PASS: ");
            vprintf(msg, args);
            if(!HasFlag(GLOBAL_flags, NO_COLOR)){
                printf("\x1b[0m");
            }
            printf("\n");
            Return SUCCESS;
        }
    }
}

status Test_Runner(MemCtx *m, char *suiteName, TestSet *tests){
    int i = 0;
    TestSet *set = tests;
    char *name = NULL;
    int pass = 0;
    int fail = 0;
#ifdef DEBUG_STACK
    DebugStack_Push(bytes("Test_Runner"), 
        (Abstract *)String_Make(DebugM, bytes(suiteName)));
#endif
    if(HasFlag(GLOBAL_flags, HTML_OUTPUT)){
        printf("<div class=\"suite\">\n    <span class=\"label\">Suite %s</span>\n", suiteName);
    }
    while(set->func != NULL){
        if(HasFlag(GLOBAL_flags, HTML_OUTPUT)){
            printf("<div class=\"set\">\n    <span class=\"set-label\">%s</span>\n", set->name);
            printf("    <span class=\"status\" data-status=\"%d\">%s</span>\n    <p>%s</p>\n    <ol class=\"tests\">\n",
                set->status, statusCstr(set->status), set->description);
        }else{
            printf("[Testing %s]\n", set->name);
        }
        status r = set->func(m);
        if(HasFlag(GLOBAL_flags, HTML_OUTPUT)){
            printf("    </ol>\n</div>\n");
        }
        if(HasFlag(r, ERROR) || !HasFlag(r, SUCCESS)){
            fail++;
            /*
            break;
            */
        }
        pass++;
        i+= 2;
        set++;
    }
    if(HasFlag(GLOBAL_flags, HTML_OUTPUT)){
        printf("\n    <span class=\"result\">Suite %s pass:%d fail:%d</span>\n", suiteName, pass, fail);
    }else if(HasFlag(GLOBAL_flags, NO_COLOR)){
        printf("Suite %s pass:%d fail:%d\n", suiteName, pass, fail);
    }else{
        if(!fail){
            printf("\x1b[%dmSuite %s \x1b[1;%dmpass:%d\x1b[0;%dm fail:%d\x1b[0m\n", COLOR_GREEN, suiteName, COLOR_GREEN, pass, COLOR_GREEN, fail);
        }else{
            printf("\x1b[%dmSuite %s pass:%d \x1b[1;%dmfail:%d\x1b[0m\n", COLOR_RED, suiteName, pass, COLOR_RED,  fail);
        }
    }
    if(HasFlag(GLOBAL_flags, HTML_OUTPUT)){
        printf("</div>\n");
    }
    return !fail ? SUCCESS : ERROR;
}
