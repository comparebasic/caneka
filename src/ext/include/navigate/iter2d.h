typedef struct iter2d {
    Type type;
    MemCh *m;
    Iter it;
    Span *coord;
    StrVec *path;
} Iter2d;

void Iter2d_InstInit(Iter2d *it2d, Inst *inst, Iter *it);
void Iter2d_InstNext(Iter2d *it2d);
void *Iter2d_Get(Iter *it2d);
Span *Iter2d_GetCoords(Iter *it2d);
StrVec *Iter2d_GetPath(Iter *it2d);
Iter2d *Iter2d_Make(MemCh *m);
