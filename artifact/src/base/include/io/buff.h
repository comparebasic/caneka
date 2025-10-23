#define BUFF_CYCLE_MAX 512

enum send_recv_flags {
    BUFF_ASYNC = 1 << 8,
    BUFF_FD = 1 << 9,
    BUFF_SOCKET = 1 << 10,
    BUFF_STRVEC = 1 << 11,
    BUFF_FLUSH = 1 << 12,
    BUFF_UNBUFFERED = 1 << 13,
    BUFF_SLURP = 1 << 14,
};

typedef struct buff {
    Type type;
    i32 fd;
    MemCh *m;
    StrVec *v;
    struct {
        Str *s;
        i32 idx;
    } tail;
    struct {
        Str *s;
        i64 total;
        i32 idx;
    } unsent;
} Buff;

status Buff_AddBytes(Buff *bf, byte *bytes, word length);
Buff *Buff_Make(MemCh *m, word flags);

status Buff_SetFd(Buff *bf, i32 fd);
status Buff_UnsetFd(Buff *bf);
status Buff_UnsetSocket(Buff *bf);

status Buff_GetStr(Buff *bf, Str *s);
status Buff_GetToVec(Buff *bf, StrVec *v);

status Buff_Add(Buff *bf, Str *s);
status Buff_AddVec(Buff *bf, StrVec *v);
status Buff_Unbuff(Buff *bf, byte *bytes, i64 length, i64 *offset);
status Buff_UnbuffFd(MemCh *m, i32 fd, byte *bytes, i64 length, word flags, i64 *offset);
status Buff_Send(Buff *bf);
status Buff_SendToFd(Buff *bf, i32 fd);
status Buff_AddSend(Buff *bf, Str *s);
status Buff_SendAll(Buff *bf, StrVec *v);
status Buff_ReadAmount(Buff *bf, i64 amount);
status Buff_Read(Buff *bf);
