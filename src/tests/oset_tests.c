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

    Span *p = Span_Make(m, TYPE_SPAN);
    Span_Add(p, (Abstract *)String_Make(m, bytes("poo-head")));
    Span_Add(p, (Abstract *)String_Make(m, bytes("fancy-pants")));
    Span_Add(p, (Abstract *)String_Make(m, bytes("final-fight")));

    String *os = Oset_To(m, NULL, (Abstract *)p);
    r |= Test(String_EqualsBytes(os, bytes("span/3=[s/8=poo-head;s/11=fancy-pants;s/11=final-fight;]")), "Span to oset string is equal, have '%s'", os->bytes);

    Span *tbl = Span_Make(m, TYPE_TABLE);
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("A")),(Abstract *)String_Make(m, bytes("Apples")));
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("B")),(Abstract *)String_Make(m, bytes("Bicycles")));
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("C")),(Abstract *)String_Make(m, bytes("Cake")));
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("D")),(Abstract *)String_Make(m, bytes("Dinasaurs")));

    os = Oset_To(m, NULL, (Abstract *)tbl);
    r |= Test(String_EqualsBytes(os, bytes("tbl/4={A:s/6=Apples;B:s/8=Bicycles;C:s/4=Cake;D:s/9=Dinasaurs;}")), "Table to oset string is equal, have '%s'", os->bytes);

    p = Span_Make(m, TYPE_SPAN);
    Span_Add(p, (Abstract *)String_Make(m, bytes("one")));
    Span_Add(p, (Abstract *)String_Make(m, bytes("two")));
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("List")),(Abstract *)p);
    os = Oset_To(m, NULL, (Abstract *)tbl);
    r |= Test(String_EqualsBytes(os, bytes("tbl/5={List:span/2=[s/3=one;s/3=two;]A:s/6=Apples;B:s/8=Bicycles;C:s/4=Cake;D:s/9=Dinasaurs;}")), "Table to oset string is equal, have '%s'", os->bytes);



    MemCtx_Free(m);

    return r;
}
