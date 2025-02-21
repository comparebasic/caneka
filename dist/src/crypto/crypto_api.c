#include <external.h>
#include <caneka.h>

String *Crypto_RandomString(MemCtx *m, int length){
   File file;
   File_Init(&file, String_Make(m, bytes("/dev/random")), NULL, NULL);
   file.abs = file.path;
   File_Read(m, &file, NULL, 0, length);
   file.data->type.state |= FLAG_STRING_BINARY;
   return file.data;
}
