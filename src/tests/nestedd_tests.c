#include <external.h>
#include <caneka.h>

status NestedDFlat_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    String *s;
    status r = SUCCESS;

    Span *tbl = NULL;
    NestedD *nd = NULL;
    Abstract *value = NULL;
    Single *sg = NULL;

    tbl = Span_Make(m, TYPE_TABLE);
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("yes")), (Abstract *)Int_Wrapped(m, TRUE));
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("no")), (Abstract *)Int_Wrapped(m, FALSE));
    nd = NestedD_Make(m, tbl);

    sg = (Single *)NestedD_Get(nd, (Abstract *)String_Make(m, bytes("yes")));
    r |= Test(sg->val.value == TRUE, "Test if nested value is expected, have %ld", sg->val.value);

    sg = (Single *)NestedD_Get(nd, (Abstract *)String_Make(m, bytes("no")));
    r |= Test(sg->val.value == FALSE, "Test if nested second value is expected, have %ld", sg->val.value);

    MemCtx_Free(m);
    return r;
}

status NestedDWith_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    String *s;
    status r = SUCCESS;

    Span *tbl = NULL;
    NestedD *nd = NULL;
    Abstract *value = NULL;
    Single *sg = NULL;

    Span *session = Span_Make(m, TYPE_TABLE);
    Table_Set(session, (Abstract *)String_Make(m, bytes("scid")), (Abstract *)String_Make(m, bytes("a87c782a")));
    Table_Set(session, (Abstract *)String_Make(m, bytes("domain")), (Abstract *)String_Make(m, bytes("customer")));
    struct timespec ts = {1709645616, 277122774};
    Table_Set(session, (Abstract *)String_Make(m, bytes("expires")), (Abstract *)I64_Wrapped(m, Time64_FromSpec(&ts)));

    tbl = Span_Make(m, TYPE_TABLE);
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("name")), (Abstract *)String_Make(m, bytes("SuppaSuppa")));
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("title")), (Abstract *)String_Make(m, bytes("Master Of It All")));
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("session")), (Abstract *)session);

    nd = NestedD_Make(m, tbl);

    value = NestedD_Get(nd, (Abstract *)String_Make(m, bytes("name")));
    r = Test(String_EqualsBytes((String *)value, bytes("SuppaSuppa")),
        "Name on main level is as expected, found '%s'", ((String *)value)->bytes);

    NestedD_With(m, nd, (Abstract *)String_Make(m, bytes("session")));

    value = NestedD_Get(nd, (Abstract *)String_Make(m, bytes("scid")));
    r |= Test(String_EqualsBytes((String *)value, bytes("a87c782a")), 
        "With session scid is as expected, found '%s'", ((String *)value)->bytes);

    sg = (Single *)NestedD_Get(nd, (Abstract *)String_Make(m, bytes("expires")));
    i64 expected_tm = Time64_FromSpec(&ts);
    r |= Test(sg->val.value == expected_tm,
        "With session time is as expected, found '%s'", String_FromI64(m, (sg->val.value))->bytes);

    NestedD_Outdent(nd);

    value = NestedD_Get(nd, (Abstract *)String_Make(m, bytes("title")));

    String *expected_s = String_Make(m, bytes("Master Of It All"));
    r |= Test(String_EqualsBytes((String *)value, bytes("Master Of It All")), "Title after outdent on main found '%s'", ((String *)value)->bytes);

    MemCtx_Free(m);
    return r;
}

