#define STR_MAX MEM_SLAB_SIZE
#define STR_DEFAULT 512 
typedef struct str {
    Type type;
    word length;
    word alloc;
    byte *bytes;
} Str;

/* copy bytes into a new string */
Str *Str_From(MemCtx *m, byte *bytes, word length);
/* make a string that points to bytes */
Str *Str_Ref(MemCtx *m, byte *bytes, word length, word alloc);
/* setup an existing string struct */
status Str_Init(Str *s, byte *bytes, word length, word alloc);
/* make a blank allocated string */
Str *Str_Make(MemCtx *m, word alloc);
char *Str_ToCstr(MemCtx *m, Str *s);
/* write to a file descriptor */
i64 Str_ToFd(Str *s, int fd);
Str *Str_FromAbs(MemCtx *m, Abstract *a);
Str *Str_FromTyped(MemCtx *m, void *v, cls type);
i64 Out(MemCtx *m, char *fmt, ...);
