#define STRING_CHUNK_SIZE 1023
#define MAX_BASE10 20
typedef struct string {
    uchar *bytes;
    i64 length;
    i64 allocated;
    struct string *next;
} String;

String *String_Make(MemCtx *m, uchar *bytes);
String *String_From(MemCtx *m, char *cstr);
status String_Add(MemCtx *m, String *a, String *b);
status String_AddCstr(MemCtx *m, String *a, char *chars, int length);
i64 String_Length(String *s);
status String_Equals(String *a, String *b);
status String_EqualsCStr(String *a, char *cstr);
String *String_FromInt(MemCtx *m, int i);
String *String_FromRange(MemCtx *m, struct strcursor_range *range);
