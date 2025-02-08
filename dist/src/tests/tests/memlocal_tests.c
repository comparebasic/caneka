#include <external.h>
#include <caneka.h>

char *cstr = ""
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

char *cstrTwo = ""
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

char *cstrThree = ""
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
    printf("to: \n");
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

    MemCtx_Free(m);
    DebugStack_Pop();
    return r;
}

status MemLocal_Tests(MemCtx *gm){
    DebugStack_Push("MemLocal_Tests", TYPE_CSTR);
    status r = READY;
    MemCtx *m = MemCtx_Make();

    Span *ml = MemLocal_Make(TYPE_SPAN);

    char buff[PATH_BUFFLEN];
    String *path = String_Make(m, bytes(getcwd(buff, PATH_BUFFLEN)));
    char *cstr = "/tmp/mstore";
    String_AddBytes(m, path, bytes(cstr), strlen(cstr));

    r |= MemLocal_Persist(m, ml, path, NULL);
    Span *mlLoaded = MemLocal_Load(m, path, NULL);
    r |= MemLocal_Destroy(m, path, NULL);

    MemCtx_Free(m);
    DebugStack_Pop();
    return r;
}
