StrVec *StrVec_ToHex(MemCh *m, StrVec *v);
StrVec *StrVec_FromHex(MemCh *m, StrVec *v);
Str *Str_ToHex(MemCh *m, Str *s);
Str *Str_FromHex(MemCh *m, Str *s);
Str *Bytes_ToHexStr(MemCh *m, byte *b, i16 length);
status Raw_FromHex(MemCh *m, Str *s, void *b, i64 sz);
