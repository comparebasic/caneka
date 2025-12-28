enum doc_html_tag_idx {
    DOC_HTML_OPEN = 1,
    DOC_HTML_TITLE,
    DOC_HTML_MOD_COMMENT,
    DOC_HTML_DESC,
    DOC_HTML_FUNC_SECTION,
    DOC_HTML_FUNC_START,
    DOC_HTML_FUNC_NAME,
    DOC_HTML_FUNC_DECL,
    DOC_HTML_FUNC_COMMENT,
    DOC_HTML_FUNC_END,
    DOC_HTML_CLOSE,
};

status Doc_To(Buff *bf, DocComp *comp, ToSFunc func);
status Doc_ToHtmlToS(Buff *bf, void *a, word tagIdx, word flags);
