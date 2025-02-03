#include <external.h>
#include <caneka.h>

#define TERMIO_TEST_PATH "./dist/fixtures/build_termio_output.2025-02-03" 

static status capture(word captureKey, int matchIdx, String *s, Abstract *source){
    DPrint((Abstract *)s, COLOR_YELLOW, 
        "TermIoCaptured: %s", Class_ToString(captureKey)); 
    return SUCCESS;
}

status TermIoFormat_Tests(MemCtx *gm){
    DebugStack_Push("TermIoFormat_Tests", TYPE_CSTR); 
    status r = READY;
    MemCtx *m = MemCtx_Make();
    
    String *fname = String_Make(m, bytes(TERMIO_TEST_PATH));
    File *file = File_Make(m, fname, NULL, NULL);
    file->abs = file->path;
    r |= File_Load(m, file, NULL);
    r |= Test((r & ERROR) == 0 && (file->type.state & FILE_LOADED) != 0, "File loaded and without error");

    Roebling *rbl = TermIo_RblMake(m, file->data, capture, NULL);

    DPrint((Abstract *)rbl, COLOR_PURPLE, "Rbl: ");
    Roebling_RunCycle(rbl);
    Roebling_RunCycle(rbl);
    DPrint((Abstract *)rbl, COLOR_PURPLE, "Rbl: ");

    MemCtx_Free(m);

    DebugStack_Pop();
    return r;
}
