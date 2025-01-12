#include <external.h>
#include <caneka.h>
#include <tests/mock_109strings.h>

String *zero_s;
String *one_s;
String *two_s;
String *three_s;
String *four_s;
String *five_s;
String *six_s;
String *seven_s;
String *eight_s;
String *nine_s;
String *ten_s;
String *eleven_s;
String *twelve_s;
String *thirteen_s;
String *fourteen_s;
String *fifteen_s;
String *sixteen_s;
String *seventeen_s;
String *eighteen_s;
String *nineteen_s;
String *twenty_s;
String *twentyOne_s;
String *twentyTwo_s;
String *twentyThree_s;
String *twentyFour_s;
String *twentyFive_s;
String *twentySix_s;
String *twentySeven_s;
String *twentyEight_s;
String *twentyNine_s;
String *thirty_s;
String *thirtyOne_s;
String *thirtyTwo_s;
String *thirtyThree_s;
String *thirtyFour_s;
String *thirtyFive_s;
String *thirtySix_s;
String *thirtySeven_s;
String *thirtyEight_s;
String *thirtyNine_s;
String *fourty_s;
String *fourtyOne_s;
String *fourtyTwo_s;
String *fourtyThree_s;
String *fourtyFour_s;
String *fourtyFive_s;
String *fourtySix_s;
String *fourtySeven_s;
String *fourtyEight_s;
String *fourtyNine_s;
String *fifty_s;
String *fiftyOne_s;
String *fiftyTwo_s;
String *fiftyThree_s;
String *fiftyFour_s;
String *fiftyFive_s;
String *fiftySix_s;
String *fiftySeven_s;
String *fiftyEight_s;
String *fiftyNine_s;
String *sixty_s;
String *sixtyOne_s;
String *sixtyTwo_s;
String *sixtyThree_s;
String *sixtyFour_s;
String *sixtyFive_s;
String *sixtySix_s;
String *sixtySeven_s;
String *sixtyEight_s;
String *sixtyNine_s;
String *seventy_s;
String *seventyOne_s;
String *seventyTwo_s;
String *seventyThree_s;
String *seventyFour_s;
String *seventyFive_s;
String *seventySix_s;
String *seventySeven_s;
String *seventyEight_s;
String *seventyNine_s;
String *eighty_s;
String *eightyOne_s;
String *eightyTwo_s;
String *eightyTwo_s;
String *eightyThree_s;
String *eightyFour_s;
String *eightyFour_s;
String *eightyFive_s;
String *eightySix_s;
String *eightySeven_s;
String *eightyEight_s;
String *eightyNine_s;
String *eightyNine_s;
String *ninety_s;
String *ninetyOne_s;
String *ninetyTwo_s;
String *ninetyThree_s;
String *ninetyFour_s;
String *ninetyFive_s;
String *ninetySix_s;
String *ninetySeven_s;
String *ninetyEight_s;
String *ninetyNine_s;
String *oneHundred_s;
String *oneHundredOne_s;
String *oneHundredTwo_s;
String *oneHundredThree_s;
String *oneHundredFour_s;
String *oneHundredFive_s;
String *oneHundredSix_s;
String *oneHundredSeven_s;
String *oneHundredEight_s;
String *oneHundredNine_s;

