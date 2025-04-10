i64 StrVec_ToFd(StrVec *v, int fd);
StrVec *StrVec_ReAlign(MemCh *m, StrVec *orig);
status StrVec_NextSlot(StrVec *v, Cursor *curs);
i32 StrVec_GetIdx(StrVec *v, Str *s);
status StrVec_Add(StrVec *v, Str *s);
status StrVec_AddVec(StrVec *v, StrVec *v2);
status StrVec_AddBytes(MemCh *m, StrVec *v, byte *ptr, i64 length);
StrVec *StrVec_Make(MemCh *m);
