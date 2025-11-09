#include <external.h>
#include <caneka.h>

static boolean change = FALSE;
static struct termios orig_tios;
static struct termios current_tios;

status SetOriginalTios(){
   int r = tcgetattr(STDIN_FILENO, &current_tios);
   if(r != -1){
       memcpy(&orig_tios, &current_tios, sizeof(struct termios));
       return SUCCESS;
   }
   return ERROR;
}

status RawMode(boolean enable){
    if(enable){
       int r = tcgetattr(STDIN_FILENO, &current_tios);
       if(r != -1){
           current_tios.c_lflag &= ~(ICANON|ISIG|ECHO);
           current_tios.c_cc[VMIN] = 1;
           current_tios.c_cc[VTIME] = 0;
       }
   }else{
       if(!change){
            return NOOP;
       }
       memcpy(&current_tios, &orig_tios, sizeof(struct termios));
       current_tios.c_lflag |= ECHO;
   }

   change = TRUE;
   if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &current_tios) != -1){
       return SUCCESS;
   }

   return ERROR; 
}
