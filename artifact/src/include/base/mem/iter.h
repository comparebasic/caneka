status Iter_Next(Iter *it);
status Iter_Query(Iter *it);
status Iter_Reset(Iter *it);
void Iter_Init(Iter *it, Span *p);
void Iter_Setup(Iter *it, Span *p, status op, i32 idx);
Iter *Iter_Make(MemCh *m, Span *p);
