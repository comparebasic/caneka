#include <external.h>
#include <caneka.h>

static char *ponyTailCstr = "" \
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

static char *ponyTailTwoCstr = ""
    "The first equation of state for hair was developed by C. F. van Wyk in 1946.[7]"
    "\n"
    "Scientists in the UK have formulated a mathematical model that predicts the "
    "shape of a ponytail given the length and random curvature (or curliness) of a "
    "sample of individual hairs. The Ponytail Shape Equation provides an "
    "understanding of how a ponytail is swelled by the outward pressure which arises"
    "from interactions between the component hairs.[8]"
    "\n"
    "The researchers developed a general continuum theory for a bundle of hairs, "
    "treating each hair as an elastic filament with random intrinsic curvature. From"
    "this they created a differential equation for the shape of the bundle relating "
    "the elasticity, gravity, and orientational disorder and extracted a simple "
    "equation of state to relate the swelling pressure to the measured random "
    "curvatures of individual hairs.[9][10] The equation itself is a fourth order "
    "non linear differential equation.[9]"
    "\n"
    "The Rapunzel number is a ratio used in this equation to calculate the effects "
    "of gravity on hair relative to its length.[9]"
    "\n"
    "Ra ≡ ⁠L/l⁠ "
    "\n"
    "This number determines whether a ponytail looks like a fan or whether it arcs "
    "over and becomes nearly vertical at the bottom. A short ponytail of springy "
    "hair with a low Rapunzel number, fans outward. A long ponytail with a high "
    "Rapunzel number, hangs down, as the pull of gravity overwhelms the springiness."
    "\n"
    "It is now also known why jogger's ponytails swing side to side.[11] An up and "
    "down motion is too unstable: a ponytail cannot sway forward and backward "
    "because the jogger's head is in the way. Any slight jostling causes the up and "
    "down movement to become a side to side sway. "
    "\n"
    "The research on the shape of the ponytail won the authors the Ig Nobel for "
    "Physics in 2012.[12]"
    "\n"
    "The Rapunzel number is important for the computer graphics and animation "
    "industry, as it helps animators resolve challenges relating to the realistic "
    "digital representation of hair and hair movement.[8]"
    ;

status StrVec_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();

    StrVec *vc = StrVec_Make(m);
    StrVec_Add(vc, Str_CstrRef(m, "hi dude"));
    StrVec_Add(vc, Str_CstrRef(m, ", what a wild ride!"));
    Str *s = Str_CstrRef(m, "hi dude, what a wild ride!");
    void *args1[] = {vc, s, NULL};
    r |= Test(Equals((Abstract *)vc, (Abstract *)s), "Testing StrVec and Str '_+' vs '_t'", args1);
    StrVec *vr = StrVec_ReAlign(m, vc);
    void *args2[] = {vr, s, NULL};
    r |= Test(Equals((Abstract *)vr, (Abstract *)s), "Testing ReAligned StrVec and Str '_+' vs '_t'", args2);

    Str *st = Str_CstrRef(m, "time");
    Str *sa = Str_CstrRef(m, "afterwards");
    Str *sf = Str_CstrRef(m, "four");
    char *cstr = ", all alone";
    Stream *sm = Stream_MakeStrVec(m);
    i32 twentyNine = 29;
    i64 fiveK = 5987263;
    cls type = TYPE_STR;

    void *args3[] = {st, sa, &type, sf, sf, &twentyNine, &fiveK, vc, NULL};
    StrVec_Fmt(sm, "^DRy.Bold|Red|Yellow^0 then so quit '_t' '_T' _d _D _i4 _i8 _+", args3);

    s = Str_CstrRef(m, "\x1b[1;41;33mBold|Red|Yellow\x1b[0m then so quit 'time' '\x1b[1m\"afterwards\"\x1b[22m' \x1b[1m\"four\"\x1b[22m Str<4/5:\x1b[1m\"four\"\x1b[22m> 29 5987263 hi dude, what a wild ride!");

    r |= Test(Equals((Abstract *)sm->dest.curs->v, (Abstract *)s),
        "Testing StrVec and StrVec from Fmt via Stream", NULL);

    /*
    StrVec_Add(vc, Str_CstrRef(m, "\n\n"));
    StrVec_Add(vc, Str_CstrRef(m, ponyTailCstr));
    StrVec_Add(vc, Str_CstrRef(m, ponyTailTwoCstr));
    */

    MemCh_Free(m);

    return r;
}
