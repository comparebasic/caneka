typedef struct error_msg {
    Type type;
    void *lineInfo[4];
    struct str *fmt;
    void **args;
} ErrorMsg;

ErrorMsg *ErrorMsg_Make(struct mem_ctx *m, 
    char *func, char *file, int line, char *fmt, void *args[]);
void ErrorMsg_Fmt(struct buff *bf, ErrorMsg *msg);
