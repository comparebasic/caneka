enum serve_types {
    _SERVE_PROTO_START,
    PROTO_HTTP_1_0,
    PROTO_HTTP_1_1,
    PROTO_HTTP_1_2,
    PROTO_HTTP_2,
    PROTO_HTTP_3,
    _SERVE_PROTO_END,
};

typedef struct ip_address {
    Type type;
    cls proto;
    i32 port;
    union {
        quad v4;
        quad v6[2];
    } ip;
} IpAddress;
