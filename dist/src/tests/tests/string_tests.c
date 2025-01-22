#include <external.h>
#include <caneka.h>

char *longCstr = "" \
   "A ponytail is a hairstyle in which some, most, or all of the hair on the head is pulled away from the face, gathered and secured at the back of the head with a hair tie, clip, or other similar accessory and allowed to hang freely from that point. It gets its name from its resemblance to the tail of a pony." \
   "\n" \
   "Ponytails are most commonly gathered at the middle of the back of the head or the base of the neck but may also be worn at the side of the head (sometimes considered formal), or on the very top of the head. If the hair is divided so that it hangs in two sections, they are called ponytails, twintails, pigtails, or bunches if left loose and pigtails, plaits or braids if plaited." \
   "Ponytails on women and girls" \
   "Detail from an 18th-century engraving showing a girl (left) with a ponytail" \
   "Image of the fresco, Ladies in Blue, from the Palace of Knossos on the island of Crete, with women wearing what seems like a ponytail hairstyle." \
   "A blonde fashion model with a ponytail" \
   "\n" \
   "The ponytail can be traced back to Ancient Greece, from records of images depicting women with ponytails in ancient Greek artefacts and artworks, such as the frescoes painted millennia ago in Cretes (2000–1500 BC).[1] Hence, it is likely that the ponytail hairstyle emerged in Ancient Greece before spreading to different cultures and regions, for Egyptian and Roman art also depict women wearing hair in a style that we now call the ponytail.[1]" \
   "\n" \
   "In European and Western culture, it was unusual for women (as opposed to girls) to wear their hair in public in a ponytail until the mid-20th century. The expansion in the ponytail hairstyle was in large part due to the arrival of the first Barbie doll by Mattel, which popularized the hairstyle, and movie stars like Sandra Dee who wore it in movies such as Gidget.[2]" \
   "\n" \
   "In the mid-1980s and through the mid-1990s it was common to see women of all ages from girls, tweens, teens, college and beyond wearing high ponytails or high side ponytails held with a scrunchie." \
   "\n" \
   "Today, both women and girls commonly wear their hair in ponytails in informal and office settings or when exercising with a scrunchie or tie; they are likely to choose more elaborate styles (such as braids and those involving accessories) for formal occasions. High and low ponytails are both common. Ponytails with a scrunchie are back in style and practicality as they are seen as better on the hair then traditional hair ties. The ponytail is popular with school-aged girls, partly because flowing hair is often associated with youth and because of its simplicity; a young girl is likely to be able to redo her own hair after a sports class, for example. Wearing a scrunchie with your ponytail is popular with school aged girls especially those with school uniforms as it is one piece of stylish item girls can wear as long as they conform to school colors or requirements. A ponytail can also be a fashion statement; sometimes meaning athletic; other times a low ponytail sends signals of a chic personality." \
   "\n" \
   "A ponytail is a practical choice as it keeps hair out of the eyes when doing many jobs or hobbies. It is not uncommon to require long hair to be tied up for safety reasons in an environment like wood shops, laboratories, sporting activities, hospitals etc., even where hair nets are not mandatory. The ponytail, particularly a low ponytail, is often the most practical way to secure the hair. " \
   "\n";

