#define MEDI_VALUE 0
#define MEDI_FLOOR 1 
#define MEDI_MEDIAN 2 
#define MEDI_CEILING 3

/*
typedef struct medi_rec {
    util meaning;
    NVal val;
    NVal fl;
    NVal md;
    NVal ceil;
}
*/

NGram *MediTree_Make(MemCh *m, Buff *bf);
void MediTree_Add(MemCh *m, NGram *ng, i32 value, util meaning);
util MediTree_Find(MemCh *m, NGram *ng, i32 value);
void MediTree_Query(MemCh *m, NGram *ng, i32 value, util meaning);
