#include <external.h>
#include <caneka.h>

AuthCred *AuthCred_Make(MemCh *m);
status AuthCred_Resolve(MemCh *m, AuthCred *cred, AuthCred *target, Abstract *authentee);
