status IoUtil_Exists(Str *path);
status IoUtil_Unlink(Str *path);
status IoUtil_AddSlash(StrVec *path);
Str *IoUtil_GetCwdPath(MemCh *m, Str *path);
Str *IoUtil_GetAbsPath(MemCh *m, Str *path);
boolean IoUtil_CmpUpdated(MemCh *m, Str *a, Str *b, Access *ac);
StrVec *IoUtil_Fname(MemCh *m, StrVec *path);
StrVec *IoUtil_BasePath(MemCh *m, StrVec *path);
