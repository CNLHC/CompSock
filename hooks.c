#include "stdio.h"
#include <sys/socket.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <pthread.h>
#include "compress.h"


#define INIT() init_lib_wrapper(__FUNCTION__)
#define SETUP_SYM(X) do { if (! true_ ## X ) true_ ## X = load_sym( # X, X ); } while(0)

pthread_once_t init_once = PTHREAD_ONCE_INIT;
static void setup_hooks(void) ;


static void do_init(void) {
	srand(time(NULL));
	setup_hooks();
}

static void init_lib_wrapper(const char* caller) {
#ifndef DEBUG
	(void) caller;
#endif
	//if(!init_l) PDEBUG("%s called from %s\n", __FUNCTION__,  caller);
	pthread_once(&init_once, do_init);
}
static void* load_sym(char* symname, void* proxyfunc) {
	void *funcptr = dlsym(RTLD_NEXT, symname);

	if(!funcptr) {
		fprintf(stderr, "Cannot load symbol '%s' %s\n", symname, dlerror());
		exit(1);
	} else {
		fprintf(stderr, "loaded symbol '%s'" " real addr %p  wrapped addr %p\n", symname, funcptr, proxyfunc);
	}
	if(funcptr == proxyfunc) {
		fprintf(stdout,"circular reference detected, aborting!\n");
		abort();
	}
	return funcptr;
}
typedef ssize_t (*sendto_t) (int sockfd, const void *buf, size_t len, int flags,
			     const struct sockaddr *dest_addr, socklen_t addrlen);
typedef ssize_t (*send_t) (int sockfd, const void *buf, size_t len, int flags);
       
typedef  ssize_t (*write_t)(int fd, const void *buf, size_t count);


sendto_t true_sendto;
write_t true_write;
send_t true_send;
write_t true_write;

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
	       const struct sockaddr *dest_addr, socklen_t addrlen) {
    INIT();
    fprintf(stdout,"hello world\n");
    return 0 ;
}

char buffer[1<<29];

ssize_t write(int fd, const void *buf, size_t count){
    INIT();
    int csize= compress_buffer(buf,count,buffer,count*2);
    //fprintf(stdout,"write: raw %ld bytes\t compressed %d bytes\n",count,csize);
    return true_write(fd,buf,count);
}


ssize_t send(int sockfd, const void *buf, size_t len, int flags){
    INIT();
    fprintf(stdout,"len:%ld content:%s\n",len,(char *) buf);
    return true_send(sockfd,buf,len,flags);
}

static void setup_hooks(void) {
	SETUP_SYM(sendto);
	SETUP_SYM(write);
	SETUP_SYM(send);
	SETUP_SYM(write);
}


