#define BUFF_CYCLE_MAX 512

enum send_recv_flags {
    BUFF_ASYNC = 1 << 8,
    BUFF_FD = 1 << 9,
    BUFF_SOCKET = 1 << 10,
    BUFF_FLUSH = 1 << 12,
    BUFF_UNBUFFERED = 1 << 13,
    BUFF_SLURP = 1 << 14,
    BUFF_CLOBBER = 1 << 15,
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
        word _;
        word offset;
        i32 idx;
        i64 total;
    } unsent;
    struct stat st;
} Buff;

status Buff_SetFd(Buff *bf, i32 fd);
status Buff_SetSocket(Buff *bf, i32 fd);
status Buff_UnsetFd(Buff *bf);
status Buff_UnsetSocket(Buff *bf);

boolean Buff_IsEmpty(Buff *bf);
status Buff_Stat(Buff *bf);

status Buff_GetStr(Buff *bf, Str *s);
status Buff_RevGetStr(Buff *bf, Str *s);
status Buff_GetToVec(Buff *bf, StrVec *v);

status Buff_Add(Buff *bf, Str *s);
status Buff_AddVec(Buff *bf, StrVec *v);
status Buff_AddBytes(Buff *bf, byte *bytes, i64 length);
status Buff_Flush(Buff *bf);

status Buff_Read(Buff *bf);
status Buff_ReadAmount(Buff *bf, i64 amount);
status Buff_ReadToStr(Buff *bf, Str *s);

status Buff_Pipe(Buff *to, Buff *from);

status Buff_PosAbs(Buff *bf, i64 position);
status Buff_Pos(Buff *bf, i64 position);
status Buff_PosEnd(Buff *bf);

Buff *Buff_From(MemCh *m, StrVec *v);
Buff *Buff_Make(MemCh *m, word flags);
