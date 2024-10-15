#include <external.h>
#include <caneka.h>
#include <testsuite.h>

static quad exampleDelayTicks(status r){
    if(r == SUCCESS){
        return 0;
    }else if(r == NOOP){
        return 1;
    }else if(r == INCOMING || r == RESPONDING){
        return 2;
    }
    return 0;
}

status Queue_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();

    Queue *q = Queue_Make(m, exampleDelayTicks);
    String *zero = String_Make(m, bytes("zero"));
    String *one = String_Make(m, bytes("one"));
    String *two = String_Make(m, bytes("two"));
    String *six = String_Make(m, bytes("six"));
    String *thirtySeven = String_Make(m, bytes("thirty-seven"));
    String *sevenHundred = String_Make(m, bytes("seven-hundred"));
    String *dog = String_Make(m, bytes("dog"));
    String *cat = String_Make(m, bytes("cat"));
    String *bear = String_Make(m, bytes("bear"));
    String *panda = String_Make(m, bytes("panda"));
    String *donkey = String_Make(m, bytes("donkey"));
    String *poodle = String_Make(m, bytes("poodle"));
    String *carrot = String_Make(m, bytes("carrot"));
    String *pinnaple = String_Make(m, bytes("pinnaple"));
    String *kangaroo = String_Make(m, bytes("kangaroo"));
    String *iowa = String_Make(m, bytes("iowa"));
    String *indiana = String_Make(m, bytes("indiana"));
    String *idaho = String_Make(m, bytes("idaho"));
    String *idaOhiOwa = String_Make(m, bytes("ida-ohi-owa"));
    String *newYork = String_Make(m, bytes("new-york"));
    String *beach = String_Make(m, bytes("beach"));
    String *ocean = String_Make(m, bytes("ocean"));
    
    String *midwest = String_Make(m, bytes("midwest"));

    Queue_Add(q, (Abstract *)zero);
    Queue_Add(q, (Abstract *)one);
    Queue_Add(q, (Abstract *)two);
    Queue_Remove(q, 1);

    Queue_Add(q, (Abstract *)six);

    QueueIdx *qidx = NULL;
    qidx = Span_Get((Span *)q, 1);
    r |= Test(qidx->item == six, "six has replaed idx 1");

    Queue_Add(q, (Abstract *)thirtySeven);
    Queue_Add(q, (Abstract *)sevenHundred);
    Queue_Add(q, (Abstract *)dog);
    Queue_Add(q, (Abstract *)cat);
    Queue_Add(q, (Abstract *)bear);
    Queue_Add(q, (Abstract *)panda);
    Queue_Add(q, (Abstract *)donkey);
    Queue_Add(q, (Abstract *)poodle);
    Queue_Add(q, (Abstract *)carrot);
    Queue_Add(q, (Abstract *)pinnaple);
    Queue_Add(q, (Abstract *)kangaroo);
    Queue_Add(q, (Abstract *)iowa);
    Queue_Add(q, (Abstract *)indiana);
    Queue_Add(q, (Abstract *)idaho);
    Queue_Add(q, (Abstract *)idaOhiOwa);
    Queue_Add(q, (Abstract *)newYork);
    Queue_Add(q, (Abstract *)beach);
    Queue_Add(q, (Abstract *)ocean);

    Queue_Remove(q, 17);
    Queue_Add(q, (Abstract *)midwest);

    qidx = Span_Get((Span *)q, 17);
    r |= Test(qidx->item == midwest, "midwest has replaed idx 17");

    MemCtx_Free(m);

    r |= SUCCESS;
    return r;
}

status QueueNext_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();

    Queue *q = Queue_Make(m, exampleDelayTicks);
    String *s = NULL;
    int max = 67;
    int count = 0;
    for (int i = 0; i < max; i++){
        s = all109_s[i];
        Queue_Add(q, (Abstract *)s);
        count++;
    }
    r |= Test(q->span.nvalues == max, "Expect queue to have 67 values, have %d\n", q->span.nvalues);
    
    QueueIdx *qidx = NULL;

    qidx = Queue_Next(q);
    s = (String *)qidx->item;
    r |= Test(s == zero_s, "Expect first item to be zero_s, have %s\n", (char *)s->bytes);

    qidx = Queue_Next(q);
    s = (String *)qidx->item;
    r |= Test(s == one_s, "Expect second item to be one_s, have %s\n", (char *)s->bytes);

    for(int i = 2; i < max; i++){
        qidx = Queue_Next(q);
        s = (String *)qidx->item;
        r |= Test(s == all109_s[i], "Expect item %d to be from all109_s(%s), have %s", i, (char *)all109_s[i]->bytes, (char *)s->bytes);
        if((r & ERROR) != 0){
            break;
        }
    }

    MemCtx_Free(m);

    return r;
}

status QueueMixed_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();

    Queue *q = Queue_Make(m, exampleDelayTicks);

    MemCtx_Free(m);

    r |= SUCCESS;
    return r;
}
