#include <external.h>
#include <caneka.h>

Str *Str_FromAnsi(MemCtx *m, char **_ptr, char *end){
    char *ptr = *_ptr;
    char c;
    Str *s = Str_Make(m, ANSI_ESCAPE_MAX);
    byte *_s = s->bytes; 
    byte *b = s->bytes; 
    byte *e = s->bytes+s->alloc-1;
    *(b++) = KEY_ESCAPE;
    *(b++) = '[';
    while(b <= e && ptr <= end){
        c = *(ptr);
        if(c == '0'){
            if(b > _s){
                *(b++) = ';';
            }
            *(b++) = '0';
        }else if(c == 'd'){
            *(b++) = '2';
            *(b++) = '2';
        }else if(c == 'D'){
            *(b++) = '1';
        }else if(c == 'r'){
            if(b > _s){
                *(b++) = ';';
            }
            *(b++) = '3';
            *(b++) = '1';
        }else if(c == 'R'){
            if(b > _s){
                *(b++) = ';';
            }
            *(b++) = '4';
            *(b++) = '1';
        }else if(c == 'g'){
            if(b > _s){
                *(b++) = ';';
            }
            *(b++) = '3';
            *(b++) = '2';
        }else if(c == 'G'){
            if(b > _s){
                *(b++) = ';';
            }
            *(b++) = '4';
            *(b++) = '2';
        }else if(c == 'y'){
            if(b > _s){
                *(b++) = ';';
            }
            *(b++) = '3';
            *(b++) = '3';
        }else if(c == 'Y'){
            if(b > _s){
                *(b++) = ';';
            }
            *(b++) = '4';
            *(b++) = '3';
        }else if(c == 'b'){
            if(b > _s){
                *(b++) = ';';
            }
            *(b++) = '3';
            *(b++) = '4';
        }else if(c == 'B'){
            if(b > _s){
                *(b++) = ';';
            }
            *(b++) = '4';
            *(b++) = '4';
        }else if(c == 'p'){
            if(b > _s){
                *(b++) = ';';
            }
            *(b++) = '3';
            *(b++) = '5';
        }else if(c == 'P'){
            if(b > _s){
                *(b++) = ';';
            }
            *(b++) = '4';
            *(b++) = '5';
        }else if(c == 'c'){
            if(b > _s){
                *(b++) = ';';
            }
            *(b++) = '3';
            *(b++) = '6';
        }else if(c == 'C'){
            if(b > _s){
                *(b++) = ';';
            }
            *(b++) = '4';
            *(b++) = '6';
        }else if(c == 'd'){
            if(b > _s){
                *(b++) = ';';
            }
            *(b++) = '3';
            *(b++) = '7';
        }else if(c == 'D'){
            if(b > _s){
                *(b++) = ';';
            }
            *(b++) = '4';
            *(b++) = '7';
        }else if(c == 'x'){
            if(b > _s){
                *(b++) = ';';
            }
            *(b++) = '3';
            *(b++) = '0';
        }else if(c == 'X'){
            if(b > _s){
                *(b++) = ';';
            }
            *(b++) = '4';
            *(b++) = '0';
        }else if(c == '.'){
            break;
        }else{
            ptr--;
            break;
        }
        ptr++;
    }
    *(b++) = 'm';
    s->length = (word)(b - s->bytes);
    *_ptr = ptr;
    return s;
}
