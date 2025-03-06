#include <external.h>
#include <caneka.h>

static char *cstr = ""
    "Most of the efficiency comes from allowing integers and static indexes to "
    "be used more often than string interpretation during runtime. Remembring "
    "the position of something without needing the steps you took to find it "
    "again can speed things up."
    "\n"
    "The combination of the Span and the basic type system make retrieving an "
    "item from a Lookup structure built from a simple enum possible throughout "
    "the language."
    "\n"
    "This combined with the simplicity of the re-query based Table for key-value "
    "store behavior makes Caneka an easy to understand light-weight to run, "
    "powerful language."
    "\n"
    ;

static char *cstrTwo = ""
    "Given that cycles are a better representation of computer processing than "
    "objects, there is high potential in this approach to a general purpose "
    "programming language."
    "\n"
    "Computers are usually applied to solve collaborative or evolving problems, "
    "the langauge focuses on patterns more than structure, in short, the Array "
    "is more powerful than the Object."
    "\n"
    "The paradigm for this type of software engineering deviates from "
    "conventional methodologies using a perspective called Cycle-Oriented "
    "Programming."
    ;

static char *cstrThree = ""
    "Caneka is based on a common computer science challenge: How to handle "
    "something, and move on, when the order is not the same. The langauge "
    "automatically populates virtual stacks and queues to handle tasks that "
    "would otherwise involve extensive handling. Consider the real world example "
    "featured in our termination analogy."
    ;

status MemLocal_ToFromTests(MemCtx *gm){
    DebugStack_Push("MemLocal_ToFromTests", TYPE_CSTR);
    status r = READY;
    MemCtx *m = MemCtx_Make();

    Span *ml = MemLocal_Make(TYPE_SPAN);

    String *one = String_Make(ml->m, bytes(cstr));

    i64 len = String_Length(one);
    MemLocal_To(ml->m, (Abstract *)one);
    r |= Test(one->type.of-HTYPE_LOCAL == TYPE_STRING_CHAIN, "String type has been adjusted to local");

    MemLocal_From(ml->m, (Abstract *)one);
    r |= Test(one->type.of == TYPE_STRING_CHAIN, "String type has been restored to local");
    r |= Test(String_Length(one) == len, "Original length is the same, indicating multiple string segment addresses, expected %ld, have %ld", len, String_Length(one));

    String *two = String_Make(ml->m, bytes(cstrTwo));
    String *three = String_Make(ml->m, bytes(cstrThree));
    Hashed *h = Hashed_Make(ml->m, (Abstract *)two);
    h->value = (Abstract *)three;

    Hashed *next = Hashed_Make(ml->m, (Abstract *)String_Make(ml->m, bytes("Hi")));
    next->value = (Abstract *)String_Make(ml->m, bytes("There"));
    h->next = next;

    MemLocal_To(ml->m, (Abstract *)h);
    r |= Test(h->type.of-HTYPE_LOCAL == TYPE_HASHED, "Type hashed has been changed");
    r |= Test(two->type.of-HTYPE_LOCAL == TYPE_STRING_CHAIN, "Type hashed item has been changed");
    r |= Test(three->type.of-HTYPE_LOCAL == TYPE_STRING_CHAIN, "Type hashed value has been changed");
    MemLocal_From(ml->m, (Abstract *)h);
    r |= Test(h->type.of == TYPE_HASHED, "Type hashed has been restored");

    Span *p = Span_Make(ml->m);
    String *pOne_s =String_Make(ml->m, bytes(cstr)); 
    String *pTwo_s =String_Make(ml->m, bytes(cstrTwo)); 
    String *pThree_s =String_Make(ml->m, bytes(cstrThree)); 
    Span_Set(p, 0, (Abstract *)pOne_s);
    Span_Set(p, 100, (Abstract *)pTwo_s);
    Span_Set(p, 101, (Abstract *)pThree_s);
    MemLocal_To(ml->m, (Abstract *)p);
    r |= Test(p->type.of-HTYPE_LOCAL == TYPE_SPAN, "Type has been changed for span");
    r |= Test(pOne_s->type.of-HTYPE_LOCAL == TYPE_STRING_CHAIN, "Type sting has been changed for span item one");
    r |= Test(pTwo_s->type.of-HTYPE_LOCAL == TYPE_STRING_CHAIN, "Type sting has been changed for span item two");
    r |= Test(pThree_s->type.of-HTYPE_LOCAL == TYPE_STRING_CHAIN, "Type sting has been changed for span item three");
    MemLocal_From(ml->m, (Abstract *)p);
    r |= Test(p->type.of == TYPE_SPAN, "Type has been restored for span");
    r |= Test(pOne_s->type.of == TYPE_STRING_CHAIN, "Type sting has been restored for span item one");
    r |= Test(pTwo_s->type.of == TYPE_STRING_CHAIN, "Type sting has been restored for span item two");
    r |= Test(pThree_s->type.of == TYPE_STRING_CHAIN, "Type sting has been restored for span item three");

    Span *tbl = Span_Make(ml->m);
    String *pTblOne_s =String_Make(ml->m, bytes(cstr)); 
    String *pKeyOne_s =String_Make(ml->m, bytes("one")); 
    String *pTblTwo_s =String_Make(ml->m, bytes(cstrTwo)); 
    String *pKeyTwo_s =String_Make(ml->m, bytes("two")); 
    String *pTblThree_s =String_Make(ml->m, bytes(cstrThree)); 
    String *pKeyThree_s =String_Make(ml->m, bytes("three")); 
    Table_Set(tbl, (Abstract *)pKeyOne_s, (Abstract *)pTblOne_s);
    Table_Set(tbl, (Abstract *)pKeyTwo_s, (Abstract *)pTblTwo_s);
    Table_Set(tbl, (Abstract *)pKeyThree_s, (Abstract *)pTblThree_s);
    MemLocal_To(ml->m, (Abstract *)tbl);
    r |= Test(tbl->type.of-HTYPE_LOCAL == TYPE_TABLE, "Type hashed has been changed for table");
    r |= Test(pTblOne_s->type.of-HTYPE_LOCAL == TYPE_STRING_CHAIN, "Type string has been changed for table item one");
    r |= Test(pTblTwo_s->type.of-HTYPE_LOCAL == TYPE_STRING_CHAIN, "Type string has been changed for table item two");
    r |= Test(pTblThree_s->type.of-HTYPE_LOCAL == TYPE_STRING_CHAIN, "Type string has been changed for table item three");
    MemLocal_From(ml->m, (Abstract *)tbl);
    r |= Test(tbl->type.of == TYPE_TABLE, "Type hashed has been restored for table");
    r |= Test(pTblOne_s->type.of == TYPE_STRING_CHAIN, "Type string has been restored for table item one");
    r |= Test(pTblTwo_s->type.of == TYPE_STRING_CHAIN, "Type string has been restored for table item two");
    r |= Test(pTblThree_s->type.of == TYPE_STRING_CHAIN, "Type string has been restored for table item three");

    MemCtx_Free(m);
    MemCtx_Free(ml->m);
    DebugStack_Pop();
    return r;
}

