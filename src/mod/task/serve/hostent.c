#include <external.h>
#include <caneka.h>

void HostEnt_OpenTcp(MemCh *m, HostEnt *ent){
    void *ar[] = {
        ent,
        NULL
    };
    Out("^y.Attempting to bind to @^0\n", ar);

    if(ent->addr == NULL){
        ent->type.state |= ERROR;
        void *ar[] = {ent, NULL};
        Error(ent->m, FUNCNAME, FILENAME, LINENUMBER,
            "Unsupported address type @", ar);
        return;
    }

    if(ent->addr->type.of == TYPE_NET_ADDR6){
        ent->pfd->fd = socket(AF_INET6, SOCK_STREAM, 0);
    }else{
        ent->pfd->fd = socket(AF_INET, SOCK_STREAM, 0);
    }
    if (fcntl(ent->pfd->fd, F_SETFL, O_NONBLOCK) == -1) {
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd setting nonblock", NULL);
		return;
    }

    i32 one = 1;
    if(setsockopt(ent->pfd->fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(i32)) < 0 || 
        setsockopt(ent->pfd->fd, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(i32)) < 0) {
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd setting reuse addr or port", NULL);
		return;
	}

    void *sin_addr = NULL;
    socklen_t len = 0;
    if(ent->addr->type.of == TYPE_NET_ADDR6){
        sin_addr = &ent->addr->net.ip6addr;
        len = sizeof(ent->addr->net.ip6addr);
    }else{
        sin_addr = &ent->addr->net.ip4addr;
        len = sizeof(ent->addr->net.ip4addr);
    }

	if(bind(ent->pfd->fd, (struct sockaddr*)sin_addr, len) != 0){
        void *ar[] = {ent, NULL};
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd binding: @", ar);
        ent->type.state |= ERROR;
		return;
    }

	if(listen(ent->pfd->fd, TCP_LISTEN_BACKLOG) != 0){
        ent->type.state |= ERROR;
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd listening", NULL);
		return;
    };

    if(ent->pfd->fd <= 0){
        void *ar[] = {ent, NULL};
        ent->type.state |= ERROR;
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error opening Ent: @", NULL);
		return;
    }

    ent->pfd->events = POLLIN;
    ent->pfd->revents = 0;
}

HostEnt *HostEnt_FromClient(MemCh *m, HostEnt *ent){
    return HostEnt_Make(m, Clone(m, ent->client.addr));
}

void HostEnt_WipeClient(MemCh *m, HostEnt *ent){
    ent->client.fd = -1;
    if(ent->client.addr == NULL){
        if(ent->addr != NULL){
            if(ent->addr->type.of == TYPE_NET_ADDR4){
                ent->client.addr = NetAddr_Make4(m);
            }else if(ent->addr->type.of == TYPE_NET_ADDR6){
                ent->client.addr = NetAddr_Make6(m);
            }
        }
    }else{
        cls typeOf = ent->client.addr->type.of;
        memset(ent->client.addr, 0, sizeof(NetAddr));
        ent->client.addr->type.of = typeOf;
        NetAddr_Setup(m, ent->client.addr);
    }
}

status HostEnt_Accept(MemCh *m, HostEnt *ent){
    struct sockaddr *cliaddr = NULL; 
    socklen_t len = 0;
    ent->client.fd = -1;
    if(ent->addr->type.of == TYPE_NET_ADDR4){
        cliaddr = (struct sockaddr *)&ent->client.addr->net.ip4addr;
        len = sizeof(ent->client.addr->net.ip4addr);
    }else if(ent->addr->type.of == TYPE_NET_ADDR6){
        cliaddr = (struct sockaddr *)&ent->client.addr->net.ip6addr;
        len = sizeof(ent->client.addr->net.ip6addr);
    }

    if(len){
        ent->client.fd = accept(ent->pfd->fd, cliaddr, &len);
    }

    if(ent->client.fd > 0){
        return SUCCESS;
    }

    return NOOP;
}

HostEnt *HostEnt_FromName(MemCh *m, Str *name, Str *service){
    struct addrinfo *info;
    if(getaddrinfo(Str_Cstr(m, name), Str_Cstr(m, service), NULL, &info) != 0){
        void *ar[] = {
            name, service, NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER, 
            "Error getting network address info for $/$", ar);
        return NULL;
    }
    
    NetAddr *addr = NetAddr_Make4(m);
    /* ToDo: copy lookup into info NetAddr */
    HostEnt *ent = HostEnt_Make(m, addr);
    ent->name = name;
    return ent;
}

util HostEnt_Hash(void *a){
    HostEnt *ent = (HostEnt *)a;
    util u = (util)ent->addr->net.ip4addr.sin_port;
    return (u << 32) + (util)ent->addr->net.ip4addr.sin_addr.s_addr;
}

boolean HostEnt_Equals(void *_a, void *_b){
    HostEnt *a = (HostEnt *)_a;
    HostEnt *b = (HostEnt *)_b;
    if(a->type.of != b->type.of){
        return FALSE;
    }
    NetAddr *netA = a->addr;
    NetAddr *netB = b->addr;
    return netA->net.ip4addr.sin_addr.s_addr == netB->net.ip4addr.sin_addr.s_addr && 
        netA->net.ip4addr.sin_port == netB->net.ip4addr.sin_port;
}

HostEnt *HostEnt_Make(MemCh *m, NetAddr *addr){
    HostEnt *ent = MemCh_AllocOf(m, sizeof(HostEnt), TYPE_HOST_ENT);
    ent->type.of = TYPE_HOST_ENT;
    ent->m = m;
    ent->addr = addr;
    return ent;
}

