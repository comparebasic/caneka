#include <external.h>
#include <caneka.h>

status FileDB_Open(FileDB *fdb){
    DebugStack_Push(NULL, 0);

    MemCh *m = fdb->m;

    File_Open(fdb->bf, fdb->fpath, O_CREAT|O_RDWR);

    Str *entry = Str_Make(m, sizeof(BinSegHeader)*2);
    i16 latestId = 0;
    if(Buff_IsEmpty(fdb->bf)){
        BinSegHeader *hdr = (BinSegHeader *)entry->bytes;
        hdr->kind = BINSEG_TYPE_INDEX;
        entry->length = sizeof(BinSegHeader);
        entry = Str_ToHex(m, entry);
        Buff_Add(fdb->bf, entry);
        Buff_Flush(fdb->bf);
    }else if(fdb->bf->type.state & ERROR){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error checking FileDb Buff size", NULL);
        return ERROR;
    }else{
        Buff_Pos(fdb->bf, 0);
        Buff_GetStr(fdb->bf, entry);
        entry = Str_FromHex(m, entry);
        BinSegHeader *hdr = (BinSegHeader *)entry->bytes;
        latestId = hdr->id;
    }

    Buff_PosEnd(fdb->bf);

    fdb->ctx = BinSegCtx_Make(fdb->bf,
        NULL, NULL, (BINSEG_VISIBLE|BINSEG_REVERSED));
    fdb->ctx->latestId = latestId;

    fdb->ctx->type.state |= PROCESSING;
    fdb->type.state |= PROCESSING;

    fdb->ctx->type.state |= fdb->type.state & DEBUG;

    DebugStack_Pop();
    return fdb->type.state;
}

status FileDB_Close(FileDB *fdb){
    status r = READY;
    Abstract *args[5];
    MemCh *m = fdb->m;
    File_Close(fdb->bf);
    File_Open(fdb->bf, fdb->fpath, O_RDWR);

    if(!Buff_IsEmpty(fdb->bf)){
        Str *entry = Str_Make(m, sizeof(BinSegHeader)*2);
        Buff_PosEnd(fdb->bf);
        Buff_RevGetStr(fdb->bf, entry);
        if(entry->length == entry->alloc){;
            Buff_PosAbs(fdb->bf, 0);
            entry = Str_FromHex(m, entry);
            BinSegHeader *hdr = (BinSegHeader *)entry->bytes;
            hdr->id = fdb->ctx->latestId;
            entry = Str_ToHex(m, entry);
            Buff_Add(fdb->bf, entry);
            Buff_Flush(fdb->bf);
            if(fdb->type.state & DEBUG){
                args[0] = (Abstract *)Ptr_Wrapped(m, hdr, TYPE_BINSEG_HEADER);
                args[1] = NULL;
                Out("^y.Close hdr &^0\n", args);
            }
            r |= SUCCESS;
        }else{
            r |= ERROR;
        }
    }

    File_Close(fdb->bf);
    return r;
}

status FileDB_Add(FileDB *fdb, i16 id, Abstract *a){
    DebugStack_Push(NULL, 0);
    BinSegCtx *ctx = fdb->ctx;
    if(ctx->type.state & PROCESSING){ 
        BinSegCtx_Send(ctx, a, id);
        if(ctx->type.state & DEBUG){
            Abstract *args[3];
            args[0] = (Abstract *)ctx;
            args[1] = NULL;
            Out("^y.FileDB_Add &^0.\n", args);
        }
        DebugStack_Pop();
        return ctx->type.state;
    }else{
        Error(fdb->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error FileDB_Add does not have the PROCESSING flag to indicate an open FileDB",
            NULL);
        DebugStack_Pop();
        return ERROR;
    }
}

Table *FileDB_ToTbl(FileDB *fdb, Table *keys){
    if(fdb->bf != NULL && fdb->bf->type.state & (BUFF_FD|BUFF_SOCKET)){
        File_Close(fdb->bf);
    }
    File_Open(fdb->bf, fdb->fpath, O_RDONLY);
    fdb->ctx->bf = fdb->bf;
    fdb->ctx->keys = keys;
    BinSegCtx_Load(fdb->ctx);
    return fdb->ctx->tbl;
}

FileDB *FileDB_Make(MemCh *m, Str *fpath){
    FileDB *fdb = (FileDB *)MemCh_AllocOf(m, sizeof(FileDB), TYPE_FILEDB);
    fdb->type.of = TYPE_FILEDB;
    fdb->m = m;
    fdb->bf = Buff_Make(m, BUFF_UNBUFFERED|BUFF_FLUSH);
    fdb->fpath = fpath;
    return fdb;
}
