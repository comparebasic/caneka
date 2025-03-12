#include <external.h>
#include <caneka.h>

#include "../mem/mem_chapter.c"
#include "../mem/mem_ctx.c"
#include "../mem/span.c"
#include "../mem/iter.c"
#include "../mem/mem_debug.c"
#include "../core/ifc.c"
#include "../core/init.c"
#include "../core/equals.c"
#include "../str/str.c"
#include "../str/strvec.c"
#include "../str/str_makers.c"
#include "../str/strvec_fmt.c"
#include "../str/str_utils.c"
#include "../str/str_debug.c"
#include "../str/str_equals.c"
#include "../termio/ansi_str.c"
#include "../debug/debug.c"
#include "./mocks.c"
#include "../sequence/lookup.c"
#include "../debug/debug_flags.c"
/*
#include "../debug/debug_typestrings.c"
*/

char *longCstr = "" \
   "A ponytail is a hairstyle in which some, most, or all of the hair on the head is pulled away from the face, gathered and secured at the back of the head with a hair tie, clip, or other similar accessory and allowed to hang freely from that point. It gets its name from its resemblance to the tail of a pony.\n" \
   "\n" \
   "Ponytails are most commonly gathered at the middle of the back of the head or the base of the neck but may also be worn at the side of the head (sometimes considered formal), or on the very top of the head. If the hair is divided so that it hangs in two sections, they are called ponytails, twintails, pigtails, or bunches if left loose and pigtails, plaits or braids if plaited." \
   "Ponytails on women and girls" \
   "Detail from an 18th-century engraving showing a girl (left) with a ponytail" \
   "Image of the fresco, Ladies in Blue, from the Palace of Knossos on the island of Crete, with women wearing what seems like a ponytail hairstyle." \
   "A blonde fashion model with a ponytail.\n" \
   "\n" \
   "The ponytail can be traced back to Ancient Greece, from records of images depicting women with ponytails in ancient Greek artefacts and artworks, such as the frescoes painted millennia ago in Cretes (2000–1500 BC).[1] Hence, it is likely that the ponytail hairstyle emerged in Ancient Greece before spreading to different cultures and regions, for Egyptian and Roman art also depict women wearing hair in a style that we now call the ponytail.[1]\n" \
   "\n" \
   "In European and Western culture, it was unusual for women (as opposed to girls) to wear their hair in public in a ponytail until the mid-20th century. The expansion in the ponytail hairstyle was in large part due to the arrival of the first Barbie doll by Mattel, which popularized the hairstyle, and movie stars like Sandra Dee who wore it in movies such as Gidget.[2]\n" \
   "\n" \
   "In the mid-1980s and through the mid-1990s it was common to see women of all ages from girls, tweens, teens, college and beyond wearing high ponytails or high side ponytails held with a scrunchie.\n" \
   "\n" \
   "Today, both women and girls commonly wear their hair in ponytails in informal and office settings or when exercising with a scrunchie or tie; they are likely to choose more elaborate styles (such as braids and those involving accessories) for formal occasions. High and low ponytails are both common. Ponytails with a scrunchie are back in style and practicality as they are seen as better on the hair then traditional hair ties. The ponytail is popular with school-aged girls, partly because flowing hair is often associated with youth and because of its simplicity; a young girl is likely to be able to redo her own hair after a sports class, for example. Wearing a scrunchie with your ponytail is popular with school aged girls especially those with school uniforms as it is one piece of stylish item girls can wear as long as they conform to school colors or requirements. A ponytail can also be a fashion statement; sometimes meaning athletic; other times a low ponytail sends signals of a chic personality.\n" \
   "\n" \
   "A ponytail is a practical choice as it keeps hair out of the eyes when doing many jobs or hobbies. It is not uncommon to require long hair to be tied up for safety reasons in an environment like wood shops, laboratories, sporting activities, hospitals etc., even where hair nets are not mandatory. The ponytail, particularly a low ponytail, is often the most practical way to secure the hair.\n" \
   "\n";


