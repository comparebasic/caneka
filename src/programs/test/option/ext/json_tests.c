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
  "            \"0.0.0.0\": {\n"
  "                \"type\": \"ip4\",\n"
  "                \"port\": 8080,\n"
  "                \"handler\": \"http-static\",\n"
  "                \"dir\": \"public\"\n"
  "            },\n"
  "            \"::\": {\n"
  "                \"type\": \"ip6\",\n"
  "                \"port\": 8443,\n"
  "                \"handler\": \"https-static\",\n"
  "                \"tls-cert\": \"/var/fixtures/server.crt\",\n"
  "                \"tls-key\": \"/var/fixtures/server.key\",\n"
  "                \"dir\": \"public\"\n"
  "            },\n"
  "            \"cmd.txt\": {\n"
  "                \"type\": \"file\",\n"
  "                \"handler\": \"file-commands\"\n"
  "            }\n"
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

    char *json[] = {
        jsonSimpleStr,
        jsonSimpleNlStr,
        jsonComplexStr,
        NULL
    };

    char **dptr = json;
    while(*dptr != NULL){
        Str *s = S(m, *dptr);
        args[0] = s;
        args[1] = NULL;
        Out("^p. Parsing @^0\n", args);
        Json_From(m, s);
        dptr++;
    }

    r |= ERROR;

    Return(m, r);
}
