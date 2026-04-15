status Login_Tests(MemCh *m);
status Session_Tests(MemCh *m);
status Password_Tests(MemCh *m);

static TestSet PassportTests[] = {
    {
        "Password Tests",
        Password_Tests,
        "Password tests with variable salt and funny.",
        FEATURE_COMPLETE,
    },
    {
        "Session Tests",
        Session_Tests,
        "Session Open/Close tests.",
        FEATURE_COMPLETE,
    },
    {
        "Login Tests",
        Login_Tests,
        "User login tests.",
        FEATURE_COMPLETE,
    },
    {
        NULL,
        NULL,
        NULL,
        0,
    }
};
