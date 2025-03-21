#ifndef __CNK_EXTERNAL_HEADER
#define __CNK_EXTERNAL_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <unistd.h>
#include <termios.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <netinet/in.h>

#ifdef OPENSSL
    #include <openssl/evp.h>
    #include <openssl/pem.h>
    #include <openssl/ecdsa.h>
    #include <openssl/err.h>
    #include <openssl/conf.h>
#endif

#endif
