status File_Open(Buff *bf, void *_fpath, word ioFlags);
status File_Close(Buff *bf);
StrVec *File_ToVec(MemCh *m, Str *path);
status File_Unlink(MemCh *m, Str *path);
boolean File_Exists(Buff *bf, Str *path);
status File_Rename(MemCh *m, Str *to, Str *from);
status File_Stat(MemCh *m, Str *path, struct stat *st);
void File_ModTime(MemCh *m, Str *path, struct timespec *tm);
boolean File_PathExists(MemCh *m, Str *path);
