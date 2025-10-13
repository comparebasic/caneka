#include <external.h>
#include <caneka.h>

status FileDB_Open(FileDB *fdb){
    if((fdb->f->type.state & PROCESSING) == 0 && (File_Open(fdb->f) & PROCESSING) == 0){
        Error(ErrStream->m, (Abstract *)fdb, FUNCNAME, FILENAME, LINENUMBER,
            "Error opening File for FileDB", NULL);
        return ERROR;
    }
    MemCh *m = fdb->f->sm->m;

    fdb->ctx = BinSegCtx_Make(fdb->f->sm,
        NULL, NULL, (BINSEG_VISIBLE|BINSEG_REVERSED));

    File_Close(fdb->f);
    fdb->f->type.state &= ~STREAM_APPEND;
    File_Open(fdb->f);

    Str *entry = Str_Make(m, sizeof(BinSegHeader)*2);
    entry->length = entry->alloc;
    if(Stream_FillStr(fdb->ctx->sm, entry) & SUCCESS){;
        entry = Str_FromHex(m, entry);
        BinSegHeader *hdr = (BinSegHeader *)entry->bytes;
        fdb->ctx->latestId = hdr->id;
    }else{
        Str *entry = Str_Make(m, sizeof(BinSegHeader)*2);
        BinSegHeader *hdr = (BinSegHeader *)entry->bytes;
        hdr->kind = BINSEG_TYPE_INDEX;
        entry = Str_ToHex(m, entry);
        Stream_Bytes(fdb->f->sm, entry->bytes, entry->length);
    }

    File_Close(fdb->f);
    fdb->f->type.state |= STREAM_APPEND;
    File_Open(fdb->f);

    fdb->ctx->type.state |= PROCESSING;
    fdb->type.state |= PROCESSING;

    return fdb->type.state;
}

status FileDB_Close(FileDB *fdb){
    status r = READY;
    MemCh *m = fdb->f->sm->m;
    File_Close(fdb->f);
    fdb->f->type.state &= ~STREAM_APPEND;
    File_Open(fdb->f);

    Str *entry = Str_Make(m, sizeof(BinSegHeader)*2);
    entry->length = entry->alloc;
    if(Stream_FillStr(fdb->ctx->sm, entry) & SUCCESS){;
        entry = Str_FromHex(m, entry);
        BinSegHeader *hdr = (BinSegHeader *)entry->bytes;
        hdr->total = fdb->ctx->latestId;
        entry = Str_ToHex(m, entry);
        Stream_Bytes(fdb->f->sm, entry->bytes, entry->length);
        r |= SUCCESS;
    }else{
        r |= ERROR;
    }

    File_Close(fdb->f);
    return r;
}

status FileDB_Add(FileDB *fdb, i16 id, Abstract *a){
    BinSegCtx *ctx = fdb->ctx;
    if(ctx->type.state & PROCESSING){ 
        BinSegCtx_Send(ctx, a, ctx->func(ctx, NULL));
        return ctx->type.state;
    }else{
        Error(ctx->sm->m, (Abstract *)ctx, FUNCNAME, FILENAME, LINENUMBER,
            "Error FileDB_Add does not have the PROCESSING flag to indicate an open FileDB",
            NULL);
        return ERROR;
    }
}

Table *FileDB_ToTbl(FileDB *fdb, Table *keys){
    BinSegCtx *ctx = fdb->ctx;
    ctx->keys = keys;
    BinSegCtx_LoadStream(ctx);
    return ctx->tbl;
}

FileDB *FileDB_Make(MemCh *m, Str *path){
    FileDB *fdb = (FileDB *)MemCh_AllocOf(m, sizeof(FileDB), TYPE_FILEDB);
    fdb->type.of = TYPE_FILEDB;
    fdb->f = File_Make(m, path, NULL, (STREAM_CREATE|STREAM_TO_FD));
    return fdb;
}
