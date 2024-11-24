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

    /*
    r = TestCase_Eval((Typed_Truthy(value) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 2,  "Truthy of 'yes' is ", LifeCycle_FlagToCStr(value->lifecycle)));
    if(r != SUCCESS){
        return;
    }

    value = NestedD_Get(m, nd, (Typed *)Nstr(m, "no"));
    r = TestCase_Eval((Typed_Truthy(value) != SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 2,  "Truthy of 'no' is ", LifeCycle_FlagToCStr(value->lifecycle)));
    if(r != SUCCESS){
        return;
    }

    value = NestedD_Get(m, nd, (Typed *)Nstr(m, "not-a-thing"));
    r = TestCase_Eval((Typed_Truthy(value) != SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 2,  "Truthy of 'not-a-thing' is ", LifeCycle_ToCStr(value)));
    if(r != SUCCESS){
        return;
    }
    */

    MemCtx_Free(m);
    return r;
}

status NestedDWith_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    String *s;
    status r = SUCCESS;

    /*

    Span *tbl = NULL;
    NestedD *nd = NULL;
    Typed *value = NULL;
    lifecycle_t r = ERROR;

    struct timespec ts = {1709645616, 277122774};
    Typed *session[] = {
        (Typed *)Nstr(m, "scid"), (Typed *)Nstr(m, "a87c782a"),
        (Typed *)Nstr(m, "domain"), (Typed *)Nstr(m, "customer"),
        (Typed *)Nstr(m, "expires"), (Typed *)Time_NewTimeStamp(m, &ts),
        NULL,
    };

    Typed *items[] = {
        (Typed *)Nstr(m, "name"), (Typed *)Nstr(m, "SuppaSuppa"),
        (Typed *)Nstr(m, "title"), (Typed *)Nstr(m, "Master Of It All"),
        (Typed *)Nstr(m, "session"), (Typed *)Table_FromLiteral(m, session),
        NULL,
    };

    tbl = Table_FromLiteral(m, items);
    nd = NestedD_Make(m, tbl);

    value = NestedD_Get(m, nd, (Typed *)Nstr(m, "name"));
    r = TestCase_Eval((value != NULL && value->type == TYPE_STRING && String_Equals((String *)value, String_Static("SuppaSuppa")) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 3,  "Name on main level is as expected, found ", "%S", Typed_ToString(m, value)));
    if(r != SUCCESS){
        return;
    }

    NestedD_With(m, nd, (Typed *)Nstr(m, "session"));

    value = NestedD_Get(m, nd, (Typed *)Nstr(m, "scid"));
    r = TestCase_Eval((value != NULL && value->type == TYPE_STRING && String_Equals((String *)value, String_Static("a87c782a")) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 3,  "With session scid is as expected, found ", "%S", Typed_ToString(m, value)));
    if(r != SUCCESS){
        return;
    }

    value = NestedD_Get(m, nd, (Typed *)Nstr(m, "expires"));
    Time *expected_tm = Time_NewTimeStamp(m, &ts);
    r = TestCase_Eval((value != NULL && value->type == TYPE_TIME && Typed_Equals(value, (Typed *)expected_tm) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 6,  "With session time is as expected, found ", "%S", Typed_ToString(m, value), " expected ", "%S", Typed_ToString(m, (Typed *)expected_tm)));
    if(r != SUCCESS){
        return;
    }

    NestedD_Outdent(m, nd);
    value = NestedD_Get(m, nd, (Typed *)Nstr(m, "title"));
    String *expected_s = Nstr(m, "Master Of It All");
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

status NestedDFor_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    String *s;
    status r = SUCCESS;

    /*
    Span *tbl = NULL;
    NestedD *nd = NULL;
    Typed *value = NULL;
    String *expected_s = NULL;
    Typed *expected = NULL;
    lifecycle_t r = ERROR;

    Typed *hobby1[] = {
        (Typed *)Nstr(m, "name"), (Typed *)Nstr(m, "Heavy Metal"),
        (Typed *)Nstr(m, "category"), (Typed *)Nstr(m, "music"),
        (Typed *)Nstr(m, "status"), (Typed *)Typed_Make(m, CYCLE_STATE_ALPHA),
        NULL,
    };

    Typed *hobby2[] = {
        (Typed *)Nstr(m, "name"), (Typed *)Nstr(m, "Knitting"),
        (Typed *)Nstr(m, "category"), (Typed *)Nstr(m, "sewing"),
        (Typed *)Nstr(m, "status"), (Typed *)Typed_Make(m, CYCLE_STATE_DELTA),
        NULL,
    };

    Typed *hobby3[] = {
        (Typed *)Nstr(m, "name"), (Typed *)Nstr(m, "Ramen"),
        (Typed *)Nstr(m, "category"), (Typed *)Nstr(m, "cooking"),
        (Typed *)Nstr(m, "status"), (Typed *)Typed_Make(m, CYCLE_STATE_ECHO),
        NULL,
    };

    Span *hobbies_sp = Span_Make(m, SPAN_DIM_SIZE);
    Span_Add(m, hobbies_sp, (Typed *)Table_FromLiteral(m, hobby1));
    Span_Add(m, hobbies_sp, (Typed *)Table_FromLiteral(m, hobby2));
    Span_Add(m, hobbies_sp, (Typed *)Table_FromLiteral(m, hobby3));

    Typed *items[] = {
        (Typed *)Nstr(m, "name"), (Typed *)Nstr(m, "SuppaSuppa"),
        (Typed *)Nstr(m, "title"), (Typed *)Nstr(m, "Master Of It All"),
        (Typed *)Nstr(m, "hobbies"), (Typed *)hobbies_sp,
        NULL,
    };

    tbl = Table_FromLiteral(m, items);
    nd = NestedD_Make(m, tbl);

    value = NestedD_Get(m, nd, (Typed *)Nstr(m, "name"));
    r = TestCase_Eval((value != NULL && value->type == TYPE_STRING && String_Equals((String *)value, String_Static("SuppaSuppa")) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 3,  "Name on main level is as expected, found ", "%S", Typed_ToString(m, value)));
    if(r != SUCCESS){
        return;
    }

    NestedD_For(m, nd, (Typed *)Nstr(m, "hobbies"));

    / * first round * /
    value = NestedD_Get(m, nd, (Typed *)Nstr(m, "name"));
    expected_s = Nstr(m, "Heavy Metal");
    r = TestCase_Eval((value != NULL && value->type == TYPE_STRING && String_Equals((String *)value, expected_s) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 7,  "Nested FOR 'name' is as expected, found '", "%S", Typed_ToString(m, value), "' expected '", "%S", expected_s, "'"));
    if(r != SUCCESS){
        return;
    }

    value = NestedD_Get(m, nd, (Typed *)Nstr(m, "category"));
    expected_s = Nstr(m, "music");
    r = TestCase_Eval((value != NULL && value->type == TYPE_STRING && String_Equals((String *)value, expected_s) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 7,  "Nested FOR 'category' is as expected, found '", "%S", value, "' expected '", "%S", expected_s, "'"));
    if(r != SUCCESS){
        return;
    }

    value = NestedD_Get(m, nd, (Typed *)Nstr(m, "status"));
    expected = Typed_Make(m, CYCLE_STATE_ALPHA);
    r = TestCase_Eval((value != NULL && value->type == TYPE_TYPED && Typed_Equals(value, expected) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 7, "Nested FOR 'status' is as expected, found '", "%S", value, "' expected '", "%S", expected, "'"));
    if(r != SUCCESS){
        return;
    }

    / * second round * /
    r = NestedD_Next(m, nd);
    r = TestCase_Eval((r == SUCCESS),
        tc,
        suite, 
        (Typed *)Nstr(suite->m, "Next returns SUCCESS for a second"));
    if(r != SUCCESS){
        return;
    }
    value = NestedD_Get(m, nd, (Typed *)Nstr(m, "name"));
    expected_s = Nstr(m, "Knitting");
    r = TestCase_Eval((value != NULL && value->type == TYPE_STRING && String_Equals((String *)value, expected_s) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 7,  "Nested FOR 'name' is as expected, found '", "%S", Typed_ToString(m, value), "' expected '", "%S", expected_s, "'"));
    if(r != SUCCESS){
        return;
    }

    value = NestedD_Get(m, nd, (Typed *)Nstr(m, "category"));
    expected_s = Nstr(m, "sewing");
    r = TestCase_Eval((value != NULL && value->type == TYPE_STRING && String_Equals((String *)value, expected_s) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 7,  "Nested FOR 'category' is as expected, found '", "%S", Typed_ToString(m, value), "' expected '", "%S", expected_s, "'"));
    if(r != SUCCESS){
        return;
    }

    value = NestedD_Get(m, nd, (Typed *)Nstr(m, "status"));
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

    value = NestedD_Get(m, nd, (Typed *)Nstr(m, "name"));
    expected_s = Nstr(m, "Ramen");
    r = TestCase_Eval((value != NULL && value->type == TYPE_STRING && String_Equals((String *)value, expected_s) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 7,  "Nested FOR 'name' is as expected, found '", "%S", Typed_ToString(m, value), "' expected '", "%S", expected_s, "'"));
    if(r != SUCCESS){
        return;
    }

    value = NestedD_Get(m, nd, (Typed *)Nstr(m, "category"));
    expected_s = Nstr(m, "cooking");
    r = TestCase_Eval((value != NULL && value->type == TYPE_STRING && String_Equals((String *)value, expected_s) == SUCCESS),
        tc,
        suite, 
        (Typed *)String_FormatN(suite->m, 7,  "Nested FOR 'category' is as expected, found '", "%S", Typed_ToString(m, value), "' expected '", "%S", expected_s, "'"));
    if(r != SUCCESS){
        return;
    }

    value = NestedD_Get(m, nd, (Typed *)Nstr(m, "status"));
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
    value = NestedD_Get(m, nd, (Typed *)Nstr(m, "title"));
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