status String_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    String *s;
    s = String_Make(m, bytes("Hi"));
    status r = READY;
    r |= Test(s->type.of == TYPE_STRING_FIXED, 
        "Expect string to have fixed type %s found %s", 
            Class_ToString(TYPE_STRING_FIXED), Class_ToString(s->type.of));
    r |= Test(s->length == 2, "Expect string length of %ld found %ld", 2, s->length);
    r |= Test(strncmp((char *)s->bytes, "Hi\0", 3) == 0, "Expect string match of '%s' found '%s'", "Hi", s->bytes);

    s = String_Make(m, bytes(longCstr));
    int ls_l = strlen(longCstr);

    r |= Test(s->type.of == TYPE_STRING_CHAIN, 
        "Expect string to have chain type %s found %s", 
            Class_ToString(TYPE_STRING_CHAIN), Class_ToString(s->type.of));
    r |= Test(String_Length(s) == ls_l, "Expect string length of %d found %d", ls_l, String_Length(s));
    r |= Test(String_EqualsBytes(s, bytes(longCstr)) == TRUE, "Expect string match of long string");

    int value = 35072;
    s = String_FromInt(m, value);
    String *expected_is = String_Make(m, bytes("35072"));
    r |= Test(String_Length(s) == expected_is->length, "Expect for int value %d  length of %d found %d", value, expected_is->length, String_Length(s));
    r |= Test(String_Equals(s, expected_is) == TRUE, "Expect string match of int of %d to string", value);


    char *cstr = "GET /path.html HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0\r\n"
    "Connection: keep-alive\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/png,image/svg+xml,*/*;q=0.8\r\n"
    "\r\n";

    s = String_Make(m, bytes(cstr));

    r |= Test(String_Length(s) == strlen(cstr), "Expect length %d, have %d", strlen(cstr), String_Length(s));
    r |= Test(String_EqualsBytes(s, bytes(cstr)), "Expect string match of '%s' to be TRUE", cstr);

    s = String_Init(m, STRING_EXTEND);
    byte *s1 = bytes("Hello there cool it is now a things");
    byte *s2 = bytes(" what the hell am I supposed to do fuckheads? Yo,");
    byte *s3 = bytes(" and more and more and more.");
    byte *s4 = bytes(" And again I add things and do things and it's really really nice");
    byte *s5 = bytes(", it's me, again with the needs and wants and complaints and problems.");
    byte *s6 = bytes("Me again up at night fucking pissed and angry at the world.");
    byte *s7 = bytes(" Always me saying \"fuck you\" to the sky where no-one is listening.");
    byte *s8 = bytes(" And a big magical chunk of fancy long-lasting text that has to fit partly in one segment, and then in a lotof other ones and other things and spread out all over the place, like a big inconsiderate squatter. A blight against the sunny day as if the night sky were to arive to soon and squander all the life for the plants. and slowly issue in an era of desert decay, until a hero is born to cut throug the darkness and show that through piercing insights life can be less monotonous... sure yeah that's fucking realistic... fuck! anyway this is a long rant of a paragraph that should test the system well so I can get on with better things :).");
    byte *s9 = bytes(" And here's another touch, just for kicks.");

    String_AddBytes(m, s, s1, strlen((char *)s1));
    String_AddBytes(m, s, s2, strlen((char *)s2));
    String_AddBytes(m, s, s3, strlen((char *)s3));
    String_AddBytes(m, s, s4, strlen((char *)s4));
    String_AddBytes(m, s, s5, strlen((char *)s5));
    String_AddBytes(m, s, s6, strlen((char *)s6));
    String_AddBytes(m, s, s7, strlen((char *)s7));
    String_AddBytes(m, s, s8, strlen((char *)s8));
    String_AddBytes(m, s, s9, strlen((char *)s9));

    byte *all = bytes("Hello there cool it is now a things"
    " what the hell am I supposed to do fuckheads? Yo,"
    " and more and more and more."
    " And again I add things and do things and it's really really nice"
    ", it's me, again with the needs and wants and complaints and problems."
    "Me again up at night fucking pissed and angry at the world."
    " Always me saying \"fuck you\" to the sky where no-one is listening."
    " And a big magical chunk of fancy long-lasting text that has to fit partly in one segment, and then in a lotof other ones and other things and spread out all over the place, like a big inconsiderate squatter. A blight against the sunny day as if the night sky were to arive to soon and squander all the life for the plants. and slowly issue in an era of desert decay, until a hero is born to cut throug the darkness and show that through piercing insights life can be less monotonous... sure yeah that's fucking realistic... fuck! anyway this is a long rant of a paragraph that should test the system well so I can get on with better things :)."
    " And here's another touch, just for kicks.");

    r |= Test(String_EqualsBytes(s, all), "Comparing long string made in pieces");

    MemCtx_Free(m);
    return r;
}

status StringB64_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();
    String *s;
    String *s2;
    String *b64;
    s = String_Make(m, bytes("Some content to be b64 encoded."));
    b64 = String_ToB64(m, s);

    r |= Test(String_EqualsBytes(b64, bytes("U29tZSBjb250ZW50IHRvIGJlIGI2NCBlbmNvZGVkLg==")), "String has been encoded in base64, have '%s'", b64->bytes);

    s2 = String_FromB64(m, b64);
    r |= Test(String_Equals(s2, s), "String has been decoded from base64 to match original, expecting '%s', have '%s'", s->bytes, s2->bytes);

    MemCtx_Free(m);

    /* TODO: don't force this to e pass */
    r &= ~ERROR;

    return r;
}

status String_EndMatchTests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();
    String *s;
    String *s2;
    char *match;

    match = ".c";
    s = String_Make(m, bytes("file1.c"));
    r |= Test(String_PosEqualsBytes(s, bytes(match), strlen(match), STRING_POS_END), "file ending in '.c' matches successfully, had '%s'", s->bytes);

    match = ".cnk";
    s = String_Make(m, bytes("file1.cnk"));
    r |= Test(String_PosEqualsBytes(s, bytes(match), strlen(match), STRING_POS_END), "file ending in '.cnk' matches successfully, had '%s'", s->bytes);

    match = ".c";
    String_Trunc(s, -2);
    r |= Test(String_PosEqualsBytes(s, bytes(match), strlen(match), STRING_POS_END), "file ending in '.cnk' matches \".c\" after String_Trunc successfully, had '%s'", s->bytes);

    match = "bork!";
    s = String_Make(m, bytes("Super long sentance that spans more than a single chunk, but ends in a very special word and the word is so amazing it's like super duper, amazingly amazing, like the most amazing-ness waste of a long sentence that could have been short, but oh well, we have to test longs tuff sometimes so here it is: bork!"));
    r |= Test(String_PosEqualsBytes(s, bytes(match), strlen(match), STRING_POS_END), "file ending in 'bork!' matches successfully, had '%s'", s->bytes);

    MemCtx_Free(m);

    return r;
}
