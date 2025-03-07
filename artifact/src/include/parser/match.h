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
    MATCH_SEARCH = 1 << 13, 
    MATCH_LEAVE = 1 << 14,
    MATCH_ACCEPT_EMPTY = 1 << 15
};

typedef struct pat_match {
    PatCharDef *startDef;
    PatCharDef *endDef;
    PatCharDef *curDef;
    PatCharDef *startTermDef;
} PatMatch;

typedef struct match {
    Type type; 
    PatMatch pat;
    StrSnip snip;
    String *backlog;
    int jump;
    int remaining;
    word counter;
    word captureKey;
} Match;

Match *Match_Make(MemCtx *m, String *s, word flags);
status Match_SetPattern(Match *mt, PatCharDef *def, String *backlog);
status Match_SetString(MemCtx *m, Match *mt, String *s, String *backlog);
status Match_Feed(MemCtx *m, Match *mt, word c);
status Match_FeedString(MemCtx *m, Match *mt, String *s, int offset);
status Match_FeedEnd(MemCtx *m, Match *mt);
status Match_AddFlagsToStr(MemCtx *m, String *s, word flag);
int Match_Total(Match *mt);
PatCharDef *Match_GetDef(Match *mt);
