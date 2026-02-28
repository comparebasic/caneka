#include <external.h>
#include <caneka.h>

#include <gen/salt.h>
#include <gen/funny.h>

status Password_OnStr(MemCh *m, Str *dest, Str *pw, Str *salt){
    /*
    if(pw->length < 2 || pw->length > PASSWORD_UNIT_LENGTH ||
            dest->alloc != DIGEST_SIZE ||
            salt->length != PASSWORD_UNIT_LENGTH){ 
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "An argument to Password_OnStr has an innapropriate size", NULL);
        return ERROR;
    }

    byte slate[PASSWORD_UNIT_LENGTH*2];
    Str s = {
        .type = {TYPE_STR, STRING_CONST},
        .length = 0,
        .alloc = PASSWORD_UNIT_LENGTH,
        .bytes = slate
    };
    i16 funny = _funny & (PASSWORD_UNIT_LENGTH-1);

    word firstTwo = *((word *)pw->bytes);
    i16 pos = (i16)((firstTwo+funny) & (PASSWORD_UNIT_LENGTH-1));

    if((pos+pw->length) > PASSWORD_UNIT_LENGTH){
        i16 length = (pos+pw->length)-PASSWORD_UNIT_LENGTH;
        i16 offset = pw->length - length;
        memcpy(slate, pw->bytes+offset, length);

        i16 taken = offset;
        offset = length;
        length = PASSWORD_UNIT_LENGTH-(pw->length);
        memcpy(slate+offset, salt->bytes+taken, length);

        offset += length;
        memcpy(slate+offset, pw->bytes, taken);
    }else{
        i16 offset = PASSWORD_UNIT_LENGTH-pos;
        if(pos > 0){
            memcpy(slate, salt->bytes+offset, pos);
        }

        offset = pos;
        memcpy(slate+offset, pw->bytes, pw->length);
        offset += pw->length;

        i16 length = PASSWORD_UNIT_LENGTH-offset;
        memcpy(slate+offset, salt->bytes, length);
    }

    memcpy(slate+PASSWORD_UNIT_LENGTH, slate, PASSWORD_UNIT_LENGTH-pos);

    s.length = PASSWORD_UNIT_LENGTH+(PASSWORD_UNIT_LENGTH-pos);

    if(Str_ToSha256(m, &s, (digest *)dest->bytes) & SUCCESS){
        dest->length = DIGEST_SIZE;
        return SUCCESS;
    }
    */
    return ERROR|NOOP;
}

status Password_Set(Buff *bf, Str *pw, Str *salt){
    return ZERO; 
}

status Password_Check(Buff *bf, Str *pw, Str *salt){
    return ZERO; 
}