status NestedDFor_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    String *s;
    status r = SUCCESS;

    Span *tbl = NULL;
    NestedD *nd = NULL;
    Abstract *value = NULL;
    Single *sg = NULL;
    status state = READY;

    Span *hobby1 = Span_Make(m, TYPE_TABLE);
    Table_Set(hobby1, (Abstract *)String_Make(m, bytes("name")),
        (Abstract *)String_Make(m, bytes("Heavy Metal")));
    Table_Set(hobby1, (Abstract *)String_Make(m, bytes("category")),
        (Abstract *)String_Make(m, bytes("music")));
    Table_Set(hobby1, (Abstract *)String_Make(m, bytes("status")),
        (Abstract *)Int_Wrapped(m, 2));

    Span *hobby2 = Span_Make(m, TYPE_TABLE);
    Table_Set(hobby2, (Abstract *)String_Make(m, bytes("name")),
        (Abstract *)String_Make(m, bytes("Knitting")));
    Table_Set(hobby2, (Abstract *)String_Make(m, bytes("category")),
        (Abstract *)String_Make(m, bytes("sewing")));
    Table_Set(hobby2, (Abstract *)String_Make(m, bytes("status")),
        (Abstract *)Int_Wrapped(m, 7));

    Span *hobby3 = Span_Make(m, TYPE_TABLE);
    Table_Set(hobby3, (Abstract *)String_Make(m, bytes("name")),
        (Abstract *)String_Make(m, bytes("Ramen")));
    Table_Set(hobby3, (Abstract *)String_Make(m, bytes("category")),
        (Abstract *)String_Make(m, bytes("cooking")));
    Table_Set(hobby3, (Abstract *)String_Make(m, bytes("status")),
        (Abstract *)Int_Wrapped(m, 99));

    Span *hobbies_sp = Span_Make(m, TYPE_SPAN);
    Span_Add(hobbies_sp, (Abstract *)hobby1);
    Span_Add(hobbies_sp, (Abstract *)hobby2);
    Span_Add(hobbies_sp, (Abstract *)hobby3);

    tbl = Span_Make(m, TYPE_TABLE);
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("name")), (Abstract *)String_Make(m, bytes("SuppaSuppa")));
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("title")), (Abstract *)String_Make(m, bytes("Master Of It All")));
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("hobbies")), (Abstract *)hobbies_sp);

    nd = NestedD_Make(m, tbl);

    value = NestedD_Get(nd, (Abstract *)String_Make(m, bytes("name")));
    r |= Test(String_EqualsBytes((String *)value, bytes("SuppaSuppa")), "Name on main level is as expected, found %s", ((String *)value)->bytes);

    NestedD_For(nd, (Abstract *)String_Make(m, bytes("hobbies")));

    state = NestedD_Next(nd);
    r |= Test((state & END) == 0, "Not yet done");
    value = NestedD_Get(nd, (Abstract *)String_Make(m, bytes("name")));
    r |= Test(String_EqualsBytes((String *)value, bytes("Heavy Metal")),
        "Nested FOR 'name' is as expected, found '%s'", ((String *)value)->bytes);

    value = NestedD_Get(nd, (Abstract *)String_Make(m, bytes("category")));
    r |= Test(String_EqualsBytes((String *)value, bytes("music")), 
        "Nested FOR 'category' is as expected, found '%s'", ((String *)value)->bytes);

    sg = (Single *)NestedD_Get(nd, (Abstract *)String_Make(m, bytes("status")));
    r |= Test(sg->val.value == 2, "Nested FOR 'status' is as expected, found '%ld'", sg->val.value);

    state = NestedD_Next(nd);
    r |= Test((state & END) == 0, "Not yet done");
    value = NestedD_Get(nd, (Abstract *)String_Make(m, bytes("name")));
    r |= Test(String_EqualsBytes((String *)value, bytes("Knitting")),
        "Nested FOR 'name' is as expected, found '%s'", ((String *)value)->bytes);

    value = NestedD_Get(nd, (Abstract *)String_Make(m, bytes("category")));
    r |= Test(String_EqualsBytes((String *)value, bytes("sewing")), 
        "Nested FOR 'category' is as expected, found '%s'", ((String *)value)->bytes);

    sg = (Single *)NestedD_Get(nd, (Abstract *)String_Make(m, bytes("status")));
    r |= Test(sg->val.value == 7, "Nested FOR 'status' is as expected, found '%ld'", sg->val.value);

    state = NestedD_Next(nd);
    r |= Test((state & END) == 0, "Not yet done");
    value = NestedD_Get(nd, (Abstract *)String_Make(m, bytes("name")));
    r |= Test(String_EqualsBytes((String *)value, bytes("Ramen")),
        "Nested FOR 'name' is as expected, found '%s'", ((String *)value)->bytes);

    value = NestedD_Get(nd, (Abstract *)String_Make(m, bytes("category")));
    r |= Test(String_EqualsBytes((String *)value, bytes("cooking")), 
        "Nested FOR 'category' is as expected, found '%s'", ((String *)value)->bytes);

    sg = (Single *)NestedD_Get(nd, (Abstract *)String_Make(m, bytes("status")));
    r |= Test(sg->val.value == 99, "Nested FOR 'status' is as expected, found '%ld'", sg->val.value);


    state = NestedD_Next(nd);
    r |= Test((state & END) != 0, "Nested_Next is done");


    MemCtx_Free(m);
    return r;
}
