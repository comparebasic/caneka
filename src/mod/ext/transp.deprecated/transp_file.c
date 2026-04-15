#include <external.h>
#include <caneka.h>

TranspFile *TranspFile_Make(MemCh *m){
    TranspFile *tfile = (TranspFile *)MemCh_Alloc(m, TYPE_TRANSP_FILE);
    tfile->type.of = TYPE_TRANSP_FILE;
    return tfile;
}
