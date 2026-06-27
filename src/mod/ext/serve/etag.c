#include <external.h>
#include <caneka.h>

Str *Etag(MemCh *m, i32 idx, Str *name, util parity){
    Str *s = Str_Make(m, ETAG_MAX+1);
    quad nameParity = HalfParity_From(name);

    byte *ptr = s->bytes;
    byte *end = s->bytes+s->length-1;
    ptr += Str_I64OnBytes(ptr, end, idx);
    ptr++;
    *ptr = '-';
    i64 length = sizeof(nameParity);
    Bytes_ToHexOntoStr(m, (byte *)&nameParity, ptr, s, length);
    ptr += length;
    *ptr = '-';
    Bytes_ToHexOntoStr(m, (byte *)&parity, ptr, s, sizeof(parity));

    return s;
}
