#include <external.h>
#include <caneka.h>

status TcpTask_ReadToRbl(Step *st, Task *tsk){
    struct pollfd *pfd = TcpTask_GetPollFd(tsk)
    ProtoCtx *proto = asIfc(tsk->data, TYPE_PROTO);

    byte buff[SERV_READ_SIZE];
    buff[0] = '\0';
    ssize_t l = recv(pfd->fd, buff, SERV_READ_SIZE, 0);
    status r = NOOP;
    if(l > 0){
        Str *s = Str_From(tsk->m, buff, l);
        Cursor_Add(proto->rbl->curs, s);
        r |= Roebling_Run(proto->rbl);
    }
    
    st->type.state |= (proto->rbl->type.state & (SUCCESS|ERROR));
    return st->type.state;
}

status TcpTask_WriteFromOut(Step *st, Task *tsk){
    struct pollfd *pfd = TcpTask_GetPollFd(tsk)
    ProtoCtx *proto = asIfc(tsk->data, TYPE_PROTO);

    ssize_t l = 0;
    i32 total = 0;
    Str s;
    Str_Init(&s, NULL, 0, 0);
    while(total < SERV_READ_SIZE && (proto->out->type.state & END) == 0){
        if(Cursor_SetNextStr(proto->out, &s, SERV_READ_SIZE-total) & NOOP){
            break;
        }
        if(s->length > 0){
            l = write(pfd->fd, s->bytes, s->length); 
            if(l > 0){
                Cursor_Incr(proto->out, (i32)l);
                total += l;
            }else{
                if(l < 0){
                    tsk->type.state |= ERROR;
                } 
                break;
            }
        }else{
            break;
        }
    }
    
    st->type.state |= (curs->type.state & (SUCCESS|ERROR|END));
    return st->type.state;
}
