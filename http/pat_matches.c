#include <external.h>
#include <filestore.h>

PatCharDef HttpV_RangeDef[] = {
    {PAT_TERM, 'H', 'H'},
    {PAT_COUNT|PAT_TERM, 'T', 2},
    {PAT_TERM, 'P', 'P'},
    {PAT_TERM, '/', '/'},
    {PAT_TERM, '1', '1'},
    {PAT_TERM, '.', '.'},
    {PAT_TERM, '0', '1'},
    {0,0,0}
};

PatCharDef EndNl_RangeDef[] = {
    {PAT_ANY|PAT_TERM, ' ', ' '},
    {PAT_ANY|PAT_TERM, '\t', '\t'},
    {PAT_TERM, '\r', '\r'},
    {PAT_TERM, '\n', '\n'},
    {0,0,0}
};
