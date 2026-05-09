#include <external.h>
#include <caneka.h>
#include <test_module.h>


status Fmt_Tests(MemCh *m){
    status r = READY;
    void *args[5];

    Str *s = NULL;

    StrVec *vc = StrVec_Make(m);
    StrVec_Add(vc, Str_CstrRef(m, "hi dude"));
    StrVec_Add(vc, Str_CstrRef(m, ", what a wild ride!"));

    Str *st = Str_CstrRef(m, "time");
    Str *sa = Str_CstrRef(m, "afterwards");
    Str *sf = Str_CstrRef(m, "four");
    Str *sf2 = Str_Clone(m, sf);
    sf2->type.state |= DEBUG;
    char *cstr = ", all alone";
    Buff *bf = Buff_Make(m, ZERO);

    i32 twentyNine = 29;
    i64 fiveK = 5987263;
    void *args3[] = {
        st,
        sa,
        sf,
        sf2,
        I32_Wrapped(m, twentyNine),
        I64_Wrapped(m, fiveK),
        vc,
        NULL
    };

    
    Fmt(bf, "^DRy.Bold|Red|Yellow^0 then so quit '$' '@' @ & $ $ $", args3);

    if(Ansi_HasColor()){
        s = Str_FromCstr(m, "\x1b[1;41;33mBold|Red|Yellow\x1b[0m then so quit 'time'"
            " '\"\x1b[1mafterwards\x1b[22m\"' \"\x1b[1mfour\x1b[22m\" Str<[\x1b[1mDEBUG\x1b[22m]"
            " 4/5:\"\x1b[1mfour\x1b[22m\"> 29 5987263 hi dude, what a wild ride!",
            ZERO);

        s->type.state |= DEBUG;
        void *args[] = {
            s, bf->v, NULL
        };
        r |= Test(Equals(bf->v, s),
            "Testing StrVec and StrVec from Fmt via Buff expected:$, have:$", args);
    }

    return r;
}

status FmtMem_Tests(MemCh *m){
    status r = READY;
    void *args[5];

    StrVec *v = StrVec_Make(m);
    StrVec_Add(v, S(m, "Hi there, this is really cool."));

    MemCh *mm = MemCh_Make();
    Buff *bf = Buff_Make(mm, BUFF_UNBUFFERED|BUFF_CLOBBER);

    Span *p = Span_Make(mm);
    Span_Add(p, S(mm, "Halo?"));

    args[0] = mm;
    args[1] = NULL;
    Out("^p.MemCh:&\n", args);

    File_Open(bf, S(m, "dist/test/fmt-output.txt"), O_CREAT|O_TRUNC|O_WRONLY);  
    r |= Test((bf->type.state & ERROR) == 0, "Destination file opened", NULL);

    i64 orig = 0;
    MemCh_CountBytes(bf->m, &orig);

    args[0] = S(m, "a number");
    args[1] = I32_Wrapped(m, 27);
    args[2] = NULL;

    Fmt(bf, "^p.@ ^E.@^0\n", args);

    i64 updated = 0;
    MemCh_CountBytes(bf->m, &updated);

    args[0] = Str_MemCount(m, orig);
    args[1] = Str_MemCount(m, updated);
    args[2] = Str_MemCount(m, updated - orig);
    args[3] = NULL;
    r |= Test(updated == orig, "Memory has remained consistent after Fmt, Start:@ Updated:@ Delta:@", args);

    args[0] = mm;
    args[1] = NULL;
    Out("^p.MemCh:&\n", args);

    r |= ERROR;
    return r;
}
