#include <external.h>
#include <caneka.h>

static int connectToServer(){
	struct sockaddr_in server;
	int incoming_sc = socket(AF_INET , SOCK_STREAM, 0);
	server.sin_addr.s_addr = inet_addr("0.0.0.0");
	server.sin_family = AF_INET;
	server.sin_port = htons( TEST_PORT );

    int r = connect(incoming_sc , (struct sockaddr *)&server , sizeof(server));
    if(r == 0){
        fcntl(incoming_sc, F_SETFL, O_NONBLOCK);
        return incoming_sc;
    }else{
        ExitError(1, "Error connecting to test socket %s\n", strerror(errno));
        close(incoming_sc);
        return -1;
    }
}

int ServeTests_ForkRequest(MemCtx *m, char *msg, ReqTestSpec specs[]){
    int child = fork();
    if(child == (pid_t)-1){
        ExitError(1, "Exit: %s - Fork", msg); 
    }else if(!child){
        char *buff[TEST_BUFF_SIZE+1];
        int fd = connectToServer();
        ssize_t l = 0;
        ReqTestSpec *spec = (ReqTestSpec *)specs;
        while(spec->direction != TEST_SERVE_END){
            while(spec->delay-- > 0){
                Delay();
            }
            if(spec->direction == TEST_SEND){
                while(spec->pos < spec->length){
                    l = send(fd, spec->content, strlen(spec->content), 0);
                    status r = Test(l != -1, "SubProcess - %s: %d of Content sent: '%s'", msg, l,  spec->content);
                    if((r & SUCCESS) == 0){
                        ExitError(1, "Exit: %s - Send problem", msg);
                    }else{
                        spec->pos += l;
                    }
                }
            }else if(spec->direction == TEST_RECV){
                while(spec->pos < spec->length){
                    ssize_t l = recv(fd, buff, TEST_BUFF_SIZE, 0);
                    if(l != -1){
                        if(l > (spec->length-spec->pos)){
                            ExitError(1, "Exit: %s - Content too long", msg);
                        }
                        status r = Test(strncmp((char *)buff, spec->content+spec->pos, l) != 0, "SubProcess - %s: Content matches '%s'", msg, buff);
                        if((r & SUCCESS) == 0){
                            ExitError(1, "Exit: %s - Content mismatch", msg);
                        }else{
                            spec->pos += l;
                        }
                    }
                }
            }
            spec++;
        }
        close(fd);
        exit(0);
        return SUCCESS;
    }

    return child;
}
