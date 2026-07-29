#include <external.h>
#include <caneka.h>

static i32 openAddrToFd(HostEnt *ent){
    i32 port = ent->port;
    i32 fd = 0;
	struct sockaddr_in serv_addr;
    NetAddr *addr = (NetAddr *)ent->addr;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
    if(ent->addr == NULL){
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }else if(ent->addr->type.of == TYPE_NET_ADDR4){
        NetAddr *net = (NetAddr *)ent->addr;
        serv_addr.sin_addr.s_addr = addr->net.ip4addr.sin_addr.s_addr;
    }else{
        void *ar[] = {ent->addr, NULL};
        Error(ent->m, FUNCNAME, FILENAME, LINENUMBER,
            "Unsopported address type @", ar);
        return -1;
    }

	serv_addr.sin_port = htons(port);

    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd setting nonblock", NULL);
		return -1;
    }

    i32 one = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(i32)) < 0) {
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd setting reuse addr", NULL);
		return -1;
	}
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(i32)) < 0) {
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd setting reuse addr", NULL);
		return -1;
	}

	if(bind(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0){
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd binding", NULL);
		return -1;
    }

	if(listen(fd, TCP_LISTEN_BACKLOG) != 0){
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd listening", NULL);
		return -1;
    };

    return fd;
}


void HostEnt_OpenTcp(HostEnt *ent){
    i32 fd = openAddrToFd(ent);
    if(fd > 0){
        ent->pfd->fd = fd;
        ent->pfd->events = POLLIN;
        ent->pfd->revents = 0;
    }else{
        ent->type.state |= ERROR;
    }
}

HostEnt *HostEnt_FromName(MemCh *m, Str *name, Str *service){
    HostEnt *ent = HostEnt_Make(m);
    ent->name = name;
    if(getaddrinfo(Str_Cstr(m, name), Str_Cstr(m, service), NULL, &ent->info) != 0){
        void *ar[] = {
            name, service, NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER, 
            "Error getting network address info for $/$", ar);
        ent->type.state |= ERROR;
    }
    return ent;
}

util HostEnt_Hash(void *a){
    HostEnt *ent = (HostEnt *)a;
    Single *sg = (Single *)ent->addr;
    return (util)sg->val.i;
}

HostEnt *HostEnt_Make(MemCh *m){
    HostEnt *h = MemCh_AllocOf(m, sizeof(HostEnt), TYPE_HOST_ENT);
    h->type.of = TYPE_HOST_ENT;
    h->m = m;
    return h;
}

