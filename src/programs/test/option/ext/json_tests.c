#include <external.h>
#include <caneka.h>
#include <test_module.h>

char *jsonSimpleStr = "{\"hi\": \"there\"}"
    ;

char *jsonSimpleNlStr = "{\n\"hi\": \"there\"\n}"
    ;

char *jsonComplexStr = ""
  "{\n"
  "    \"endpoints\": {\n"
  "        \"0.0.0.0\": {\n"
  "            \"type\": \"ip4\",\n"
  "            \"port\": 8080,\n"
  "            \"handler\": \"http-static\",\n"
  "            \"dir\": \"public\"\n"
  "        },\n"
  "        \"::\": {\n"
  "            \"type\": \"ip6\",\n"
  "            \"port\": 8443,\n"
  "            \"handler\": \"https-static\",\n"
  "            \"tls-cert\": \"/var/fixtures/server.crt\",\n"
  "            \"tls-key\": \"/var/fixtures/server.key\",\n"
  "            \"dir\": \"public\"\n"
  "        },\n"
  "        \"cmd.txt\": {\n"
  "            \"type\": \"file\",\n"
  "            \"handler\": \"file-commands\"\n"
  "        }\n"
  "    },\n"
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
    r |= Test(n != NULL, "Json has been parsed: $ -> @^0", args);
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
    r |= Test(n != NULL, "Json has been parsed: $ -> @^0", args);
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
    r |= Test(n != NULL, "Json has been parsed: $ -> @^0", args);

    StrVec *path = Path_DotPath(m, S(m, "endpoints.::"));
    Node *value = Inst_GetByPath(n, path);

    args[0] = Inst_GetChild(value, K(m, "type"));
    args[1] = K(m, "ip6");
    args[2] = NULL;
    r |= Test(Equals(args[0], args[1]), 
        "Expected key/value pair is in Node expected @, have @", args);

    Return(m, r);
}
