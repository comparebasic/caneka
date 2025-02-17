/* Read only iteration of the bytes of a string, commonly used to iterate over
 * structs stored as a byte array.
 *
 * iterStr.shelf is only returned from iterStr.Get if there is a break in the
 * bytes composing the size requested by iterStr.sz. in all other cases the
 * returned address lies within the bytes of the string object iterStr.s.
 *
 */

enum iterstr_flags {
    /* flags used from Iter
    FLAG_ITER_LAST = 1 << 10,
    FLAG_ITER_REVERSE = 1 << 11,
    */
    /* flags used from cursor 
    FLAG_CURSOR_SEGMENTED = 1 << 12,
    */
    FLAG_ITERSTR_PAD = 1 << 13,
};

typedef struct iter_string {
    Type type;
    int idx;
    size_t sz;
    Cursor cursor;
    i64 count;
    void *shelf;
} IterStr;

status IterStr_Next(IterStr *it);
void *IterStr_Get(IterStr *it);
IterStr *IterStr_Make(MemCtx *m, String *s, size_t sz, void *shelf);
status IterStr_Init(IterStr *it, String *s, size_t sz, void *shelf);
