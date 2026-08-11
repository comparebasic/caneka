#include <external.h>
#include <caneka.h>

Node *Serveneka_Setup(MemCh *m, CliArgs *cli){
    Str *configKey = K(m, "config");
    StrVec *configPath = IoUtil_GetAbsVec(m, CliArgs_Get(cli, configKey));
    IoUtil_TrimDir(m, configPath);
    return Json_FromPath(m, configPath);
}
