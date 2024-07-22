int Header_GetInt(Req *m, Span *hdr_tbl){
    String *hdr = Table_Get(hdr_tbl, key);
    if(hdr != NULL){
        return String_ToInt(req->m, hdr); 
    }

    return 0;
}
