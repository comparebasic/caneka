#define PAT_CHAR_MAX_LENGTH 64

enum pat_flags {
    PAT_END = 0, /* E */
    PAT_TERM = 1 << 0, /* X */
    PAT_OPTIONAL = 1 << 1, /* P */
    PAT_MANY = 1 << 2, /* M */
    PAT_ANY = 1 << 3, /* N */
    PAT_INVERT = 1 << 4, /* I */
    PAT_COUNT = 1 << 5, /* C */
    PAT_INVERT_CAPTURE = 1 << 6, /* G */
    PAT_KO = 1 << 7, /* K */
    PAT_KO_TERM = 1 << 8, /* O */
    PAT_SINGLE = 1 << 9, /* S */
    PAT_LEAVE  = 1 << 10, /* L */
    PAT_CMD = 1 << 11, /* D */
    PAT_GO_ON_FAIL = 1 << 12, /* T */
    PAT_CONSUME = 1 << 13, /* U */
};

enum match_flags {
    MATCH_GOTO = 1 << 8,
    MATCH_INVERTED = 1 << 9,
    MATCH_TERM_FOUND = 1 << 10,
    MATCH_KO_INVERT = 1 << 11,
    MATCH_KO = 1 << 12,
    ANCHOR_START = 1 << 13,
    SEARCH = 1 << 14, 
    MATCH_LEAVE = 1 << 15
};

typedef struct patchardef {
    word flags;
    word from;
    word to;
} PatCharDef;

typedef struct pat_match {
    PatCharDef *startDef;
    PatCharDef *endDef;
    PatCharDef *curDef;
    PatCharDef *startTermDef;
} PatMatch;

typedef struct match {
    Type type; 
    PatMatch pat;
    String *snips;
    int jump;
    int remaining;
    word counter;
    word captureKey;
} Match;

Match *Match_Make(MemCtx *m, String *s, word flags);
status Match_SetPattern(Match *mt, PatCharDef *def);
status Match_SetString(MemCtx *m, Match *mt, String *s);
status Match_Feed(Match *mt, word c);
status Match_FeedString(Match *mt, String *s, int offset);
status Match_FeedEnd(Match *mt);
status Match_AddFlagsToStr(MemCtx *m, String *s, word flag);
PatCharDef *Match_GetDef(Match *mt);
#define Match_Total(x) ((x)->count+(x)->tail+(x)->lead)
