#include <external.h>
#include <caneka.h>

char *fmtCstr = ""
    "= Hidy Ho\n"
    "\n"
    "And this is a first paragraph\n"
    "with a second line.\n"
    "\n"
    ".fancy\n"
    "Second single sentance with details _link=here@http://example.com.\n"
    "\n"
    "_image=Image one@image.png\n"
    "\n"
    "- bullet one.\n"
    "- bullet two\nwith two lines.\n"
    "- bullet three\n  with two lines as well.\n"
    "- bullet four.\n"
    "\n"
    "== Sub\n"
    "Section\n"
    "\n"
    "A paragraph in the sub section.\n"
    "\n"
    "= Table Title\n"
    "\n"
    "+Column A,Column B,Column C\n"
    "Apple, 1, 37\n"
    "Bannana, 2, 39\n"
    "Cantelope, 3, 39\n"
    "\n"
    "And the final text here.\n"
;

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
    Table_Set(nd->atts, (Abstract *)I16_Wrapped(m, FORMATTER_INDENT), (Abstract *)v);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "Hidy Ho"));
    nd->child = (Abstract *)v;
    Span_Add((Span *)prev->child, (Abstract *)nd);

    nd = Node_Make(m, ZERO, prev);
    nd->typeOfChild = TYPE_STRVEC;
    nd->captureKey = FORMATTER_PARAGRAPH;
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "And this is a first paragraph with a second line."));
    nd->child = (Abstract *)v;
    Span_Add((Span *)prev->child, (Abstract *)nd);

    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "fancy"));
    Span_Add((Span *)prev->child, (Abstract *)v);

    nd = Node_Make(m, ZERO, prev);
    nd->typeOfChild = TYPE_SPAN;
    nd->captureKey = FORMATTER_PARAGRAPH;
    nd->atts = Table_Make(m);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "fancy"));
    Table_Set(nd->atts, (Abstract *)I16_Wrapped(m, FORMATTER_CLASS), (Abstract *)v);
    nd->child = (Abstract *)Span_Make(m);
    Span_Add((Span *)prev->child, (Abstract *)nd);

    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "Second single sentance with details "));
    Span_Add((Span*)nd->child, (Abstract *)v);

    Node *cur = Node_Make(m, ZERO, nd);
    cur->captureKey = FORMATTER_TAG;
    cur->atts = Table_Make(m);
    Table_Set(cur->atts, (Abstract *)I16_Wrapped(m, FORMATTER_TAG),
        (Abstract *)Str_CstrRef(m, "link"));
    Table_Set(cur->atts, (Abstract *)I16_Wrapped(m, FORMATTER_LABEL),
        (Abstract *)Str_CstrRef(m, "here"));
    Table_Set(cur->atts, (Abstract *)I16_Wrapped(m, FORMATTER_URL),
        (Abstract *)Str_CstrRef(m, "http://example.com"));
    Span_Add((Span*)nd->child, (Abstract *)cur);

    cur = Node_Make(m, ZERO, nd);
    cur->captureKey = FORMATTER_TAG;
    cur->atts = Table_Make(m);
    Table_Set(cur->atts, (Abstract *)I16_Wrapped(m, FORMATTER_TAG),
        (Abstract *)Str_CstrRef(m, "image"));
    Table_Set(cur->atts, (Abstract *)I16_Wrapped(m, FORMATTER_LABEL),
        (Abstract *)Str_CstrRef(m, "Image one"));
    Table_Set(cur->atts, (Abstract *)I16_Wrapped(m, FORMATTER_URL),
        (Abstract *)Str_CstrRef(m, "image.png"));
    Span_Add((Span*)nd->child, (Abstract *)cur);

    cur = Node_Make(m, ZERO, nd);
    cur->captureKey = FORMATTER_BULLET;
    cur->typeOfChild = TYPE_SPAN;
    cur->child = (Abstract *)Span_Make(m);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "bullet one."));
    Span_Add((Span *)cur->child, (Abstract *)v);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "bullet two with two lines."));
    Span_Add((Span *)cur->child, (Abstract *)v);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "bullet three   with two lines as well."));
    Span_Add((Span *)cur->child, (Abstract *)v);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "bullet four."));
    Span_Add((Span *)cur->child, (Abstract *)v);
    Span_Add((Span*)nd->child, (Abstract *)cur);

    nd = Node_Make(m, ZERO, prev);
    nd->typeOfChild = TYPE_STRVEC;
    nd->captureKey = FORMATTER_INDENT;
    nd->atts = Table_Make(m);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "=="));
    Table_Set(nd->atts, (Abstract *)I16_Wrapped(m, FORMATTER_INDENT), (Abstract *)v);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "Sub Section"));
    nd->child = (Abstract *)v;
    Span_Add((Span *)prev->child, (Abstract *)nd);

    nd = Node_Make(m, ZERO, prev);
    nd->typeOfChild = TYPE_STRVEC;
    nd->captureKey = FORMATTER_PARAGRAPH;
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "A paragraph in the sub section."));
    nd->child = (Abstract *)v;
    Span_Add((Span *)prev->child, (Abstract *)nd);

    nd = Node_Make(m, ZERO, prev);
    nd->typeOfChild = TYPE_STRVEC;
    nd->captureKey = FORMATTER_INDENT;
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "="));
    nd->atts = Table_Make(m);
    Table_Set(nd->atts, (Abstract *)I16_Wrapped(m, FORMATTER_INDENT), (Abstract *)v);
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "Table Title"));
    nd->child = (Abstract *)v;
    Span_Add((Span *)prev->child, (Abstract *)nd);

    nd = Node_Make(m, ZERO, prev);
    nd->typeOfChild = TYPE_RELATION;
    nd->captureKey = FORMATTER_TABLE;

    Relation *rel = Relation_Make(m, 0, NULL);

    Relation_AddValue(rel, (Abstract *)Str_CstrRef(m, "Column A"));
    Relation_AddValue(rel, (Abstract *)Str_CstrRef(m, "Column B"));
    Relation_AddValue(rel, (Abstract *)Str_CstrRef(m, "Column C"));
    Relation_HeadFromValues(rel);

    Relation_SetValue(rel, 0, 0, (Abstract *)Str_CstrRef(m, "Apple"));
    Relation_SetValue(rel, 0, 1, (Abstract *)I32_Wrapped(m, 1));
    Relation_SetValue(rel, 0, 2, (Abstract *)I32_Wrapped(m, 37));

    Relation_SetValue(rel, 1, 0, (Abstract *)Str_CstrRef(m, "Bannana"));
    Relation_SetValue(rel, 1, 1, (Abstract *)I32_Wrapped(m, 2));
    Relation_SetValue(rel, 1, 2, (Abstract *)I32_Wrapped(m, 39));

    Relation_SetValue(rel, 2, 0, (Abstract *)Str_CstrRef(m, "Cantelope"));
    Relation_SetValue(rel, 2, 1, (Abstract *)I32_Wrapped(m, 3));
    Relation_SetValue(rel, 2, 2, (Abstract *)I32_Wrapped(m, 39));

    nd->child = (Abstract *)rel;
    Span_Add((Span *)prev->child, (Abstract *)nd);

    nd = Node_Make(m, ZERO, prev);
    nd->typeOfChild = TYPE_STRVEC;
    nd->captureKey = FORMATTER_PARAGRAPH;
    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "And the final text here."));
    nd->child = (Abstract *)v;
    Span_Add((Span *)prev->child, (Abstract *)nd);

    return expected;
}

