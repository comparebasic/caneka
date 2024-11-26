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

    NestedD_Next(nd);
    value = NestedD_Get(nd, (Abstract *)String_Make(m, bytes("name")));
    r |= Test(String_EqualsBytes((String *)value, bytes("Heavy Metal")),
        "Nested FOR 'name' is as expected, found '%s'", ((String *)value)->bytes);

    value = NestedD_Get(nd, (Abstract *)String_Make(m, bytes("category")));
    r |= Test(String_EqualsBytes((String *)value, bytes("music")), 
        "Nested FOR 'category' is as expected, found '%s'", ((String *)value)->bytes);

    sg = (Single *)NestedD_Get(nd, (Abstract *)String_Make(m, bytes("status")));
    r |= Test(sg->val.value == 2, "Nested FOR 'status' is as expected, found '%ld'", sg->val.value);

    NestedD_Next(nd);
    /*
    r = NestedD_Next(m, nd);
    r = TestCase_Eval((r == SUCCESS),
        tc,
        suite, 
        (Typed *)Nstr(suite->m, "Next returns SUCCESS for a second"));
    if(r != SUCCESS){
        return;
    }
    value = NestedD_Get(m, nd, (Abstract *)String_Make(m, bytes("name")));
    expected_s = Nstr(m, "Knitting");
    r = TestCase_Eval((value != NULL && value->type == TYPE_STRING && String_Equals((String *)value, expected_s) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 7,  "Nested FOR 'name' is as expected, found '", "%S", Typed_ToString(m, value), "' expected '", "%S", expected_s, "'"));
    if(r != SUCCESS){
        return;
    }

    value = NestedD_Get(m, nd, (Abstract *)String_Make(m, bytes("category")));
    expected_s = Nstr(m, "sewing");
    r = TestCase_Eval((value != NULL && value->type == TYPE_STRING && String_Equals((String *)value, expected_s) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 7,  "Nested FOR 'category' is as expected, found '", "%S", Typed_ToString(m, value), "' expected '", "%S", expected_s, "'"));
    if(r != SUCCESS){
        return;
    }

    value = NestedD_Get(m, nd, (Abstract *)String_Make(m, bytes("status")));
    expected = Typed_Make(m, CYCLE_STATE_DELTA);
    r = TestCase_Eval((value != NULL && value->type == TYPE_TYPED && Typed_Equals(value, expected) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 7,  "Nested FOR 'status' is as expected, found '", "%S", Typed_ToString(m, value), "' expected '", "%S", expected, "'"));
    if(r != SUCCESS){
        return;
    }

    / * second round * /
    r = NestedD_Next(m, nd);
    r = TestCase_Eval((r == SUCCESS),
        tc,
        suite, 
        (Typed *)Nstr(suite->m, "Next returns SUCCESS for a third"));
    if(r != SUCCESS){
        return;
    }

    value = NestedD_Get(m, nd, (Abstract *)String_Make(m, bytes("name")));
    expected_s = Nstr(m, "Ramen");
    r = TestCase_Eval((value != NULL && value->type == TYPE_STRING && String_Equals((String *)value, expected_s) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 7,  "Nested FOR 'name' is as expected, found '", "%S", Typed_ToString(m, value), "' expected '", "%S", expected_s, "'"));
    if(r != SUCCESS){
        return;
    }

    value = NestedD_Get(m, nd, (Abstract *)String_Make(m, bytes("category")));
    expected_s = Nstr(m, "cooking");
    r = TestCase_Eval((value != NULL && value->type == TYPE_STRING && String_Equals((String *)value, expected_s) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 7,  "Nested FOR 'category' is as expected, found '", "%S", Typed_ToString(m, value), "' expected '", "%S", expected_s, "'"));
    if(r != SUCCESS){
        return;
    }

    value = NestedD_Get(m, nd, (Abstract *)String_Make(m, bytes("status")));
    expected = Typed_Make(m, CYCLE_STATE_ECHO);
    r = TestCase_Eval((value != NULL && value->type == TYPE_TYPED && Typed_Equals(value, (Typed *)expected) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 7,  "Nested FOR 'status' is as expected, found '", "%S", Typed_ToString(m, value), "' expected '", "%S", expected, "'"));
    if(r != SUCCESS){
        return;
    }
    
    / * check if it's finished * /
    r = NestedD_Next(m, nd);
    r = TestCase_Eval((r == MISS),
        tc,
        suite, 
        (Typed *)Nstr(suite->m, "Next returns MISS after three records"));
    if(r != SUCCESS){
        return;
    }

    / * check outdent values are present * /
    NestedD_Outdent(m, nd);
    value = NestedD_Get(m, nd, (Abstract *)String_Make(m, bytes("title")));
    expected_s = Nstr(m, "Master Of It All");
    r = TestCase_Eval((value != NULL && value->type == TYPE_STRING && String_Equals((String *)value, expected_s) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 7,  "Title after outdent on main found '", "%S", Typed_ToString(m, value), "' expected '", "%S", expected_s, "'"));
    if(r != SUCCESS){
        return;
    }

    return;
    */

    MemCtx_Free(m);
    return r;
}
