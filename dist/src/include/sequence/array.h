typedef Abstract ** Array;

Array Array_Make(MemCtx *m, int count);
Array Array_MakeFrom(MemCtx *m, int count, ...);
int Array_Length(void **arr);
