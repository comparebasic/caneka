#include <external.h>
#include <caneka.h>

word StepperFlags = ZERO;
void Stepper(Abstract *a){
    if((a->type.state & DEBUG) == 0){
        return; 
    }
    SetOriginalTios();

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
            }else if(buff[i] == 'n'){
                StepperFlags |= STEPPER_END;
            }else if(buff[i] == 'p'){
                DPrint(a, COLOR_PURPLE, "Stepper: ");
            }else if(buff[i] == 'c'){
                StepperFlags |= STEPPER_BYPASS;
            }
        }
    }
    RawMode(FALSE);
}


