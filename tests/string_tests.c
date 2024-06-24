#include <external.h>
#include <filestore.h>
#include <testsuite.h>

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
    s = String_From(m, bytes("Hi"));
    status r = TEST_OK;
    r |= Test(s->type.of == TYPE_STRING_FIXED, 
        "Expect string to have fixed type %s found %s", 
            Class_ToString(TYPE_STRING_FIXED), Class_ToString(s->type.of));
    r |= Test(s->length == 2, "Expect string length of %ld found %ld", 2, s->length);
    r |= Test(strncmp((char *)s->bytes, "Hi\0", 3) == 0, "Expect string match of '%s' found '%s'", "Hi", s->bytes);

    s = String_From(m, bytes(longCstr));
    int ls_l = strlen(longCstr);
    Debug_Print((String *)s, TYPE_STRING_CHAIN,"Long string variable", COLOR_DARK, FALSE);

    r |= Test(s->type.of == TYPE_STRING_CHAIN, 
        "Expect string to have chain type %s found %s", 
            Class_ToString(TYPE_STRING_CHAIN), Class_ToString(s->type.of));
    r |= Test(String_Length(s) == ls_l, "Expect string length of %d found %d", ls_l, String_Length(s));
    r |= Test(String_EqualsBytes(s, bytes(longCstr)) == TRUE, "Expect string match of long string");

    int value = 35072;
    s = String_FromInt(m, value);
    String *expected_is = String_From(m, bytes("35072"));
    r |= Test(String_Length(s) == expected_is->length, "Expect for int value %d  length of %d found %d", value, expected_is->length, String_Length(s));
    r |= Test(String_Equals(s, expected_is) == TRUE, "Expect string match of int of %d to string", value);

    MemCtx_Free(m);
    return r;
}
