enum range_flags {
    PAT_TERM = 1 << 0,
    PAT_MANY = 1 << 1,
    PAT_ANY = 1 << 2,
    PAT_INVERT = 1 << 3,
    PAT_COUNT = 1 << 4,
    ANCHOR_UNTIL = 1 << 5,
    ANCHOR_CONTAINS = 1 << 6,
    ANCHOR_START = 1 << 7,
};

#define PAT_WHITESPACE(flags) \
    {(flags), ' ', ' '}, \
    {(flags), '\n', '\n'}, \
    {(flags), '\r', '\r'}, \
    {(flags)|PAT_TERM, '\t', '\t'}

#define PAT_INT \
    PAT_TERM, '0', '9'

#define PAT_FLOAT \
    PAT_TERM, '0', '9', \
    PAT_ANY|PAT_TERM, '0', '9', \
    PAT_TERM, '.', '.', \
    PAT_MANY|PAT_TERM, '0', '9'

#define PAT_ALPHA \
    PAT_ANY, 'a', 'z' \
    PAT_ANY|PAT_TERM, 'A', 'Z'

#define PAT_UPPER_ALPHA \
    PAT_ANY|PAT_TERM, 'A', 'Z'

#define PAT_LOWER_ALPHA \
    PAT_ANY|PAT_TERM, 'a', 'z'

typedef struct range_chardef {
    byte flags;
    word from;
    word to;
} PatCharDef;

typedef struct match {
    Type type; 
    word flags;
    status state;
    String *s; 
    int position;
    word defPosition;
    int anchor;
    int intval;
} Match;

Match *Match_Make(MemCtx *m, String *s, int anchor, int intval);
Match *Match_MakePat(MemCtx *m, byte *defs, word npats,  int anchor, int intval);
int Match_PatLength(PatCharDef *def);
status Match_Feed(Match *mt, byte c);
void Match_Reset(Match *mt);