status MemLocal_Tests(MemCtx *gm){
    DebugStack_Push("MemLocal_ToFromTests", TYPE_CSTR);
    status r = READY;
    MemCtx *m = MemCtx_Make();

    Span *ml = MemLocal_Make(TYPE_SPAN);

    String *one = String_Make(ml->m, bytes(cstr));
    i64 len = String_Length(one);
    Span_Set(ml, 0, (Abstract *)one);

    Span *tbl = Span_Make(ml->m);
    String *pTblOne_s = String_Make(ml->m, bytes(cstr)); 
    String *pKeyOne_s = String_Make(ml->m, bytes("one")); 
    String *pTblTwo_s = String_Make(ml->m, bytes(cstrTwo)); 
    String *pKeyTwo_s = String_Make(ml->m, bytes("two")); 
    String *pTblThree_s = String_Make(ml->m, bytes(cstrThree)); 
    String *pKeyThree_s = String_Make(ml->m, bytes("three")); 
    String *expTwo_s = String_Clone(m, pTblTwo_s);
    Table_Set(tbl, (Abstract *)pKeyOne_s, (Abstract *)pTblOne_s);
    Table_Set(tbl, (Abstract *)pKeyTwo_s, (Abstract *)pTblTwo_s);
    Table_Set(tbl, (Abstract *)pKeyThree_s, (Abstract *)pTblThree_s);
    Span_Set(ml, 1, (Abstract *)tbl);

    char buff[PATH_BUFFLEN];
    String *path = String_Make(m, bytes(getcwd(buff, PATH_BUFFLEN)));
    char *cstr = "/tmp/mstore";
    String_AddBytes(m, path, bytes(cstr), strlen(cstr));

    r |= MemLocal_Persist(m, ml, path, NULL);
    Span *mlLoaded = MemLocal_Load(m, path, NULL);

    r |= MemLocal_Destroy(m, path, NULL);
    r |= Test((r & ERROR) == 0, "MemLocal destroy has no errors");

    String *rOne = (String *)Span_Get(mlLoaded, 0); 
    Span *rtbl = (Span *)Span_Get(mlLoaded, 1);
    String *rTblTwo_s = (String *)Table_Get(tbl, (Abstract *)pKeyTwo_s); 

    r |= Test(String_Length(rOne) == len, "Result string length matches");
    r |= Test(String_Equals(rTblTwo_s, expTwo_s), "Result key string matches");

    MemCtx_Free(m);
    MemCtx_Free(ml->m);
    MemCtx_Free(mlLoaded->m);
    DebugStack_Pop();
    return r;
}
