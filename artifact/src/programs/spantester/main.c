#include <external.h>
#include <caneka.h>

#include "./mocks.c"
#include "../../base/mem/span.c"
#include "../../base/mem/iter.c"

typedef struct cint_pair {
    int i;
    char *cstr;
} CharIntPair;


CharIntPair ToTest[] = {
    {0, "ZERO"},
    {1, "ONE"},
    {7, "SEVEN"},
    {0, NULL},
};

CharIntPair ToTestDim1[] = {
    {0, "ZERO"},
    {1, "ONE"},
    {7, "SEVEN"},
    {16, "SEXTEEN"},
    {63, "SIXTY-THREE"},
    {64, "SIXTY-FOUR"},
    {0, NULL},
};

CharIntPair ToTestDim4[] = {
    {0, "ZERO"},
    {1, "ONE"},
    {7, "SEVEN"},
    {16, "SEXTEEN"},
    {63, "SIXTY-THREE"},
    {64, "SIXTY-FOUR"},
    {4073, "FourK73"},
    {70123, "SeventyK123"},
    {0, NULL},
};

static status testPairs(char *desc, CharIntPair *testPairs){
    printf("\x1b[1m[%s]\x1b[22m\n", desc);
    status r = READY;
    Span *p = Span_Make(NULL);
    /*
    p->type.state |= DEBUG;
    */
    CharIntPair *cpair = testPairs;
    int idx = -1;
    while(cpair->cstr != NULL){
        printf("Set cpair:%d/%s\n", cpair->i,cpair->cstr);
        Span_Set(p, cpair->i, (Abstract *)cpair->cstr);
        cpair++;
    }

    cpair = testPairs;
    while(cpair->cstr != NULL){
        char *cstr = Span_Get(p, cpair->i);
        int color = (r & ERROR) == 0 ? 32 : 31;
        if(strncmp(cpair->cstr, cstr, strlen(cpair->cstr)) == 0){
            r |= SUCCESS;
        }else{
            color = 31;
            r |= ERROR;
        }
        printf("Get cpair:%d/%s -> \x1b[%dm%s\x1b[0m\n", cpair->i,cpair->cstr, color, cstr);
        cpair++;
    }

    Iter it;
    Iter_Init(&it, p);
    /*
    it.type.state |= DEBUG;
    */
    while((Iter_Next(&it) & END) == 0){
        char *cstr = (char *)Iter_Get(&it);
        if(cstr != NULL){
            printf("    \x1b[33m%s\x1b[0m\n", cstr);
        }
    }

    return r;
}

int main(int argc, char *argv[]){
    testPairs("Test to 7", ToTest);
    testPairs("Test to 64", ToTestDim1);
    testPairs("Test to 70123", ToTestDim4);

    return 0;
}
