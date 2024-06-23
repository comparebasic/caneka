#include "external.h"
#include "filestore.h"

byte HTTPV_RangeDef[] = {
    RANGE_SINGLE|RANGE_TERM, 'H', 'H',
    RANGE_COUNT|RANGE_TERM, 'T', 2,
    RANGE_SINGLE|RANGE_TERM, 'P', 'P',
    RANGE_SINGLE|RANGE_TERM, '/', '/',
    RANGE_SINGLE|RANGE_TERM, '1', '1',
    RANGE_SINGLE|RANGE_TERM, '.', '.',
    RANGE_SINGLE|RANGE_TERM, '0', '1',
    0,
};
