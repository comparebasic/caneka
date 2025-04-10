status Dir_Destroy(MemCh *m, Str *path, Access *access);
status Dir_Gather(MemCh *m, Str *path, Span *sp);
status Dir_Exists(MemCh *m, Str *path);
status Dir_Climb(MemCh *m, Str *path, DirFunc dir, FileFunc file, Abstract *source);
status Dir_CheckCreate(MemCh *m, Str *path);
