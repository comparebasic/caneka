#include <external.h>
#include <caneka.h>

Inst *Etag(MemCh *m, i32 idx, StrVec *name, util parity, struct timespec *ts){
    quad nameParity = HalfParity_FromVec(name);

    Str *nameParityHex = Bytes_ToHexStr(m, (byte *)&nameParity, sizeof(quad));

    Str *parityHex = Bytes_ToHexStr(m, (byte *)&parity, sizeof(util));

    Inst *etag = Inst_Make(m, TYPE_ETAG);
    Seel_Set(etag, K(m, "name"), name);
    Seel_Set(etag, K(m, "idx-str"), Str_FromI64(m, (i64)idx));
    Seel_Set(etag, K(m, "parity"), parityHex);
    Seel_Set(etag, K(m, "path-parity"), nameParityHex);
    Seel_Set(etag, K(m, "tag"), Etag_ToStr(etag));
    Seel_Set(etag, K(m, "last-modified"), Time_ToRStr(m, ts));

    return etag;
}

Str *Etag_ToStr(Inst *etag){
    MemCh *m = etag->m;
    Str *s = Str_Make(m, ETAG_MAX+1);
    Str_AddStr(s, Seel_Get(etag, K(m, "path-parity")));
    Str_Add(s, (byte *)"-", 1);
    Str_AddStr(s, Seel_Get(etag, K(m, "parity")));
    Str_Add(s, (byte *)"-", 1);
    Str_AddStr(s, Seel_Get(etag, K(m, "idx-str")));
    return s;
}

boolean Etag_CompareStr(Inst *etag, Str *etagStr){
    MemCh *m = etag->m;
    byte *b = etagStr->bytes;
    i16 length = etagStr->length;

    Str *nameParity = Str_Take(m, etagStr, sizeof(quad)*2);
    if(*etagStr->bytes != '-'){
        void *ar[] = {
            etagStr,
            NULL
        };
        Error(m, FILENAME, FUNCNAME, LINENUMBER, "Malformed etag", NULL);
        return FALSE;
    }else{
        Str_Incr(etagStr, 1);
    }
    Str *parity = Str_Take(m, etagStr, sizeof(util)*2);
    if(*etagStr->bytes != '-'){
        void *ar[] = {
            etagStr,
            NULL
        };
        Error(m, FILENAME, FUNCNAME, LINENUMBER, "Malformed etag", NULL);
        return FALSE;
    }else{
        Str_Incr(etagStr, 1);
    }
    Str *idxStr = Clone(m, etagStr);

    etagStr->bytes = b;
    etagStr->length = length;

    return Equals(Seel_Get(etag, K(m, "parity")), parity) &&
        Equals(Seel_Get(etag, K(m, "path-parity")), nameParity) &&
        Equals(Seel_Get(etag, K(m, "name")), nameParity);
}

status Etag_ClsInit(MemCh *m){
    status r = READY;
    Table *tbl = Table_Make(m);
    Table_Set(tbl, S(m, "name"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, S(m, "atts"), I16_Wrapped(m, TYPE_TABLE));
    Table_SetHashed(tbl, S(m, "children"), I16_Wrapped(m, TYPE_TABLE));
    Table_Set(tbl, S(m, "idx-str"), I16_Wrapped(m, TYPE_STR));
    Table_Set(tbl, S(m, "parity"), I16_Wrapped(m, TYPE_STR));
    Table_Set(tbl, S(m, "path-parity"), I16_Wrapped(m, TYPE_STR));
    Table_Set(tbl, S(m, "tag"), I16_Wrapped(m, TYPE_STR));
    Table_Set(tbl, S(m, "last-modified"), I16_Wrapped(m, TYPE_STR));
    r |= Seel_Seel(m, tbl, S(m, "Etag"), TYPE_ETAG);
    return r;
}
