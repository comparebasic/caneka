#define SALT_LENGTH 512
typedef struct salt {
    Type type;
    i32 fact;
    i32 length;
    byte *salt512;
} Salt;

status Salt_FromBuff(Buff *bf, Str *dest);
status Salt_ToBuff(Buff *bf, Str *dest, Str *path);
/* Generated Salt Struct will be added here */
