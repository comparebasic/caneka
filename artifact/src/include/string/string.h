/* 
String
string library

related: core/string.h
*/

enum string_pos {
    STRING_POS_ALL = 1,
    STRING_POS_END,
    STRING_POS_START,
};

enum string_flags {
    FLAG_STRING_CONTIGUOUS = 1 << 8,
    FLAG_STRING_IS_NUM = 1 << 9,
    FLAG_STRING_IS_BOOL = 1 << 10,
    FLAG_STRING_IS_CASH = 1 << 11,
    FLAG_STRING_WHITESPACE = 1 << 12,
    FLAG_STRING_TEXT = 1 << 13,
    FLAG_STRING_BINARY = 1 << 14,
    FLAG_STRING_HEX = 1 << 15,
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
} String;

#define STRING_EXTEND -1

#define bytes(cstr) ((byte *)cstr)

String *String_Make(MemCtx *m, byte *bytes);
status String_Add(MemCtx *m, String *dest, String *toAdd);
status String_AddBytes(MemCtx *m, String *a, byte *chars, int length);
i64 String_Length(String *s);
String *String_Init(MemCtx *m, int expected);
String *String_Next(String *s);
i64 String_GetSegSize(String *s);
char *String_ToChars(MemCtx *m, String *s);
status String_AddSub(MemCtx *m, String *s, String *b, i64 start, i64 end);

/* makers */
status String_AddAsciiSrc(MemCtx *m, String *s, byte c);
status String_AddInt(MemCtx *m, String *s, int i);
status String_AddMemCount(MemCtx *m, String *s, i64 mem);
status String_AddBitPrint(MemCtx *m, String *s, byte *b, size_t length, boolean extended);
status String_AddI64(MemCtx *m, String *s, i64 i);
String *String_FromInt(MemCtx *m, int i);
String *String_FromI64(MemCtx *m, i64 i);
status String_MakeLower(String *s);
status String_MakeUpper(String *s);
String *String_FromAbs(MemCtx *m, Abstract *a);
String *String_FromFd(MemCtx *m, int fd);
String *String_Prefixed(MemCtx *m, String *s, String *prefix);

/* equals */
boolean String_Equals(String *a, String *b);
boolean String_EqualsBytes(String *a, byte *cstr);
boolean String_PosEqualsBytes(String *a, byte *cstr, int length, word pos_fl);
boolean String_EndsWith(String *a, String *b);
boolean String_StartsWith(String *a, String *b);

/* conversions */
String *String_ToCamel(MemCtx *m, String *s);
String *String_ToHex(MemCtx *m, String *s);
String *String_FromHex(MemCtx *m, String *s);
String *String_ToB64(MemCtx *m, String *s);
String *String_FromB64(MemCtx *m, String *s);
String *String_ToEscaped(MemCtx *m, String *s);
status String_ToSlab(String *a, void *sl, size_t sz);
String *String_Clone(MemCtx *m, String *s);
String *String_Sub(MemCtx *m, String *s, i64 start, i64 length);
status String_AddBitPrint(MemCtx *m, String *s, byte *bt, size_t length, boolean extended);
String *String_SubMatch(MemCtx *m, String *s, struct match *mt);

/* utils */
struct span *String_SplitToSpan(MemCtx *m, String *s, String *sep);
status String_Trunc(String *s, i64 len);
String *String_MakeFull(MemCtx *m, byte *bytes);
String *String_MakeFixed(MemCtx *m, byte *bytes, int length);
status String_Reset(String *s);
