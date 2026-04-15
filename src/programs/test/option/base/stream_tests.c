#include <external.h>
#include <caneka.h>
#include <test_module.h>

status Stream_Tests(MemCh *m){
    status r = READY;
    Buff *bf = Buff_Make(m, ZERO);
    Str *s = Str_CstrRef(m, "Hello");
    Buff_AddBytes(bf, s->bytes, s->length);

    s = Str_CstrRef(m, ", it's really really really sunny outside!");
    Buff_AddBytes(bf, s->bytes, s->length);

    s = Str_CstrRef(m, " And I hear it's going to get even warmer up towards the top of the mountain. It's going to be a good climb, even thought it's really windy up there. Hiking at this time of year takes a lot of determination. Not becuase hiking is challenging, but because, it takes planning, and then a lot of steps, one foot in front of the other, to reach the peak.");
    Buff_AddBytes(bf, s->bytes, s->length);

    s = Str_CstrRef(m, "I'm bringing restaurant supply sourced dehydrated hashbrowns, and a sour-dough starter to make bread and breakfast in the mountains. All we need is cooking gas and stream water to eat like champtions on the wilderness :)");
    Buff_AddBytes(bf, s->bytes, s->length);

    char *cstr =  "Hello"
        ", it's really really really sunny outside!"
        " And I hear it's going to get even warmer up towards the top of the mountain. It's going to be a good climb, even thought it's really windy up there. Hiking at this time of year takes a lot of determination. Not becuase hiking is challenging, but because, it takes planning, and then a lot of steps, one foot in front of the other, to reach the peak."
        "I'm bringing restaurant supply sourced dehydrated hashbrowns, and a sour-dough starter to make bread and breakfast in the mountains. All we need is cooking gas and stream water to eat like champtions on the wilderness :)";

    StrVec *vo = StrVec_From(m, Str_CstrRef(m, cstr));
    r |= Test(Equals(vo, bf->v), 
        "Comparing StrVec built up slowly, and one built all at once ", NULL);

    return r;
}
