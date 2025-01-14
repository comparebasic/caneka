#include <external.h>
#include <caneka.h>

int DEBUG_STACK_COLOR = COLOR_GREEN;

DebugStackEntry _stackDebug[DEBUG_STACK_MAX+1];
int _stackIdx = 0;

static void DebugStackEntry_init(DebugStackEntry *entry){
    memset(entry, 0, sizeof(DebugStackEntry));
    entry->type.of = TYPE_DEBUG_STACK_ENTRY;
    entry->name.type.of = TYPE_STRING_CHAIN;
    entry->file.type.of = TYPE_STRING_CHAIN;
    entry->idx = _stackIdx;
}

static void sigH(int sig, siginfo_t *info, void *ptr){
    Fatal("Sig", 0);
    exit(1);
}

status DebugStack_Init(){
    memset(&_stackDebug, 0, sizeof(_stackDebug));
    _stackIdx = 0;

    struct sigaction a;
    struct sigaction b;
    memset(&a, 0, sizeof(struct sigaction));
    a.sa_flags = SA_NODEFER;
    a.sa_sigaction = sigH;
    sigaction(SIGSEGV, &a, NULL);

    memset(&b, 0, sizeof(struct sigaction));
    b.sa_flags = SA_NODEFER;
    b.sa_sigaction = sigH;
    sigaction(SIGINT, &b, NULL);

    return SUCCESS;
}

void _DebugStack_Push(byte *name, Abstract *a, char *file, int line){
    DebugStackEntry *entry = ((DebugStackEntry *)_stackDebug)+_stackIdx;
    DebugStackEntry_init(entry);
    String_AddBytes(DebugM, &entry->name, name, strlen((char *)name));
    String_AddBytes(DebugM, &entry->file, bytes(file), strlen((char *)file));
    entry->line = line;
    entry->idx = _stackIdx;
    entry->a = a;
    _stackIdx++;
}

void DebugStack_Pop(){
    _stackIdx--;
}

void DebugStack_SetRef(Abstract *a){
    DebugStackEntry *entry = (((DebugStackEntry *)_stackDebug)+(_stackIdx-1));
    if(entry->type.of == TYPE_DEBUG_STACK_ENTRY){
        entry->a = a;
    }
}

int DebugStack_Print(){
    if(_stackIdx > 0){
        int idx = _stackIdx;
        boolean top = TRUE;
        while(--idx >= 0){
            DebugStackEntry *entry = (((DebugStackEntry *)_stackDebug)+idx);
            if(top){
                entry->type.state |= ERROR;
                top = FALSE;
            }
            Debug_Print((void *)entry, 0, "    ", DEBUG_STACK_COLOR, TRUE);
            printf("\n");
        }
        return _stackIdx;
    }
    return 0;
}
