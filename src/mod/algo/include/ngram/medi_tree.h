#define MEDI_FLOOR 0
#define MEDI_MEDIAN 1
#define MEDI_CEILING 2

NGram *MediTree_Make(MemCh *m, Buff *bf);
void MediTree_Add(MemCh *m, Ngram *ng, i32 value);
i32 MediTree_Find(MemCh *m, Ngram *ng, i32 value);
void MediTree_Query(MemCh *m, Ngram *ng, i32 value);
