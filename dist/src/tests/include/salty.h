#define SALTY_TEA_ROUNDS 4
#define SALTY_VALUE_SIZE (sizeof(uint32_t) * 2)
#define SALTY_KEY_SEGSIZE  (sizeof(uint32_t) * 4)
#define SALTY_KEY_MINSIZE SALTY_KEY_SEGSIZE / 2

typedef void (*Salty_Proc)(unsigned int rounds, uint32_t v[2], uint32_t const key[4]);

String *CB_Phrase(MemCtx *m);
String *Salty_MakeKey(MemCtx *m, String *s);
status Salty_Enc(MemCtx *m, String *key, String *s);
status Salty_Dec(MemCtx *m, String *key, String *s);
static status Salty_process(MemCtx *m, String *key, String *s, Salty_Proc proc);
