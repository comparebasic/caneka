#include <external.h>
#include <caneka.h>
#include <test_module.h>

status Http_Tests(MemCh *m){
    Debug_Push(m, NULL);

    void *args[5];
    status r = READY;

    Return(m, r);
}
