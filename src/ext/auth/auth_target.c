#include <external.h>
#include <caneka.h>

#include <gen/salt.h>

status AuthFuncI(MemCh *m, AuthTarget *target, digest *hash){
    /*
    if(target->bin.i == 0){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Cannot authenticate a zero util", NULL);
        return ERROR;
    }

    i32 len = SALT_LENGTH;
    byte b[SALT_LENGTH];
    Str s = {
        .type = {TYPE_STR, STRING_CONST},
        .length = SALT_LENGTH,
        .alloc = SALT_LENGTH,
        .bytes = b
    };

    i32 offset = (target->bin.i * _salt.fact % len);
    i32 remains = len - sizeof(i32) - offset;

    byte *ptr = s.bytes;
    memcpy(ptr, _salt.salt512 + offset, remains);
    ptr += remains;

    memcpy(ptr, &target->bin.i, sizeof(i32));
    ptr += sizeof(i32);

    memcpy(ptr, _salt.salt512, offset);
    
    return Str_ToSha256(m, &s, hash);
    */
    return ZERO;
}

status AuthFuncU(MemCh *m, AuthTarget *target, digest *hash){
    /*
    if(target->bin.u == 0){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Cannot authenticate a zero util", NULL);
        return ERROR;
    }

    i32 len = SALT_LENGTH;
    byte b[SALT_LENGTH];
    Str s = {
        .type = {TYPE_STR, STRING_CONST},
        .length = SALT_LENGTH,
        .alloc = SALT_LENGTH,
        .bytes = b
    };

    i32 offset = (target->bin.u * _salt.fact % len);
    i32 remains = len - sizeof(util) - offset;

    byte *ptr = s.bytes;
    memcpy(ptr, _salt.salt512 + offset, remains);
    ptr += remains;

    memcpy(ptr, &target->bin.u, sizeof(util));
    ptr += sizeof(util);

    memcpy(ptr, _salt.salt512, offset);
    
    return Str_ToSha256(m, &s, hash);
}



status AuthFuncVec(MemCh *m, AuthTarget *target, digest *hash){
    as(target->bin.text, TYPE_STRVEC);
    if(target->bin.text->total == 0){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Cannot authenticate a null or zero length text body", NULL);
        return ERROR;
    }

    i32 len = SALT_LENGTH;
    byte b[SALT_LENGTH];
    Str s = {
        .type = {TYPE_STR, STRING_CONST},
        .length = SALT_LENGTH,
        .alloc = SALT_LENGTH,
        .bytes = b
    };

    if(target->bin.text->total > len){
        return StrVec_ToSha256(m, target->bin.text, hash);
    }else{
        i32 offset = target->bin.text->total;
        i32 remains = len - offset;

        byte *ptr = s.bytes;
        memcpy(ptr, _salt.salt512 + offset, remains);
        ptr += remains;

        Iter it;
        Iter_Init(&it, target->bin.text->p);
        while((Iter_Next(&it) & END) == 0){
            Str *sa = (Str *)Iter_Get(&it);
            memcpy(ptr, sa->bytes, sa->length);
        }
        
        return Str_ToSha256(m, &s, hash);
    }
}

status AuthFuncIp6(MemCh *m, AuthTarget *target, digest *hash){
    util *pair = target->bin.u128;
    if((pair[0] + pair[1]) == 0){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Cannot authenticate a zero ipv6", NULL);
        return ERROR;
    }

    i32 len = SALT_LENGTH;
    byte b[SALT_LENGTH];
    Str s = {
        .type = {TYPE_STR, STRING_CONST},
        .length = SALT_LENGTH,
        .alloc = SALT_LENGTH,
        .bytes = b
    };

    i32 offset = ((pair[1] + pair[2]) * _salt.fact % len);
    i32 remains = len - sizeof(pair) - offset;

    byte *ptr = s.bytes;
    memcpy(ptr, _salt.salt512 + offset, remains);
    ptr += remains;

    memcpy(ptr, pair, sizeof(util) * 2);
    ptr += sizeof(util) * 2;

    memcpy(ptr, &_salt.salt512, offset);
    
    return Str_ToSha256(m, &s, hash);
    */
    return ZERO;
}

AuthTarget *AuthTarget_Make(MemCh *m, word flags){
    AuthTarget *tg = (AuthTarget *)MemCh_AllocOf(m, sizeof(AuthTarget), TYPE_AUTH_TARGET);
    tg->type.of = TYPE_AUTH_TARGET;
    tg->type.state = flags;
    return tg;
}

status AuthTarget_Init(MemCh *m, digest *hash);
