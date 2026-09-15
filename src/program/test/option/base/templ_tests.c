#include <external.h>
#include <caneka.h>
#include <test_module.h>

status Templ_Tests(MemCh *m){
    void *args[3];
    status r = READY;
    m->level++;

    Templ *templ = Templ_FromCstr(m, 
        "^D.Hi there &, it's % degrees in @{location}^d.");

    void *arr[3];

    Table *tbl = Table_Make(m);
    Table_Set(tbl, K(m, "location"), S(m, "Everywhere"));
    arr[2] = tbl;

    char *names[] = { "Samual", "Shin", "Sara", "Sataya" };
    i32 nums[] = {73, 80, 20, -12};
    char *locations[] = { "DaisyTown", "Mexico", "Gotham", "Iowa"};
    char *expected[] = {
        "\x1b[1mHi there Samual, it's 73 degrees in DaisyTown\x1b[22m",
        "\x1b[1mHi there Shin, it's 80 degrees in Mexico\x1b[22m",
        "\x1b[1mHi there Sara, it's 20 degrees in Gotham\x1b[22m",
        "\x1b[1mHi there Sataya, it's -12 degrees in Iowa\x1b[22m",
    };

    for(i32 i = 0; i < 4; i++){
        arr[0] = S(m, names[i]);
        arr[1] = I32_Wrapped(m, nums[i]);
        Table_Set(tbl, K(m, "location"), S(m, locations[i]));
        
        StrVec *v = Templ_ToVec(m, templ, arr);

        args[0] = S(m, expected[i]);
        args[1] = v;
        args[2] = NULL;

        r |= Test(Equals(args[0], args[1]),
            "StrVec from Templ, expected @, have @", args); 
    }

    return r;
}