int main(int argc, char *argv[]){
    MemChapter *cp = MemChapter_Make(NULL);
    if(cp != NULL){
        printf("MemChapter created successfully\n");
    }

    MemCtx *m = MemCtx_Make();
    Caneka_Init(m);
    if(m != NULL){
        printf("MemCtx created successfully\n");
    }
    MemCtx *debugM = MemCtx_Make();

    /*

    MemCtx_Print(debugM, (Abstract *)m, 0, COLOR_BLUE, TRUE, "MemCtx: ");
    Out("\n");
    Span_Print((Abstract *)&m->p, 0, "Mem Span: ", COLOR_CYAN, TRUE);
    Out("\n");
    MemChapter_Print((Abstract *)cp, 0, "MemChapter: ", COLOR_BLUE, TRUE);
    Out("\n");
    Span_Print((Abstract *)&cp->pages, 0, "Chapter Span: ", COLOR_CYAN, TRUE);
    Out("\n");

    i64 max = 1024;
    for(i64 i = 0; i < max; i++){
        i64 *p = MemCtx_Alloc(m, sizeof(i64));
        memcpy(p, &i, sizeof(i64));
    }

    printf("added first round\n");
    MemCtx_Print((Abstract *)m, 0, "MemCtx: ", COLOR_BLUE, TRUE);
    printf("\n");
    Span_Print((Abstract *)&m->p, 0, "Mem Span: ", COLOR_CYAN, TRUE);
    printf("\n");
    MemChapter_Print((Abstract *)cp, 0, "MemChapter: ", COLOR_BLUE, TRUE);
    printf("\n");
    Span_Print((Abstract *)&cp->pages, 0, "Chapter Span: ", COLOR_CYAN, TRUE);
    printf("\n");

    m->type.range++;
    i64 i = max;
    max *= 2;
    for(; i < max; i++){
        i64 *p = MemCtx_Alloc(m, sizeof(i64));
        memcpy(p, &i, sizeof(i64));
    }

    MemCtx_Print((Abstract *)m, 0, "MemCtx: ",
        COLOR_BLUE, TRUE);
    printf("\n");
    MemChapter_Print((Abstract *)cp, 0, "MemChapter: ",
        COLOR_BLUE, TRUE);
    printf("\n");

    i = max;
    max *= 4;
    for(; i < max; i++){
        i64 *p = MemCtx_Alloc(m, sizeof(i64));
        memcpy(p, &i, sizeof(i64));
    }

    Span_Print((Abstract *)&m->p, 0, "MemCtx Span (after x4): ", COLOR_CYAN, TRUE);
    printf("\n");
    MemCtx_Print((Abstract *)m, 0, "MemCtx (after x4): ", 
        COLOR_BLUE, TRUE);
    printf("\n");
    MemChapter_Print((Abstract *)cp, 0, "MemChapter (after x4): ", 
        COLOR_BLUE, TRUE);
    printf("\n");

    MemCtx_Free(m);
    MemCtx_Print((Abstract *)m, 0, "MemCtx: ", COLOR_BLUE, TRUE);
    printf("\n");
    MemChapter_Print((Abstract *)cp, 0, "MemChapter After Free 1: ", COLOR_BLUE, TRUE);

    fflush(stdout);
    Str_ToFd(three, 0);
    printf("\n");
    StrVec *v = StrVec_Fmt(m, "The %_Rynumber%_0 is %_y%i4%_0\n", -3038);
    printf("\n");
    StrVec_ToFd(v, 0);
    printf("\n");
    Span_Print((Abstract *)v->it.values, 0, "StrVecSpan: ", 33, TRUE);
    printf("\n");

    v = StrVec_Make(m);
    char *cstr = "Hidy Ho\n";
    StrVec_AddBytes(m, v, (byte *)cstr, strlen(cstr));
    StrVec_ToFd(v, 0);
    Span_Print((Abstract *)v->it.values, 0, "StrVecSpan: ", 33, TRUE);
    printf("\n");

    v = StrVec_Make(m);
    StrVec_AddBytes(m, v, (byte *)longCstr, strlen(longCstr));
    StrVec_ToFd(v, 0);
    Span_Print((Abstract *)v->it.values, 0, "StrVecSpan: ", 33, TRUE);
    Out(m, "\n\n");
    */
    
    Str *three = Str_FromI64(m, -3038);
    Out(m, "value is ^r_D^0 ^yor^0 _d ^csimply ^0_v\n", three, three, three);

    char *cstr = "-3038";
    Str *three_s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr));
    Out(m, "Str is _D\n", three_s);
    if(Equals((Abstract *)three, (Abstract *)three_s)){
        Out(m, "Yay _D and _D equal!\n", three, three_s);
    }

    cstr = "And a bit more, across a few boundaries!";
    Str *a_s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr));
    Str *b_s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr));
    if(Equals((Abstract *)a_s, (Abstract *)b_s)){
        Out(m, "^g^Yay _D and _D equal!^0\n", a_s, b_s);
    }
    cstr = "And a bit more, across a few boundaries.";
    Str *c_s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr));
    if(!Equals((Abstract *)a_s, (Abstract *)c_s)){
        Out(m, "^gAnd Nope _D and _D equal!^0\n", a_s, c_s);
    }

    StrVec *v = StrVec_Make(m);
    cstr = "Part one of the Saga";
    StrVec_Add(v, Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)));

    cstr = ".";
    StrVec_Add(v, Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)));
    StrVec_Add(v, Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)));
    StrVec_Add(v, Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)));

    cstr = "Part one of the Saga...";
    Str *d_s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr));

    if(Equals((Abstract *)d_s, (Abstract *)v)){
        Out(m, "^g^And Yes _D  equal a StrVec!^0\n", d_s);
    }else{
        Out(m, "^rAnd Yes _D  equal a StrVec!^0\n", d_s);
    }

    cstr = "Part one of the Saga!!!";
    Str *e_s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr));
    if(!Equals((Abstract *)e_s, (Abstract *)v)){
        Out(m, "^g^And Nope _D  equal a StrVec with a later change!^0\n", e_s);
    }else{
        Out(m, "^r^And Nope _D  equal a StrVec with a later change!^0\n", e_s);
    }

    cstr = "Part Two of the Saga!!!";
    Str *f_s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr));
    if(!Equals((Abstract *)f_s, (Abstract *)v)){
        Out(m, "^g^And Nope _D  equal a StrVec with an early and later change!^0\n", f_s);
    }else{
        Out(m, "^r^And Nope _D  equal a StrVec with an early and later change!^0\n", f_s);
    }

    StrVec *v2 = StrVec_Make(m);
    StrVec_Add(v2, Str_Ref(m, (byte *)"Pa", 2, 2));
    cstr = "rt one of the Saga.";
    StrVec_Add(v2, Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)));
    cstr = "..";
    StrVec_Add(v2, Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)));

    if(Equals((Abstract *)v2, (Abstract *)v)){
        Out(m, "^g^And Yes two equal a StrVecs equal: _D ^0\n", v2);
    }else{
        Out(m, "^r^And Yes two equal a StrVecs equal: _D^0\n", v2);
    }


    exit(0);
}
