status File_Open(Buff *bf, Str *fpath, word ioFlags);
status File_Close(Buff *bf);
StrVec *File_ToVec(MemCh *m, Str *path);
status File_Unlink(MemCh *m, Str *path);
