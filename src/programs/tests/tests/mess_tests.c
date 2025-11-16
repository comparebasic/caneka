#include <external.h>
#include <caneka.h>

Mess *make_Expected(MemCh *m){
    Mess *expected = Mess_Make(m);

    Node *nd = NULL;
    Node *prev = NULL;
    StrVec *v;
    nd = Node_Make(m, ZERO, NULL); 
    nd->typeOfChild = TYPE_SPAN;
    nd->child = (Abstract *)Span_Make(m);
    expected->root = expected->current = nd;
    prev = nd;

    nd->child = (Abstract*)Span_Make(m);
    nd->typeOfChild = TYPE_SPAN;
    
    nd = Node_Make(m, ZERO, prev);
    nd->typeOfChild = TYPE_STRVEC;
    nd->captureKey = FORMATTER_INDENT;
    nd->atts = Table_Make(m);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "="));
    Table_Set(nd->atts, I16_Wrapped(m, FORMATTER_INDENT), v);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "Hidy Ho"));
    nd->child = (Abstract *)v;
    Span_Add((Span *)prev->child, nd);

    nd = Node_Make(m, ZERO, prev);
    nd->typeOfChild = TYPE_STRVEC;
    nd->captureKey = FORMATTER_PARAGRAPH;
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "And this is a first paragraph with a second line."));
    nd->child = (Abstract *)v;
    Span_Add((Span *)prev->child, nd);

    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "fancy"));
    Span_Add((Span *)prev->child, v);

    nd = Node_Make(m, ZERO, prev);
    nd->typeOfChild = TYPE_SPAN;
    nd->captureKey = FORMATTER_PARAGRAPH;
    nd->atts = Table_Make(m);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "fancy"));
    Table_Set(nd->atts, I16_Wrapped(m, FORMATTER_CLASS), v);
    nd->child = (Abstract *)Span_Make(m);
    Span_Add((Span *)prev->child, nd);

    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "Second single sentance with details "));
    Span_Add((Span*)nd->child, v);

    Node *cur = Node_Make(m, ZERO, nd);
    cur->captureKey = FORMATTER_TAG;
    cur->atts = Table_Make(m);
    Table_Set(cur->atts, I16_Wrapped(m, FORMATTER_TAG),
        Str_CstrRef(m, "link"));
    Table_Set(cur->atts, I16_Wrapped(m, FORMATTER_LABEL),
        Str_CstrRef(m, "here"));
    Table_Set(cur->atts, I16_Wrapped(m, FORMATTER_URL),
        Str_CstrRef(m, "http://example.com"));
    Span_Add((Span*)nd->child, cur);

    cur = Node_Make(m, ZERO, prev);
    cur->captureKey = FORMATTER_TAG;
    cur->atts = Table_Make(m);
    Table_Set(cur->atts, I16_Wrapped(m, FORMATTER_TAG),
        Str_CstrRef(m, "image"));
    Table_Set(cur->atts, I16_Wrapped(m, FORMATTER_LABEL),
        Str_CstrRef(m, "Image one"));
    Table_Set(cur->atts, I16_Wrapped(m, FORMATTER_URL),
        Str_CstrRef(m, "image.png"));
    Span_Add((Span*)prev->child, cur);

    cur = Node_Make(m, ZERO, prev);
    cur->captureKey = FORMATTER_BULLET;
    cur->typeOfChild = TYPE_SPAN;
    cur->child = (Abstract *)Span_Make(m);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "bullet one."));
    Span_Add((Span *)cur->child, v);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "bullet two with two lines."));
    Span_Add((Span *)cur->child, v);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "bullet three   with two lines as well."));
    Span_Add((Span *)cur->child, v);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "bullet four."));
    Span_Add((Span *)cur->child, v);
    Span_Add((Span*)prev->child, cur);

    nd = Node_Make(m, ZERO, prev);
    nd->typeOfChild = TYPE_STRVEC;
    nd->captureKey = FORMATTER_INDENT;
    nd->atts = Table_Make(m);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "=="));
    Table_Set(nd->atts, I16_Wrapped(m, FORMATTER_INDENT), v);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "Sub Section"));
    nd->child = (Abstract *)v;
    Span_Add((Span *)prev->child, nd);

    nd = Node_Make(m, ZERO, prev);
    nd->typeOfChild = TYPE_STRVEC;
    nd->captureKey = FORMATTER_PARAGRAPH;
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "A paragraph in the sub section."));
    nd->child = (Abstract *)v;
    Span_Add((Span *)prev->child, nd);

    nd = Node_Make(m, ZERO, prev);
    nd->typeOfChild = TYPE_STRVEC;
    nd->captureKey = FORMATTER_INDENT;
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "="));
    nd->atts = Table_Make(m);
    Table_Set(nd->atts, I16_Wrapped(m, FORMATTER_INDENT), v);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "Table Title"));
    nd->child = (Abstract *)v;
    Span_Add((Span *)prev->child, nd);

    nd = Node_Make(m, ZERO, prev);
    nd->typeOfChild = TYPE_RELATION;
    nd->captureKey = FORMATTER_TABLE;

    Relation *rel = Relation_Make(m, 0, NULL);

    Relation_AddValue(rel, Str_CstrRef(m, "Column A"));
    Relation_AddValue(rel, Str_CstrRef(m, "Column B"));
    Relation_AddValue(rel, Str_CstrRef(m, "Column C"));
    Relation_HeadFromValues(rel);

    Relation_SetValue(rel, 0, 0, Str_CstrRef(m, "Apple"));
    Relation_SetValue(rel, 0, 1, I32_Wrapped(m, 1));
    Relation_SetValue(rel, 0, 2, I32_Wrapped(m, 37));

    Relation_SetValue(rel, 1, 0, Str_CstrRef(m, "Bannana"));
    Relation_SetValue(rel, 1, 1, I32_Wrapped(m, 2));
    Relation_SetValue(rel, 1, 2, I32_Wrapped(m, 39));

    Relation_SetValue(rel, 2, 0, Str_CstrRef(m, "Cantelope"));
    Relation_SetValue(rel, 2, 1, I32_Wrapped(m, 3));
    Relation_SetValue(rel, 2, 2, I32_Wrapped(m, 39));

    nd->child = (Abstract *)rel;
    Span_Add((Span *)prev->child, nd);

    nd = Node_Make(m, ZERO, prev);
    nd->typeOfChild = TYPE_STRVEC;
    nd->captureKey = FORMATTER_PARAGRAPH;
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "And the final text here."));
    nd->child = (Abstract *)v;
    Span_Add((Span *)prev->child, nd);

    return expected;
}

status Mess_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *s = NULL; 

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./examples/example.fmt"));
    StrVec *content = File_ToVec(m, path);
    Cursor *curs = Cursor_Make(m, content);

    Roebling *rbl = NULL;
    rbl = FormatFmt_Make(m, curs, NULL);
    Roebling_Run(rbl);

    Mess *expected = make_Expected(m);

    boolean result = (Mess_Compare(m, (Mess *)rbl->dest, expected) & SUCCESS) != 0;
    r |= Test(result, "Mess has been built as expected", NULL);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
