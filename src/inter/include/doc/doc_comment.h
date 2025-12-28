typedef Inst DocComment;

enum doc_comment_props {
    DOC_COMMENT_BODY = 0,
    DOC_COMMENT_REFS = 1,
};

status DocComment_Init(MemCh *m);
