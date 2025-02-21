#include <external.h>
#include <caneka.h>
#include <builder.h>

#include "mem/mem.c"
#include "core/core.c"
#include "core/error.c"
#include "string/string.c"
#include "string/string_makers.c"
#include "debug/debug_flags.c"
#include "debug/debug_typestrings.c"
#include "debug/debug_stack.c"
#include "builder/debug_mocks.c"
#include "crypto/sane.c"
/* span */
#include "sequence/slab.c"
#include "sequence/spandef.c"
#include "sequence/span.c"
#include "sequence/spanquery.c"
#include "sequence/iter.c"
/* file status */
#include "persist/file_status.c"
/* dir */
#include "persist/dir.c"
/* spawn */
#include "persist/procdets.c"
#include "persist/subprocess.c"

status BuildCtx_Init(BuildCtx *ctx){
    memset(ctx, 0, sizeof(BuildCtx));
    ctx->type.of = TYPE_BUILDCTX;
    return SUCCESS;
}

status Build(BuildCtx *ctx){
    return NOOP;
}
