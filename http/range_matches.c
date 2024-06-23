#include "external.h"
#include "filestore.h"

byte HTTPV_RangeDef[] = {
    PAT_SINGLE|PAT_TERM, 'H', 'H',
    PAT_COUNT|PAT_TERM, 'T', 2,
    PAT_SINGLE|PAT_TERM, 'P', 'P',
    PAT_SINGLE|PAT_TERM, '/', '/',
    PAT_SINGLE|PAT_TERM, '1', '1',
    PAT_SINGLE|PAT_TERM, '.', '.',
    PAT_SINGLE|PAT_TERM, '0', '1',
    0,0,0
};
