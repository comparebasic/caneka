#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <poll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <netinet/in.h>

/* openssl */
#ifdef OPENSSL
    #include <openssl/ec.h>
    #include <openssl/ecdsa.h>
    #include <openssl/obj_mac.h>
#endif

