enum transp_file_flags {
    TRANSP_FILE_INDEX = 1 << 8,
};

typedef struct transp_file {
    Type type;
    StrVec *name;
    StrVec *fullName;
    StrVec *src;
    StrVec *dest;
} TranspFile;

TranspFile *TranspFile_Make(MemCh *m);
