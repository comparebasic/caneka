#define SALT_LENGTH 512
typedef struct salt {
    Type type;
    i32 fact;
    i32 length;
    byte *salt512;
} Salt;
