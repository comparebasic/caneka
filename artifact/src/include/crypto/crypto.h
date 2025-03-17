#define IS_VISIBLE(x) ((x) >= 32 || (x) == '\n' || (x) == '\r' || (x) == '\t')

static inline boolean TextCharFilter(byte *b, i64 length);
struct str *Crypto_RandomString(MemCtx *m, int length);
