#include <external.h>
#include <caneka.h>

Str *Str_ConsumeAnsi(MemCh *m, char **_ptr, char *end, boolean consume){
    char *ptr = *_ptr;
    char c;
    Str *s = Str_Make(m, ANSI_ESCAPE_MAX);
    byte *_s = s->bytes; 
    byte *b = s->bytes; 
    byte *e = s->bytes+s->alloc-1;
    *(b++) = KEY_ESCAPE;
    *(b++) = '[';
    byte *start = b;
    while(b <= e && ptr <= end){
        c = *(ptr);
        if(c == '0'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '0';
        }else if(c == 'E'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '7';
        }else if(c == 'e'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '2';
            *(b++) = '7';
        }else if(c == 'L'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '5';
        }else if(c == 'l'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '2';
            *(b++) = '5';
        }else if(c == 'U'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '4';
        }else if(c == 'u'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '2';
            *(b++) = '4';
        }else if(c == 'I'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '3';
        }else if(c == 'i'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '2';
            *(b++) = '3';
        }else if(c == 'd'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '2';
            *(b++) = '2';
        }else if(c == 'D'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '1';
        }else if(c == 'r'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '3';
            *(b++) = '1';
        }else if(c == 'R'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '4';
            *(b++) = '1';
        }else if(c == 'g'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '3';
            *(b++) = '2';
        }else if(c == 'G'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '4';
            *(b++) = '2';
        }else if(c == 'y'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '3';
            *(b++) = '3';
        }else if(c == 'Y'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '4';
            *(b++) = '3';
        }else if(c == 'b'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '3';
            *(b++) = '4';
        }else if(c == 'B'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '4';
            *(b++) = '4';
        }else if(c == 'p'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '3';
            *(b++) = '5';
        }else if(c == 'P'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '4';
            *(b++) = '5';
        }else if(c == 'c'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '3';
            *(b++) = '6';
        }else if(c == 'C'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '4';
            *(b++) = '6';
        }else if(c == 'k'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '3';
            *(b++) = '7';
        }else if(c == 'K'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '4';
            *(b++) = '7';
        }else if(c == 'x'){
            if(b > start){
                *(b++) = ';';
            }
            *(b++) = '3';
            *(b++) = '0';
        }else if(c == 'X'){
            if(b > start){
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
    if(consume){
        *_ptr = ptr;
    }
    return s;
}

Str *Str_FromAnsi(MemCh *m, char **_ptr, char *end){
    return Str_ConsumeAnsi(m, _ptr, end, FALSE);
}

Str *Str_AnsiCstr(MemCh *m, char *cstr){
    i64 length = strlen(cstr);
    return Str_FromAnsi(m, &cstr, cstr+(length-1));
}

