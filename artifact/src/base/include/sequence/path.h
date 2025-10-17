status Path_RangeOf(MemCh *m, StrVec *path, word sep, Coord *cr);
status Path_Around(MemCh *m, StrVec *path, word sep, Coord *cr);
StrVec *Path_Make(MemCh *m, Str *s, Span *sep);
status Path_Annotate(MemCh *m, StrVec *v, Span *sep);
StrVec *Path_Base(MemCh *m, StrVec *path);
Str *Path_StrAdd(MemCh *m, StrVec *path, Str *seg);
status PathStr_StrAdd(Str *s, Str *seg);
status Path_AddSlash(MemCh *m, StrVec *path);
