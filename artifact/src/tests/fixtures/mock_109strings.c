#include <external.h>
#include <caneka.h>
#include <tests.h>

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

char *toBorNot2B = ""
    "If you value and admire the poople close to you, friends, family, co-workers "
    "and bosses. And you value products and services you can be a part of: loca "
    "restuarants, community organizaions, local art productions. You're sel "
    "confidence, agency, and optamism is likely fairly high."
    "\n"
    "Contrast this with the experience of only admiring far away celebrities "
    "pundints, guru's, influencers, and authors you will likely never meet. And  "
    "dependence on products and media made far away that you will likely never b "
    "part of making."
    "\n"
    "This far away world is lonelier and your confidence and optamism are likel "
    "much lower. Let's bring it closer."
    "\n"
    "Every product from Compare Basic is intended to increase self-expression "
    "utalize personal experience, increase agency, and deepen insights and analysis."
    "\n"
    "The software technology is developed from the ground up to re-imagine ho "
    "computer software can be authored with the intentions of the users composition "
    "and developers comprehension in mind."
    ;

char * expectedCstr = ""    
    "SWYgeW91IHZhbHVlIGFuZCBhZG1pcmUgdGhlIHBvb3BsZSBjbG9zZSB0byB5b3UsIGZyaWVuZHMs"
    "IGZhbWlseSwgY28td29ya2VycyBhbmQgYm9zc2VzLiBBbmQgeW91IHZhbHVlIHByb2R1Y3RzIGFu"
    "ZCBzZXJ2aWNlcyB5b3UgY2FuIGJlIGEgcGFydCBvZjogbG9jYSByZXN0dWFyYW50cywgY29tbXVu"
    "aXR5IG9yZ2FuaXphaW9ucywgbG9jYWwgYXJ0IHByb2R1Y3Rpb25zLiBZb3UncmUgc2VsIGNvbmZp"
    "ZGVuY2UsIGFnZW5jeSwgYW5kIG9wdGFtaXNtIGlzIGxpa2VseSBmYWlybHkgaGlnaC4KQ29udHJh"
    "c3QgdGhpcyB3aXRoIHRoZSBleHBlcmllbmNlIG9mIG9ubHkgYWRtaXJpbmcgZmFyIGF3YXkgY2Vs"
    "ZWJyaXRpZXMgcHVuZGludHMsIGd1cnUncywgaW5mbHVlbmNlcnMsIGFuZCBhdXRob3JzIHlvdSB3"
    ;

