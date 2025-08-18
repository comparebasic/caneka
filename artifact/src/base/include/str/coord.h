typedef struct coord {
    Type type;
    i16 a;
    i16 b;
} Coord;

Coord *Coord_Make(MemCh *m, word a, word b);
StrVec *StrVec_CoordCopy(MemCh *m, StrVec *v, Coord *cr);
