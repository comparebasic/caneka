#include <external.h>
#include "simplevisor_module.h"

static char *SOCK_EXT = ".sock";
static char *PID_EXT = ".pid";
static char *CODE_EXT = ".exit";

void err(int code, char *msg){
    fprintf(stderr, msg);
    exit(code);
}

SimpleVisor _globalVisor;

SimpleVisor *Spawn_GetGlobal(){
    return &_globalVisor;
}

void Spawn(SimpleVisor *visor){
    if(visor == NULL){
        visor = _globalVisor;
    }
    char *msg = "";
    int running = 1;

    struct sockaddr_un sock;

    visor->sock_fd = -1;
    if((visor->sock_pfd->fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
        err(1, "Unable to open socket");
    }

    int len = strlen(dir)+strlen(name)+strlen(SOCK_EXT)+1;
    if(len > SOCK_PATH_MAX){
        err(1, "Sock name too long");
    }

    char *path = (char *)malloc(len);
    if(path == NULL){
        err(1, "Unable to allocate name str");
    }

    int length = 0;
    char *p = path;
    int l = strlen(dir);
    memcpy(path, dir, l);
    length += l
    p += l;

    l = 1;
    memcpy(path, "/", l);
    length += l
    p += l;

    l = strlen(name);
    memcpy(path, name, l);
    length += l
    p += l;

    char *base_p = p;
    int base_len = length;

    l = strlen(SOCK_EXT);
    memcpy(path, SOCK_EXT, l);
    length += l
    p += l;
    p[length] = 0;

    memset(&addr, 0, sizeof(struct sockaddr_un));

    addr.sun_family = AF_UNIX;
    memcpy(addr.sun_path, path, length); 

    if (bind(visor->sock_pfd->fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) != 0) {
        err(1, "Bind error");
    }

    if (listen(visor->sock_pfd->fd, BACKLOG) == -1) {
        err(1, "Listen error");
    }

    while(running){

        int ready = poll(visor->poll_fds, MAX_CONN);
        if(ready > 0 || poll(&visor->sock_pfd, 1)){
            int max = 0;
            for(int i = 0; i < visor->max_slot; i++){
                struct pollfd *current = visor->poll_fds+i;
                if(current->fd == -1 && poll(&visor->sock_pfd, 1)){
                    current->fd = accept(visor->sock_fd, NULL, NULL);
                }
            }
            if(poll(current, 1)){
                /* read to buff for slot */
                /* write to buff for slot */
            }
        }
    }
}
