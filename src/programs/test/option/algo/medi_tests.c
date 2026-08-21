#include <external.h>
#include <caneka.h>
#include <test_module.h>


status MediTree_Tests(MemCh *m){
    Debug_Push(m, NULL);

    status r = READY;
    void *args[5];

    Buff *bf = Buff_Make(m, BUFF_UNBUFFERED|BUFF_CLOBBER|BUFF_DATASYNC);
    Str *path = IoUtil_GetAbsPath(m, S(m, "fixtures/test/medi.ngram"));
    File_Open(bf, path, O_TRUNC|O_CREAT|O_RDWR);

    void *ar[] = {
        path,
        bf,
    };
    Out("Opened file @ -> @\n", ar);
    NGram *medi = MediTree_Make(m, bf);
    medi->type.state |= DEBUG;

    i32 vals[] = { 3, 5, 4, 0, 15, 302, 1, 107, 0 };

    i64 idx = 0;
    while(vals[idx] > 0){
        MediTree_Add(m, medi, vals[idx], idx);
        idx++;
    }

    NGram_PrintFromStart(OutStream, medi);

    idx = 0;
    while(vals[idx] > 0){
        args[0] = I64_Wrapped(m, idx);
        args[1] = I64_Wrapped(m, MediTree_Find(m, medi, vals[idx]));
        args[2] = NULL;
        r|= Test(Equals(args[0], args[1]), 
            "Value found in MediTree matches, expected $, have $", args);

        idx++;
    }

    File_Close(bf);

    Return(m, r);
}
