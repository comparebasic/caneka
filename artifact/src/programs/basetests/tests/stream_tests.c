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
    "aWxsIGxpa2VseSBuZXZlciBtZWV0LiBBbmQgIGRlcGVuZGVuY2Ugb24gcHJvZHVjdHMgYW5kIG1l"
    "ZGlhIG1hZGUgZmFyIGF3YXkgdGhhdCB5b3Ugd2lsbCBsaWtlbHkgbmV2ZXIgYiBwYXJ0IG9mIG1h"
    "a2luZy4KVGhpcyBmYXIgYXdheSB3b3JsZCBpcyBsb25lbGllciBhbmQgeW91ciBjb25maWRlbmNl"
    "IGFuZCBvcHRhbWlzbSBhcmUgbGlrZWwgbXVjaCBsb3dlci4gTGV0J3MgYnJpbmcgaXQgY2xvc2Vy"
    "LgpFdmVyeSBwcm9kdWN0IGZyb20gQ29tcGFyZSBCYXNpYyBpcyBpbnRlbmRlZCB0byBpbmNyZWFz"
    "ZSBzZWxmLWV4cHJlc3Npb24gdXRhbGl6ZSBwZXJzb25hbCBleHBlcmllbmNlLCBpbmNyZWFzZSBh"
    "Z2VuY3ksIGFuZCBkZWVwZW4gaW5zaWdodHMgYW5kIGFuYWx5c2lzLgpUaGUgc29mdHdhcmUgdGVj"
    "aG5vbG9neSBpcyBkZXZlbG9wZWQgZnJvbSB0aGUgZ3JvdW5kIHVwIHRvIHJlLWltYWdpbmUgaG8g"
    "Y29tcHV0ZXIgc29mdHdhcmUgY2FuIGJlIGF1dGhvcmVkIHdpdGggdGhlIGludGVudGlvbnMgb2Yg"
    "dGhlIHVzZXJzIGNvbXBvc2l0aW9uIGFuZCBkZXZlbG9wZXJzIGNvbXByZWhlbnNpb24gaW4gbWlu"
    "ZC4="
    ;

status Stream_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();

    MemCh_Free(m);
    return r;
}

status StreamB64_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *s;
    Str *s2;
    Str *b64;
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

    MemCh_Free(m);
    return r;
}

