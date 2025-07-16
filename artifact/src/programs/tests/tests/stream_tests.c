#include <external.h>
#include <caneka.h>

status Stream_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();
    Stream *sm = Stream_MakeStrVec(m);
    Str *s = Str_CstrRef(m, "Hello");
    Stream_Bytes(sm, s->bytes, s->length);

    s = Str_CstrRef(m, ", it's really really really sunny outside!");
    Stream_Bytes(sm, s->bytes, s->length);

    s = Str_CstrRef(m, " And I hear it's going to get even warmer up towards the top of the mountain. It's going to be a good climb, even thought it's really windy up there. Hiking at this time of year takes a lot of determination. Not becuase hiking is challenging, but because, it takes planning, and then a lot of steps, one foot in front of the other, to reach the peak.");
    Stream_Bytes(sm, s->bytes, s->length);

    s = Str_CstrRef(m, "I'm bringing restaurant supply sourced dehydrated hashbrowns, and a sour-dough starter to make bread and breakfast in the mountains. All we need is cooking gas and stream water to eat like champtions on the wilderness :)");
    Stream_Bytes(sm, s->bytes, s->length);

    char *cstr =  "Hello"
        ", it's really really really sunny outside!"
        " And I hear it's going to get even warmer up towards the top of the mountain. It's going to be a good climb, even thought it's really windy up there. Hiking at this time of year takes a lot of determination. Not becuase hiking is challenging, but because, it takes planning, and then a lot of steps, one foot in front of the other, to reach the peak."
        "I'm bringing restaurant supply sourced dehydrated hashbrowns, and a sour-dough starter to make bread and breakfast in the mountains. All we need is cooking gas and stream water to eat like champtions on the wilderness :)";

    StrVec *vo = StrVec_FromBytes(m, (byte *)cstr, strlen(cstr));
    r |= Test(Equals((Abstract *)vo,(Abstract *)sm->dest.curs->v), 
        "Comparing StrVec built up slowly, and one built all at once ", NULL);

    MemCh_Free(m);
    return r;
}

status StreamB64_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *s;
    Str *s2;
    Str *b64;
    /*
    s = Str_CstrRef(m, "Some content to be b64 encoded.");
    b64 = Str_ToB64(m, s);

    r |= Test(String_EqualsBytes(b64, bytes("U29tZSBjb250ZW50IHRvIGJlIGI2NCBlbmNvZGVkLg==")), "Str has been encoded in base64, have '%s'", b64->bytes);

    s2 = Str_FromB64(m, b64);
    r |= Test(String_Equals(s2, s), "String has been decoded from base64 to match original, expecting '%s', have '%s'", s->bytes, s2->bytes);

    s = Str_CstrRef(m, toBorNot2B);
    b64 = Str_ToB64(m, s);

    Str *exp = Str_CstrRef(m, expectedCstr);
    r |= Test(String_Equals(b64, exp), "String has been encoded in base64,\nexpecting:\n%s\nhave:\n'%s'", expectedCstr, String_ToChars(DebugM, b64));

    s2 = Str_FromB64(m, b64);
    r |= Test(String_Equals(s2, s), "String has been decoded from base64 to match original,\n expecting:\n'%s', \nhave:\n'%s'",
        String_ToChars(DebugM, s), String_ToChars(DebugM,s2));
    */

    MemCh_Free(m);
    return r;
}

