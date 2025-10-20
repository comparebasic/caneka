#include <external.h>
#include <caneka.h>

#include <auth_inc/salt.h>

status AuthFuncI(MemCh *m, AuthTarget *target, digest *hash){
    if(target->val->bin.i == 0){
        Error(m, FUNCNAME, FILENAME, LICENCE,
            "Cannot authenticate a zero util", NULL);
        return ERROR;
    }

    i32 len = 512;
    byte b[512];
    Str s = {
        .type = {TYPE_STR, STRING_CONST},
        .length = 512,
        .alloc = 512,
        .bytes = &b
    }

    i32 offset = (target->val->bin.i * _salt.start % len);
    i32 remains = len - sizeof(i32) - offset;

    byte *ptr = s.bytes;
    memcpy(ptr, (&_salt.salt512) + offset, remains);
    ptr += remains;

    memcpy(ptr, &target->val.bin.i, sizeof(i32));
    ptr += sizeof(i32);

    memcpy(ptr, &_salt.salt512, offset);
    
    return Str_ToSha256(m, &s, hash);
}

status AuthFuncU(MemCh *m, AuthTarget *target, digest *hash){
    if(target->val->bin.u == 0){
        Error(m, FUNCNAME, FILENAME, LICENCE,
            "Cannot authenticate a zero util", NULL);
        return ERROR;
    }

    i32 len = 512;
    byte b[512];
    Str s = {
        .type = {TYPE_STR, STRING_CONST},
        .length = 512,
        .alloc = 512,
        .bytes = &b
    }

    i32 offset = (target->val->bin.u * _salt.start % len);
    i32 remains = len - sizeof(util) - offset;

    byte *ptr = s.bytes;
    memcpy(ptr, (&_salt.salt512) + offset, remains);
    ptr += remains;

    memcpy(ptr, &target->val.bin.u, sizeof(util));
    ptr += sizeof(util);

    memcpy(ptr, &_salt.salt512, offset);
    
    return Str_ToSha256(m, &s, hash);
}



status AuthFuncVec(MemCh *m, AuthTarget *target, digest *hash){
    as(target->val.text, TYPE_STRVEC);
    if(target->val.text->total == 0){
        Error(m, FUNCNAME, FILENAME, LICENCE,
            "Cannot authenticate a null or zero length text body", NULL);
        return ERROR;
    }

    i32 len = 512;
    byte b[512];
    Str s = {
        .type = {TYPE_STR, STRING_CONST},
        .length = 512,
        .alloc = 512,
        .bytes = &b
    }

    if(target->val.text->length > len){
        return StrVec_ToSha256(m, target->val.text, hash);
    }else{
        i32 offset = target->val.text->total;
        i32 remains = len - offset;

        byte *ptr = s.bytes;
        memcpy(ptr, (&_salt.salt512) + offset, remains);
        ptr += remains;

        Iter it;
        Iter_Init(&it, target->val.text->p);
        while((Iter_Next(&it) & END) == 0){
            Str *sa = (Str *)Iter_Get(&it);
            memcpy(ptr, sa->bytes, sa->length);
        }
        
        return Str_ToSha256(m, &s, hash);
    }
}

status AuthFuncIp6(MemCh *m, AuthTarget *target, digest *hash){
    util pair[2] = (util *)target->val.u128;
    if((pair[0] + pair[1]) == 0){
        Error(m, FUNCNAME, FILENAME, LICENCE,
            "Cannot authenticate a zero ipv6", NULL);
        return ERROR;
    }

    i32 len = 512;
    byte b[512];
    Str s = {
        .type = {TYPE_STR, STRING_CONST},
        .length = 512,
        .alloc = 512,
        .bytes = &b
    }

    i32 offset = ((pair[1] + pair[2]) * _salt.start % len);
    i32 remains = len - sizeof(pair) - offset;

    byte *ptr = s.bytes;
    memcpy(ptr, (&_salt.salt512) + offset, remains);
    ptr += remains;

    memcpy(ptr, &pair, sizeof(pair));
    ptr += sizeof(pair);

    memcpy(ptr, &_salt.salt512, offset);
    
    return Str_ToSha256(m, &s, hash);

}

AuthTarget *AuthTarget_Make(MemCh *m, type flags){
    AuthTarget *tg = (AuthTarget *)MemCh_AllocOf(m, sizeof(AuthTarget), TYPE_AUTH_TARGET);
    tg->type.of = TYPE_AUTH_TARGET;
    tg->type.state = flags;
    return tg;
}

status AuthTarget_Init(MemCh *m, digest *hash);
