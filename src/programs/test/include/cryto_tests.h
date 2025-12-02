status Crypto_Tests(MemCh *m);
static TestSet CryptoTests[] = {
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

