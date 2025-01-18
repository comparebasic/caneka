#include <external.h>
#include <caneka.h>

static char *longStrLines[] = { 
    "The Brooklyn Bridge contains four main cables, which descend from the tops of \n",
    "the suspension towers and help support the deck. Two are located to the outside \n",
    "of the bridge's roadways, while two are in the median of the roadways.[7] Each \n",
    "main cable measures 15.75 inches (40.0 cm) in diameter and contains 5,282 \n",
    "parallel, galvanized steel wires wrapped closely together in a cylindrical \n",
    "shape.[6][46][47] These wires are bundled in 19 individual strands, with 278 \n",
    "wires to a strand.[46] This was the first use of bundling in a suspension \n",
    "bridge and took several months for workers to tie together.[48] Since the \n",
    "2000s, the main cables have also supported a series of 24-watt LED lighting \n",
    "fixtures, referred to as \"necklace lights\" due to their shape.[49] \n",
    "\n",
    "In addition, either 1,088,[25] 1,096,[50] or 1,520 galvanized steel wire \n",
    "suspender cables hang downward from the main cables.[23] Another 400 cable \n",
    "stays extend diagonally from the towers. The vertical suspender cables and \n",
    "diagonal cable stays hold up the truss structure around the bridge \n",
    "deck.[23][25][50] The bridge's suspenders originally used wire rope, which was \n",
    "replaced in the 1980s with galvanized steel made by Bethlehem Steel.[23][51] \n",
    "The vertical suspender cables measure 8 to 130 feet (2.4 to 39.6 m) long, and \n",
    "the diagonal stays measure 138 to 449 feet (42 to 137 m) long.[50] \n",
    "\n",
    NULL,
};

static char *longStrNoNl[] = {
    "The Brooklyn Bridge contains four main cables, which descend from the tops of ",
    "the suspension towers and help support the deck. Two are located to the outside ",
    "of the bridge's roadways, while two are in the median of the roadways.[7] Each ",
    "main cable measures 15.75 inches (40.0 cm) in diameter and contains 5,282 ",
    "parallel, galvanized steel wires wrapped closely together in a cylindrical ",
    "shape.[6][46][47] These wires are bundled in 19 individual strands, with 278 ",
    "wires to a strand.[46] This was the first use of bundling in a suspension ",
    "bridge and took several months for workers to tie together.[48] Since the ",
    "2000s, the main cables have also supported a series of 24-watt LED lighting ",
    "fixtures, referred to as \"necklace lights\" due to their shape.[49] ",
    "In addition, either 1,088,[25] 1,096,[50] or 1,520 galvanized steel wire ",
    "suspender cables hang downward from the main cables.[23] Another 400 cable ",
    "stays extend diagonally from the towers. The vertical suspender cables and ",
    "diagonal cable stays hold up the truss structure around the bridge ",
    "deck.[23][25][50] The bridge's suspenders originally used wire rope, which was ",
    "replaced in the 1980s with galvanized steel made by Bethlehem Steel.[23][51] ",
    "The vertical suspender cables measure 8 to 130 feet (2.4 to 39.6 m) long, and ",
    "the diagonal stays measure 138 to 449 feet (42 to 137 m) long.[50] ",
    NULL,
};

status StrSnip_Tests(MemCtx *gm){
        status r = READY;
    MemCtx *m = MemCtx_Make();

    String *s = NULL;
    String *exp = NULL;
    String *res = NULL;
    String *sns = NULL;

    s = String_Make(m, bytes("You Be The \\\"Best\\\""));
    exp = String_Make(m, bytes("You Be The \"Best\""));
    res = String_Init(m, STRING_EXTEND);
    sns = String_Init(m, STRING_EXTEND);
    sns->type.state |= FLAG_STRING_CONTIGUOUS;

    StrSnip sn;
    StrSnip_Init(&sn, SUCCESS, 0, 11);
    String_AddBytes(m, sns, bytes(&sn), sizeof(StrSnip));
    StrSnip_Init(&sn, NOOP, 11, 1);
    String_AddBytes(m, sns, bytes(&sn), sizeof(StrSnip));
    StrSnip_Init(&sn, SUCCESS, 12, 5);
    String_AddBytes(m, sns, bytes(&sn), sizeof(StrSnip));
    StrSnip_Init(&sn, NOOP, 17, 1);
    String_AddBytes(m, sns, bytes(&sn), sizeof(StrSnip));
    StrSnip_Init(&sn, SUCCESS, 18, 1);
    String_AddBytes(m, sns, bytes(&sn), sizeof(StrSnip));

    res = StrSnipStr_ToString(m, sns, s);

    r |= Test(String_Equals(res, exp), "Expected String without quotes, have '%s'", String_ToChars(m, res));

    MemCtx_Free(m);
    return r;
}

static PatCharDef textNl[] = {
    {PAT_KO, '\\','\\'},
    {PAT_KO|PAT_KO_TERM, '\n','\n'},
    {PAT_MANY|PAT_TERM, 0, 255},
    {PAT_END, 0, 0}
};

status StrSnipBoundry_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();

    String *s = NULL;
    String *exp = NULL;
    String *res = NULL;
    String *sns = String_Init(m, STRING_EXTEND);
    sns->type.state |= FLAG_STRING_CONTIGUOUS;
    String *expectedSns = String_Init(m, STRING_EXTEND);
    expectedSns->type.state |= FLAG_STRING_CONTIGUOUS;
    Match mt;
    StrSnip sn;

    s = String_Init(m, STRING_EXTEND);
    char **cptr = longStrLines;
    i64 pos = 0;
    i64 adj = pos;
    while(*cptr != NULL){
        i64 len = strlen(*cptr);
        adj = pos;
        String_AddBytes(m, s, bytes(*cptr), len);
        if(*cptr[0] != '\n'){
            StrSnip_Init(&sn, SUCCESS, pos, len-1);
            String_AddBytes(m, expectedSns, bytes(&sn), sizeof(StrSnip));
            StrSnip_Init(&sn, NOOP, pos+len-1, 1);
            String_AddBytes(m, expectedSns, bytes(&sn), sizeof(StrSnip));
        }else{
            StrSnip_Init(&sn, NOOP, pos, 1);
            String_AddBytes(m, expectedSns, bytes(&sn), sizeof(StrSnip));
        }
        pos += len;
        cptr++;
    }

    cptr = longStrNoNl;
    exp = String_Init(m, STRING_EXTEND);
    while(*cptr != NULL){
        String_AddBytes(m, exp, bytes(*cptr), strlen(*cptr));
        cptr++;
    }

    Match_SetPattern(&mt, textNl, sns);
    StrSnip_Init(&sn, SUCCESS, 0, 0);
    IterStr it;
    IterStr_Init(&it, s, 1);
    while((IterStr_Next(&it) & END) == 0){
        byte *b = (byte *)IterStr_Get(&it);
        Match_Feed(m, &mt, *b);
        if((mt.type.state & SUCCESS) != 0){
            i64 pos = it.cursor.offset+it.cursor.local;
            Match_SetPattern(&mt, textNl, sns);
            mt.snip.start = pos+1;
        }
    }

    r |= Test(String_Equals(sns, expectedSns), "Expect SNS from match to equals expected SNS built manually");

    res = StrSnipStr_ToString(m, sns, s);
    r |= Test(String_Equals(res, exp), "Expected String without quotes or newlines, have '%s'", String_ToChars(m, res));

    MemCtx_Free(m);

    return r;
}
