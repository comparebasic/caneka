#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#ifdef LINUX
	#include <sys/epoll.h>
#endif
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