/* expected 

Mess Mess<DEBUG current:N<ZERO/READY/TYPE_STRVEC captureKey(FORMATTER_PARAGRAPH) parent(Str<10/11:"_TYPE_ZERO">) atts:NULL value:_TYPE_ZERO/NULL child:TYPE_STRVEC> value:StrVec<1/24 [0: Str<24/512:"And the final text here.">]> [
    N<ZERO/READY/TYPE_SPAN captureKey(_TYPE_ZERO) parent(NULL) atts:NULL value:_TYPE_ZERO/NULL child:TYPE_SPAN>[
      N<ZERO/READY/TYPE_STRVEC captureKey(FORMATTER_INDENT) parent(Str<10/11:"_TYPE_ZERO">) atts:Tbl<SUCCESS 1nvalues Wi16<141>=StrVec<1/2 [0: Str<2/512:"= ">]>> value:_TYPE_ZERO/NULL child:TYPE_STRVEC>
        StrVec<1/7 [0: Str<7/512:"Hidy Ho">]>
      N<ZERO/READY/TYPE_STRVEC captureKey(FORMATTER_PARAGRAPH) parent(Str<10/11:"_TYPE_ZERO">) atts:NULL value:_TYPE_ZERO/NULL child:TYPE_STRVEC>
        StrVec<3/49 [0: Str<29/512:"And this is a first paragraph">, 1: Str<1/2:" ">, 2: Str<19/512:"with a second line.">]>
      StrVec<1/5 [0: Str<5/512:"fancy">]>
      N<ZERO/READY/TYPE_SPAN captureKey(FORMATTER_PARAGRAPH) parent(Str<10/11:"_TYPE_ZERO">) atts:Tbl<SUCCESS 1nvalues Wi16<142>=StrVec<1/5 [0: Str<5/512:"fancy">]>> value:_TYPE_ZERO/NULL child:TYPE_SPAN>[
        StrVec<1/36 [0: Str<36/512:"Second single sentance with details ">]>
        N<ZERO/READY/_TYPE_ZERO captureKey(FORMATTER_TAG) parent(Str<19/20:"FORMATTER_PARAGRAPH">) atts:Tbl<SUCCESS 3nvalues Wi16<152>=StrVec<1/4 [0: Str<4/512:"link">]>, Wi16<154>=StrVec<1/4 [0: Str<4/512:"here">]>, Wi16<155>=StrVec<1/18 [0: Str<18/512:"http://example.com">]>> value:_TYPE_ZERO/NULL child:_TYPE_ZERO>
        N<ZERO/READY/_TYPE_ZERO captureKey(FORMATTER_TAG) parent(Str<19/20:"FORMATTER_PARAGRAPH">) atts:Tbl<SUCCESS 3nvalues Wi16<152>=StrVec<1/5 [0: Str<5/512:"image">]>, Wi16<154>=StrVec<1/9 [0: Str<9/512:"Image one">]>, Wi16<155>=StrVec<1/9 [0: Str<9/512:"image.png">]>> value:_TYPE_ZERO/NULL child:_TYPE_ZERO>
        N<ZERO/READY/TYPE_SPAN captureKey(FORMATTER_BULLET) parent(Str<19/20:"FORMATTER_PARAGRAPH">) atts:NULL value:_TYPE_ZERO/NULL child:TYPE_SPAN>[
          StrVec<1/11 [0: Str<11/512:"bullet one.">]>
          StrVec<3/26 [0: Str<10/512:"bullet two">, 1: Str<1/2:" ">, 2: Str<15/512:"with two lines.">]>
          StrVec<3/38 [0: Str<12/512:"bullet three">, 1: Str<1/2:" ">, 2: Str<25/512:"  with two lines as well.">]>
          StrVec<1/12 [0: Str<12/512:"bullet four.">]>]]
      N<ZERO/READY/TYPE_STRVEC captureKey(FORMATTER_INDENT) parent(Str<10/11:"_TYPE_ZERO">) atts:Tbl<SUCCESS 1nvalues Wi16<141>=StrVec<1/3 [0: Str<3/512:"== ">]>> value:_TYPE_ZERO/NULL child:TYPE_STRVEC>
        StrVec<3/11 [0: Str<3/512:"Sub">, 1: Str<1/2:" ">, 2: Str<7/512:"Section">]>
      N<ZERO/READY/TYPE_STRVEC captureKey(FORMATTER_PARAGRAPH) parent(Str<10/11:"_TYPE_ZERO">) atts:NULL value:_TYPE_ZERO/NULL child:TYPE_STRVEC>
        StrVec<1/31 [0: Str<31/512:"A paragraph in the sub section.">]>
      N<ZERO/READY/TYPE_STRVEC captureKey(FORMATTER_INDENT) parent(Str<10/11:"_TYPE_ZERO">) atts:Tbl<SUCCESS 1nvalues Wi16<141>=StrVec<1/2 [0: Str<2/512:"= ">]>> value:_TYPE_ZERO/NULL child:TYPE_STRVEC>
        StrVec<1/11 [0: Str<11/512:"Table Title">]>
      N<ZERO/READY/TYPE_RELATION captureKey(FORMATTER_TABLE) parent(Str<10/11:"_TYPE_ZERO">) atts:NULL value:_TYPE_ZERO/NULL child:TYPE_RELATION>
        Rel<3x3 StrVec<1/8 [0: Str<8/512:"Column A">]>,StrVec<1/8 [0: Str<8/512:"Column B">]>,StrVec<1/8 [0: Str<8/512:"Column C">]> [
  StrVec<1/5 [0: Str<5/512:"Apple">]>,StrVec<1/2 [0: Str<2/512:" 1">]>,StrVec<1/3 [0: Str<3/512:" 37">]>,
  StrVec<1/7 [0: Str<7/512:"Bannana">]>,StrVec<1/2 [0: Str<2/512:" 2">]>,StrVec<1/3 [0: Str<3/512:" 39">]>,
  StrVec<1/9 [0: Str<9/512:"Cantelope">]>,StrVec<1/2 [0: Str<2/512:" 3">]>,StrVec<1/3 [0: Str<3/512:" 39">]>,
]>
      N<ZERO/READY/TYPE_STRVEC captureKey(FORMATTER_PARAGRAPH) parent(Str<10/11:"_TYPE_ZERO">) atts:NULL value:_TYPE_ZERO/NULL child:TYPE_STRVEC>
        StrVec<1/24 [0: Str<24/512:"And the final text here.">]>]
]>

*/

status Mess_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *s = NULL; 
    StrVec *v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, fmtCstr));
    Cursor *curs = Cursor_Make(m, v);

    Roebling *rbl = NULL;
    rbl = FormatFmt_Make(m, curs, NULL);
    Roebling_Run(rbl);

    Mess *expected = make_Expected(m);

    r |= Test((Mess_Compare(m, rbl->mess, expected) & SUCCESS) != 0,
        "Mess has been built as expected", NULL);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
