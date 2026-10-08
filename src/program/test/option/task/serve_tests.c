#include <external.h>
#include <caneka.h>
#include <test_module.h>


status Serve_Tests(MemCh *m){
    Debug_Push(m, NULL);

    status r = READY;
    void *args[5];

    /* query the server and check the response here */

    r |= ERROR;
    
    Return(m, r);
}
