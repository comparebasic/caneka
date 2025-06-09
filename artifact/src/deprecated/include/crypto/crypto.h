#define IS_VISIBLE(x) ((x) >= 32 || (x) == '\n' || (x) == '\r' || (x) == '\t')

struct str *Crypto_RandomStr(MemCh *m, int length);
