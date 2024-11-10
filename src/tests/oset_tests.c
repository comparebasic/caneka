#include <external.h>
#include <caneka.h>

status Oset_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();

    String *s = String_Make(m, bytes("hi:s/5=there;"));
    Abstract *a = Abs_FromOset(m, s);
    r |= Test(String_EqualsBytes((String *)a, bytes("there")), "String equals expected, have '%s'", ((String *)a)->bytes);

    s = String_Make(m, bytes("count:i64/2=17;"));
    a = Abs_FromOset(m, s);

    r |= Test(a->type.of == TYPE_WRAPPED_I64, "Have wrapped I64");
    Single *wr = (Single *)a;
    r |= Test(wr->val.value == 17, "Value of I64 matches, have %ld", wr->val.value);

    MemCtx_Free(m);

    return r;
}
