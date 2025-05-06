#include <external.h>
#include <caneka.h>

static Span *stack = NULL;
static boolean _fuse = TRUE;
static void sigH(int sig, siginfo_t *info, void *ptr){
    if(_fuse){
        _fuse = FALSE;
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Sig Seg Fault", NULL);
    }else{
        write(0, "Double SigH\n", strlen("Double SigH\n"));
    }
    exit(1);
}

static void setSigs(){
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
}

int DEBUG_STACK_COLOR = COLOR_GREEN;
int _stackIdx = 0;

status DebugStack_Init(MemCh *m){
    _stackIdx = 0;
    stack = Span_Make(m);
    setSigs();

    i32 idx;
    MemPage *pg = MemCh_GetPage(m, stack, &idx);
    return SUCCESS;
}

void _DebugStack_Push(char *cstr, char *fname, void *ref, word typeOf, int line, int pos){
    stack->m->type.range--;

    StackEntry *entry = MemCh_Alloc(stack->m, sizeof(StackEntry));
    entry->funcName = cstr;
    entry->fname = fname;
    entry->ref = ref;
    entry->typeOf = typeOf;
    entry->line = line;
    entry->pos = pos;

    Span_Set(stack, _stackIdx++, (Abstract*)entry);

    stack->m->type.range++;
}

void DebugStack_Pop(){
    _stackIdx--;
    stack->max_idx = _stackIdx;
}

void DebugStack_SetRef(void *v, word typeOf){
    StackEntry *entry = Span_Get(stack, _stackIdx-1);
    if(entry == NULL){
        return;
    }
    entry->ref = v;
    entry->typeOf = typeOf;
}

i32 DebugStack_Print(Stream *sm, word flags){
    Iter it;
    Iter_Init(&it, stack);
    boolean first = TRUE;
    while((Iter_Prev(&it) & END) == 0){
        StackEntry *entry = (StackEntry*)it.value;
        if(entry != NULL){
            Stream_Bytes(sm, (byte *)"    \x1b[1;33m", 12);
            Stream_Bytes(sm, (byte *)entry->funcName, strlen(entry->funcName));
            Stream_Bytes(sm, (byte *)"\x1b[22m:", 6);
            Stream_Bytes(sm, (byte *)entry->fname, strlen(entry->fname));
            Stream_Bytes(sm, (byte *)":", 1);
            byte lineNo[MAX_BASE10+1];
            byte *b = lineNo;
            i64 length = Str_I64OnBytes(&b, entry->line);
            Stream_Bytes(sm, (byte *)b, length);
            if(flags & MORE){
                Abstract *args[] = {entry->ref, NULL};
                if(entry->ref != NULL && entry->typeOf != 0){
                    Fmt(sm, "^y@^0", args);
                }else if(entry->ref != NULL && entry->typeOf == 0){
                    Fmt(sm, "^y@^0", args);
                }
            }
            Stream_Bytes(sm, (byte *)"\x1b[0m\n", 5);
        }
    }
    return 0;
}
