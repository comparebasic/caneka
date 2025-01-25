#include <external.h>
#include <caneka.h>

word StepperFlags = ZERO;
static int bypassFor = 0;
static String *num_s = NULL;
void Stepper(Abstract *a){
    if((a->type.state & DEBUG) == 0 || (bypassFor > 0 && --bypassFor > 0)){
        return; 
    }
    SetOriginalTios();
    bypassFor = 0;
    if(num_s == NULL){
        num_s = String_Init(DebugM, STRING_EXTEND);
    }else{
        String_Reset(num_s);
    }
    char buff[SERV_READ_SIZE];
    int delay = ACCEPT_LONGDELAY_MILLIS;
    struct pollfd pfd = {
        0,
        SOCK_IN,
        POLLHUP
    };
    printf("\x1b[1;30mStepping > n = next, c = continue, p = print, q = quit\x1b[0m\n");
    RawMode(TRUE);
    StepperFlags &= ~STEPPER_END;
    while((StepperFlags & (STEPPER_BYPASS|STEPPER_END)) == 0){
        poll(&pfd, 1, delay);
        ssize_t l = read(pfd.fd, buff, SERV_READ_SIZE);
        for(int i = 0; i < l; i++){
            if(buff[i] == 'q' || buff[i] == 3){
                Fatal("Quit from Stepper", 0);
            }else if(buff[i] >= '0' && buff[i] <= '9'){
                if(num_s->length == 0){
                    printf("bypass for:");
                }
                printf("%c", buff[i]);
                fflush(stdout);
                String_AddBytes(DebugM, num_s, bytes(buff+i), 1);
            }else if(buff[i] == 'n'){
                StepperFlags |= STEPPER_END;
                if(num_s->length > 0){
                    bypassFor = Int_FromString(num_s);
                }
                printf("\n");
            }else if(buff[i] == 'p'){
                DPrint(a, COLOR_PURPLE, "Stepper: ");
            }else if(buff[i] == 'c'){
                StepperFlags |= STEPPER_BYPASS;
            }
        }
    }
    RawMode(FALSE);
}


