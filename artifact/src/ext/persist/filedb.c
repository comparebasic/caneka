#include <external.h>
#include <caneka.h>

status FileDB_Open(FileDB *fdb){
    DebugStack_Push(NULL, 0);
    /*
    MemCh *m = fdb->m;

    fdb->f->type.state |= STREAM_FROM_FD;
    File_Open(fdb->bf, fdb->path, O_CREAT|O_WRONLY);

    Str *entry = Str_Make(m, sizeof(BinSegHeader)*2);
    i16 latestId = 0;
    if(lseek(fdb->f->sm->fd, 0, SEEK_END) == 0){
        BinSegHeader *hdr = (BinSegHeader *)entry->bytes;
        hdr->kind = BINSEG_TYPE_INDEX;
        entry->length = sizeof(BinSegHeader);
        entry = Str_ToHex(m, entry);
        Stream_Bytes(fdb->f->sm, entry->bytes, entry->length);
    }else{
        lseek(fdb->f->sm->fd, 0, SEEK_SET);
        entry->length = entry->alloc;
        Stream_FillStr(fdb->f->sm, entry);
        entry = Str_FromHex(m, entry);
        BinSegHeader *hdr = (BinSegHeader *)entry->bytes;
        latestId = hdr->id;
    }

    File_Close(fdb->bf);
    fdb->f->type.state &= ~STREAM_FROM_FD;

    fdb->f->type.state |= STREAM_APPEND;
    File_Open(fdb->f);
    lseek(fdb->f->sm->fd, 0, SEEK_END);

    fdb->ctx = BinSegCtx_Make(fdb->f->sm,
        NULL, NULL, (BINSEG_VISIBLE|BINSEG_REVERSED));
    fdb->ctx->latestId = latestId;
    fdb->ctx->sm = fdb->f->sm;

    fdb->ctx->type.state |= PROCESSING;
    fdb->type.state |= PROCESSING;

    */
    DebugStack_Pop();
    return fdb->type.state;
}

status FileDB_Close(FileDB *fdb){
    status r = READY;
    /*
    MemCh *m = fdb->m;
    File_Close(fdb->f);
    fdb->f->type.state &= ~STREAM_APPEND;
    fdb->f->type.state |= ~STREAM_FROM_FD;
    File_Open(fdb->f);
    lseek(fdb->f->sm->fd, 0, SEEK_SET);

    Str *entry = Str_Make(m, sizeof(BinSegHeader)*2);
    entry->length = entry->alloc;
    if(Stream_FillStr(fdb->ctx->sm, entry) & SUCCESS){;
        lseek(fdb->f->sm->fd, 0, SEEK_SET);
        entry = Str_FromHex(m, entry);
        BinSegHeader *hdr = (BinSegHeader *)entry->bytes;
        hdr->id = fdb->ctx->latestId;
        entry = Str_ToHex(m, entry);
        Stream_Bytes(fdb->f->sm, entry->bytes, entry->length);
        r |= SUCCESS;
    }else{
        r |= ERROR;
    }

    File_Close(fdb->f);
    */
    return r;
}

status FileDB_Add(FileDB *fdb, i16 id, Abstract *a){
    return NOOP;
    /*
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
        Error(ctx->sm->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error FileDB_Add does not have the PROCESSING flag to indicate an open FileDB",
            NULL);
        DebugStack_Pop();
        return ERROR;
    }
    */
}

Table *FileDB_ToTbl(FileDB *fdb, Table *keys){
    /*
    File_Close(fdb->f);
    fdb->f->type.state &= ~(STREAM_APPEND|STREAM_TO_FD|STREAM_CREATE|STREAM_STRVEC);
    fdb->f->type.state |= STREAM_FROM_FD;
    File_Open(fdb->f);
    fdb->ctx->sm = fdb->f->sm;
    fdb->ctx->keys = keys;
    BinSegCtx_LoadStream(fdb->ctx);
    */
    return fdb->ctx->tbl;
}

FileDB *FileDB_Make(MemCh *m, Str *fpath){
    FileDB *fdb = (FileDB *)MemCh_AllocOf(m, sizeof(FileDB), TYPE_FILEDB);
    fdb->type.of = TYPE_FILEDB;
    fdb->m = m;
    fdb->bf = Buff_Make(m, ZERO);
    fdb->fpath = fpath;
    return fdb;
}
