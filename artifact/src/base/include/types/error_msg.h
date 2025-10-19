typedef struct error_msg {
    Type type;
    char *func;
    char *file;
    i32 lineno;
    char *fmt;
    Abstract **args;
} ErrorMsg;

i64 ErrorMsg_ToStream(struct stream *sm, ErrorMsg *msg);
