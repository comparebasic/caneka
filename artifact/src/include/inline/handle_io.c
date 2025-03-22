#ifndef __HANDLE_IO
#define __HANDLE_IO 1
static inline i64 handleIo(StrVec *v, i32 fd, Str *s){
    if(v == NULL && fd >= 0){
        return Str_ToFd(s, fd);
    }else{
        return StrVec_Add(v, s);
    }
}

static inline i64 handleVecIo(StrVec *v, i32 fd, StrVec *v2){
    if(v == NULL && fd >= 0){
        return StrVec_ToFd(v2, fd);
    }else{
        return StrVec_AddVec(v, v2);
    }
}

static inline boolean TextCharFilter(byte *b, i64 length){
    byte *end = b+length;
    while(b < end){
        byte c = *b;
        if(!IS_VISIBLE(c)){
            return FALSE;
        }
        b++;
    }
    return TRUE;
}
#endif