String *all109_s[] = {
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


status Make109Strings(MemCtx *m){
    all109_s[0] = zero_s = String_Make(m, bytes("zero"));
    all109_s[1] = one_s = String_Make(m, bytes("one"));
    all109_s[2] = two_s = String_Make(m, bytes("two"));
    all109_s[3] = three_s = String_Make(m, bytes("three"));
    all109_s[4] = four_s = String_Make(m, bytes("four"));
    all109_s[5] = five_s = String_Make(m, bytes("five"));
    all109_s[6] = six_s = String_Make(m, bytes("six"));
    all109_s[7] = seven_s = String_Make(m, bytes("seven"));
    all109_s[8] = eight_s = String_Make(m, bytes("eight"));
    all109_s[9] = nine_s = String_Make(m, bytes("nine"));
    all109_s[10] = ten_s = String_Make(m, bytes("ten"));
    all109_s[11] = eleven_s = String_Make(m, bytes("eleven"));
    all109_s[12] = twelve_s = String_Make(m, bytes("twelve"));
    all109_s[13] = thirteen_s = String_Make(m, bytes("thirteen"));
    all109_s[14] = fourteen_s = String_Make(m, bytes("fourteen"));
    all109_s[15] = fifteen_s = String_Make(m, bytes("fifteen"));
    all109_s[16] = sixteen_s = String_Make(m, bytes("sixteen"));
    all109_s[17] = seventeen_s = String_Make(m, bytes("seventeen"));
    all109_s[18] = eighteen_s = String_Make(m, bytes("eighteen"));
    all109_s[19] = nineteen_s = String_Make(m, bytes("nineteen"));
    all109_s[20] = twenty_s = String_Make(m, bytes("twenty"));
    all109_s[21] = twentyOne_s = String_Make(m, bytes("twentyOne"));
    all109_s[22] = twentyTwo_s = String_Make(m, bytes("twentyTwo"));
    all109_s[23] = twentyThree_s = String_Make(m, bytes("twentyThree"));
    all109_s[24] = twentyFour_s = String_Make(m, bytes("twentyFour"));
    all109_s[25] = twentyFive_s = String_Make(m, bytes("twentyFive"));
    all109_s[26] = twentySix_s = String_Make(m, bytes("twentySix"));
    all109_s[27] = twentySeven_s = String_Make(m, bytes("twentySeven"));
    all109_s[28] = twentyEight_s = String_Make(m, bytes("twentyEight"));
    all109_s[29] = twentyNine_s = String_Make(m, bytes("twentyNine"));
    all109_s[30] = thirty_s = String_Make(m, bytes("thirty"));
    all109_s[31] = thirtyOne_s = String_Make(m, bytes("thirtyOne"));
    all109_s[32] = thirtyTwo_s = String_Make(m, bytes("thirtyTwo"));
    all109_s[33] = thirtyThree_s = String_Make(m, bytes("thirtyThree"));
    all109_s[34] = thirtyFour_s = String_Make(m, bytes("thirtyFour"));
    all109_s[35] = thirtyFive_s = String_Make(m, bytes("thirtyFive"));
    all109_s[36] = thirtySix_s = String_Make(m, bytes("thirtySix"));
    all109_s[37] = thirtySeven_s = String_Make(m, bytes("thirtySeven"));
    all109_s[38] = thirtyEight_s = String_Make(m, bytes("thirtyEight"));
    all109_s[39] = thirtyNine_s = String_Make(m, bytes("thirtyNine"));
    all109_s[40] = fourty_s = String_Make(m, bytes("fourty"));
    all109_s[41] = fourtyOne_s = String_Make(m, bytes("fourtyOne"));
    all109_s[42] = fourtyTwo_s = String_Make(m, bytes("fourtyTwo"));
    all109_s[43] = fourtyThree_s = String_Make(m, bytes("fourtyThree"));
    all109_s[44] = fourtyFour_s = String_Make(m, bytes("fourtyFour"));
    all109_s[45] = fourtyFive_s = String_Make(m, bytes("fourtyFive"));
    all109_s[46] = fourtySix_s = String_Make(m, bytes("fourtySix"));
    all109_s[47] = fourtySeven_s = String_Make(m, bytes("fourtySeven"));
    all109_s[48] = fourtyEight_s = String_Make(m, bytes("fourtyEight"));
    all109_s[49] = fourtyNine_s = String_Make(m, bytes("fourtyNine"));
    all109_s[50] = fifty_s = String_Make(m, bytes("fifty"));
    all109_s[51] = fiftyOne_s = String_Make(m, bytes("fiftyOne"));
    all109_s[52] = fiftyTwo_s = String_Make(m, bytes("fiftyTwo"));
    all109_s[53] = fiftyThree_s = String_Make(m, bytes("fiftyThree"));
    all109_s[54] = fiftyFour_s = String_Make(m, bytes("fiftyFour"));
    all109_s[55] = fiftyFive_s = String_Make(m, bytes("fiftyFive"));
    all109_s[56] = fiftySix_s = String_Make(m, bytes("fiftySix"));
    all109_s[57] = fiftySeven_s = String_Make(m, bytes("fiftySeven"));
    all109_s[58] = fiftyEight_s = String_Make(m, bytes("fiftyEight"));
    all109_s[59] = fiftyNine_s = String_Make(m, bytes("fiftyNine"));
    all109_s[60] = sixty_s = String_Make(m, bytes("sixty"));
    all109_s[61] = sixtyOne_s = String_Make(m, bytes("sixtyOne"));
    all109_s[62] = sixtyTwo_s = String_Make(m, bytes("sixtyTwo"));
    all109_s[63] = sixtyThree_s = String_Make(m, bytes("sixtyThree"));
    all109_s[64] = sixtyFour_s = String_Make(m, bytes("sixtyFour"));
    all109_s[65] = sixtyFive_s = String_Make(m, bytes("sixtyFive"));
    all109_s[66] = sixtySix_s = String_Make(m, bytes("sixtySix"));
    all109_s[67] = sixtySeven_s = String_Make(m, bytes("sixtySeven"));
    all109_s[68] = sixtyEight_s = String_Make(m, bytes("sixtyEight"));
    all109_s[69] = sixtyNine_s = String_Make(m, bytes("sixtyNine"));
    all109_s[70] = seventy_s = String_Make(m, bytes("seventy"));
    all109_s[71] = seventyOne_s = String_Make(m, bytes("seventyOne"));
    all109_s[72] = seventyTwo_s = String_Make(m, bytes("seventyTwo"));
    all109_s[73] = seventyThree_s = String_Make(m, bytes("seventyThree"));
    all109_s[74] = seventyFour_s = String_Make(m, bytes("seventyFour"));
    all109_s[75] = seventyFive_s = String_Make(m, bytes("seventyFive"));
    all109_s[76] = seventySix_s = String_Make(m, bytes("seventySix"));
    all109_s[77] = seventySeven_s = String_Make(m, bytes("seventySeven"));
    all109_s[78] = seventyEight_s = String_Make(m, bytes("seventyEight"));
    all109_s[79] = seventyNine_s = String_Make(m, bytes("seventyNine"));
    all109_s[80] = eighty_s = String_Make(m, bytes("eighty"));
    all109_s[81] = eightyOne_s = String_Make(m, bytes("eightyOne"));
    all109_s[82] = eightyTwo_s = String_Make(m, bytes("eightyTwo"));
    all109_s[82] = eightyTwo_s = String_Make(m, bytes("eightyTwo"));
    all109_s[83] = eightyThree_s = String_Make(m, bytes("eightyThree"));
    all109_s[84] = eightyFour_s = String_Make(m, bytes("eightyFour"));
    all109_s[85] = eightyFive_s = String_Make(m, bytes("eightyFive"));
    all109_s[86] = eightySix_s = String_Make(m, bytes("eightySix"));
    all109_s[87] = eightySeven_s = String_Make(m, bytes("eightySeven"));
    all109_s[88] = eightyEight_s = String_Make(m, bytes("eightyEight"));
    all109_s[89] = eightyNine_s = String_Make(m, bytes("eightyNine"));
    all109_s[90] = ninety_s = String_Make(m, bytes("ninety"));
    all109_s[91] = ninetyOne_s = String_Make(m, bytes("ninetyOne"));
    all109_s[92] = ninetyTwo_s = String_Make(m, bytes("ninetyTwo"));
    all109_s[93] = ninetyThree_s = String_Make(m, bytes("ninetyThree"));
    all109_s[94] = ninetyFour_s = String_Make(m, bytes("ninetyFour"));
    all109_s[95] = ninetyFive_s = String_Make(m, bytes("ninetyFive"));
    all109_s[96] = ninetySix_s = String_Make(m, bytes("ninetySix"));
    all109_s[97] = ninetySeven_s = String_Make(m, bytes("ninetySeven"));
    all109_s[98] = ninetyEight_s = String_Make(m, bytes("ninetyEight"));
    all109_s[99] = ninetyNine_s = String_Make(m, bytes("ninetyNine"));
    all109_s[100] = oneHundred_s = String_Make(m, bytes("oneHundred"));
    all109_s[101] = oneHundredOne_s = String_Make(m, bytes("oneHundredOne"));
    all109_s[102] = oneHundredTwo_s = String_Make(m, bytes("oneHundredTwo"));
    all109_s[103] = oneHundredThree_s = String_Make(m, bytes("oneHundredThree"));
    all109_s[104] = oneHundredFour_s = String_Make(m, bytes("oneHundredFour"));
    all109_s[105] = oneHundredFive_s = String_Make(m, bytes("oneHundredFive"));
    all109_s[106] = oneHundredSix_s = String_Make(m, bytes("oneHundredSix"));
    all109_s[107] = oneHundredSeven_s = String_Make(m, bytes("oneHundredSeven"));
    all109_s[108] = oneHundredEight_s = String_Make(m, bytes("oneHundredEight"));
    all109_s[109] = oneHundredNine_s = String_Make(m, bytes("oneHundredNine"));
    all109_s[110] = NULL;

    return SUCCESS;
}
