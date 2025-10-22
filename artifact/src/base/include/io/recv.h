status Recv_Get(SendRecv *sr, Str *s);
status Recv_GetToVec(SendRecv *sr, StrVec *v);
status Recv_GetToFd(SendRecv *sr, i32 fd, i64 length, i64 *offset);