Str *zero_s;
Str *one_s;
Str *two_s;
Str *three_s;
Str *four_s;
Str *five_s;
Str *six_s;
Str *seven_s;
Str *eight_s;
Str *nine_s;
Str *ten_s;
Str *eleven_s;
Str *twelve_s;
Str *thirteen_s;
Str *fourteen_s;
Str *fifteen_s;
Str *sixteen_s;
Str *seventeen_s;
Str *eighteen_s;
Str *nineteen_s;
Str *twenty_s;
Str *twentyOne_s;
Str *twentyTwo_s;
Str *twentyThree_s;
Str *twentyFour_s;
Str *twentyFive_s;
Str *twentySix_s;
Str *twentySeven_s;
Str *twentyEight_s;
Str *twentyNine_s;
Str *thirty_s;
Str *thirtyOne_s;
Str *thirtyTwo_s;
Str *thirtyThree_s;
Str *thirtyFour_s;
Str *thirtyFive_s;
Str *thirtySix_s;
Str *thirtySeven_s;
Str *thirtyEight_s;
Str *thirtyNine_s;
Str *fourty_s;
Str *fourtyOne_s;
Str *fourtyTwo_s;
Str *fourtyThree_s;
Str *fourtyFour_s;
Str *fourtyFive_s;
Str *fourtySix_s;
Str *fourtySeven_s;
Str *fourtyEight_s;
Str *fourtyNine_s;
Str *fifty_s;
Str *fiftyOne_s;
Str *fiftyTwo_s;
Str *fiftyThree_s;
Str *fiftyFour_s;
Str *fiftyFive_s;
Str *fiftySix_s;
Str *fiftySeven_s;
Str *fiftyEight_s;
Str *fiftyNine_s;
Str *sixty_s;
Str *sixtyOne_s;
Str *sixtyTwo_s;
Str *sixtyThree_s;
Str *sixtyFour_s;
Str *sixtyFive_s;
Str *sixtySix_s;
Str *sixtySeven_s;
Str *sixtyEight_s;
Str *sixtyNine_s;
Str *seventy_s;
Str *seventyOne_s;
Str *seventyTwo_s;
Str *seventyThree_s;
Str *seventyFour_s;
Str *seventyFive_s;
Str *seventySix_s;
Str *seventySeven_s;
Str *seventyEight_s;
Str *seventyNine_s;
Str *eighty_s;
Str *eightyOne_s;
Str *eightyTwo_s;
Str *eightyTwo_s;
Str *eightyThree_s;
Str *eightyFour_s;
Str *eightyFour_s;
Str *eightyFive_s;
Str *eightySix_s;
Str *eightySeven_s;
Str *eightyEight_s;
Str *eightyNine_s;
Str *eightyNine_s;
Str *ninety_s;
Str *ninetyOne_s;
Str *ninetyTwo_s;
Str *ninetyThree_s;
Str *ninetyFour_s;
Str *ninetyFive_s;
Str *ninetySix_s;
Str *ninetySeven_s;
Str *ninetyEight_s;
Str *ninetyNine_s;
Str *oneHundred_s;
Str *oneHundredOne_s;
Str *oneHundredTwo_s;
Str *oneHundredThree_s;
Str *oneHundredFour_s;
Str *oneHundredFive_s;
Str *oneHundredSix_s;
Str *oneHundredSeven_s;
Str *oneHundredEight_s;
Str *oneHundredNine_s;

Str *all109_s[] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    /* extra NULL */
    NULL,
};


