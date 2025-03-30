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

CharIntPair ToTestDim2[] = {
    {0, "ZERO"},
    {1, "ONE"},
    {7, "SEVEN"},
    {16, "SEXTEEN"},
    {63, "SIXTY-THREE"},
    {64, "SIXTY-FOUR"},
    {279, "TWO-79"},
    {0, NULL},
};

CharIntPair ToTestDim4[] = {
    {0, "ZERO"},
    {1, "ONE"},
    {7, "SEVEN"},
    {16, "SEXTEEN"},
    {63, "SIXTY-THREE"},
    {64, "SIXTY-FOUR"},
    {279, "TWO-79"},
    {4073, "FourK73"},
    {10777, "Ten777"},
    {70123, "SeventyK123"},
    {0, NULL},
};

CharIntPair ToTest137ValuesDim2[] = {
    {0, "Item0/00"},
    {1, "Item1/01"},
    {2, "Item2/02"},
    {3, "Item3/03"},
    {4, "Item4/04"},
    {5, "Item5/05"},
    {6, "Item6/06"},
    {7, "Item7/07"},
    {8, "Item8/08"},
    {9, "Item9/09"},
    {10, "Item10/0a"},
    {11, "Item11/0b"},
    {12, "Item12/0c"},
    {13, "Item13/0d"},
    {14, "Item14/0e"},
    {15, "Item15/0f"},
    {16, "Item16/10"},
    {17, "Item17/11"},
    {18, "Item18/12"},
    {19, "Item19/13"},
    {20, "Item20/14"},
    {21, "Item21/15"},
    {22, "Item22/16"},
    {23, "Item23/17"},
    {24, "Item24/18"},
    {25, "Item25/19"},
    {26, "Item26/1a"},
    {27, "Item27/1b"},
    {28, "Item28/1c"},
    {29, "Item29/1d"},
    {30, "Item30/1e"},
    {31, "Item31/1f"},
    {32, "Item32/20"},
    {33, "Item33/21"},
    {34, "Item34/22"},
    {35, "Item35/23"},
    {36, "Item36/24"},
    {37, "Item37/25"},
    {38, "Item38/26"},
    {39, "Item39/27"},
    {40, "Item40/28"},
    {41, "Item41/29"},
    {42, "Item42/2a"},
    {43, "Item43/2b"},
    {44, "Item44/2c"},
    {45, "Item45/2d"},
    {46, "Item46/2e"},
    {47, "Item47/2f"},
    {48, "Item48/30"},
    {49, "Item49/31"},
    {50, "Item50/32"},
    {51, "Item51/33"},
    {52, "Item52/34"},
    {53, "Item53/35"},
    {54, "Item54/36"},
    {55, "Item55/37"},
    {56, "Item56/38"},
    {57, "Item57/39"},
    {58, "Item58/3a"},
    {59, "Item59/3b"},
    {60, "Item60/3c"},
    {61, "Item61/3d"},
    {62, "Item62/3e"},
    {63, "Item63/3f"},
    {64, "Item64/40"},
    {65, "Item65/41"},
    {66, "Item66/42"},
    {67, "Item67/43"},
    {68, "Item68/44"},
    {69, "Item69/45"},
    {70, "Item70/46"},
    {71, "Item71/47"},
    {72, "Item72/48"},
    {73, "Item73/49"},
    {74, "Item74/4a"},
    {75, "Item75/4b"},
    {76, "Item76/4c"},
    {77, "Item77/4d"},
    {78, "Item78/4e"},
    {79, "Item79/4f"},
    {80, "Item80/50"},
    {81, "Item81/51"},
    {82, "Item82/52"},
    {83, "Item83/53"},
    {84, "Item84/54"},
    {85, "Item85/55"},
    {86, "Item86/56"},
    {87, "Item87/57"},
    {88, "Item88/58"},
    {89, "Item89/59"},
    {90, "Item90/5a"},
    {91, "Item91/5b"},
    {92, "Item92/5c"},
    {93, "Item93/5d"},
    {94, "Item94/5e"},
    {95, "Item95/5f"},
    {96, "Item96/60"},
    {97, "Item97/61"},
    {98, "Item98/62"},
    {99, "Item99/63"},
    {100, "Item100/64"},
    {101, "Item101/65"},
    {102, "Item102/66"},
    {103, "Item103/67"},
    {104, "Item104/68"},
    {105, "Item105/69"},
    {106, "Item106/6a"},
    {107, "Item107/6b"},
    {108, "Item108/6c"},
    {109, "Item109/6d"},
    {110, "Item110/6e"},
    {111, "Item111/6f"},
    {112, "Item112/70"},
    {113, "Item113/71"},
    {114, "Item114/72"},
    {115, "Item115/73"},
    {116, "Item116/74"},
    {117, "Item117/75"},
    {118, "Item118/76"},
    {119, "Item119/77"},
    {120, "Item120/78"},
    {121, "Item121/79"},
    {122, "Item122/7a"},
    {123, "Item123/7b"},
    {124, "Item124/7c"},
    {125, "Item125/7d"},
    {126, "Item126/7e"},
    {127, "Item127/7f"},
    {128, "Item128/80"},
    {129, "Item129/81"},
    {130, "Item130/82"},
    {131, "Item131/83"},
    {132, "Item132/84"},
    {133, "Item133/85"},
    {134, "Item134/86"},
    {135, "Item135/87"},
    {136, "Item136/88"},
    {137, "Item137/89"},
    {0, NULL},
};

static status testPairs(char *desc, CharIntPair *testPairs){
    printf("\x1b[1m[%s]\x1b[22m\n", desc);
    status r = READY;
    Span *p = Span_Make(NULL);
    p->type.state |= DEBUG;
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
        if(strncmp(cpair->cstr, cstr, strlen(cpair->cstr)) == 0){
            r |= SUCCESS;
        }else{
            r |= ERROR;
        }

        int color = (r & ERROR) == 0 ? 32 : 31;
        printf("Get cpair:%d/%s -> \x1b[%dm%s\x1b[0m\n",
            cpair->i,cpair->cstr, color, cstr);
        cpair++;
    }

    cpair = testPairs;
    Iter it;
    Iter_Init(&it, p);
    it.type.state |= DEBUG;
    while((Iter_Next(&it) & END) == 0){
        char *cstr = (char *)it.value;
        if(cstr != NULL){
            if(strncmp(cpair->cstr, cstr, strlen(cpair->cstr)) == 0){
                r |= SUCCESS;
            }else{
                r |= ERROR;
            }
        
            int color = (r & ERROR) == 0 ? 32 : 31;
            printf("    \x1b[%dm%s\x1b[0m\n", color, cstr);
            cpair++;
        }
    }

    return r;
}

int main(int argc, char *argv[]){
    testPairs("Test to 7", ToTest);
    testPairs("Test to 64", ToTestDim1);
    testPairs("Test to 279", ToTestDim2);
    /*
    testPairs("Test to 70123", ToTestDim4);
    testPairs("Test to 137 Items", ToTest137ValuesDim2);
    */

    return 0;
}

