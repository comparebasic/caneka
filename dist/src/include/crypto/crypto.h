#define IS_VISIBLE(x) ((x) >= 32 || (x) == '\n' || (x) == '\r' || (x) == '\t')

#define SHA256_BLOCK_SIZE 64
#define SHA256_DIGEST_SIZE 32

typedef struct sha256_state {
    Type type;
	uint64_t length;
	uint32_t state[8], curlen;
	uint8_t buf[SHA256_BLOCK_SIZE];
} Sha256;


void Sha256_init(struct sha256_state *md);
int Sha256_process(struct sha256_state *md, const unsigned char *in,
				   unsigned long inlen);
int Sha256_done(struct sha256_state *md, unsigned char *out);

struct string *Cont(MemCtx *m, byte *byte);
struct string *Buff(MemCtx *m, byte *content);
boolean TextCharFilter(byte *b, i64 length);

String *String_Sha256(MemCtx *m, String *s);
status Sha256_AddString(Sha256 *sha, String *s);
String *Sha256_Get(MemCtx *m, Sha256 *sha);
String *String_Sha256(MemCtx *m, String *s);
status EcKeyPair_Make(MemCtx *m, String *priv, String *pub);
String *Sign_Ecdsa(MemCtx *m, String *s, String *priv);
boolean Verify_Ecdsa(MemCtx *m, String *s, String *priv);
String *Crypto_RandomString(MemCtx *m, int length);
