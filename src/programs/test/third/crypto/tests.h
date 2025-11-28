static TestSet InterTests[] = {
    {
        "ThirdParty/Api",
        NULL,
        NULL,
        SECTION_LABEL,
    },
    {
        "Crypto Tests",
        Crypto_Tests,
        "Cryptography functionality testing.",
        FEATURE_COMPLETE,
    },
};

status Crypto_Tests(MemCh *gm);
