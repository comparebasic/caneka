extern Lookup CampMap;

enum cam_flags {
    CAM_ADD = 1 << 8,
    CAM_REMOVE = 1 << 9,
    CAM_UPDATE = 1 << 10,
    CAM_NEW = 1 << 11,
    CAM_OPER = 1 << 12,
};

typedef struct cam_def {
    Type type;
    Type objType;
    SourceFunc func;
    Span *opers;
} CamDef;

typedef struct cam {
    Type type;
    i32 idx;
    i32 parent;
    quad parity;
    quad author;
    void *data;
} Cam;

Cam *Cam_Make(MemCh *m, i32 target, i32 idx, util parity, util unique, void *data);
CamDef *CamDef_Make(MemCh *m, cls typeOf, SourceFunc func, Span *opers);
