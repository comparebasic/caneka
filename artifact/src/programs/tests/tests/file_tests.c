#include <external.h>
#include <caneka.h>

char *exampleCstr = ""
    "= Hidy Ho\n"
    "\n"
    "And this is a first paragraph\n"
    "with a second line.\n"
    "\n"
    ".fancy\n"
    "Second single sentance with details and a link _link=here@http://example.com.\n"
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

status File_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();
    status r = READY;

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./examples/example.fmt"));
    File *f = File_Make(m, path, NULL, STREAM_STRVEC);
    File_Open(f);
    File_Read(f, FILE_READ_MAX);
    File_Close(f);

    Str *s = Str_CstrRef(m, exampleCstr);
    s->type.state |= DEBUG;
    f->sm->dest.curs->v->type.state |= DEBUG;
    r |= Test(Equals((Abstract *)s, (Abstract *)f->sm->dest.curs->v), "File has expected content", NULL);

    MemCh_Free(m);
    DebugStack_Pop();

    r |= SUCCESS;

    return r;
}
