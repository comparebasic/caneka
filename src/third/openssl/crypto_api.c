#include <external.h>
#include <caneka.h>

Str *Crypto_RandomStr(MemCh *m, int length){
   File file;
   File_Init(&file, Str_CstrRef(m, "/dev/random"), NULL, NULL);
   file.abs = file.path;
   File_Read(m, &file, NULL, 0, length);
   Str *s = file.sm->dest.curs->it.value;
   s->type.state |= STRING_BINARY;
   return s;
}
