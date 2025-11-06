enum tag_atts {
    TAG_CLOSE = 1 << 8,
    TAG_SELFCLOSE = 1 << 8,
};

i64 Tag_Out(Buff *bf, Abstract *name, word flags);
