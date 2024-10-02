#include <external.h>
#include <caneka.h>

status ProtoHttp_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    Match *mt = NULL;
    String *s = NULL;

    word alpha[] = {PAT_MANY, 'a', 'z', PAT_MANY, 'A', 'Z', PAT_END, 0, 0};
    mt = Match_MakePat(m, bytes(alpha), ANCHOR_START);
    Debug_Print((void *)mt, 0, "Match Alpha: ", COLOR_CYAN, TRUE);

    return r;
}
