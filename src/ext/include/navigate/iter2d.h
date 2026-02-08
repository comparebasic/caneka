typedef struct iter2d {
    Type type;
    MemCh *m;
    Iter it;
    Span *coord;
    StrVec *path;
} Iter2d;

void Iter2d_InstNext(Iter2d *it2d);
void *Iter2d_Get(Iter2d *it2d);
StrVec *Iter2d_GetPath(Iter2d *it2d);
Span *Iter2d_GetCoord(Iter2d *it2d);
status Iter2d_State(Iter2d *it2d);
Iter2d *Iter2d_Make(MemCh *m, Inst *inst);
