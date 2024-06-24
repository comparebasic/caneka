typedef struct stringmin {
    Type type;
    word length;
    byte bytes[32];
} StringMin;

typedef struct string {
    Type type;
    word length;
    byte bytes[STRING_CHUNK_SIZE];
    struct string *next;
} String;

#define bytes(cstr) ((byte *)cstr)

String *String_Make(MemCtx *m, byte *bytes);
String *String_MakeFixed(MemCtx *m, byte *bytes, int length);
String *String_From(MemCtx *m, byte *bytes);
status String_Add(MemCtx *m, String *a, String *b);
status String_AddBytes(MemCtx *m, String *a, byte *chars, int length);
i64 String_Length(String *s);
status String_Equals(String *a, String *b);
status String_EqualsBytes(String *a, byte *cstr);
String *String_FromInt(MemCtx *m, int i);
String *String_FromRange(MemCtx *m, struct strcursor_range *range);
String *String_Init(MemCtx *m, int expected);
