#include <external.h>
#include <caneka.h>

status Password_OnStr(MemCh *m, Str *dest, Str *pw, Str *salt){
    byte slate[PASSWORD_UNIT_LENGTH];
    if(pw->length < 2 || pw->length > PASSWORD_UNIT_LENGTH ||
            dest->alloc != DIGEST_SIZE ||
            salt->length != PASSWORD_UNIT_LENGTH){ 
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "An argument to Password_OnStr has an innapropriate size", NULL);
        return ERROR;
    }

    word *firstTwo = (word *)pw->bytes;
    i16 shift = (i16)(*firstTwo & (PASSWORD_UNIT_LENGTH-1));
    i16 pos = (shift+pw->length)-PASSWORD_UNIT_LENGTH;

    if(pos < 0){
        i16 length = abs((i32)pos);
        i16 offset = pw->length - length;
        memcpy(slate, pw->bytes+offset, length);

        offset = length;
        i16 gapend = PASSWORD_UNIT_LENGTH-(pw->length-offset);
        length = gapend - offset;
        memcpy(slate+offset, salt->bytes, length);

        length = PASSWORD_UNIT_LENGTH-gapend;
        offset = gapend;
        memcpy(slate+offset, pw->bytes, length);
    }else{
        memcpy(slate, salt->bytes, pos);

        i16 offset = pos;
        memcpy(slate+offset, pw->bytes, pw->length);
        offset += pw->length;

        i16 length = PASSWORD_UNIT_LENGTH-offset;
        memcpy(slate, salt->bytes+pos, length);
    }

    Str s = {
        .type = {TYPE_STR, STRING_CONST},
        .length = PASSWORD_UNIT_LENGTH,
        .alloc = PASSWORD_UNIT_LENGTH,
        .bytes = slate
    };

    if(Str_ToSha256(m, &s, (digest *)dest->bytes) & SUCCESS){
        dest->length = DIGEST_SIZE;
        return SUCCESS;
    }
    return ERROR|NOOP;
}

status Password_Set(Buff *bf, Str *pw, Str *salt){
    return ZERO; 
}

status Password_Check(Buff *bf, Str *pw, Str *salt){
    return ZERO; 
}
