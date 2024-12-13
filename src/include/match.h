#define PAT_CHAR_MAX_LENGTH 64

enum pat_flags {
    PAT_END = 0, /* E */
    PAT_TERM = 1 << 0, /* X */
    PAT_OPTIONAL = 1 << 1, /* P */
    PAT_MANY = 1 << 2, /* M */
    PAT_ANY = 1 << 3, /* N */
    PAT_INVERT = 1 << 4, /* I */
    PAT_COUNT = 1 << 5, /* C */
    PAT_SET_NOOP = 1 << 6, /* U */
    PAT_NO_CAPTURE = 1 << 7, /* G */
    PAT_ALL = 1 << 8, /* A */
    PAT_KO = 1 << 9, /* K */
    PAT_SINGLE = 1 << 10, /* S */
    PAT_CONSUME  = 1 << 11, /* O */
    PAT_LEAVE  = 1 << 12, /* L */
    PAT_CMD = 1 << 13, /* D */
    PAT_GO_ON_FAIL = 1 << 14, /* T */
};

enum match_flags {
    /* */
    ANCHOR_UNTIL = 1 << 11,
    ANCHOR_CONTAINS = 1 << 12,
    ANCHOR_START = 1 << 13,
    SEARCH = 1 << 14, 
    SUCCESS_EMPTY = 1 << 15,
};

#define patText \
    PAT_OPTIONAL|PAT_MANY, '\t', '\t', PAT_OPTIONAL|PAT_MANY, '\r', '\r', PAT_OPTIONAL|PAT_MANY, '\n', '\n', PAT_INVERT|PAT_MANY|PAT_TERM, 0, 31

#define patWhiteSpace \
    PAT_MANY, '\t', '\t', PAT_MANY, '\r', '\r', PAT_MANY, '\n', '\n', PAT_MANY|PAT_TERM, ' ', ' ' 

#define TEXT_DEF patText, PAT_END, 0, 0
#define NL_DEF PAT_TERM, '\n', '\n', PAT_END, 0, 0
#define WS_REQUIRED PAT_MANY, ' ',' ', PAT_MANY, '\t','\t', \
    PAT_MANY, '\r','\r', PAT_MANY|PAT_TERM, '\n','\n'
#define WS_OPTIONAL PAT_OPTIONAL|PAT_ANY, ' ',' ', PAT_OPTIONAL|PAT_ANY, '\t','\t', \
    PAT_OPTIONAL|PAT_ANY, '\r','\r', PAT_OPTIONAL|PAT_ANY|PAT_TERM, '\n','\n'

#define WS_INVERT_MANY PAT_INVERT|PAT_MANY, ' ',' ', PAT_INVERT|PAT_MANY, '\t','\t', \
    PAT_INVERT|PAT_MANY,'\r','\r', PAT_INVERT|PAT_MANY, '\n','\n'

#define UPPER_DEF PAT_TERM, 'A', 'Z' 

typedef struct range_chardef {
    word flags;
    word from;
    word to;
} PatCharDef;

typedef struct _pat_match {
    PatCharDef *startDef;
    PatCharDef *endDef;
    PatCharDef *curDef;
    PatCharDef *startTermDef;
} _patMatch;

typedef struct _string_match {
    String *s;
    int position;
}_stringMatch;

typedef struct match {
    Type type; 
    union {
        _patMatch pat;
        _stringMatch str;
    } def;
    int jump;
    int remaining;
    int count;
    int lead;
    int tail;
    word counter;
    word captureKey;
} Match;

Match *Match_Make(MemCtx *m, String *s, word flags);
status Match_SetPattern(Match *mt, PatCharDef *def);
status Match_SetString(Match *mt, String *s);
status Match_Feed(Match *mt, byte c);
status Match_FeedEnd(Match *mt);
PatCharDef *Match_GetDef(Match *mt);
