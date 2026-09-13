#include <external.h>
#include <caneka.h>
#include <test_module.h>

char *jsonSimpleStr = "{\"hi\": \"there\"}"
    ;

char *jsonSimpleNlStr = "{\n\"hi\": \"there\"\n}"
    ;

char *jsonComplexStr = ""
    "{\n"
    "    \"endpoints\": [\n"
    "        {\n"
    "            \"interfaces\": [\n"
    "                {\"ip4\":\"0.0.0.0\", \"port\": 8080, \"proto\":\"http\"},\n"
    "                {\"ip4\":\"0.0.0.0\", \"port\": 8443, \"proto\":\"https\"},\n"
    "                {\"ip6\":\"::\", \"port\": 8080, \"proto\":\"http\"},\n"
    "                {\"ip6\":\"::\", \"port\": 8443, \"proto\":\"https\"}\n"
    "            ],\n"
    "            \"handler\": \"http-static\",\n"
    "            \"dir\": \"public\"\n"
    "        },\n"
    "        {\n"
    "            \"interfaces\": [\n"
    "                {\"file\":\"cmd.txt\", \"proto\":\"cmd-file\"}\n"
    "            ],\n"
    "            \"handler\": \"file-command\"\n"
    "        }\n"
    "    ],\n"
    "    \"logs\": {\n"
    "        \"requests\": \"log/req\",\n"
    "        \"error\": \"log/err\",\n"
    "        \"commands\": \"log/cmd\"\n"
    "    }\n"
    "}\n"
    ;

status Json_Tests(MemCh *m){
    Debug_Push(m, NULL);
    status r = READY;
    void *args[5];

    Str *s = NULL;
    Node *n = NULL;

    s = S(m, jsonSimpleStr);
    n = Json_From(m, s);
    args[0] = s;
    args[1] = n;
    args[2] = NULL;
    r |= Test(n != NULL, "Json has been parsed: $ -> $^0", args);
    args[0] = K(m, "there");
    args[1] = Inst_GetChild(n, K(m, "hi"));
    args[2] = NULL;
    r |= Test(Equals(args[0], args[1]), 
        "Expected key/value pair is in Node expected @, have @", args);

    s = S(m, jsonSimpleNlStr);
    n = Json_From(m, s);
    args[0] = s;
    args[1] = n;
    args[2] = NULL;
    r |= Test(n != NULL, "Json has been parsed: $ -> $^0", args);
    args[0] = K(m, "there");
    args[1] = Inst_GetChild(n, K(m, "hi"));
    args[2] = NULL;
    r |= Test(Equals(args[0], args[1]), 
        "Expected key/value pair is in Node expected @, have @", args);

    s = S(m, jsonComplexStr);
    n = Json_From(m, s);
    args[0] = s;
    args[1] = n;
    args[2] = NULL;
    r |= Test(n != NULL, "Json has been parsed: $ -> $^0", args);

    Span *path = Span_Make(m);
    Span_Add(path, S(m, "endpoints"));
    Span_Add(path, I32_Wrapped(m, 0));
    Span_Add(path, S(m, "interfaces"));
    Span_Add(path, I32_Wrapped(m, 1));
    Node *value = Inst_GetByPath(n, path);

    args[0] = Inst_GetChild(value, K(m, "proto"));
    args[1] = Inst_GetChild(value, K(m, "port"));
    args[2] = K(m, "https");
    args[3] = I32_Wrapped(m, 8443);
    args[4] = NULL;
    r |= Test(Equals(args[0], args[2]) && Equals(args[1], args[3]), 
        "Expected key/value pair is in Node expected @/@, have @/@", args);

    Return(m, r);
}
