
#include <external.h>
#include <caneka.h>
#include <testsuite.h>

status Match_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    Span *p;
    status r = READY;

    Match mt;
    String *s;

    s = String_Make(m, bytes("poo"));
    Match_SetString(&mt, s);

    s = String_Make(m, bytes("no"));
    for(int i = 0; i < s->length; i++){
        Match_Feed(&mt, s->bytes[i]);
    }

    r |= Test(mt.type.state != COMPLETE, "Non match has unsuccessful state found %s", State_ToString(mt.type.state)); 
    s = String_Make(m, bytes("poo"));
    for(int i = 0; i < s->length; i++){
        Match_Feed(&mt, s->bytes[i]);
    }
    r |= Test(mt.type.state == COMPLETE, "Matching string has successful state found %s", State_ToString(mt.type.state)); 

    Debug_Print((void *)&mt, 0, "Match: ", COLOR_CYAN, TRUE);
    printf("\n");

    MemCtx_Free(m);
    return r;
}
