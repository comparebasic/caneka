#include <external.h>
#include <caneka.h>

status TextFilter_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();
    String *s;

    s = String_Make(m, bytes("\tHi There\r\n"));

    r |= Test(TextCharFilter(s->bytes, s->length), "normal text is fine, '%s'", s->bytes);

    byte cstr[] = {3, 21, 31, 'H', 'e', 'l', 'l', 'o', ' ','T','h','e','r','e', 2, '\0'};
    
    s = String_Make(m, cstr);
    r |= Test(!TextCharFilter(s->bytes, s->length), "normal text is fine, '%s'", String_Present(m, (Abstract *)s)->bytes);

    s = Buff(m);
    r |= Test(String_AddBytes(m, s, bytes("\tHi There\r\n"), strlen("\tHi There\r\n")) == SUCCESS, "SUCCESS from adding valid chars have, '%s'",  String_Present(m, (Abstract *)s)->bytes);

    int len = s->length;
    r |= Test(String_AddBytes(m, s, cstr, strlen((char *)cstr)) == ERROR, "ERROR from adding invalid chars have, '%s'",  String_Present(m, (Abstract *)s)->bytes);
    r |= Test(s->length == len, "Length unchanged from erronous bytes add request, have, '%s'",  String_Present(m, (Abstract *)s)->bytes);

    MemCtx_Free(m);
    return r;
}
