#include <external.h>
#include <caneka.h>

extern Lookup *AttsTableLookup;

i16 AttTable_Offset(Abstract *a, Abstract *key);
status AttTable_Init(MemCh *m);
status AttTable_Att(MemCh *m, Abstract *a, Abstract *key, Abstract **value);
Single *AttTable_Get(Abstract *a, Abstract *key);
