#include <external.h>
#include <caneka.h>

char *cstr = ""
    "Most of the efficiency comes from allowing integers and static indexes to"
    "be used more often than string interpretation during runtime. Remembring"
    "the position of something without needing the steps you took to find it"
    "again can speed things up."
    "\n"
    "The combination of the Span and the basic type system make retrieving an"
    "item from a Lookup structure built from a simple enum possible throughout"
    "the language."
    "\n"
    "This combined with the simplicity of the re-query based Table for key-value"
    "store behavior makes Caneka an easy to understand light-weight to run,"
    "powerful language."
    "\n"
    ;

status MemLocal_Tests(MemCtx *gm){
    DebugStack_Push("MemLocal_Test", TYPE_CSTR);
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

    String *two = String_Make(ml->m, bytes(cstr));
    String *three = String_Make(ml->m, bytes(cstr));
    Hashed *h = Hashed_Make(ml->m, (Abstract *)two);
    h->value = (Abstract *)three;

    Hashed *next = Hashed_Make(ml->m, (Abstract *)String_Make(ml->m, bytes("Hi")));
    next->value = (Abstract *)String_Make(ml->m, bytes("There"));
    h->next = next;

    MemLocal_To(ml->m, (Abstract *)h);

    char buff[PATH_BUFFLEN];
    String *path = String_Make(m, bytes(getcwd(buff, PATH_BUFFLEN)));
    char *cstr = "/tmp/mstore";
    String_AddBytes(m, path, bytes(cstr), strlen(cstr));

    MemLocal_Persist(m, ml, path, NULL);
    Span *mlLoaded = MemLocal_Load(m, path, NULL);
    DPrint((Abstract *)mlLoaded, COLOR_CYAN, "ml: ");

    /*
    MemLocal_Destroy(m, path, NULL);
    */

    MemCtx_Free(m);

    DebugStack_Pop();
    return r;
}
