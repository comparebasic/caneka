#include <external.h>
#include <caneka.h>

status Oset_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();
    Span *p;
    String *s;
    String *os;
    Abstract *a;

    s = String_Make(m, bytes("hi:s/5=there;"));
    a = Abs_FromOset(m, s);
    r |= Test(String_EqualsBytes((String *)a, bytes("there")), "String equals expected, have '%s'", ((String *)a)->bytes);

    s = String_Make(m, bytes("count:i64/2=17;"));
    a = Abs_FromOset(m, s);

    r |= Test(a->type.of == TYPE_WRAPPED_I64, "Have wrapped I64");
    Single *wr = (Single *)a;
    r |= Test(wr->val.value == 17, "Value of I64 matches, have %ld", wr->val.value);

    s = String_Make(m, bytes("span/2=(s/2=hi;s/5=there;)"));
    a = Abs_FromOset(m, s);
    p = (Span *)a;

    r |= Test((p->type.of == TYPE_SPAN), "Span from oset has correct type have '%s'", Class_ToString(p->type.of));
    r |= Test((p->nvalues == 2), "Span from oset has correct number of values have '%d'", p->nvalues);

    os = (String *)Span_Get(p, 0);
    r |= Test((os->length == 2), "Span from oset string 1 length is equal, have '%d'", os->length);
    r |= Test(String_EqualsBytes(os, bytes("hi")), "Span from oset string 1 equal, have '%s'", os->bytes);

    os = (String *)Span_Get(p, 1);
    r |= Test((os->length == 5), "Span from oset string 2 length is equal, have '%d'", os->length);
    r |= Test(String_EqualsBytes(os, bytes("there")), "Span from oset string 2 equal, have '%s'", os->bytes);

    p = Span_Make(m, TYPE_SPAN);
    Span_Add(p, (Abstract *)String_Make(m, bytes("poo-head")));
    Span_Add(p, (Abstract *)String_Make(m, bytes("fancy-pants")));
    Span_Add(p, (Abstract *)String_Make(m, bytes("final-fight")));

    os = Oset_To(m, NULL, (Abstract *)p);
    r |= Test(String_EqualsBytes(os, bytes("span/3=(s/8=poo-head;s/11=fancy-pants;s/11=final-fight;)")), "Span to oset string is equal, have '%s'", os->bytes);

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
    r |= Test(String_EqualsBytes(os, bytes("tbl/5={List:span/2=(s/3=one;s/3=two;)A:s/6=Apples;B:s/8=Bicycles;C:s/4=Cake;D:s/9=Dinasaurs;}")), "Table to oset string is equal, have '%s'", os->bytes);


    p = Span_Make(m, TYPE_SPAN);
    Span_Add(p, (Abstract *)String_Make(m, bytes("fist")));
    Span_Add(p, (Abstract *)String_Make(m, bytes("second")));
    Span_Set(p, 12, (Abstract *)String_Make(m, bytes("thirteenth")));

    os = Oset_To(m, NULL, (Abstract *)p);
    r |= Test(String_EqualsBytes(os, bytes("span/3=[0:s/4=fist;1:s/6=second;12:s/10=thirteenth;]")), "Span with gaps to have index keys is equal, have '%s'", os->bytes);


    MemCtx_Free(m);

    return r;
}
