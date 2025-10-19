typedef struct error_msg {
    Type type;
    Abstract *lineInfo[4];
    struct str *fmt;
    Abstract **args;
} ErrorMsg;

i64 ErrorMsg_ToStream(struct stream *sm, ErrorMsg *msg);
ErrorMsg *ErrorMsg_Make(struct mem_ctx *m, 
    char *func, char *file, int line, char *fmt, Abstract **args);
