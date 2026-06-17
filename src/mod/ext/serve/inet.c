#include <external.h>
#include <caneka.h> 


Str *Ip4_ToStr(MemCh *m, quad ip4){
    byte *ib = (byte *)&ip4;
    Str *s = Str_Make(m, IP4_STR_ALLOC);
    for(i32 i = 0; i < 4; i++){
        Str_AddIByte(s, ib[i]);
        if(i < 3){
            Str_Add(s, (byte *)".", 1);
        }
    }
    return s;
}

quad Quad_ToIp4(byte a, byte b, byte c, byte d){
    quad ip4 = 0;
    ip4 += a ;
    ip4 += b << 8;
    ip4 += c << 16;
    ip4 += d << 24;
    return ip4;
}

quad Str_ToIp4(MemCh *m, Str *s){
    quad ip4 = 0;
    i8 segment = 0;
    byte *b = s->bytes;
    byte *ptr = s->bytes;
    byte *end = s->bytes + (s->length-1);
    Str ns =  {
        .type = {TYPE_STR, STRING_CONST},
        .length = 0,
        .alloc = 0,
        .bytes = NULL
    };
    while(ptr <= end){
        if(*ptr == '.' || ptr == end){
            ns.length = (ptr) - b;
            if(ptr == end){
                ns.length++;
            }
            ns.alloc = ns.length; 
            ns.bytes = b;
            ip4 += Int_FromStr(&ns) << (8 * segment);
            b = ptr+1;
            segment++;
        }
        ptr++;
    }
    return ip4;
}
