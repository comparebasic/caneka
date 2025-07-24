status Path_Exists(Str *path);
status Path_Unlink(Str *path);
Str *Path_GetCwdPath(MemCh *m, Str *path);
Str *Path_GetAbsPath(MemCh *m, Str *path);
status Path_AddSlash(StrVec *path);
Str *Path_BasePath(MemCh *m, StrVec *path);
Str *Path_FileName(MemCh *m, StrVec *path);
boolean Path_CmpUpdated(MemCh *m, Str *a, Str *b, Access *ac);
