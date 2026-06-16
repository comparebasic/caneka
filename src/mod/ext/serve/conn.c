#include <external.h>
#include <caneka.h>

status Conn_InetConnect(Buff *bf, HostEnt *h, i32 port){
    MemCh *m = bf->m;
    struct sockaddr_in server;
    i32 fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to make socket", NULL);
        bf->type.state |= ERROR;
        return bf->type.state;
    }

    quad *ip4 = HostEnt_AddrIp4(h);
    Str *s = Ip4_ToStr(m, *ip4);
    server.sin_addr.s_addr = inet_addr((char *)s->bytes);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if(connect(fd, (struct sockaddr *)&server, sizeof(server)) == -1){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to connect", NULL);
        bf->type.state |= ERROR;
        return bf->type.state;
    }

    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
            "Buff setting nonblock", NULL);
        return ERROR;
    }

    Buff_SetSocket(bf, fd);
    return SUCCESS;
}