status Make109Strs(MemCh *m){
    all109_s[0] = zero_s = Str_CstrRef(m, "zero");
    all109_s[1] = one_s = Str_CstrRef(m, "one");
    all109_s[2] = two_s = Str_CstrRef(m, "two");
    all109_s[3] = three_s = Str_CstrRef(m, "three");
    all109_s[4] = four_s = Str_CstrRef(m, "four");
    all109_s[5] = five_s = Str_CstrRef(m, "five");
    all109_s[6] = six_s = Str_CstrRef(m, "six");
    all109_s[7] = seven_s = Str_CstrRef(m, "seven");
    all109_s[8] = eight_s = Str_CstrRef(m, "eight");
    all109_s[9] = nine_s = Str_CstrRef(m, "nine");
    all109_s[10] = ten_s = Str_CstrRef(m, "ten");
    all109_s[11] = eleven_s = Str_CstrRef(m, "eleven");
    all109_s[12] = twelve_s = Str_CstrRef(m, "twelve");
    all109_s[13] = thirteen_s = Str_CstrRef(m, "thirteen");
    all109_s[14] = fourteen_s = Str_CstrRef(m, "fourteen");
    all109_s[15] = fifteen_s = Str_CstrRef(m, "fifteen");
    all109_s[16] = sixteen_s = Str_CstrRef(m, "sixteen");
    all109_s[17] = seventeen_s = Str_CstrRef(m, "seventeen");
    all109_s[18] = eighteen_s = Str_CstrRef(m, "eighteen");
    all109_s[19] = nineteen_s = Str_CstrRef(m, "nineteen");
    all109_s[20] = twenty_s = Str_CstrRef(m, "twenty");
    all109_s[21] = twentyOne_s = Str_CstrRef(m, "twentyOne");
    all109_s[22] = twentyTwo_s = Str_CstrRef(m, "twentyTwo");
    all109_s[23] = twentyThree_s = Str_CstrRef(m, "twentyThree");
    all109_s[24] = twentyFour_s = Str_CstrRef(m, "twentyFour");
    all109_s[25] = twentyFive_s = Str_CstrRef(m, "twentyFive");
    all109_s[26] = twentySix_s = Str_CstrRef(m, "twentySix");
    all109_s[27] = twentySeven_s = Str_CstrRef(m, "twentySeven");
    all109_s[28] = twentyEight_s = Str_CstrRef(m, "twentyEight");
    all109_s[29] = twentyNine_s = Str_CstrRef(m, "twentyNine");
    all109_s[30] = thirty_s = Str_CstrRef(m, "thirty");
    all109_s[31] = thirtyOne_s = Str_CstrRef(m, "thirtyOne");
    all109_s[32] = thirtyTwo_s = Str_CstrRef(m, "thirtyTwo");
    all109_s[33] = thirtyThree_s = Str_CstrRef(m, "thirtyThree");
    all109_s[34] = thirtyFour_s = Str_CstrRef(m, "thirtyFour");
    all109_s[35] = thirtyFive_s = Str_CstrRef(m, "thirtyFive");
    all109_s[36] = thirtySix_s = Str_CstrRef(m, "thirtySix");
    all109_s[37] = thirtySeven_s = Str_CstrRef(m, "thirtySeven");
    all109_s[38] = thirtyEight_s = Str_CstrRef(m, "thirtyEight");
    all109_s[39] = thirtyNine_s = Str_CstrRef(m, "thirtyNine");
    all109_s[40] = fourty_s = Str_CstrRef(m, "fourty");
    all109_s[41] = fourtyOne_s = Str_CstrRef(m, "fourtyOne");
    all109_s[42] = fourtyTwo_s = Str_CstrRef(m, "fourtyTwo");
    all109_s[43] = fourtyThree_s = Str_CstrRef(m, "fourtyThree");
    all109_s[44] = fourtyFour_s = Str_CstrRef(m, "fourtyFour");
    all109_s[45] = fourtyFive_s = Str_CstrRef(m, "fourtyFive");
    all109_s[46] = fourtySix_s = Str_CstrRef(m, "fourtySix");
    all109_s[47] = fourtySeven_s = Str_CstrRef(m, "fourtySeven");
    all109_s[48] = fourtyEight_s = Str_CstrRef(m, "fourtyEight");
    all109_s[49] = fourtyNine_s = Str_CstrRef(m, "fourtyNine");
    all109_s[50] = fifty_s = Str_CstrRef(m, "fifty");
    all109_s[51] = fiftyOne_s = Str_CstrRef(m, "fiftyOne");
    all109_s[52] = fiftyTwo_s = Str_CstrRef(m, "fiftyTwo");
    all109_s[53] = fiftyThree_s = Str_CstrRef(m, "fiftyThree");
    all109_s[54] = fiftyFour_s = Str_CstrRef(m, "fiftyFour");
    all109_s[55] = fiftyFive_s = Str_CstrRef(m, "fiftyFive");
    all109_s[56] = fiftySix_s = Str_CstrRef(m, "fiftySix");
    all109_s[57] = fiftySeven_s = Str_CstrRef(m, "fiftySeven");
    all109_s[58] = fiftyEight_s = Str_CstrRef(m, "fiftyEight");
    all109_s[59] = fiftyNine_s = Str_CstrRef(m, "fiftyNine");
    all109_s[60] = sixty_s = Str_CstrRef(m, "sixty");
    all109_s[61] = sixtyOne_s = Str_CstrRef(m, "sixtyOne");
    all109_s[62] = sixtyTwo_s = Str_CstrRef(m, "sixtyTwo");
    all109_s[63] = sixtyThree_s = Str_CstrRef(m, "sixtyThree");
    all109_s[64] = sixtyFour_s = Str_CstrRef(m, "sixtyFour");
    all109_s[65] = sixtyFive_s = Str_CstrRef(m, "sixtyFive");
    all109_s[66] = sixtySix_s = Str_CstrRef(m, "sixtySix");
    all109_s[67] = sixtySeven_s = Str_CstrRef(m, "sixtySeven");
    all109_s[68] = sixtyEight_s = Str_CstrRef(m, "sixtyEight");
    all109_s[69] = sixtyNine_s = Str_CstrRef(m, "sixtyNine");
    all109_s[70] = seventy_s = Str_CstrRef(m, "seventy");
    all109_s[71] = seventyOne_s = Str_CstrRef(m, "seventyOne");
    all109_s[72] = seventyTwo_s = Str_CstrRef(m, "seventyTwo");
    all109_s[73] = seventyThree_s = Str_CstrRef(m, "seventyThree");
    all109_s[74] = seventyFour_s = Str_CstrRef(m, "seventyFour");
    all109_s[75] = seventyFive_s = Str_CstrRef(m, "seventyFive");
    all109_s[76] = seventySix_s = Str_CstrRef(m, "seventySix");
    all109_s[77] = seventySeven_s = Str_CstrRef(m, "seventySeven");
    all109_s[78] = seventyEight_s = Str_CstrRef(m, "seventyEight");
    all109_s[79] = seventyNine_s = Str_CstrRef(m, "seventyNine");
    all109_s[80] = eighty_s = Str_CstrRef(m, "eighty");
    all109_s[81] = eightyOne_s = Str_CstrRef(m, "eightyOne");
    all109_s[82] = eightyTwo_s = Str_CstrRef(m, "eightyTwo");
    all109_s[82] = eightyTwo_s = Str_CstrRef(m, "eightyTwo");
    all109_s[83] = eightyThree_s = Str_CstrRef(m, "eightyThree");
    all109_s[84] = eightyFour_s = Str_CstrRef(m, "eightyFour");
    all109_s[85] = eightyFive_s = Str_CstrRef(m, "eightyFive");
    all109_s[86] = eightySix_s = Str_CstrRef(m, "eightySix");
    all109_s[87] = eightySeven_s = Str_CstrRef(m, "eightySeven");
    all109_s[88] = eightyEight_s = Str_CstrRef(m, "eightyEight");
    all109_s[89] = eightyNine_s = Str_CstrRef(m, "eightyNine");
    all109_s[90] = ninety_s = Str_CstrRef(m, "ninety");
    all109_s[91] = ninetyOne_s = Str_CstrRef(m, "ninetyOne");
    all109_s[92] = ninetyTwo_s = Str_CstrRef(m, "ninetyTwo");
    all109_s[93] = ninetyThree_s = Str_CstrRef(m, "ninetyThree");
    all109_s[94] = ninetyFour_s = Str_CstrRef(m, "ninetyFour");
    all109_s[95] = ninetyFive_s = Str_CstrRef(m, "ninetyFive");
    all109_s[96] = ninetySix_s = Str_CstrRef(m, "ninetySix");
    all109_s[97] = ninetySeven_s = Str_CstrRef(m, "ninetySeven");
    all109_s[98] = ninetyEight_s = Str_CstrRef(m, "ninetyEight");
    all109_s[99] = ninetyNine_s = Str_CstrRef(m, "ninetyNine");
    all109_s[100] = oneHundred_s = Str_CstrRef(m, "oneHundred");
    all109_s[101] = oneHundredOne_s = Str_CstrRef(m, "oneHundredOne");
    all109_s[102] = oneHundredTwo_s = Str_CstrRef(m, "oneHundredTwo");
    all109_s[103] = oneHundredThree_s = Str_CstrRef(m, "oneHundredThree");
    all109_s[104] = oneHundredFour_s = Str_CstrRef(m, "oneHundredFour");
    all109_s[105] = oneHundredFive_s = Str_CstrRef(m, "oneHundredFive");
    all109_s[106] = oneHundredSix_s = Str_CstrRef(m, "oneHundredSix");
    all109_s[107] = oneHundredSeven_s = Str_CstrRef(m, "oneHundredSeven");
    all109_s[108] = oneHundredEight_s = Str_CstrRef(m, "oneHundredEight");
    all109_s[109] = oneHundredNine_s = Str_CstrRef(m, "oneHundredNine");
    all109_s[110] = NULL;

    return SUCCESS;
}
