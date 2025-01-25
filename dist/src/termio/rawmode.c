#include <external.h>
#include <caneka.h>

struct termios orig_tios;
struct termios current_tios;

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
           /*
           current_tios.c_lflag &= ~(ICANON|ISIG|ECHO);
           current_tios.c_cflag |= (CS8);
           current_tios.c_oflag &= ~(OPOST);
           */
           current_tios.c_cc[VMIN] = 1;
           current_tios.c_cc[VTIME] = 0;
       }
   }else{
       memcpy(&current_tios, &orig_tios, sizeof(struct termios));
       current_tios.c_lflag |= ECHO;
   }

   if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &current_tios) != -1){
       return SUCCESS;
   }

   return ERROR; 
}
