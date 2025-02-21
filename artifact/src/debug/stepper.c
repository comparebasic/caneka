#include <external.h>
#include <caneka.h>

static Span *steps = NULL;

status Steps_Init(MemCtx *m){
    steps = Span_Make(m, TYPE_TABLE);
    return SUCCESS;
}

Step *Stepper_Make(MemCtx *m, Abstract *a, word flags, String *desc, String *num_s){
    Step *st = MemCtx_Alloc(m, sizeof(Step)); 
    st->type.of = TYPE_STEP;
    st->type.state = flags;
    st->desc = desc;
    st->num_s = num_s;
    st->a = a;
    st->Func = Stepper_GetDescFunc(a->type.of);
    a->type.state |= DEBUG;
    Table_Set(steps, a, (Abstract *)st);
    return st;
}

status Stepper_BypassFor(Step *st, int num){
    st->bypassFor += num;
    return SUCCESS;
}

void Stepper(MemCtx *m, Abstract *a){
    DebugStack_Push(NULL, 0);
    if((a->type.state & DEBUG) == 0){ 
        DebugStack_Pop();
        return; 
    }
    
    Step *st = (Step *)Table_Get(steps, a);
    if(st->bypassFor > 0 && --(st->bypassFor) > 0){
        DebugStack_Pop();
        return; 
    }

    if(st->Func != NULL){
        st->Func(m, st->a, (Abstract *)st->desc);
    }

    SetOriginalTios();
    st->bypassFor = 0;
    if(st->num_s == NULL){
        st->num_s = String_Init(DebugM, STRING_EXTEND);
    }else{
        String_Reset(st->num_s);
    }
    char buff[SERV_READ_SIZE];
    int delay = ACCEPT_LONGDELAY_MILLIS;
    struct pollfd pfd = {
        0,
        SOCK_IN,
        POLLHUP
    };

    Debug_Print((void *)st->desc, 0, "\rStepping> ", COLOR_DARK, FALSE);
    printf("\x1b[%dm n = next, c = continue, p = print, q = quit\x1b[0m\n", COLOR_DARK);

    RawMode(TRUE);
    st->type.state &= ~STEPPER_END;
    while((st->type.state & (STEPPER_BYPASS|STEPPER_END)) == 0){
        poll(&pfd, 1, delay);
        ssize_t l = read(pfd.fd, buff, SERV_READ_SIZE);
        for(int i = 0; i < l; i++){
            if(buff[i] == 'q' || buff[i] == 3){
                RawMode(FALSE);
                Fatal("Quit from Stepper", 0);
            }else if(buff[i] >= '0' && buff[i] <= '9'){
                if(st->num_s->length == 0){
                    printf("bypass for:");
                }
                printf("%c", buff[i]);
                fflush(stdout);
                String_AddBytes(DebugM, st->num_s, bytes(buff+i), 1);
            }else if(buff[i] == 'n'){
                st->type.state |= STEPPER_END;
                if(st->num_s->length > 0){
                    st->bypassFor = Int_FromString(st->num_s);
                }
                printf("\n");
            }else if(buff[i] == 'p'){
                DPrint(st->a, COLOR_PURPLE, "Stepper: ");
            }else if(buff[i] == 'c'){
                st->type.state |= STEPPER_BYPASS;
            }
        }
    }
    RawMode(FALSE);
    DebugStack_Pop();
    return;
}


