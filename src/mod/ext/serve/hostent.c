#include <external.h>
#include <caneka.h>

static i32 openPortToFd(HostEnt *ent){
    i32 port = ent->port;
    i32 fd = 0;
	struct sockaddr_in serv_addr;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
    if(ent->addr == NULL){
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }else if(ent->addr->type.of == TYPE_NET_ADDR4){
        NetAddr *net = (NetAddr *)ent->addr;
        serv_addr.sin_addr.s_addr = net.ip4addr.s_addr;
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
    i32 fd = openPortToFd(ent->port);
    if(fd > 0){
        ent->pfd->fd = fd;
        ent->pfd->events = POLLIN;
        ent->pfd->revents = 0;
    }else{
        ent->type.state |= ERROR;
    }
}

HostEnt *HostEnt_FromName(MemCh *m, Str *name, Str *service){
    HostEnt *h = HostEnt_Make(m);
    h->name = name;
    h->addrs = Span_Make(m);
    if(getaddrinfo(Str_Cstr(m, name), Str_Cstr(m, service), NULL, &h->info) != 0){
        void *ar[] = {
            name, service, NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER, 
            "Error getting network address info for $/$", ar);
        h->type.state |= ERROR;
    }else{
        struct addrinfo *info = h->info;
        while(info != NULL){
            if(info->ai_family == AF_INET){
                struct sockaddr_in *addr = (struct sockaddr_in *)info->ai_addr;
                quad ip4 = addr->sin_addr.s_addr;
                Span_Add(h->addrs, U32_Wrapped(m, ip4));
            }
            info = info->ai_next;
        }
    }
    return h;
}

quad *HostEnt_AddrIp4(HostEnt *h){
    if(h->type.state & ERROR){
        return 0;
    }
    Single *sg = Span_Get(h->addrs, 0);
    return &sg->val.i;
}

util HostEnt_Hash(void *a){
    HostEnt *ent = (HostEnt *)a;
    Single *sg = Span_Get(h->addrs, 0);
    return (util)sg->val.i;
}

util *HostEnt_AddrIp6(HostEnt *h){
    return NULL;
}

HostEnt *HostEnt_Make(MemCh *m){
    HostEnt *h = MemCh_AllocOf(m, sizeof(HostEnt), TYPE_HOST_ENT);
    h->type.of = TYPE_HOST_ENT;
    h->m = m;
    return h;
}

