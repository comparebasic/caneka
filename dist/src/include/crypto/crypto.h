#define IS_VISIBLE(x) ((x) >= 32 || (x) == '\n' || (x) == '\r' || (x) == '\t')

struct string *Cont(MemCtx *m, byte *byte);
struct string *Buff(MemCtx *m, byte *content);
boolean TextCharFilter(byte *b, i64 length);
String *Crypto_RandomString(MemCtx *m, int length);
