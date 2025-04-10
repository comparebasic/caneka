status File_Exists(Str *path);
status File_Unlink(Str *path);
Str *File_GetCwdPath(MemCh *m, Str *path);
Str *File_GetAbsPath(MemCh *m, Str *path);
boolean File_CmpUpdated(MemCh *m, Str *a, Str *b, Access *ac);
