#include <external.h>
#include <caneka.h>

static byte *b64_chars = bytes("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
static byte *pad4 = bytes("====");
static byte zero4[] = {0,0,0,0};
static byte b64CharValue(byte b){

    if(b >= 'A' && b <= 'Z'){
        return b - 'A';
    }else if(b >= 'a' && b <= 'z'){
        return b - 'a' + 26;
    }else if(b >= '0' && b <= '9'){
        return b - '0' + 52;
    }else if (b == '+'){
        return 62;
    }else if (b == '/'){
        return 63;
    }else if(b == '='){
        return 0;
    }else{
        printf("'%c' %hd\n", b, b);
        Fatal("Character out of range for Base64", TYPE_STRING);
        return 0;
    }
}

String *String_ToB64(MemCtx *m, String *s){
    DebugStack_Push(s, s->type.of);
    byte shelf3[3];
    byte out4[4];
    byte *b = NULL;
    String *bs = String_Init(m, STRING_EXTEND);

    IterStr it;
    IterStr_Init(&it, s, sizeof(shelf3), &shelf3);
    it.type.state |= FLAG_ITERSTR_PAD;
    while((IterStr_Next(&it) & (END|ERROR)) == 0){
        b = (byte *) IterStr_Get(&it); 
        byte b0 = *(b+0);
        byte b1 = *(b+1);
        byte b2 = *(b+2);
        out4[0] = (byte)*(b64_chars+(b0 >> 2)); 
        out4[1] = (byte)*(b64_chars+(((b0 & 3) << 4 ) | ((b1 & 240) >> 4))); 
        out4[2] = (byte)*(b64_chars+(((b1 & 15) << 2 ) | ((b2 & 192) >> 6))); 
        out4[3] = (byte)*(b64_chars+((b2 & 63))); 

        if(it.type.state & FLAG_ITER_LAST){
            i64 padding = Cursor_GetPad(&(it.cursor), sizeof(shelf3));
            String_AddBytes(m, bs, out4, sizeof(out4)-padding);
            String_AddBytes(m, bs, pad4, padding);
        }else{
            String_AddBytes(m, bs, out4, sizeof(out4));
        }
    }

    DebugStack_Pop();
    return bs;
}

String *String_FromB64(MemCtx *m, String *bs){
    DebugStack_Push(bs, bs->type.of);

    byte shelf4[4];
    byte out3[3];
    byte *b = NULL;
    String *s = String_Init(m, STRING_EXTEND);

    IterStr it;
    IterStr_Init(&it, bs, sizeof(shelf4), &shelf4);
    while((IterStr_Next(&it) & (END|ERROR)) == 0){
        b = (byte *) IterStr_Get(&it); 
        byte b0 = b64CharValue(*(b+0));
        byte b1 = b64CharValue(*(b+1));
        byte b2 = b64CharValue(*(b+2));
        byte b3 = b64CharValue(*(b+3));

        out3[0] = (b0 << 2) | (b1 >> 4);
        out3[1] = (b1 << 4) | (b2 >> 2);
        out3[2] = (b2 << 6) | b3;

        if(it.type.state & FLAG_ITER_LAST){
            int padding = 0;
            byte *_b = b;
            byte *_end = _b+3;
            while(_b <= _end){ 
                if(*_b == '='){
                    padding++;
                }
                _b++;
            }
            String_AddBytes(m, s, out3, sizeof(out3)-padding);
        }else{
            String_AddBytes(m, s, out3, sizeof(out3));
        }
    }

    s->type.state &= (it.type.state & ERROR);

    DebugStack_Pop();
    return s;
}
