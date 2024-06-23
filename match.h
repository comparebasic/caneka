enum anchor_types {
    ANCHOR_CONTAINS = 0,
    ANCHOR_START = 1,
    ANCHOR_UNTIL = 2,
};

enum range_flags {
    RANGE_TERM = 1 << 0,
    RANGE_SINGLE = 1 << 1,
    RANGE_MANY = 1 << 2,
    RANGE_ANY = 1 << 3,
    RANGE_INVERT = 1 << 4,
    RANGE_COUNT = 1 << 5,
    RANGE_COUNTTO = 1 << 6,
};

#define RANGE_WHITESPACE \
    RANGE_SINGLE ' ', ' ', \
    RANGE_SINGLE, '\n', '\n', \
    RANGE_SINGLE, '\r', '\r', \
    RANGE_SINGLE|RANGE_TERM, '\t', '\t'

#define RANGE_INT \
    RANGE_SINGLE|RANGE_TERM, '0', '9'

#define RANGE_FLOAT \
    RANGE_SINGLE|RANGE_TERM, '0', '9', \
    RANGE_ANY|RANGE_TERM, '0', '9', \
    RANGE_SINGLE|RANGE_TERM, '.', '.', \
    RANGE_MANY|RANGE_TERM, '0', '9'

#define RANGE_ALPHA \
    RANGE_ANY, 'a', 'z' \
    RANGE_ANY|RANGE_TERM, 'A', 'Z'

#define RANGE_UPPER_ALPHA \
    RANGE_ANY|RANGE_TERM, 'A', 'Z'

#define RANGE_LOWER_ALPHA \
    RANGE_ANY|RANGE_TERM, 'a', 'z'

typedef struct range_chardef {
    byte flags;
    byte from;
    byte to;
} RCharDef;

typedef struct match {
    cls type; 
    status state;
    String *s; 
    int position;
    int anchor;
    int intval;
} Match;

Match *Match_Make(MemCtx *m, String *s, int anchor, int intval);
status Match_Feed(Match *mt, uchar c);
void Match_Reset(Match *mt);
