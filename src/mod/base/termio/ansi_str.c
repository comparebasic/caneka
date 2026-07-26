#include <external.h>
#include "base_module.h"

Str *FmtBlack = NULL;
Str *FmtRed = NULL;
Str *FmtYellow = NULL;
Str *FmtGreen = NULL;
Str *FmtPurple = NULL;
Str *FmtBlue = NULL;
Str *FmtCyan = NULL;
Str *FmtDark = NULL;

Str *AnsiBlack = NULL;
Str *AnsiRed = NULL;
Str *AnsiYellow = NULL;
Str *AnsiGreen = NULL;
Str *AnsiPurple = NULL;
Str *AnsiBlue = NULL;
Str *AnsiCyan = NULL;
Str *AnsiDark = NULL;

void Ansi_SetColor(Buff *bf, boolean yn){
    if(yn) {
        bf->type.state |= BUFF_COLOR;
    }else{
        bf->type.state &= ~BUFF_COLOR;
    }
}

boolean Ansi_HasColor(Buff *bf){
    return (bf->type.state & BUFF_COLOR) != 0;
}

Str *Str_ConsumeAnsi(Buff *bf, char **_ptr, char *end, boolean consume){
    MemCh *m = bf->m;
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
            if(ptr != *_ptr){
                ptr--;
            }
            break;
        }
        ptr++;
    }
    *(b++) = 'm';

    if(bf->type.state & BUFF_COLOR){
        s->length = (word)(b - s->bytes);
    }

    if(consume){
        *_ptr = ptr;
    }

    return s;
}

Str *Str_FromAnsi(Buff *bf, char **_ptr, char *end){
    return Str_ConsumeAnsi(bf, _ptr, end, FALSE);
}

Str *Str_AnsiCstr(Buff *bf, char *cstr){
    i64 length = strlen(cstr);
    return Str_FromAnsi(bf, &cstr, cstr+(length-1));
}

status AnsiStr_Init(MemCh *m){
    if(FmtBlack == NULL){
        FmtBlack = Str_Ref(m, (byte *)"^x.", 3, 4, STRING_FMT_ANSI|STRING_CONST);
        FmtRed = Str_Ref(m, (byte *)"^r.", 3, 4, STRING_FMT_ANSI|STRING_CONST);
        FmtYellow = Str_Ref(m, (byte *)"^y.", 3, 4, STRING_FMT_ANSI|STRING_CONST);
        FmtGreen = Str_Ref(m, (byte *)"^g.", 3, 4, STRING_FMT_ANSI|STRING_CONST);
        FmtPurple = Str_Ref(m, (byte *)"^p.", 3, 4, STRING_FMT_ANSI|STRING_CONST);
        FmtBlue = Str_Ref(m, (byte *)"^b.", 3, 4, STRING_FMT_ANSI|STRING_CONST);
        FmtCyan = Str_Ref(m, (byte *)"^c.", 3, 4, STRING_FMT_ANSI|STRING_CONST);
        FmtDark = Str_Ref(m, (byte *)"^k.", 3, 4, STRING_FMT_ANSI|STRING_CONST);

        AnsiBlack = Str_Ref(m, (byte *)"\x1b[30m", 5, 6, STRING_CONST);
        AnsiRed = Str_Ref(m, (byte *)"\x1b[31m", 5, 6, STRING_CONST);
        AnsiYellow = Str_Ref(m, (byte *)"\x1b[32m", 5, 6, STRING_CONST);
        AnsiGreen = Str_Ref(m, (byte *)"\x1b[33m", 5, 6, STRING_CONST);
        AnsiPurple = Str_Ref(m, (byte *)"\x1b[34m", 5, 6, STRING_CONST);
        AnsiBlue = Str_Ref(m, (byte *)"\x1b[35m", 5, 6, STRING_CONST);
        AnsiCyan = Str_Ref(m, (byte *)"\x1b[36m", 5, 6, STRING_CONST);
        AnsiDark = Str_Ref(m, (byte *)"\x1b[37m", 5, 6, STRING_CONST);

        return SUCCESS;
    }
    return NOOP;
}
