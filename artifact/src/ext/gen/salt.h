#ifndef EXT_SALT_H
#define EXT_SALT_H
static Salt _salt = {
    .fact = 1789,
    .length = SALT_LENGTH,
    .salt512 = (byte *)"_#_"
};
#endif
