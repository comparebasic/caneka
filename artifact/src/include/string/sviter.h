typedef struct sviter {
    Type type;
    int idx;
    StrVec *vh;
    StrVecEntry *ve;
}SvIter;

status SvIter_Init(SvIter *it, StrVec *vh);
status SvIter_Next(SvIter *it);
StrVecEntry *SvIter_Get(SvIter *it);
