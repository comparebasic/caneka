#ifndef __HANDLE_IO
#define __HANDLE_IO 1
static inline i64 handleIo(StrVec *v, i32 fd, Str *s){
    if(v == NULL && fd >= 0){
        return Str_ToFd(s, fd);
    }else{
        return StrVec_Add(v, s);
    }
}
#endif
