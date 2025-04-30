#include <external.h>
#include <caneka.h>

#define TERMIO_TEST_PATH "./artifact/fixtures/build_termio_output.2025-02-03" 

static word _captureKey;
static String *_s;

static status capture(word captureKey, int matchIdx, String *s, Abstract *source){
    /*
    DPrint((Abstract *)s, COLOR_YELLOW, 
        "TermIoCaptured(%s)", Class_ToString(captureKey)); 
    */
    _captureKey = captureKey;
    _s = s;
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

    do {
        Roebling_RunCycle(rbl);
    } while((rbl->type.state & (ROEBLING_NEXT|NOOP)) == 0);
    r |= Test(_captureKey == TERMIO_TEXT && String_EqualsBytes(_s, bytes("")), "First is empty text");

    do {
        Roebling_RunCycle(rbl);
    } while((rbl->type.state & (ROEBLING_NEXT|NOOP)) == 0);
    r |= Test(_captureKey == TERMIO_MODE && String_EqualsBytes(_s, bytes("[")), "Then Command Mode '['");

    do {
        Roebling_RunCycle(rbl);
    } while((rbl->type.state & (ROEBLING_NEXT|NOOP)) == 0);
    r |= Test(_captureKey == TERMIO_NUM && String_EqualsBytes(_s, bytes("34")), "Then the number '34'");

    do {
        Roebling_RunCycle(rbl);
    } while((rbl->type.state & (ROEBLING_NEXT|NOOP)) == 0);
    r |= Test(_captureKey == TERMIO_CMD && String_EqualsBytes(_s, bytes("m")), "Then the color comand 'm'");

    do {
        Roebling_RunCycle(rbl);
    } while((rbl->type.state & (ROEBLING_NEXT|NOOP)) == 0);
    r |= Test(_captureKey == TERMIO_TEXT && String_EqualsBytes(_s, bytes("[Cleaning build]")), "Followed by some text %s/'%s'", Class_ToString(_captureKey), String_ToChars(DebugM, _s));

    do {
        Roebling_RunCycle(rbl);
    } while((rbl->type.state & (ROEBLING_NEXT|NOOP)) == 0);
    r |= Test(_captureKey == TERMIO_MODE && String_EqualsBytes(_s, bytes("[")), "Set mode");
    do {
        Roebling_RunCycle(rbl);
    } while((rbl->type.state & (ROEBLING_NEXT|NOOP)) == 0);
    r |= Test(_captureKey == TERMIO_NUM && String_EqualsBytes(_s, bytes("0")), "Set no color (0)");
    do {
        Roebling_RunCycle(rbl);
    } while((rbl->type.state & (ROEBLING_NEXT|NOOP)) == 0);
    r |= Test(_captureKey == TERMIO_CMD && String_EqualsBytes(_s, bytes("m")), "Set mode is color");
    do {
        Roebling_RunCycle(rbl);
    } while((rbl->type.state & (ROEBLING_NEXT|NOOP)) == 0);
    r |= Test(_captureKey == TERMIO_TEXT && String_EqualsBytes(_s, bytes("\n")), "Text content for newline");
    do {
        Roebling_RunCycle(rbl);
    } while((rbl->type.state & (ROEBLING_NEXT|NOOP)) == 0);
    r |= Test(_captureKey == TERMIO_MODE && String_EqualsBytes(_s, bytes("[")), "Set mode");
    do {
        Roebling_RunCycle(rbl);
    } while((rbl->type.state & (ROEBLING_NEXT|NOOP)) == 0);
    r |= Test(_captureKey == TERMIO_NUM && String_EqualsBytes(_s, bytes("33")), "Color yellow (33)");
    do {
        Roebling_RunCycle(rbl);
    } while((rbl->type.state & (ROEBLING_NEXT|NOOP)) == 0);
    r |= Test(_captureKey == TERMIO_CMD && String_EqualsBytes(_s, bytes("m")), "Set color cmd");
    do {
        Roebling_RunCycle(rbl);
    } while((rbl->type.state & (ROEBLING_NEXT|NOOP)) == 0);
    r |= Test(_captureKey == TERMIO_TEXT && String_EqualsBytes(_s, bytes("Making Directory build/libcaneka")), "Have expected text %s/'%s'", Class_ToString(_captureKey),String_ToChars(DebugM, _s));

    MemCtx_Free(m);

    DebugStack_Pop();
    return r;
}
