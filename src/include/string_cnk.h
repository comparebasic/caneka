/* 
String
string library

related: core/string.h
*/

enum string_flags {
    FLAG_STRING_IS_NUM = 1 << 9,
    FLAG_STRING_IS_BOOL = 1 << 10,
    FLAG_STRING_IS_CASH = 1 << 11,
    FLAG_STRING_WHITESPACE = 1 << 12,
    FLAG_STRING_TEXT = 1 << 13,
};

typedef struct stringmin {
    Type type;
    int length;
    byte bytes[STRING_FIXED_SIZE+1];
} StringMin;

typedef struct stringful {
    Type type;
    int length;
    byte bytes[STRING_FULL_SIZE+1];
} StringFull;

typedef struct string {
    Type type;
    int length;
    byte bytes[STRING_CHUNK_SIZE];
    struct string *next;
    MemCtx *m;
} String;

#define STRING_EXTEND -1

#define bytes(cstr) ((byte *)cstr)

String *String_Make(MemCtx *m, byte *bytes);
String *String_MakeFull(MemCtx *m, byte *bytes);
String *String_MakeFixed(MemCtx *m, byte *bytes, int length);
String *String_From(MemCtx *m, byte *bytes);
status String_Add(MemCtx *m, String *dest, String *toAdd);
status String_AddBytes(MemCtx *m, String *a, byte *chars, int length);
i64 String_Length(String *s);
boolean String_EqualsBytes(String *a, byte *cstr);
String *String_FromInt(MemCtx *m, int i);
String *String_FromI64(MemCtx *m, i64 i);
String *String_FromRange(MemCtx *m, struct strcursor_range *range);
String *String_Init(MemCtx *m, int expected);
String *String_Next(String *s);
boolean String_Equals(String *a, String *b);
boolean String_Eq(Abstract *a, void *b);
status String_AddInt(MemCtx *m, String *s, int i);
status String_AddI64(MemCtx *m, String *s, i64 i);
status String_Reset(String *s);

/* conversions */
String *String_ToHex(MemCtx *m, String *s);
String *String_ToEscaped(MemCtx *m, String *s);
char *String_ToChars(MemCtx *m, String *s);
status String_ToSlab(String *a, void *sl, size_t sz);
String *String_Clone(MemCtx *m, String *s);
