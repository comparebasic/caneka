#include <external.h>
#include <caneka.h>

status Config_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    Abstract *args[5];
    status r = READY;
    MemCh *m = MemCh_Make();

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./examples/object.config"));
    StrVec *content = File_ToVec(m, path);

    Cursor *curs = Cursor_Make(m, content);
    Roebling *rbl = FormatConfig_Make(m, curs, NULL);
    Roebling_Run(rbl);

    Object *root = FormatConfig_GetRoot(rbl);;

    StrVec *docKey = StrVec_From(m, Str_FromCstr(m, "doc", ZERO));
    Path_DotAnnotate(m, docKey);
    StrVec *tagKey = StrVec_From(m, Str_FromCstr(m, "doc.tag", ZERO));
    Path_DotAnnotate(m, tagKey);
    StrVec *footerKey = StrVec_From(m, Str_FromCstr(m, "doc.footer", ZERO));
    Path_DotAnnotate(m, footerKey);

    NodeObj *doc = Object_ByPath(root, docKey, NULL, SPAN_OP_GET);
    NodeObj *tag = Object_ByPath(root, tagKey, NULL, SPAN_OP_GET);
    NodeObj *footer = Object_ByPath(root, footerKey, NULL, SPAN_OP_GET);

    args[0] = (Abstract *)docKey;
    args[1] = NULL;
    r |= Test(doc != NULL,
        "Doc node is not NULL: @", args);

    Abstract *arr[10];
    
    arr[0] = (Abstract *)Str_FromCstr(m, "alpha", ZERO);
    arr[1] = (Abstract *)Str_FromCstr(m, "apple", ZERO);
    arr[2] = (Abstract *)Str_FromCstr(m, "bravo", ZERO);
    arr[3] = (Abstract *)Str_FromCstr(m, "bannanna", ZERO);
    arr[4] = (Abstract *)Str_FromCstr(m, "charlie", ZERO);
    arr[5] = (Abstract *)Str_FromCstr(m, "carrot", ZERO);
    arr[6] = (Abstract *)Str_FromCstr(m, "start", ZERO);
    arr[7] = (Abstract *)I32_Wrapped(m, 3);
    arr[8] = NULL;
    arr[9] = NULL;

    for(Abstract **ap = arr; *ap != NULL; ap += 2){
        Abstract *key = *ap;
        Abstract *expected = *(ap+1);
        Abstract *value = NodeObj_Att(doc, key);
        args[0] = (Abstract *)docKey;
        args[1] = key;
        args[2] = expected;
        args[3] = value;
        args[4] = NULL;
        r |= Test(Equals((Abstract *)expected, (Abstract *)value),
            "NodeObj @ att @ has expected value @: @", args);
    }

    args[0] = (Abstract *)tagKey;
    args[1] = NULL;
    r |= Test(tag != NULL,
        "Tag node is not NULL: @", args);

    arr[0] = (Abstract *)Str_FromCstr(m, "att-one", ZERO);
    arr[1] = (Abstract *)Str_FromCstr(m, "one", ZERO);
    arr[2] = (Abstract *)Str_FromCstr(m, "att-two", ZERO);
    arr[3] = (Abstract *)Str_FromCstr(m, "two", ZERO);
    arr[4] = (Abstract *)NULL;
    arr[5] = (Abstract *)NULL;

    for(Abstract **ap = arr; *ap != NULL; ap += 2){
        Abstract *key = *ap;
        Abstract *expected = *(ap+1);
        Abstract *value = NodeObj_Att(tag, key);
        args[0] = (Abstract *)tagKey;
        args[1] = key;
        args[2] = expected;
        args[3] = value;
        args[4] = NULL;
        r |= Test(Equals((Abstract *)expected, (Abstract *)value),
            "NodeObj @ att @ has expected value @: @", args);
    }

    args[0] = (Abstract *)footerKey;
    args[1] = NULL;
    r |= Test(footer != NULL,
        "Footer node is not NULL: @", args);

    arr[0] = (Abstract *)Str_FromCstr(m, "content", ZERO);
    arr[1] = (Abstract *)Str_FromCstr(m, "A secondary view of things.", ZERO);
    arr[2] = (Abstract *)NULL;
    arr[3] = (Abstract *)NULL;

    for(Abstract **ap = arr; *ap != NULL; ap += 2){
        Abstract *key = *ap;
        Abstract *expected = *(ap+1);
        Abstract *value = NodeObj_Att(footer, key);
        args[0] = (Abstract *)tagKey;
        args[1] = key;
        args[2] = expected;
        args[3] = value;
        args[4] = NULL;
        r |= Test(Equals((Abstract *)expected, (Abstract *)value),
            "NodeObj @ att @ has expected value @: @", args);
    }

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
