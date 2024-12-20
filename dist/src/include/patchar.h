typedef i32 PatTermHdr;

enum pat_term_flags {
    PAT_MANY = 1 << 2, /* M */
    PAT_ANY = 1 << 3, /* N */
    PAT_COUNT = 1 << 5, /* C */
    PAT_GO_ON_FAIL = 1 << 14, /* T */
};

enum pat_char_flags {
    PAT_END = 0, /* E */
    PAT_TERM = 1 << 0, /* X */
    PAT_OPTIONAL = 1 << 1, /* P */
    PAT_INVERT = 1 << 4, /* I */
    PAT_SET_NOOP = 1 << 6, /* U */
    PAT_NO_CAPTURE = 1 << 7, /* G */
    PAT_ALL = 1 << 8, /* A */
    PAT_KO = 1 << 9, /* K */
    PAT_SINGLE = 1 << 10, /* S */
    PAT_CONSUME  = 1 << 11, /* O */
    PAT_LEAVE  = 1 << 12, /* L */
    PAT_CMD = 1 << 13, /* D */
};
