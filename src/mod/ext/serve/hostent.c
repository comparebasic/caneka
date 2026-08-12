#include <external.h>
#include <caneka.h>

void HostEnt_OpenTcp(MemCh *m, HostEnt *ent){
    if(ent->addr == NULL){
        ent->type.state |= ERROR;
        void *ar[] = {ent, NULL};
        Error(ent->m, FUNCNAME, FILENAME, LINENUMBER,
            "Unsupported address type @", ar);
        return;
    }

    ent->pfd->fd = socket(AF_INET, SOCK_STREAM, 0);
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

	if(bind(ent->pfd->fd,
                (struct sockaddr*)&ent->addr->net.ip4addr,
                sizeof(ent->addr->net.ip4addr))
            != 0){
        ent->type.state |= ERROR;
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd binding", NULL);
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

