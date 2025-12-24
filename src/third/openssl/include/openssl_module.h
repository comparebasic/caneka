#include <crypto_api.h>

/* openssl includes */
#include <evp.h>
#include <pem.h>
#include <ecdsa.h>
#include <err.h>
#include <param_build.h>
#include <conf.h>

status OpenSsl_ExtFreeInit(MemCh *m);
status OpenSsl_Error(Buff *bf);
