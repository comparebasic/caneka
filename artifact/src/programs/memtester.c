#include <external.h>
#include <caneka.h>

#include "../mem/mem_chapter.c"
#include "../mem/mem.c"
#include "../mem/span/slab.c"
#include "../mem/span/span.c"
#include "../mem/span/spanquery.c"
#include "../mem/iter.c"
#include "../core/core.c"
#include "../core/error.c"
#include "../string/string.c"
#include "../string/string_makers.c"
#include "../string/string_equals.c"
#include "../debug/debug_flags.c"
#include "../debug/debug_typestrings.c"
#include "../debug/debug_stack.c"
#include "../builder/debug_mocks.c"
#include "../crypto/sane.c"

int main(int argc, char *argv[]){
    write(0, "hi\n", 3);
    exit(0);
}
