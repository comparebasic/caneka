status Recv_Get(SendRecv *sr, Str *s, i16 *offset);
status Recv_GetTo(SendRecv *sr, i32 fd, i16 length, i16 *offset);
status Recv_GetVec(SendRecv *sr, StrVec *v);

SendRecv *Recv_Make(MemCh *m, i32 fd, StrVec *v, word flags);
